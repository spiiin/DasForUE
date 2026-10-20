#include "DasGASCalculations.h"
#include "DasRuntime.h"
#include "AbilitySystemComponent.h"
#include "Modules/ModuleManager.h"
#include "GameFramework/Actor.h"
namespace
{
bool Evaluate(const UObject* Owner, const FDasGASCalculationDefinition& D, const FDasGASCalculationContext& C,
              float& Value)
{
    auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
    if (!R || !IsInGameThread())
        return false;
    const auto Result = R->DispatchGASCalculation(const_cast<UObject*>(Owner), D.Script, D.Entry, D.Contract, C);
    if (!Result.bSucceeded || !FMath::IsFinite(Result.FloatValue))
    {
        UE_LOG(LogTemp, Warning, TEXT("DAS_GAS_CALCULATION_FAILED %s: %s"), *D.Id, *Result.Error);
        return false;
    }
    Value = Result.FloatValue;
    return true;
}
void Release(UObject* Object)
{
    if (IsInGameThread())
        if (auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
            R->ReleaseObject(Object);
}
FDasGASCalculationContext Context(const FGameplayEffectSpec& Spec)
{
    FDasGASCalculationContext C;
    C.Level = Spec.GetLevel();
    C.Source = Spec.GetContext().GetOriginalInstigator();
    for (const auto& P : Spec.SetByCallerTagMagnitudes)
        C.SetByCaller.Add(P.Key.GetTagName(), P.Value);
    return C;
}
FAggregatorEvaluateParameters Parameters(const FGameplayEffectSpec& Spec)
{
    FAggregatorEvaluateParameters P;
    P.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    P.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    return P;
}
}
void UDasGASMagnitudeCalculation::ConfigureCaptures()
{
    RelevantAttributesToCapture.Empty();
    for (const auto& C : Definition.Captures)
        RelevantAttributesToCapture.AddUnique(C.Definition);
}
void UDasGASExecutionCalculation::ConfigureCaptures()
{
    RelevantAttributesToCapture.Empty();
    for (const auto& C : Definition.Captures)
        RelevantAttributesToCapture.AddUnique(C.Definition);
}
float UDasGASMagnitudeCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    auto C = Context(Spec);
    const auto P = Parameters(Spec);
    for (const auto& Capture : Definition.Captures)
    {
        float V = 0;
        if (!GetCapturedAttributeMagnitude(Capture.Definition, Spec, P, V) || !FMath::IsFinite(V))
            return 0;
        C.Values.Add(Capture.Name, V);
    }
    float V = 0;
    Evaluate(this, Definition, C, V);
    return V;
}
void UDasGASExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& Params,
                                                         FGameplayEffectCustomExecutionOutput& Out) const
{
    const auto& Spec = Params.GetOwningSpec();
    auto C = Context(Spec);
    const auto P = Parameters(Spec);
    if (auto* ASC = Params.GetTargetAbilitySystemComponent())
        C.Target = ASC->GetOwner();
    for (const auto& Capture : Definition.Captures)
    {
        float V = 0;
        if (!Params.AttemptCalculateCapturedAttributeMagnitude(Capture.Definition, P, V) || !FMath::IsFinite(V))
            return;
        C.Values.Add(Capture.Name, V);
    }
    float V = 0;
    if (Definition.Output.IsValid() && Evaluate(this, Definition, C, V))
        Out.AddOutputModifier(FGameplayModifierEvaluatedData(Definition.Output, Definition.Operation, V));
}
void UDasGASMagnitudeCalculation::BeginDestroy()
{
    Release(this);
    Super::BeginDestroy();
}
void UDasGASExecutionCalculation::BeginDestroy()
{
    Release(this);
    Super::BeginDestroy();
}
