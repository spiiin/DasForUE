#include "DasGASCalculations.h"
#include "DasRuntime.h"
#include "AbilitySystemComponent.h"
#include "Modules/ModuleManager.h"
#include "GameFramework/Actor.h"
namespace
{
FDasExecutionResult Evaluate(const UObject* Owner, const FDasGASCalculationDefinition& D,
                             const FDasGASCalculationContext& C)
{
    auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasExecutionResult Result;
    if (R && IsInGameThread())
        Result = R->DispatchGASCalculation(const_cast<UObject*>(Owner), D.Script, D.Entry, D.Contract, C,
                                           D.StructuredOutput);
    if (!Result.bSucceeded || (!D.StructuredOutput && !FMath::IsFinite(Result.FloatValue)))
    {
        Result.bSucceeded = false;
        UE_LOG(LogTemp, Warning, TEXT("DAS_GAS_CALCULATION_FAILED %s: %s"), *D.Id, *Result.Error);
    }
    return Result;
}
void Release(UObject* Object)
{
    if (IsInGameThread())
        if (auto* R = FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))
            R->ReleaseObject(Object);
}
bool Context(const FGameplayEffectSpec& Spec, const FDasGASCalculationDefinition& D, FDasGASCalculationContext& C)
{
    C.Level = Spec.GetLevel();
    C.Source = Spec.GetContext().GetOriginalInstigator();
    for (const auto& P : Spec.SetByCallerTagMagnitudes)
        C.SetByCaller.Add(P.Key.GetTagName(), P.Value);
    for (const auto& Caller : D.Callers)
    {
        const auto* Value = C.SetByCaller.Find(Caller.Tag);
        if (!Value || !FMath::IsFinite(*Value))
        {
            UE_LOG(LogTemp, Warning, TEXT("DAS_GAS_CALCULATION_MISSING_CALLER %s %s"), *D.Id, *Caller.Name.ToString());
            return false;
        }
        C.Values.Add(Caller.Name, *Value);
    }
    return true;
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
    FDasGASCalculationContext C;
    if (!Context(Spec, Definition, C))
        return 0;
    const auto P = Parameters(Spec);
    for (const auto& Capture : Definition.Captures)
    {
        float V = 0;
        if (!GetCapturedAttributeMagnitude(Capture.Definition, Spec, P, V) || !FMath::IsFinite(V))
            return 0;
        C.Values.Add(Capture.Name, V);
    }
    const auto Result = Evaluate(this, Definition, C);
    return Result.bSucceeded ? Result.FloatValue : 0;
}
void UDasGASExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& Params,
                                                         FGameplayEffectCustomExecutionOutput& Out) const
{
    const auto& Spec = Params.GetOwningSpec();
    FDasGASCalculationContext C;
    if (!Context(Spec, Definition, C))
        return;
    const auto P = Parameters(Spec);
    if (auto* ASC = Params.GetTargetAbilitySystemComponent())
        C.Target = ASC->GetOwner();
    for (const auto& Capture : Definition.Captures)
    {
        float V = 0;
        // Native GAS includes Execution.CalculationModifiers only in this evaluation.
        if (!Params.AttemptCalculateCapturedAttributeMagnitude(Capture.Definition, P, V) || !FMath::IsFinite(V))
            return;
        C.Values.Add(Capture.Name, V);
    }
    const auto Result = Evaluate(this, Definition, C);
    if (!Result.bSucceeded)
        return;
    TMap<FName, float> Values;
    if (Definition.StructuredOutput)
    {
        const auto* Typed = Result.StructValue.GetPtr<FDasGASExecutionResult>();
        if (!Typed)
            return;
        Values = Typed->Outputs;
    }
    else if (Definition.Output.IsValid())
        Values.Add(FName(*Definition.Output.GetName()), Result.FloatValue);
    // Validate the entire output before publishing any native modifier.
    if (Values.Num() > 16)
        return;
    for (const auto& Pair : Values)
        if (!FMath::IsFinite(Pair.Value) ||
            !Definition.Outputs.ContainsByPredicate([&](const auto& O)
                                                    { return O.Name == Pair.Key && O.Attribute.IsValid(); }))
        {
            UE_LOG(LogTemp, Warning, TEXT("DAS_GAS_CALCULATION_INVALID_OUTPUT %s"), *Definition.Id);
            return;
        }
    // Declaration order makes multiple operations on an attribute deterministic.
    for (const auto& O : Definition.Outputs)
        if (const float* Value = Values.Find(O.Name))
            Out.AddOutputModifier(FGameplayModifierEvaluatedData(O.Attribute, O.Operation, *Value));
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
