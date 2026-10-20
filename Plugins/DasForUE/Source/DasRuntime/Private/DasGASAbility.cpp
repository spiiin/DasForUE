#include "DasGASAbility.h"
#include "DasGASAuthoring.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UDasAuthoredAbilityCooldown::UDasAuthoredAbilityCooldown()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1.f));
}
UDasAuthoredGameplayAbility::UDasAuthoredGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}
UGameplayEffect* UDasAuthoredGameplayAbility::GetCostGameplayEffect() const
{
    return Cost ? Cost->GetDefaultObject<UDasAuthoredGameplayEffect>() : nullptr;
}
UGameplayEffect* UDasAuthoredGameplayAbility::GetCooldownGameplayEffect() const
{
    return CooldownSeconds > 0 ? GetMutableDefault<UDasAuthoredAbilityCooldown>() : nullptr;
}
const FGameplayTagContainer* UDasAuthoredGameplayAbility::GetCooldownTags() const
{
    return &CooldownTags;
}
bool UDasAuthoredGameplayAbility::CanActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* Info,
                                                     const FGameplayTagContainer* SourceTags,
                                                     const FGameplayTagContainer* TargetTags,
                                                     FGameplayTagContainer* RelevantTags) const
{
    return !ScriptEnding && !ScriptActivating &&
           Super::CanActivateAbility(Handle, Info, SourceTags, TargetTags, RelevantTags);
}
bool UDasAuthoredGameplayAbility::CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                                            FGameplayTagContainer* RelevantTags) const
{
    if (!Super::CheckCost(Handle, Info, RelevantTags))
        return false;
    const auto* CDO = GetCostGameplayEffect();
    if (!CDO)
        return true;
    const auto* ASC = Info ? Info->AbilitySystemComponent.Get() : nullptr;
    if (!ASC)
        return false;
    for (const auto& Mod : CDO->Modifiers)
    {
        const UDasAuthoredAttributeSet* Set = nullptr;
        for (const auto* Candidate : ASC->GetSpawnedAttributes())
            if (Candidate && Candidate->IsA(Mod.Attribute.GetAttributeSetClass()))
            {
                Set = Cast<UDasAuthoredAttributeSet>(Candidate);
                break;
            }
        float Delta = 0;
        if (!Set || !Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(GetAbilityLevel(Handle, Info), Delta))
            return false;
        const auto* Rule = Set->Rules.FindByPredicate([&](const auto& R)
                                                      { return R.Name == Mod.Attribute.GetUProperty()->GetFName(); });
        const float Minimum = Rule && Rule->HasMin ? FMath::Max(0.f, Rule->Minimum) : 0.f;
        // Temporary current-value bonuses must not hide insufficient spendable base values.
        if (!FMath::IsFinite(Delta) || Delta >= 0 || ASC->GetNumericAttribute(Mod.Attribute) + Delta < Minimum ||
            ASC->GetNumericAttributeBase(Mod.Attribute) + Delta < Minimum)
            return false;
    }
    return true;
}
void UDasAuthoredGameplayAbility::ApplyCooldown(FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* Info,
                                                FGameplayAbilityActivationInfo Activation) const
{
    if (CooldownSeconds <= 0)
        return;
    auto Spec = MakeOutgoingGameplayEffectSpec(Handle, Info, Activation, UDasAuthoredAbilityCooldown::StaticClass(),
                                               GetAbilityLevel(Handle, Info));
    if (Spec.IsValid())
    {
        Spec.Data->SetDuration(CooldownSeconds, true);
        Spec.Data->DynamicGrantedTags.AppendTags(CooldownTags);
        ApplyGameplayEffectSpecToOwner(Handle, Info, Activation, Spec);
    }
}
void UDasAuthoredGameplayAbility::ActivateAbility(FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* Info,
                                                  FGameplayAbilityActivationInfo Activation, const FGameplayEventData*)
{
    auto* ASC = Info ? Cast<UDasAuthoredAbilitySystemComponent>(Info->AbilitySystemComponent.Get()) : nullptr;
    if (!ASC || ActivationSerial == MAX_int32)
    {
        EndAbility(Handle, Info, Activation, false, true);
        return;
    }
    TGuardValue<bool> Activating(ScriptActivating, true);
    ++ActivationSerial;
    CommittedForActivation = false;
    ScriptError.Empty();
    ScriptOwner = Info->OwnerActor;
    ScriptAvatar = Info->AvatarActor;
    ScriptSystem = ASC;
    ScriptGrantId = ASC->AuthoredAbilityId(Handle);
    ScriptLevel = GetAbilityLevel(Handle, Info);
    if (!ActivateEntry.IsEmpty())
    {
        ASC->NotifyAuthoredAbility(Handle, TEXT("Activated"), true);
        if (!RunScriptHook(ActivateEntry, TEXT("Activate")))
        {
            ASC->NotifyAuthoredAbility(Handle, TEXT("Rejected"));
            if (IsActive())
                EndAbility(Handle, Info, Activation, false, true);
        }
        return;
    }
    if (!Effect || !CommitScriptAbility(ActivationSerial))
    {
        ASC->NotifyAuthoredAbility(Handle, TEXT("Rejected"));
        EndAbility(Handle, Info, Activation, false, true);
        return;
    }
    const bool Applied = ApplyScriptAbilityEffect(ActivationSerial);
    ASC->NotifyAuthoredAbility(Handle, Applied ? FName(TEXT("Activated")) : FName(TEXT("Rejected")), Applied);
    if (!Applied || AutoEnd)
        EndAbility(Handle, Info, Activation, false, !Applied);
}
void UDasAuthoredGameplayAbility::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                                             FGameplayAbilityActivationInfo Activation, bool Replicate, bool Cancelled)
{
    const bool WasActive = IsActive();
    auto* ASC = Info ? Cast<UDasAuthoredAbilitySystemComponent>(Info->AbilitySystemComponent.Get()) : nullptr;
    if (WasActive) { TGuardValue<bool> Ending(ScriptEnding,true); ClearScriptTasks(); }
    Super::EndAbility(Handle, Info, Activation, Replicate, Cancelled);
    if (WasActive && !IsActive() && ASC)
    {
        TGuardValue<bool> Ending(ScriptEnding, true);
        if (!ActivateEntry.IsEmpty())
        {
            if (Cancelled)
                RunScriptHook(CancelEntry, TEXT("Cancel"), true);
            RunScriptHook(EndEntry, TEXT("End"), Cancelled);
        }
        ASC->NotifyAuthoredAbility(Handle, Cancelled ? FName(TEXT("Cancelled")) : FName(TEXT("Ended")));
    }
}
FDasAuthoredAbilityGrant
UDasAuthoredAbilitySystemComponent::GiveAuthoredAbility(TSubclassOf<UDasAuthoredGameplayAbility> Class, int32 Level)
{
    FDasAuthoredAbilityGrant Result;
    if (!InitializeAuthoredActorInfo() || !Class || Level < 1 || Level > 10000 ||
        Class->HasAnyClassFlags(CLASS_Abstract | CLASS_NewerVersionExists))
        return Result;
    const auto* CDO = Class->GetDefaultObject<UDasAuthoredGameplayAbility>();
    if (CDO->DefinitionId.IsEmpty() || CDO->DefinitionName.IsNone() || (!CDO->Effect && CDO->ActivateEntry.IsEmpty()))
        return Result;
    for (UClass* EffectClass : {CDO->Effect.Get(), CDO->Cost.Get()})
        if (EffectClass)
        {
            const auto* Def = EffectClass->GetDefaultObject<UDasAuthoredGameplayEffect>();
            if (!Def->RequiredMagnitudes.IsEmpty())
                return Result;
            for (const auto& Mod : Def->Modifiers)
                if (!Mod.Attribute.IsValid() || !GetAttributeSubobject(Mod.Attribute.GetAttributeSetClass()))
                    return Result;
        }
    for (auto It = Abilities.CreateIterator(); It; ++It)
    {
        const auto* Spec = FindAbilitySpecFromHandle(It.Value().Handle);
        if (!Spec)
        {
            It.RemoveCurrent();
            continue;
        }
        const auto* Existing = Cast<UDasAuthoredGameplayAbility>(Spec->Ability);
        if (Existing && Existing->DefinitionName == CDO->DefinitionName)
        {
            if (Existing->GetClass() == Class && Spec->Level == Level)
            {
                Result.Target = this;
                Result.Id = It.Key();
            }
            return Result;
        }
    }
    if (Abilities.Num() >= 128 || NextAbilityId == MAX_int32)
        return Result;
    const auto Handle = GiveAbility(FGameplayAbilitySpec(Class, Level));
    if (!Handle.IsValid() || !FindAbilitySpecFromHandle(Handle))
        return Result;
    Result.Target = this;
    Result.Id = NextAbilityId++;
    Abilities.Add(Result.Id, {Handle, CDO->DefinitionName, false});
    NotifyAuthoredAbility(Handle, TEXT("Granted"));
    return Result;
}
FDasAuthoredAbilityState UDasAuthoredAbilitySystemComponent::AuthoredAbilityState(int32 Id) const
{
    FDasAuthoredAbilityState Result;
    if (!Live())
        return Result;
    const auto* Record = Abilities.Find(Id);
    const auto* Spec = Record ? FindAbilitySpecFromHandle(Record->Handle) : nullptr;
    const auto* Ability = Spec ? Cast<UDasAuthoredGameplayAbility>(Spec->Ability) : nullptr;
    if (!Ability || !AbilityActorInfo.IsValid())
        return Result;
    Result.Phase = TEXT("State");
    Result.GrantId = Id;
    const auto* Instance = Cast<UDasAuthoredGameplayAbility>(Spec->GetPrimaryInstance());
    Result.Committed = Instance && Instance->IsAuthoredCommitted();
    Result.Error = Instance ? Instance->GetScriptError() : FString();
    Result.Granted = true;
    Result.Active = Spec->IsActive();
    Result.Ability = Record->Name;
    Result.Level = Spec->Level;
    Result.CooldownRemaining = FMath::Max(0.f, Ability->GetCooldownTimeRemaining(AbilityActorInfo.Get()));
    const auto* Source = Instance ? Instance : Ability;
    Result.CanActivate = !Result.Active && Source->CanActivateAbility(Record->Handle, AbilityActorInfo.Get());
    return Result;
}
bool UDasAuthoredAbilitySystemComponent::ActivateAuthoredAbility(int32 Id)
{
    if (!Live())
        return false;
    auto* Record = Abilities.Find(Id);
    if (!Record || !FindAbilitySpecFromHandle(Record->Handle))
        return false;
    const auto Handle = Record->Handle;
    const bool PreviousSuccess = Record->ActivationSucceeded;
    Record->ActivationSucceeded = false;
    const bool Accepted = TryActivateAbility(Handle, false);
    if (!Accepted)
        NotifyAuthoredAbility(Handle, TEXT("Blocked"));
    auto* After = Abilities.Find(Id);
    if (!Accepted && After)
        After->ActivationSucceeded = PreviousSuccess;
    return Accepted && After && After->ActivationSucceeded;
}
bool UDasAuthoredAbilitySystemComponent::CancelAuthoredAbility(int32 Id)
{
    if (!Live())
        return false;
    const auto* Record = Abilities.Find(Id);
    const auto* Spec = Record ? FindAbilitySpecFromHandle(Record->Handle) : nullptr;
    if (!Spec || !Spec->IsActive())
        return false;
    const auto Handle = Record->Handle;
    CancelAbilityHandle(Handle);
    const auto* After = FindAbilitySpecFromHandle(Handle);
    return !After || !After->IsActive();
}
bool UDasAuthoredAbilitySystemComponent::FinishAuthoredAbility(int32 Id)
{
    if (!Live())
        return false;
    const auto* Record = Abilities.Find(Id);
    const auto* Spec = Record ? FindAbilitySpecFromHandle(Record->Handle) : nullptr;
    auto* Instance = Spec ? Cast<UDasAuthoredGameplayAbility>(Spec->GetPrimaryInstance()) : nullptr;
    if (!Instance || !Instance->IsActive())
        return false;
    Instance->Finish();
    return !Instance->IsActive();
}
bool UDasAuthoredAbilitySystemComponent::RemoveAuthoredAbility(int32 Id)
{
    if (!Live())
        return false;
    const auto* Record = Abilities.Find(Id);
    if (!Record)
        return false;
    const auto Handle = Record->Handle;
    if (!FindAbilitySpecFromHandle(Handle))
    {
        Abilities.Remove(Id);
        return false;
    }
    CancelAuthoredAbility(Id);
    ClearAbility(Handle);
    NotifyAuthoredAbility(Handle, TEXT("Removed"));
    Abilities.Remove(Id);
    return true;
}
void UDasAuthoredAbilitySystemComponent::NotifyAuthoredAbility(FGameplayAbilitySpecHandle Handle, FName Phase,
                                                               bool Succeeded)
{
    if (!Live())
        return;
    for (auto& Pair : Abilities)
        if (Pair.Value.Handle == Handle)
        {
            if (Succeeded)
                Pair.Value.ActivationSucceeded = true;
            if (Phase == TEXT("Rejected"))
                Pair.Value.ActivationSucceeded = false;
            auto Change = AuthoredAbilityState(Pair.Key);
            Change.Ability = Pair.Value.Name;
            Change.GrantId = Pair.Key;
            Change.Phase = Phase;
            Change.Cancelled = Phase == TEXT("Cancelled");
            const auto* Spec = FindAbilitySpecFromHandle(Handle);
            Change.Active = Spec && Spec->IsActive();
            AbilityChanged.Broadcast(Change);
            return;
        }
}

int32 UDasAuthoredAbilitySystemComponent::AuthoredAbilityId(FGameplayAbilitySpecHandle Handle) const
{
    if (!Live() || !FindAbilitySpecFromHandle(Handle))
        return 0;
    for (const auto& Pair : Abilities)
        if (Pair.Value.Handle == Handle)
            return Pair.Key;
    return 0;
}
FDasAuthoredAbilityContext UDasAuthoredGameplayAbility::ScriptAbilityContext() const
{
    FDasAuthoredAbilityContext Result;
    if (IsTemplate() || ActivationSerial <= 0)
        return Result;
    Result.Ability = const_cast<UDasAuthoredGameplayAbility*>(this);
    Result.Owner = ScriptOwner.Get();
    Result.Avatar = ScriptAvatar.Get();
    Result.System = ScriptSystem.Get();
    Result.GrantId = ScriptGrantId;
    Result.ActivationId = ActivationSerial;
    Result.Level = ScriptLevel;
    Result.Phase = ScriptPhase;
    Result.Task = CurrentTask;
    Result.Cancelled = ScriptCancelled;
    return Result;
}
bool UDasAuthoredGameplayAbility::ValidScriptActivation(int32 Id) const
{
    auto* ASC = ScriptSystem.Get();
    auto* Owner = ScriptOwner.Get();
    return IsInGameThread() && !IsTemplate() && IsActive() && !ScriptEnding && Id > 0 && Id == ActivationSerial &&
           IsValid(Owner) && !Owner->IsActorBeingDestroyed() && ASC &&
           ASC->AuthoredAbilityId(GetCurrentAbilitySpecHandle()) == ScriptGrantId && ScriptGrantId > 0;
}
bool UDasAuthoredGameplayAbility::CommitScriptAbility(int32 Id)
{
    if (!ValidScriptActivation(Id))
        return false;
    if (CommittedForActivation)
        return true;
    CommittedForActivation =
        CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
    return CommittedForActivation;
}
bool UDasAuthoredGameplayAbility::ApplyScriptAbilityEffect(int32 Id)
{
    if (!ValidScriptActivation(Id) || !CommittedForActivation || !Effect)
        return false;
    const auto Spec = MakeOutgoingGameplayEffectSpec(Effect, GetAbilityLevel());
    return Spec.IsValid() && ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
                                                            GetCurrentActivationInfo(), Spec)
                                 .WasSuccessfullyApplied();
}
bool UDasAuthoredGameplayAbility::EndScriptAbility(int32 Id)
{
    if (!ValidScriptActivation(Id))
        return false;
    Finish();
    return !IsActive();
}
bool UDasAuthoredGameplayAbility::CancelScriptAbility(int32 Id)
{
    if (!ValidScriptActivation(Id))
        return false;
    CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false);
    return !IsActive();
}
bool UDasAuthoredGameplayAbility::RunScriptHook(const FString& Entry, FName Phase, bool Cancelled)
{
    if (Entry.IsEmpty())
        return true;
    auto* Owner = ScriptOwner.Get();
    if (!IsValid(Owner) || Owner->IsActorBeingDestroyed() || !Owner->GetWorld() || Owner->GetWorld()->bIsTearingDown)
        return true; // teardown revokes execution; no new VM is started on a dead owner.
    auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
    if (!Runtime)
    {
        ScriptError = TEXT("DAS runtime is unavailable");
        return false;
    }
    TGuardValue<bool> ExecutingHook(ScriptActivating,true);
    TGuardValue<FName> PhaseScope(ScriptPhase, Phase);
    TGuardValue<bool> CancelScope(ScriptCancelled, Cancelled);
    const auto Result = Runtime->DispatchGASAbilityHook(this, Entry);
    if (!Result.bSucceeded)
    {
        ScriptError = Result.Error;
        UE_LOG(LogTemp, Warning,
               TEXT("DAS_GAS_ABILITY_HOOK_FAILED %s %s: %s"), *DefinitionName.ToString(), *Phase.ToString(),
                    *ScriptError);
    }
    return Result.bSucceeded;
}
void UDasAuthoredGameplayAbility::ReleaseScript()
{
    if (IsInGameThread())
        if (auto* Runtime = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
            Runtime->ReleaseObject(this);
}
void UDasAuthoredGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* Info,
                                                  const FGameplayAbilitySpec& Spec)
{
    Super::OnRemoveAbility(Info, Spec);
    ReleaseScript();
}
void UDasAuthoredGameplayAbility::BeginDestroy()
{
    ClearScriptTasks();
    ReleaseScript();
    Super::BeginDestroy();
}
