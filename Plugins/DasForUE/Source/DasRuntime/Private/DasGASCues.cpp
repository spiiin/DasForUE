#include "DasGASCues.h"
#include "DasGASAuthoring.h"
#include "DasRuntime.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayCueSet.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(DasSampleImpactCue, "GameplayCue.Das.Sample.Impact");
UE_DEFINE_GAMEPLAY_TAG_STATIC(DasSampleAuraCue, "GameplayCue.Das.Sample.Aura");
UE_DEFINE_GAMEPLAY_TAG_STATIC(DasSampleHooksCue, "GameplayCue.Das.Sample.Hooks");
UE_DEFINE_GAMEPLAY_TAG_STATIC(DasPlaygroundImpactCue, "GameplayCue.Das.Sample.Playground.Impact");
UE_DEFINE_GAMEPLAY_TAG_STATIC(DasPlaygroundAuraCue, "GameplayCue.Das.Sample.Playground.Aura");
FDasGASCueHit::FDasGASCueHit(const FHitResult& H)
{
    BlockingHit = H.bBlockingHit;
    StartPenetrating = H.bStartPenetrating;
    Time = H.Time;
    Distance = H.Distance;
    PenetrationDepth = H.PenetrationDepth;
    FaceIndex = H.FaceIndex;
    Item = H.Item;
    MyItem = H.MyItem;
    ElementIndex = H.ElementIndex;
    BoneName = H.BoneName;
    MyBoneName = H.MyBoneName;
    Location = H.Location;
    ImpactPoint = H.ImpactPoint;
    Normal = H.Normal;
    ImpactNormal = H.ImpactNormal;
    TraceStart = H.TraceStart;
    TraceEnd = H.TraceEnd;
    Actor = H.GetActor();
    Component = H.Component;
    PhysMaterial = H.PhysMaterial;
}
FHitResult FDasGASCueHit::ToNative() const
{
    FHitResult H(Actor.Get(), Component.Get(), ImpactPoint, ImpactNormal);
    H.bBlockingHit = BlockingHit;
    H.bStartPenetrating = StartPenetrating;
    H.Time = Time;
    H.Distance = Distance;
    H.PenetrationDepth = PenetrationDepth;
    H.FaceIndex = FaceIndex;
    H.Item = Item;
    H.MyItem = MyItem;
    H.ElementIndex = uint8(ElementIndex);
    H.BoneName = BoneName;
    H.MyBoneName = MyBoneName;
    H.Location = Location;
    H.ImpactPoint = ImpactPoint;
    H.Normal = Normal;
    H.ImpactNormal = ImpactNormal;
    H.TraceStart = TraceStart;
    H.TraceEnd = TraceEnd;
    H.PhysMaterial = PhysMaterial;
    return H;
}
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
bool UDasGASCueSession::Live() const
{
    const auto* Actor = Target.Get();
    return !Closed && IsValid(Definition) && IsValid(Actor) && !Actor->IsActorBeingDestroyed() && Actor->GetWorld() &&
           Actor->GetWorld()->IsGameWorld() && !Actor->GetWorld()->bIsTearingDown;
}
void UDasGASCueSession::Close()
{
    if (Closed)
        return;
    Closed = true;
    Pending.Reset();
    if (IsInGameThread())
        if (auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
            R->ReleaseObject(this);
}
void UDasGASCueSession::EndPlay(const EEndPlayReason::Type Reason)
{
    Close();
    Super::EndPlay(Reason);
}
void UDasGASCueSession::BeginDestroy()
{
    Close();
    Super::BeginDestroy();
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
                            DefinitionId.IsEmpty() ? nullptr : this);
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
    if (In.SourceTags.Num() > 128 || In.TargetTags.Num() > 128 || In.Context.Actors.Num() > 64 ||
        (In.Context.HasOrigin && In.Context.Origin.ContainsNaN()))
        return false;
    for (const auto* Tags : {&In.SourceTags, &In.TargetTags})
        for (const auto Tag : *Tags)
            if (!Tag.IsValid() || !FGameplayTag::RequestGameplayTag(Tag.GetTagName(), false).IsValid())
                return false;
    auto ValidActor = [&](AActor* A)
    {
        return !A || (IsValid(A) && !A->IsActorBeingDestroyed() && A->GetWorld() == GetWorld());
    };
    for (auto* A : {In.Context.Instigator.Get(), In.Context.OriginalInstigator.Get(), In.Context.EffectCauser.Get()})
        if (!ValidActor(A))
            return false;
    for (auto A : In.Context.Actors)
        if (!A || !ValidActor(A))
            return false;
    auto* ContextSource = In.Context.SourceObject.Get();
    if (ContextSource &&
        (!IsValid(ContextSource) || (ContextSource->GetWorld() && ContextSource->GetWorld() != GetWorld())))
        return false;
    if (In.Context.HasHit)
    {
        if (In.Context.Hit.ElementIndex < 0 || In.Context.Hit.ElementIndex > 255 ||
            !FMath::IsFinite(In.Context.Hit.PenetrationDepth))
            return false;
        const auto H = In.Context.Hit.ToNative();
        if (H.Location.ContainsNaN() || H.ImpactPoint.ContainsNaN() || H.Normal.ContainsNaN() ||
            H.ImpactNormal.ContainsNaN() || H.TraceStart.ContainsNaN() || H.TraceEnd.ContainsNaN() ||
            !FMath::IsFinite(H.Time) || H.Time < 0 || H.Time > 1 || !FMath::IsFinite(H.Distance) ||
            !ValidActor(H.GetActor()) ||
            (H.GetComponent() && (!IsValid(H.GetComponent()) || H.GetComponent()->GetWorld() != GetWorld())))
            return false;
    }
    Out.RawMagnitude = In.RawMagnitude;
    Out.NormalizedMagnitude = In.NormalizedMagnitude;
    Out.Location = In.Location;
    Out.Normal = In.Normal;
    Out.Instigator = In.Instigator ? In.Instigator.Get() : GetAvatarActor();
    Out.EffectCauser = In.EffectCauser ? In.EffectCauser.Get() : GetAvatarActor();
    Out.SourceObject = Source;
    Out.AbilityLevel = In.AbilityLevel;
    Out.GameplayEffectLevel = In.EffectLevel;
    Out.AggregatedSourceTags = In.SourceTags;
    Out.AggregatedTargetTags = In.TargetTags;
    Out.EffectContext = MakeEffectContext();
    if (In.Context.Valid)
    {
        Out.EffectContext.AddInstigator(In.Context.Instigator, In.Context.EffectCauser);
        Out.EffectContext.AddSourceObject(ContextSource);
    }
    else
    {
        Out.EffectContext.AddInstigator(Out.Instigator.Get(), Out.EffectCauser.Get());
        Out.EffectContext.AddSourceObject(Source);
    }
    TArray<TWeakObjectPtr<AActor>> Actors;
    for (auto A : In.Context.Actors)
        Actors.Add(A.Get());
    if (!Actors.IsEmpty())
        Out.EffectContext.AddActors(Actors);
    if (In.Context.HasOrigin)
        Out.EffectContext.AddOrigin(In.Context.Origin);
    if (In.Context.HasHit)
        Out.EffectContext.AddHitResult(In.Context.Hit.ToNative(), true);
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
    auto Sessions = MoveTemp(CueSessions);
    CueSessions.Empty();
    for (auto& Pair : Sessions)
        if (IsValid(Pair.Value))
        {
            Pair.Value->Close();
            Pair.Value->DestroyComponent();
        }
}
void UDasAuthoredAbilitySystemComponent::ReceiveAuthoredCue(FName Tag, FName Matched, FName Event,
                                                            const FGameplayCueParameters& P,
                                                            UDasGameplayCueNotify* Asset)
{
    if (!IsInGameThread() || Ended || !IsValid(GetOwner()) || GetOwner()->IsActorBeingDestroyed() || !GetWorld() ||
        !GetWorld()->IsGameWorld() || GetWorld()->bIsTearingDown || (!Asset && !WatchedCues.Contains(Matched)))
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
    R.Parameters.SourceTags = P.AggregatedSourceTags;
    R.Parameters.TargetTags = P.AggregatedTargetTags;
    if (P.EffectContext.IsValid())
    {
        const auto& C = P.EffectContext;
        auto& D = R.Parameters.Context;
        D.Valid = true;
        D.Instigator = C.GetInstigator();
        D.OriginalInstigator = C.GetOriginalInstigator();
        D.EffectCauser = C.GetEffectCauser();
        D.SourceObject = C.GetSourceObject();
        for (auto A : C.Get()->GetActors())
            if (A.IsValid())
                D.Actors.Add(A.Get());
        D.HasOrigin = C.HasOrigin();
        if (D.HasOrigin)
            D.Origin = C.GetOrigin();
        if (const auto* Hit = C.GetHitResult())
        {
            D.HasHit = true;
            D.Hit = *Hit;
        }
    }
    // Publish event snapshots independently of whether this asset declares its own DAS methods.
    if (Asset && !Asset->Hooks.IsEmpty())
    {
        auto& Session = CueSessions.FindOrAdd(Asset->GetClass());
        if (!IsValid(Session))
        {
            if (CueSessions.Num() > 64)
            {
                CueSessions.Remove(Asset->GetClass());
                UE_LOG(LogTemp, Warning, TEXT("DAS_GAS_CUE_SESSION_LIMIT"));
                GameplayCueReceived.Broadcast(R);
                return;
            }
            Session = NewObject<UDasGASCueSession>(GetOwner(), NAME_None, RF_Transient);
            Session->Target = GetAvatarActor();
            Session->Definition = Asset;
            GetOwner()->AddInstanceComponent(Session);
            Session->RegisterComponent();
        }
        if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
            Runtime->QueueGASCueHook(Session, R);
    }
    if (!Ended && IsValid(GetOwner()) && !GetOwner()->IsActorBeingDestroyed())
        GameplayCueReceived.Broadcast(R);
}
