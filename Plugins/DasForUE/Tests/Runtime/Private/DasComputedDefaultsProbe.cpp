#include "DasComputedDefaultsProbe.h"
#include "DasContainerDefaultsProbe.h"
#include "DasDefaultValueProbe.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Components/SceneComponent.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"
bool RunDasComputedDefaultsProbe(UWorld* World, FString& Report)
{
    ADasProbeActor* A = nullptr;
    ADasProbeActor* M = nullptr;
    if (!World || !World->PersistentLevel)
        return false;
    for (const auto& Actor : World->PersistentLevel->Actors)
        if (auto* P = Cast<ADasProbeActor>(Actor))
        {
            if (P->Tags.Contains(TEXT("ComputedDefault")))
                A = P;
            if (P->Tags.Contains(TEXT("ComputedManual")))
                M = P;
        }
    if (!A || !M)
        return false;
    bool Good = true;
    auto Check = [&](bool OK, const TCHAR* N)
    {
        Good &= OK;
        Report += FString::Printf(TEXT("DAS_COMPUTED_COOKED %s %s\n"), OK ? TEXT("PASS"): TEXT("FAIL"), N);
    };
    auto Int = [](UObject* O, const TCHAR* N)
    {
        auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), N) : nullptr;
        return P ? P->GetPropertyValue_InContainer(O) : MIN_int32;
    };
    FString RecursiveError;
    Check(CheckDasRecursiveDefaults(RecursiveError), TEXT("recursive_defaults_materialization_and_GC"));
    Report += RecursiveError;
    CollectGarbage(RF_NoFlags, true);
    Check(ComputedAssetContainers(A) && ComputedManualAssetsMatch(M),
          TEXT("container_only_assets_cooked_and_retained_by_GC"));
    auto* CDO = A->GetClass()->GetDefaultObject();
    Check(Int(CDO, TEXT("Score")) == 40 && Int(CDO, TEXT("Derived")) == 42 &&
                                               Int(CDO, TEXT("Manual")) == 77 &&
                                                   Int(A, TEXT("Score")) == 40 &&
                                                       Int(M, TEXT("Score")) == 111,
                                                           TEXT("baked_CDO_and_manual_overrides"));
    Check(ComputedContainers(CDO) && ComputedContainers(A) && ComputedManualHashMatches(M) &&
          ComputedArrayLast(CDO, TEXT("Numbers")) == 8 &&
                            ComputedArrayLast(M, TEXT("Numbers")) == 777 &&
                                              ComputedArrayLast(CDO, TEXT("ManualNumbers")) == 777,
                                                                TEXT("container_defaults_and_whole_manual_overrides"));
    auto* Asset = FindFProperty<FObjectPropertyBase>(CDO->GetClass(), TEXT("Asset"));
    auto* Value = Asset ? Asset->GetObjectPropertyValue_InContainer(CDO) : nullptr;
    Check(Value && Value->GetPathName() == TEXT("/Engine/BasicShapes/Cube.Cube"),
                                                TEXT("computed_asset_dependency_is_cooked"));
    auto* Pivot = FindObject<USceneComponent>(A, TEXT("Pivot"));
    Check(Pivot && Pivot->GetRelativeLocation() == FVector(0, 0, 42), TEXT("baked_component_defaults"));
    auto* F = A->FindFunction(TEXT("Read"));
    if (F)
    {
        FStructOnScope Args(F);
        FindFProperty<FIntProperty>(F, TEXT("input"))->SetPropertyValue_InContainer(Args.GetStructMemory(), 0);
        A->ProcessEvent(F, Args.GetStructMemory());
        Check(CastFieldChecked<FIntProperty>(F->GetReturnProperty())
                          ->GetPropertyValue_InContainer(Args.GetStructMemory()) == 82 &&
                  A->LastDasError.IsEmpty(),
              TEXT("runtime_source_contract_and_execution"));
    }
    else
        Check(false, TEXT("missing_runtime_function"));
    ADasProbeActor* Sibling = nullptr;
    for (const auto& Actor : World->PersistentLevel->Actors)
        if (auto* P = Cast<ADasProbeActor>(Actor))
            if (P->Tags.Contains(TEXT("ComputedSibling")))
                Sibling = P;
    Check(Sibling && Int(Sibling, TEXT("Score")) == 40 && Int(Sibling, TEXT("Derived")) == 41,
                                                              TEXT("sibling_source_defaults_are_isolated"));
    ADasProbeActor* Child = nullptr;
    for (const auto& Actor : World->PersistentLevel->Actors)
        if (auto* P = Cast<ADasProbeActor>(Actor))
            if (P->Tags.Contains(TEXT("ComputedChild")))
                Child = P;
    Check(Child && Int(Child, TEXT("Score")) == 85 && Int(Child, TEXT("Derived")) == 87,
                                                          TEXT("inherited_computed_defaults"));
    TArray<UDasScriptComponent*> Components;
    A->GetComponents(Components);
    UDasScriptComponent* Component = nullptr;
    for (auto* C : Components)
        if (C->ComponentTags.Contains(TEXT("ComputedComponent")))
            Component = C;
    Check(Component && Int(Component->GetClass()->GetDefaultObject(), TEXT("Bonus")) == 42 &&
                           Int(Component, TEXT("Bonus")) == 55, TEXT("computed_ActorComponent_defaults"));
    if (Component)
    {
        auto* Read = Component->FindFunction(TEXT("Read"));
        if (Read)
        {
            FStructOnScope Args(Read);
            FindFProperty<FIntProperty>(Read, TEXT("input"))->SetPropertyValue_InContainer(Args.GetStructMemory(), 0);
            Component->ProcessEvent(Read, Args.GetStructMemory());
            Check(CastFieldChecked<FIntProperty>(Read->GetReturnProperty())
                              ->GetPropertyValue_InContainer(Args.GetStructMemory()) == 55 &&
                      Component->LastDasError.IsEmpty(),
                  TEXT("cooked_component_source_contract"));
        }
        else
            Check(false, TEXT("component_function_missing"));
    }
    auto* Fresh = World->SpawnActor<ADasProbeActor>(A->GetClass());
    Check(Fresh && ComputedContainers(Fresh) &&
          ComputedArrayLast(Fresh, TEXT("Numbers")) == 8 && ComputedArrayLast(Fresh, TEXT("ManualNumbers")) == 777 &&
                                                                              Int(Fresh, TEXT("Score")) == 40 &&
                                                                                  Int(Fresh, TEXT("Manual")) == 77,
                                                                                      TEXT("fresh_spawn"));
    if (Fresh)
        Fresh->Destroy();
    return Good;
}
