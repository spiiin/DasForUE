#include "DasGASAbility.h"
#include "DasGASAuthoring.h"
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
    if (!ASC || !Effect || !CommitAbility(Handle, Info, Activation))
    {
        if (ASC)
            ASC->NotifyAuthoredAbility(Handle, TEXT("Rejected"));
        EndAbility(Handle, Info, Activation, false, true);
        return;
    }
    const auto Spec = MakeOutgoingGameplayEffectSpec(Handle, Info, Activation, Effect, GetAbilityLevel(Handle, Info));
    const bool Applied =
        Spec.IsValid() && ApplyGameplayEffectSpecToOwner(Handle, Info, Activation, Spec).WasSuccessfullyApplied();
    ASC->NotifyAuthoredAbility(Handle, Applied ? FName(TEXT("Activated")) : FName(TEXT("Rejected")), Applied);
    if (!Applied || AutoEnd)
        EndAbility(Handle, Info, Activation, false, !Applied);
}
void UDasAuthoredGameplayAbility::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                                             FGameplayAbilityActivationInfo Activation, bool Replicate, bool Cancelled)
{
    const bool WasActive = IsActive();
    auto* ASC = Info ? Cast<UDasAuthoredAbilitySystemComponent>(Info->AbilitySystemComponent.Get()) : nullptr;
    Super::EndAbility(Handle, Info, Activation, Replicate, Cancelled);
    if (WasActive && !IsActive() && ASC)
        ASC->NotifyAuthoredAbility(Handle, Cancelled ? FName(TEXT("Cancelled")) : FName(TEXT("Ended")));
}
FDasAuthoredAbilityGrant
UDasAuthoredAbilitySystemComponent::GiveAuthoredAbility(TSubclassOf<UDasAuthoredGameplayAbility> Class, int32 Level)
{
    FDasAuthoredAbilityGrant Result;
    if (!Live() || Sets.IsEmpty() || !Class || Level < 1 || Level > 10000 ||
        Class->HasAnyClassFlags(CLASS_Abstract | CLASS_NewerVersionExists))
        return Result;
    const auto* CDO = Class->GetDefaultObject<UDasAuthoredGameplayAbility>();
    if (CDO->DefinitionId.IsEmpty() || CDO->DefinitionName.IsNone() || !CDO->Effect)
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
    Result.Granted = true;
    Result.Active = Spec->IsActive();
    Result.Ability = Record->Name;
    Result.Level = Spec->Level;
    Result.CooldownRemaining = FMath::Max(0.f, Ability->GetCooldownTimeRemaining(AbilityActorInfo.Get()));
    Result.CanActivate = !Result.Active && Ability->CanActivateAbility(Record->Handle, AbilityActorInfo.Get());
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
    Record->Committed = false;
    const bool Accepted = TryActivateAbility(Handle, false);
    if (!Accepted)
        NotifyAuthoredAbility(Handle, TEXT("Blocked"));
    const auto* After = Abilities.Find(Id);
    return Accepted && After && After->Committed;
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
                                                               bool Committed)
{
    if (!Live())
        return;
    for (auto& Pair : Abilities)
        if (Pair.Value.Handle == Handle)
        {
            if (Committed)
                Pair.Value.Committed = true;
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
