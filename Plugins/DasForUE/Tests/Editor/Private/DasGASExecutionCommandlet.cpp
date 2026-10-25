#include "DasGASAuthoredCommandlet.h"
#include "DasGASAssetService.h"
#include "DasGASCalculations.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/ScopeExit.h"
UDasGASExecutionCommandlet::UDasGASExecutionCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASExecutionCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_EXECUTION_%s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name);
        return OK;
    };
    FEditorScriptExecutionGuard Guard;
    TMap<FString, UBlueprint*> Assets;
    FString Error;
    const auto Folder = TEXT("/Game/Tests/GASAuthored/")+ FGuid::NewGuid().ToString(EGuidFormats::Digits);
    if (!FDasGASAssetService::Apply(TEXT("gas_execution_outputs.das"), Folder, Assets, Error, true))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    TArray<TStrongObjectPtr<UBlueprint>> Keep;
    for (auto& P : Assets)
        Keep.Emplace(P.Value);
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* Source = World->SpawnActor<ADasGASAuthoredProbeHost>();
    auto* Target = World->SpawnActor<ADasGASAuthoredProbeHost>();
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    auto* A = Source->Abilities.Get();
    auto* B = Target->Abilities.Get();
    const auto Stats = Assets.FindChecked(TEXT("ExecutionStats"))->GeneratedClass.Get();
    if (!A->AddAuthoredAttributes(Stats) || !B->AddAuthoredAttributes(Stats))
        return 1;
    auto Value = [&](UDasAuthoredAbilitySystemComponent* ASC, FName Name)
    {
        return ASC->ReadAuthoredAttribute(TEXT("ExecutionStats"), Name);
    };
    auto Spec = A->MakeAuthoredSpec(Assets.FindChecked(TEXT("Strike"))->GeneratedClass.Get(), 1);
    if (!Check(!B->ApplyAuthoredSpec(Spec).Valid && Value(B, TEXT("Health")) == 100.f &&
                                                          Value(B, TEXT("Threat")) == 0.f,
                                                                TEXT("missing_required_caller_refused_before_apply")))
        return 1;
    Spec.Magnitudes.Add(TEXT("Data.Healing"), 3.f);
    const auto* Definition = CastChecked<UDasAuthoredGameplayEffect>(Spec.Definition);
    if (!Check(Definition->RequiredMagnitudes.Num() == 1 && Definition->Executions.Num() == 1 &&
                   Definition->Executions[0].CalculationModifiers.Num() == 2,
               TEXT("native_scoped_modifiers_and_required_tag_imported")))
        return 1;
    auto Result = B->ApplyAuthoredSpec(Spec);
    if (!Check(Result.Valid && Result.Applied &&
               Value(B, TEXT("Health")) == 90.f && Value(B, TEXT("Threat")) == 15.f,
                                                         TEXT("two_das_outputs_in_one_native_execution")))
        return 1;
    if (!Check(Value(A, TEXT("Power")) == 10.f && Value(B, TEXT("Armor")) == 4.f,
                                                        TEXT("scoped_capture_modifiers_do_not_change_attributes")))
        return 1;
    Spec.Magnitudes.Add(TEXT("Unexpected.Input"), 1.f);
    if (!Check(!B->ApplyAuthoredSpec(Spec).Valid && Value(B, TEXT("Health")) == 90.f,
                                                          TEXT("undeclared_caller_refused")))
        return 1;
    Spec.Magnitudes.Remove(TEXT("Unexpected.Input"));
    Spec.Magnitudes[TEXT("Data.Healing")] = 0.f;
    Result = B->ApplyAuthoredSpec(Spec);
    if (!Check(Result.Applied && Value(B, TEXT("Health")) == 83.f &&
                                       Value(B, TEXT("Threat")) == 30.f,
                                             TEXT("zero_is_present_and_scoped_modifiers_do_not_accumulate")))
        return 1;
    auto Scalar = A->MakeAuthoredSpec(Assets.FindChecked(TEXT("ScalarStrike"))->GeneratedClass.Get(), 1);
    if (!Check(B->ApplyAuthoredSpec(Scalar).Applied && Value(B, TEXT("Health")) == 82.f,
                                                             TEXT("scalar_execution_shorthand")))
        return 1;
    auto Bonus = A->MakeAuthoredSpec(Assets.FindChecked(TEXT("BonusEffect"))->GeneratedClass.Get(), 1);
    if (B->ApplyAuthoredSpec(Bonus).Valid)
        return 1;
    Bonus.Magnitudes.Add(TEXT("Data.Healing"), 3.f);
    if (!Check(B->ApplyAuthoredSpec(Bonus).Applied && Value(B, TEXT("Health")) == 85.f,
                                                            TEXT("named_caller_in_native_magnitude_calculation")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_EXECUTION_RESULT=PASS checks=%d"), Checks);
    return 0;
}
