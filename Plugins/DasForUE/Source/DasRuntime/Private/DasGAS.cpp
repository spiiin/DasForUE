#include "DasGAS.h"
#include "GameplayEffectExtension.h"
#include "NativeGameplayTags.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(DasHealingMagnitude, "Data.Healing");
UE_DEFINE_GAMEPLAY_TAG_STATIC(DasDamageMagnitude, "Data.Damage");

namespace
{
bool IsLiveHealthASC(const UDasHealthAbilitySystemComponent* ASC)
{
    const auto* Owner = IsValid(ASC) ? ASC->GetOwner() : nullptr;
    const auto* World = IsValid(Owner) ? Owner->GetWorld() : nullptr;
    return IsInGameThread() && World && World->IsGameWorld() && !World->bIsTearingDown &&
           !Owner->IsActorBeingDestroyed() && !ASC->IsBeingDestroyed() && !ASC->IsTemplate() && Owner->HasAuthority();
}
bool IsEffect(EDasHealthEffect Effect)
{
    return Effect == EDasHealthEffect::Heal || Effect == EDasHealthEffect::Damage;
}
void ConfigureInstantHealthEffect(UGameplayEffect& Effect, FGameplayTag Tag, EGameplayModOp::Type Op)
{
    Effect.DurationPolicy = EGameplayEffectDurationType::Instant;
    FGameplayModifierInfo Modifier;
    Modifier.Attribute = UDasHealthAttributeSet::HealthAttribute();
    Modifier.ModifierOp = Op;
    FSetByCallerFloat Magnitude;
    Magnitude.DataTag = Tag;
    Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
    Effect.Modifiers.Add(Modifier);
}
}

FGameplayAttribute UDasHealthAttributeSet::HealthAttribute()
{
    return FGameplayAttribute(
        FindFieldChecked<FProperty>(StaticClass(), GET_MEMBER_NAME_CHECKED(UDasHealthAttributeSet, Health)));
}
FGameplayAttribute UDasHealthAttributeSet::MaxHealthAttribute()
{
    return FGameplayAttribute(
        FindFieldChecked<FProperty>(StaticClass(), GET_MEMBER_NAME_CHECKED(UDasHealthAttributeSet, MaxHealth)));
}
void UDasHealthAttributeSet::ClampValue(const FGameplayAttribute& Attribute, float& Value) const
{
    if (Attribute == MaxHealthAttribute())
    {
        if (!FMath::IsFinite(Value) || Value <= 0)
            Value = MaxHealth.GetCurrentValue() > 0 ? MaxHealth.GetCurrentValue() : 1.0f;
    }
    else if (Attribute == HealthAttribute())
        Value =
            FMath::IsFinite(Value) ? FMath::Clamp(Value, 0.0f, MaxHealth.GetCurrentValue()) : Health.GetCurrentValue();
}
void UDasHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampValue(Attribute, NewValue);
}
void UDasHealthAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    ClampValue(Attribute, NewValue);
}
void UDasHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);
    if (Attribute == MaxHealthAttribute() && Health.GetCurrentValue() > NewValue)
        if (auto* ASC = GetOwningAbilitySystemComponent())
            ASC->SetNumericAttributeBase(HealthAttribute(), NewValue);
}
void UDasHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    auto* ASC = GetOwningAbilitySystemComponent();
    float Value = ASC->GetNumericAttributeBase(Data.EvaluatedData.Attribute);
    ClampValue(Data.EvaluatedData.Attribute, Value);
    if (Value != ASC->GetNumericAttributeBase(Data.EvaluatedData.Attribute))
        ASC->SetNumericAttributeBase(Data.EvaluatedData.Attribute, Value);
}
UDasHealGameplayEffect::UDasHealGameplayEffect()
{
    ConfigureInstantHealthEffect(*this, DasHealingMagnitude, EGameplayModOp::Additive);
}
UDasDamageGameplayEffect::UDasDamageGameplayEffect()
{
    ConfigureInstantHealthEffect(*this, DasDamageMagnitude, EGameplayModOp::Additive);
}
UDasHealthAbilitySystemComponent::UDasHealthAbilitySystemComponent()
{
    SetIsReplicatedByDefault(false);
    PrimaryComponentTick.bCanEverTick = false;
}
bool UDasHealthAbilitySystemComponent::HealthReady() const
{
    return HealthActive && IsLiveHealthASC(this) && IsValid(HealthSet) && GetOwnerActor() == GetOwner() &&
           GetAvatarActor() == GetOwner();
}
bool UDasHealthAbilitySystemComponent::InitializeHealth(float Maximum)
{
    if (!IsLiveHealthASC(this) || !FMath::IsFinite(Maximum) || Maximum <= 0)
        return false;
    if (HealthSet)
        return HealthReady();
    // This adapter owns one ASC on its Character. Do not coexist with another
    // ASC: GAS lookup for the AttributeSet must resolve this component.
    TArray<UAbilitySystemComponent*> Systems;
    GetOwner()->GetComponents<UAbilitySystemComponent>(Systems);
    if (Systems.Num() != 1 || Systems[0] != this)
        return false;
    HealthSet = NewObject<UDasHealthAttributeSet>(GetOwner());
    InitAbilityActorInfo(GetOwner(), GetOwner());
    AddAttributeSetSubobject(HealthSet.Get());
    SetNumericAttributeBase(UDasHealthAttributeSet::MaxHealthAttribute(), Maximum);
    SetNumericAttributeBase(UDasHealthAttributeSet::HealthAttribute(), Maximum);
    HealthActive = true;
    HealthChangedHandle =
        GetGameplayAttributeValueChangeDelegate(UDasHealthAttributeSet::HealthAttribute())
            .AddUObject(this, &UDasHealthAbilitySystemComponent::ForwardChange, EDasHealthAttribute::Health);
    MaximumChangedHandle =
        GetGameplayAttributeValueChangeDelegate(UDasHealthAttributeSet::MaxHealthAttribute())
            .AddUObject(this, &UDasHealthAbilitySystemComponent::ForwardChange, EDasHealthAttribute::MaxHealth);
    GetOwner()->OnTakeAnyDamage.AddUniqueDynamic(this, &UDasHealthAbilitySystemComponent::OnOwnerDamage);
    return true;
}
float UDasHealthAbilitySystemComponent::ReadHealthAttribute(EDasHealthAttribute Attribute) const
{
    if (!HealthReady())
        return 0;
    return GetNumericAttribute(Attribute == EDasHealthAttribute::Health ? UDasHealthAttributeSet::HealthAttribute()
                                                                        : UDasHealthAttributeSet::MaxHealthAttribute());
}
FDasGameplayAttributeChange UDasHealthAbilitySystemComponent::HealthSnapshot(EDasHealthAttribute Attribute) const
{
    FDasGameplayAttributeChange Snapshot;
    Snapshot.Attribute = Attribute;
    Snapshot.OldValue = Snapshot.NewValue = ReadHealthAttribute(Attribute);
    return Snapshot;
}
FDasHealthEffectSpec UDasHealthAbilitySystemComponent::MakeHealthSpec(EDasHealthEffect Effect, float Level)
{
    FDasHealthEffectSpec Spec;
    if (HealthReady() && IsEffect(Effect) && FMath::IsFinite(Level) && Level > 0)
    {
        Spec.Source = this;
        Spec.Effect = Effect;
        Spec.Level = Level;
    }
    return Spec;
}
FDasHealthEffectResult UDasHealthAbilitySystemComponent::ApplyHealthSpec(const FDasHealthEffectSpec& Spec)
{
    FDasHealthEffectResult Result;
    auto* Source = Spec.Source.Get();
    if (!HealthReady() || !Source || !Source->HealthReady() || Source->GetWorld() != GetWorld() ||
        !IsEffect(Spec.Effect) || !FMath::IsFinite(Spec.Level) || Spec.Level <= 0 || !Spec.HasMagnitude ||
        !FMath::IsFinite(Spec.Magnitude) || Spec.Magnitude < 0)
        return Result;
    const auto RequiredTag =
        Spec.Effect == EDasHealthEffect::Heal ? DasHealingMagnitude.GetTag() : DasDamageMagnitude.GetTag();
    if (Spec.MagnitudeTag != RequiredTag.GetTagName())
        return Result;
    const auto EffectClass = Spec.Effect == EDasHealthEffect::Heal ? UDasHealGameplayEffect::StaticClass()
                                                                   : UDasDamageGameplayEffect::StaticClass();
    auto Context = Source->MakeEffectContext();
    Context.AddSourceObject(Source->GetOwner());
    auto NativeSpec = Source->MakeOutgoingSpec(EffectClass, Spec.Level, Context);
    if (!NativeSpec.IsValid())
        return Result;
    // Damage uses an additive negative magnitude; callers always supply positive amounts.
    NativeSpec.Data->SetSetByCallerMagnitude(RequiredTag,
                                             Spec.Effect == EDasHealthEffect::Heal ? Spec.Magnitude : -Spec.Magnitude);
    Result.Valid = true;
    const auto Handle = ApplyGameplayEffectSpecToSelf(*NativeSpec.Data.Get());
    Result.Applied = Handle.WasSuccessfullyApplied();
    Result.HasActiveHandle = Handle.IsValid();
    return Result;
}
void UDasHealthAbilitySystemComponent::ForwardChange(const FOnAttributeChangeData& Data, EDasHealthAttribute Attribute)
{
    if (!HealthReady() || Data.OldValue == Data.NewValue)
        return;
    FDasGameplayAttributeChange Change;
    Change.Attribute = Attribute;
    Change.OldValue = Data.OldValue;
    Change.NewValue = Data.NewValue;
    AttributeChanged.Broadcast(Change);
}
void UDasHealthAbilitySystemComponent::OnOwnerDamage(AActor* Damaged, float Amount, const UDamageType*, AController*,
                                                     AActor*)
{
    if (Damaged != GetOwner() || !FMath::IsFinite(Amount) || Amount <= 0 || !HealthReady())
        return;
    auto Spec = MakeHealthSpec(EDasHealthEffect::Damage, 1);
    Spec.MagnitudeTag = DasDamageMagnitude.GetTag().GetTagName();
    Spec.Magnitude = Amount;
    Spec.HasMagnitude = true;
    ApplyHealthSpec(Spec);
}
void UDasHealthAbilitySystemComponent::DetachHealthDelegates()
{
    if (HealthChangedHandle.IsValid())
        GetGameplayAttributeValueChangeDelegate(UDasHealthAttributeSet::HealthAttribute()).Remove(HealthChangedHandle);
    if (MaximumChangedHandle.IsValid())
        GetGameplayAttributeValueChangeDelegate(UDasHealthAttributeSet::MaxHealthAttribute())
            .Remove(MaximumChangedHandle);
    HealthChangedHandle.Reset();
    MaximumChangedHandle.Reset();
    if (IsValid(GetOwner()))
        GetOwner()->OnTakeAnyDamage.RemoveDynamic(this, &UDasHealthAbilitySystemComponent::OnOwnerDamage);
}
void UDasHealthAbilitySystemComponent::EndPlay(const EEndPlayReason::Type Reason)
{
    HealthActive = false;
    DetachHealthDelegates();
    Super::EndPlay(Reason);
}
void UDasHealthAbilitySystemComponent::BeginDestroy()
{
    DetachHealthDelegates();
    Super::BeginDestroy();
}
