#include "DasGASAuthoring.h"
#include "DasGASCalculations.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagsManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

FGameplayAttribute UDasAuthoredAttributeSet::Attribute(FName Name) const
{
    for (const auto& Rule : Rules)
        if (Rule.Name == Name)
        {
            auto* P = FindFProperty<FStructProperty>(GetClass(), Name);
            if (P && P->Struct == FGameplayAttributeData::StaticStruct())
                return FGameplayAttribute(P);
        }
    return {};
}
void UDasAuthoredAttributeSet::InitializeDeclaredValues()
{
    for (const auto& Rule : Rules)
        if (auto* P = Attribute(Rule.Name).GetUProperty())
        {
            auto* Data = CastFieldChecked<FStructProperty>(P)->ContainerPtrToValuePtr<FGameplayAttributeData>(this);
            Data->SetBaseValue(Rule.Initial);
            Data->SetCurrentValue(Rule.Initial);
        }
}
void UDasAuthoredAttributeSet::Clamp(const FGameplayAttribute& Attr, float& Value) const
{
    auto* Rule = Rules.FindByPredicate([&](const auto& R) { return Attribute(R.Name) == Attr; });
    if (!Rule)
        return;
    if (!FMath::IsFinite(Value))
    {
        Value = Attr.GetNumericValue(this);
        return;
    }
    if (Rule->HasMin)
        Value = FMath::Max(Value, Rule->Minimum);
    if (Rule->HasMax)
        Value = FMath::Min(Value, Rule->Maximum);
    if (!Rule->MaximumAttribute.IsNone())
    {
        const auto Max = Attribute(Rule->MaximumAttribute);
        if (Max.IsValid())
            Value = FMath::Min(Value, Max.GetNumericValue(this));
    }
}
void UDasAuthoredAttributeSet::PreAttributeChange(const FGameplayAttribute& Attr, float& Value)
{
    Super::PreAttributeChange(Attr, Value);
    Clamp(Attr, Value);
}
void UDasAuthoredAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attr, float& Value) const
{
    Super::PreAttributeBaseChange(Attr, Value);
    Clamp(Attr, Value);
}
void UDasAuthoredAttributeSet::PostAttributeChange(const FGameplayAttribute& Attr, float Old, float Value)
{
    Super::PostAttributeChange(Attr, Old, Value);
    if (auto* ASC = GetOwningAbilitySystemComponent())
        for (const auto& Rule : Rules)
            if (!Rule.MaximumAttribute.IsNone() && Attribute(Rule.MaximumAttribute) == Attr)
            {
                const auto Child = Attribute(Rule.Name);
                float Current = Child.GetNumericValue(this);
                Clamp(Child, Current);
                if (Current != Child.GetNumericValue(this))
                    ASC->SetNumericAttributeBase(Child, Current);
            }
}
void UDasAuthoredAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    auto* ASC = GetOwningAbilitySystemComponent();
    float Value = ASC->GetNumericAttributeBase(Data.EvaluatedData.Attribute);
    Clamp(Data.EvaluatedData.Attribute, Value);
    if (Value != ASC->GetNumericAttributeBase(Data.EvaluatedData.Attribute))
        ASC->SetNumericAttributeBase(Data.EvaluatedData.Attribute, Value);
}
namespace
{
// Local authored actors can own an ASC without implementing the native C++ interface.
struct FDasAuthoredEffectContext final : FGameplayEffectContext
{
    virtual void AddInstigator(AActor* Owner, AActor* Avatar) override
    {
        FGameplayEffectContext::AddInstigator(Owner, Avatar);
        if (!InstigatorAbilitySystemComponent.IsValid())
            InstigatorAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner, true);
    }
    void SetSource(UAbilitySystemComponent* Source) { InstigatorAbilitySystemComponent = Source; }
    virtual FGameplayEffectContext* Duplicate() const override
    {
        auto* Copy = new FDasAuthoredEffectContext(*this);
        if (const auto* Hit = GetHitResult())
            Copy->AddHitResult(*Hit, true);
        return Copy;
    }
};
}
FGameplayEffectContextHandle UDasAuthoredAbilitySystemComponent::MakeEffectContext() const
{
    auto* Data = new FDasAuthoredEffectContext();
    Data->AddInstigator(GetOwnerActor(), GetAvatarActor());
    Data->SetSource(const_cast<UDasAuthoredAbilitySystemComponent*>(this));
    return FGameplayEffectContextHandle(Data);
}
UDasAuthoredAbilitySystemComponent::UDasAuthoredAbilitySystemComponent()
{
    SetIsReplicatedByDefault(false);
    PrimaryComponentTick.bCanEverTick = false;
}
bool UDasAuthoredAbilitySystemComponent::Live() const
{
    auto* Owner = GetOwner();
    auto* World = Owner ? Owner->GetWorld() : nullptr;
    return IsInGameThread() && !Ended && IsValid(Owner) && Owner->HasAuthority() && !Owner->IsActorBeingDestroyed() &&
           !IsBeingDestroyed() && !IsTemplate() && World && World->IsGameWorld() && !World->bIsTearingDown;
}
bool UDasAuthoredAbilitySystemComponent::InitializeAuthoredActorInfo()
{
    if (!Live())
        return false;
    TArray<UAbilitySystemComponent*> Components;
    GetOwner()->GetComponents(Components);
    if (Components.Num() != 1 || Components[0] != this)
        return false;
    if (GetOwnerActor())
        return GetOwnerActor() == GetOwner() && GetAvatarActor() == GetOwner();
    InitAbilityActorInfo(GetOwner(), GetOwner());
    return true;
}
bool UDasAuthoredAbilitySystemComponent::AddAuthoredAttributes(TSubclassOf<UDasAuthoredAttributeSet> Class)
{
    if (!Live() || !Class || Class->HasAnyClassFlags(CLASS_Abstract | CLASS_NewerVersionExists))
        return false;
    const auto* Defaults = Class->GetDefaultObject<UDasAuthoredAttributeSet>();
    if (Defaults->DefinitionName.IsNone() || Defaults->DefinitionId.IsEmpty() || Defaults->Rules.IsEmpty())
        return false;
    if (const auto* Existing = Sets.Find(Defaults->DefinitionName))
        return (*Existing)->GetClass() == Class;
    if (Sets.Num() >= 16)
        return false;
    if (!InitializeAuthoredActorInfo())
        return false;
    auto* Set = NewObject<UDasAuthoredAttributeSet>(GetOwner(), Class);
    for (const auto& Rule : Set->Rules)
        if (!Set->Attribute(Rule.Name).IsValid())
            return false;
    Set->InitializeDeclaredValues();
    AddAttributeSetSubobject(Set);
    Sets.Add(Defaults->DefinitionName, Set);
    for (const auto& Rule : Set->Rules)
    {
        const auto Attr = Set->Attribute(Rule.Name);
        ChangeHandles.Add(Attr, GetGameplayAttributeValueChangeDelegate(Attr).AddUObject(
                                    this, &UDasAuthoredAbilitySystemComponent::Changed, Defaults->DefinitionName));
    }
    return true;
}
FGameplayAttribute UDasAuthoredAbilitySystemComponent::FindAttribute(FName Set, FName Name) const
{
    if (!Live())
        return {};
    const auto* Value = Sets.Find(Set);
    return Value ? (*Value)->Attribute(Name) : FGameplayAttribute();
}
bool UDasAuthoredAbilitySystemComponent::HasAuthoredAttribute(FName Set, FName Name) const
{
    return FindAttribute(Set, Name).IsValid();
}
float UDasAuthoredAbilitySystemComponent::ReadAuthoredAttribute(FName Set, FName Name) const
{
    const auto Attr = FindAttribute(Set, Name);
    return Attr.IsValid() ? GetNumericAttribute(Attr) : 0;
}
FDasAuthoredAttributeChange UDasAuthoredAbilitySystemComponent::AuthoredAttributeSnapshot(FName Set, FName Name) const
{
    FDasAuthoredAttributeChange Value;
    Value.Set = Set;
    Value.Attribute = Name;
    Value.OldValue = Value.NewValue = ReadAuthoredAttribute(Set, Name);
    return Value;
}
FDasAuthoredEffectSpec
UDasAuthoredAbilitySystemComponent::MakeAuthoredSpec(TSubclassOf<UDasAuthoredGameplayEffect> Class, float Level)
{
    FDasAuthoredEffectSpec Value;
    if (!Live() || Sets.IsEmpty() || !Class || Class->HasAnyClassFlags(CLASS_Abstract | CLASS_NewerVersionExists) ||
        !FMath::IsFinite(Level) || Level <= 0)
        return Value;
    auto* Def = Class->GetDefaultObject<UDasAuthoredGameplayEffect>();
    if (Def->DefinitionId.IsEmpty() || (Def->Modifiers.IsEmpty() && Def->Executions.IsEmpty()))
        return Value;
    if (!Definitions.Contains(Def))
    {
        if (Definitions.Num() >= 32)
            return Value;
        Definitions.Add(Def);
    }
    Value.Source = this;
    Value.Definition = Def;
    Value.Level = Level;
    return Value;
}
FDasAuthoredEffectResult UDasAuthoredAbilitySystemComponent::ApplyAuthoredSpec(const FDasAuthoredEffectSpec& Spec)
{
    return ApplyAuthoredSpecInternal(Spec, nullptr);
}
FDasAuthoredEffectResult
UDasAuthoredAbilitySystemComponent::ApplyAuthoredSpecInternal(const FDasAuthoredEffectSpec& Spec, const FHitResult* Hit)
{
    FDasAuthoredEffectResult Result;
    auto* Source = Spec.Source.Get();
    auto* Def = Spec.Definition.Get();
    if (!Live() || !Source || !Source->Live() || Source->GetWorld() != GetWorld() || !IsValid(Def) ||
        !Source->Definitions.Contains(Def) || Def != Def->GetClass()->GetDefaultObject() ||
        !FMath::IsFinite(Spec.Level) || Spec.Level <= 0 || Spec.Magnitudes.Num() > 32)
        return Result;
    for (const auto& Mod : Def->Modifiers)
        if (!Mod.Attribute.IsValid() || !GetAttributeSubobject(Mod.Attribute.GetAttributeSetClass()))
            return Result;
    auto CapturesReady = [&](const UGameplayEffectCalculation* Calculation)
    {
        if (!Calculation)
            return false;
        for (const auto& C : Calculation->GetAttributeCaptureDefinitions())
        {
            auto* ASC = C.AttributeSource == EGameplayEffectAttributeCaptureSource::Source ? Source : this;
            if (!C.AttributeToCapture.IsValid() ||
                !ASC->GetAttributeSubobject(C.AttributeToCapture.GetAttributeSetClass()))
                return false;
        }
        if (const auto* E = Cast<UDasGASExecutionCalculation>(Calculation))
            if (!E->Definition.Output.IsValid() || !GetAttributeSubobject(E->Definition.Output.GetAttributeSetClass()))
                return false;
        return true;
    };
    for (const auto& M : Def->Modifiers)
        if (auto C = M.ModifierMagnitude.GetCustomMagnitudeCalculationClass();
            C && !CapturesReady(C->GetDefaultObject<UGameplayModMagnitudeCalculation>()))
            return Result;
    for (const auto& E : Def->Executions)
        if (!E.CalculationClass ||
            !CapturesReady(E.CalculationClass->GetDefaultObject<UGameplayEffectExecutionCalculation>()))
            return Result;
    for (FName Name : Def->RequiredMagnitudes)
        if (!Spec.Magnitudes.Contains(Name))
            return Result;
    for (const auto& Pair : Spec.Magnitudes)
        if (!Def->RequiredMagnitudes.Contains(Pair.Key) || !FMath::IsFinite(Pair.Value) ||
            !FGameplayTag::RequestGameplayTag(Pair.Key, false).IsValid())
            return Result;
    PruneActiveEffects();
    if (Def->DurationPolicy != EGameplayEffectDurationType::Instant &&
        (ActiveEffects.Num() >= 1024 || NextActiveId == MAX_int32))
        return Result;
    auto Context = Source->MakeEffectContext();
    Context.AddSourceObject(Source->GetOwner());
    if (Hit)
    {
        if (Hit->GetActor() != GetOwner())
            return Result;
        Context.AddHitResult(*Hit, true);
    }
    auto Native = Source->MakeOutgoingSpec(Def->GetClass(), Spec.Level, Context);
    if (!Native.IsValid())
        return Result;
    for (const auto& Pair : Spec.Magnitudes)
        Native.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(Pair.Key, false), Pair.Value);
    Result.Valid = true;
    Result.Target = this;
    const auto Handle = ApplyGameplayEffectSpecToSelf(*Native.Data.Get());
    Result.Applied = Handle.WasSuccessfullyApplied();
    Result.HasActiveHandle = Handle.IsValid() && GetActiveGameplayEffect(Handle) != nullptr;
    if (Result.HasActiveHandle)
    {
        const int32* Existing = ActiveEffects.FindKey(Handle);
        const int32 Id = Existing ? *Existing : NextActiveId++;
        ActiveEffects.Add(Id, Handle);
        Result.Target = this;
        Result.ActiveId = Id;
    }
    return Result;
}
void UDasAuthoredAbilitySystemComponent::PruneActiveEffects()
{
    for (auto It = ActiveEffects.CreateIterator(); It; ++It)
        if (!GetActiveGameplayEffect(It.Value()))
            It.RemoveCurrent();
}
FDasAuthoredEffectState UDasAuthoredAbilitySystemComponent::AuthoredEffectState(int32 ActiveId) const
{
    FDasAuthoredEffectState Result;
    if (!Live())
        return Result;
    const auto* Handle = ActiveEffects.Find(ActiveId);
    const auto* Effect = Handle ? GetActiveGameplayEffect(*Handle) : nullptr;
    if (!Effect)
        return Result;
    Result.Active = true;
    Result.Infinite = Effect->GetDuration() == UGameplayEffect::INFINITE_DURATION;
    Result.Stacks = Effect->Spec.GetStackCount();
    Result.Duration = Effect->GetDuration();
    Result.Remaining = Result.Infinite ? -1.f : FMath::Max(0.f, Effect->GetTimeRemaining(GetWorld()->GetTimeSeconds()));
    Result.Period = Effect->GetPeriod();
    return Result;
}
bool UDasAuthoredAbilitySystemComponent::RemoveAuthoredEffect(int32 ActiveId, int32 Stacks)
{
    if (!Live() || (Stacks != -1 && Stacks <= 0))
        return false;
    const auto* Found = ActiveEffects.Find(ActiveId);
    if (!Found)
        return false;
    const auto Handle = *Found;
    const bool Removed = RemoveActiveGameplayEffect(Handle, Stacks);
    PruneActiveEffects();
    return Removed;
}
void UDasAuthoredAbilitySystemComponent::Changed(const FOnAttributeChangeData& Data, FName Set)
{
    if (!Live() || Data.OldValue == Data.NewValue)
        return;
    FDasAuthoredAttributeChange Value;
    Value.Set = Set;
    Value.Attribute = Data.Attribute.GetUProperty()->GetFName();
    Value.OldValue = Data.OldValue;
    Value.NewValue = Data.NewValue;
    AttributeChanged.Broadcast(Value);
}
void UDasAuthoredAbilitySystemComponent::Detach()
{
    for (const auto& Pair : ChangeHandles)
        GetGameplayAttributeValueChangeDelegate(Pair.Key).Remove(Pair.Value);
    ChangeHandles.Empty();
}
void UDasAuthoredAbilitySystemComponent::EndPlay(const EEndPlayReason::Type Reason)
{
    Ended = true;
    Detach();
    Definitions.Empty();
    ActiveEffects.Empty();
    TargetHits.Empty();
    TargetQueries.Empty();
    TargetSelections.Empty();
    Abilities.Empty();
    Super::EndPlay(Reason);
}
void UDasAuthoredAbilitySystemComponent::BeginDestroy()
{
    Detach();
    Super::BeginDestroy();
}
