#include "DasGASCues.h"
#include "DasGASAuthoring.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayCueSet.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(DasSampleImpactCue, "GameplayCue.Das.Sample.Impact");
UE_DEFINE_GAMEPLAY_TAG_STATIC(DasSampleAuraCue, "GameplayCue.Das.Sample.Aura");
namespace
{
int32 CueRouteDepth = 0;
FGameplayTag CueTag(FName Name)
{
    const auto Tag = FGameplayTag::RequestGameplayTag(Name, false);
    const auto Root = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue"), false);
    return Tag.IsValid() && Root.IsValid() && Tag != Root && Tag.MatchesTag(Root) ? Tag : FGameplayTag();
}
bool InstallBridge(FGameplayTag Tag, UClass* Class = UDasGameplayCueNotify::StaticClass())
{
    auto* Manager = UAbilitySystemGlobals::Get().GetGameplayCueManager();
    auto* Set = Manager ? Manager->GetRuntimeCueSet() : nullptr;
    if (!Set)
        return false;
    const FSoftObjectPath Path(Class);
    auto* Existing = Set->GameplayCueData.FindByPredicate([&](const auto& D) { return D.GameplayCueTag == Tag; });
    if (Existing)
    {
        // Never take a tag away from another native/asset notify.
        if (Class == UDasGameplayCueNotify::StaticClass() && Existing->LoadedGameplayCueClass &&
            Existing->LoadedGameplayCueClass->IsChildOf(UDasGameplayCueNotify::StaticClass()))
        {
            const auto* Cue = Existing->LoadedGameplayCueClass->GetDefaultObject<UDasGameplayCueNotify>();
            if (!Cue->DefinitionId.IsEmpty() && Cue->GameplayCueTag == Tag)
                return true;
        }
        if (Existing->GameplayCueNotifyObj != Path ||
            (Existing->LoadedGameplayCueClass && Existing->LoadedGameplayCueClass != Class))
            return false;
        Existing->LoadedGameplayCueClass = Class;
        return true;
    }
    // CueSet holds a reference into GameplayCueData across this callback.
    // Adding rows here could reallocate it before native parent routing resumes.
    if (CueRouteDepth > 0)
        return false;
    int32 BridgeCount = 0;
    for (const auto& Data : Set->GameplayCueData)
        BridgeCount += Data.GameplayCueNotifyObj == FSoftObjectPath(UDasGameplayCueNotify::StaticClass()) ||
                               (Data.LoadedGameplayCueClass &&
                                Data.LoadedGameplayCueClass->IsChildOf(UDasGameplayCueNotify::StaticClass()))
                           ? 1
                           : 0;
    if (BridgeCount >= 1024)
        return false;
    Set->AddCues({FGameplayCueReferencePair(Tag, Path)});
    auto* Added = Set->GameplayCueData.FindByPredicate([&](const auto& D) { return D.GameplayCueTag == Tag; });
    if (!Added || Added->GameplayCueNotifyObj != Path)
        return false;
    Added->LoadedGameplayCueClass = Class;
    return true;
}
}
bool RegisterDasGASCueAssets(const TArray<TSubclassOf<UDasGameplayCueNotify>>& Classes)
{
    if (!IsInGameThread() || Classes.Num() > 16)
        return false;
    if (Classes.IsEmpty())
        return true;
    auto* Manager = UAbilitySystemGlobals::Get().GetGameplayCueManager();
    auto* Set = Manager ? Manager->GetRuntimeCueSet() : nullptr;
    if (!Set)
        return false;
    TSet<FName> Tags;
    int32 NewRoutes = 0;
    for (auto Class : Classes)
    {
        if (!Class || Class->HasAnyClassFlags(CLASS_Abstract | CLASS_NewerVersionExists))
            return false;
        const auto* Cue = Class->GetDefaultObject<UDasGameplayCueNotify>();
        const auto Tag = CueTag(Cue->GameplayCueTag.GetTagName());
        if (Cue->DefinitionId.IsEmpty() || !Tag.IsValid() || Tags.Contains(Tag.GetTagName()))
            return false;
        Tags.Add(Tag.GetTagName());
        const auto* Existing =
            Set->GameplayCueData.FindByPredicate([&](const auto& D) { return D.GameplayCueTag == Tag; });
        if (Existing)
        {
            if (Existing->GameplayCueNotifyObj != FSoftObjectPath(Class.Get()) ||
                (Existing->LoadedGameplayCueClass && Existing->LoadedGameplayCueClass != Class.Get()))
                return false;
        }
        else
            ++NewRoutes;
    }
    // Preflight every route before mutation; never replace unrelated notifies.
    if ((NewRoutes && CueRouteDepth) || Set->GameplayCueData.Num() + NewRoutes > 1024)
        return false;
    for (auto Class : Classes)
        if (!InstallBridge(Class->GetDefaultObject<UDasGameplayCueNotify>()->GameplayCueTag, Class.Get()))
            return false;
    return true;
}
bool UDasAuthoredGameplayEffect::PrepareCues() const
{
    return RegisterDasGASCueAssets(CueAssets);
}
UDasGameplayCueNotify::UDasGameplayCueNotify(const FObjectInitializer& Initializer) : Super(Initializer)
{
    IsOverride = false;
}
void UDasGameplayCueNotify::HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type Event,
                                              const FGameplayCueParameters& P)
{
    if (!IsInGameThread() || !IsValid(Target) || Target->IsActorBeingDestroyed())
        return;
    TGuardValue<int32> RouteScope(CueRouteDepth, CueRouteDepth + 1);
    Super::HandleGameplayCue(Target, Event, P);
    if (!IsValid(Target) || Target->IsActorBeingDestroyed())
        return;
    auto* ASC = Cast<UDasAuthoredAbilitySystemComponent>(
        UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target, true));
    if (!ASC)
        return;
    FName Phase;
    switch (Event)
    {
    case EGameplayCueEvent::Executed:
        Phase = TEXT("Executed");
        break;
    case EGameplayCueEvent::OnActive:
        Phase = TEXT("OnActive");
        break;
    case EGameplayCueEvent::WhileActive:
        Phase = TEXT("WhileActive");
        break;
    case EGameplayCueEvent::Removed:
        Phase = TEXT("Removed");
        break;
    default:
        return;
    }
    ASC->ReceiveAuthoredCue(P.OriginalTag.GetTagName(), P.MatchedTagName.GetTagName(), Phase, P,
                            !DefinitionId.IsEmpty());
}
bool UDasAuthoredAbilitySystemComponent::WatchAuthoredCue(FName Name)
{
    const auto Tag = CueTag(Name);
    if (!InitializeAuthoredActorInfo() || !Tag.IsValid() || (!WatchedCues.Contains(Name) && WatchedCues.Num() >= 64) ||
        !InstallBridge(Tag))
        return false;
    WatchedCues.Add(Name);
    return true;
}
bool UDasAuthoredAbilitySystemComponent::UnwatchAuthoredCue(FName Name)
{
    // Deliver Removed first: do not drop the listener to a still-owned looping cue.
    return Live() && !AddedCues.Contains(Name) && WatchedCues.Remove(Name) > 0;
}
bool UDasAuthoredAbilitySystemComponent::MakeAuthoredCueParameters(const FDasGASCueParameters& In,
                                                                   FGameplayCueParameters& Out)
{
    if (!InitializeAuthoredActorInfo() || !FMath::IsFinite(In.RawMagnitude) ||
        !FMath::IsFinite(In.NormalizedMagnitude) || In.NormalizedMagnitude < 0 || In.NormalizedMagnitude > 1 ||
        In.Location.ContainsNaN() || In.Normal.ContainsNaN() ||
        (!In.Normal.IsNearlyZero() && !In.Normal.IsNormalized()) || In.AbilityLevel < 0 || In.AbilityLevel > 10000 ||
        In.EffectLevel < 0 || In.EffectLevel > 10000)
        return false;
    for (auto* Actor : {In.Instigator.Get(), In.EffectCauser.Get()})
        if (Actor && (!IsValid(Actor) || Actor->IsActorBeingDestroyed() || Actor->GetWorld() != GetWorld()))
            return false;
    auto* Source = In.SourceObject.Get();
    if (Source && (!IsValid(Source) || (Source->GetWorld() && Source->GetWorld() != GetWorld())))
        return false;
    Out.RawMagnitude = In.RawMagnitude;
    Out.NormalizedMagnitude = In.NormalizedMagnitude;
    Out.Location = In.Location;
    Out.Normal = In.Normal;
    Out.Instigator = In.Instigator ? In.Instigator.Get() : GetAvatarActor();
    Out.EffectCauser = In.EffectCauser ? In.EffectCauser.Get() : GetAvatarActor();
    Out.SourceObject = Source;
    Out.AbilityLevel = In.AbilityLevel;
    Out.GameplayEffectLevel = In.EffectLevel;
    Out.EffectContext = MakeEffectContext();
    return true;
}
bool UDasAuthoredAbilitySystemComponent::ExecuteAuthoredCue(FName Name, const FDasGASCueParameters& In)
{
    const auto Tag = CueTag(Name);
    FGameplayCueParameters Parameters;
    if (!Tag.IsValid() || !MakeAuthoredCueParameters(In, Parameters))
        return false;
    ExecuteGameplayCue(Tag, Parameters);
    return true;
}
bool UDasAuthoredAbilitySystemComponent::AddAuthoredCue(FName Name, const FDasGASCueParameters& In)
{
    const auto Tag = CueTag(Name);
    FGameplayCueParameters Parameters;
    if (!Tag.IsValid() || AddedCues.Num() >= 64 || AddedCues.Contains(Name) ||
        !MakeAuthoredCueParameters(In, Parameters) || HasMatchingGameplayTag(Tag))
        return false;
    AddedCues.Add(Name);
    AddGameplayCue(Tag, Parameters);
    return true;
}
bool UDasAuthoredAbilitySystemComponent::RemoveAuthoredCue(FName Name)
{
    if (!Live() || !AddedCues.Remove(Name))
        return false;
    RemoveGameplayCue(CueTag(Name));
    return true;
}
bool UDasAuthoredAbilitySystemComponent::AuthoredCueActive(FName Name) const
{
    const auto Tag = CueTag(Name);
    return Live() && Tag.IsValid() && IsGameplayCueActive(Tag);
}
void UDasAuthoredAbilitySystemComponent::ClearAuthoredCues()
{
    auto Pending = MoveTemp(AddedCues);
    AddedCues.Empty();
    for (auto Name : Pending)
        RemoveGameplayCue(CueTag(Name));
    WatchedCues.Empty();
}
void UDasAuthoredAbilitySystemComponent::ReceiveAuthoredCue(FName Tag, FName Matched, FName Event,
                                                            const FGameplayCueParameters& P, bool AuthoredAsset)
{
    if (!IsInGameThread() || Ended || !IsValid(GetOwner()) || GetOwner()->IsActorBeingDestroyed() || !GetWorld() ||
        !GetWorld()->IsGameWorld() || GetWorld()->bIsTearingDown || (!AuthoredAsset && !WatchedCues.Contains(Matched)))
        return;
    FDasGASCueEvent R;
    R.Tag = Tag;
    R.MatchedTag = Matched;
    R.Event = Event;
    R.Target = GetAvatarActor();
    R.Parameters.RawMagnitude = P.RawMagnitude;
    R.Parameters.NormalizedMagnitude = P.NormalizedMagnitude;
    R.Parameters.Location = P.Location;
    R.Parameters.Normal = P.Normal;
    R.Parameters.Instigator = P.Instigator.Get();
    R.Parameters.EffectCauser = P.EffectCauser.Get();
    R.Parameters.SourceObject = const_cast<UObject*>(P.SourceObject.Get());
    R.Parameters.AbilityLevel = P.AbilityLevel;
    R.Parameters.EffectLevel = P.GameplayEffectLevel;
    GameplayCueReceived.Broadcast(R);
}
