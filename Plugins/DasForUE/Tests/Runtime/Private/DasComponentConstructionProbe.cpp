#include "DasComponentConstructionProbe.h"
#include "DasRuntime.h"
#include "DasScriptComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

bool RunDasComponentConstructionProbe(UClass* Class, UWorld* World, FString& Report)
{
    bool Good = true;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        Good &= OK;
        Report += FString::Printf(TEXT("DAS_COMPONENT_CONSTRUCTION %s %s\n"), OK ? TEXT("PASS"): TEXT("FAIL"), Name);
    };
    auto Int = [](UObject* O, const TCHAR* N)
    {
        auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), N) : nullptr;
        return P ? P->GetPropertyValue_InContainer(O) : MIN_int32;
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const int32 Before = R.PersistentContextCount();
    if (!World || !Class || !Class->IsChildOf(UDasScriptComponent::StaticClass()))
        return false;
    TStrongObjectPtr<AActor> Owner(World->SpawnActor<AActor>());
    if (!Owner)
        return false;
    auto* C = NewObject<UDasScriptComponent>(Owner.Get(), Class, TEXT("StandaloneConstruction"), RF_Transactional);
    Owner->AddInstanceComponent(C);
    C->RegisterComponent();
    Check(C->IsRegistered() && C->LastDasError.IsEmpty() &&
          Int(C, TEXT("ConstructionValue")) == 21 && Int(C, TEXT("ConstructionCalls")) == 1,
                                                         TEXT("native_owner_first_registration_once"));
    C->UnregisterComponent();
    C->RegisterComponent();
    Check(Int(C, TEXT("ConstructionCalls")) == 1 && C->LastDasError.IsEmpty(),
              TEXT("reregistration_does_not_reconstruct"));
    if (World->HasBegunPlay())
        Check(Int(C, TEXT("InitializeCalls")) == 1 && Int(C, TEXT("BeginSaw")) == 21,
                                                          TEXT("dynamic_construction_precedes_Initialize_BeginPlay"));
#if WITH_EDITOR
    if (!World->IsGameWorld())
    {
        TWeakObjectPtr<UDasScriptComponent> Original(C);
        auto* Score = FindFProperty<FIntProperty>(Class, TEXT("Score"));
        Score->SetPropertyValue_InContainer(C, 9);
        FPropertyChangedEvent Event(Score, EPropertyChangeType::ValueSet);
        C->PostEditChangeProperty(Event);
        Check(Original.IsValid() && Int(C, TEXT("ConstructionValue")) == 27 &&
                                        Int(C, TEXT("ConstructionCalls")) == 2 &&
                                            Int(C, TEXT("InitializeCalls")) == 0,
                                                TEXT("editor_component_property_edit_reconstructs_once"));
        if (Original.IsValid())
        {
            Score->SetPropertyValue_InContainer(C, -1);
            const auto Failed = C->RunDasConstruction();
            Check(!Failed.bSucceeded &&
                  Failed.Error.Contains(TEXT("managed callbacks are forbidden during ConstructionScript")) &&
                                        Int(C, TEXT("ConstructionCalls")) == 3 && R.SubscriptionCount(C) == 0,
                                            TEXT("component_construction_rejects_timers"));
            Score->SetPropertyValue_InContainer(C, 9);
            const auto Recovered = C->RunDasConstruction();
            Check(Recovered.bSucceeded && C->LastDasError.IsEmpty() &&
                  Int(C, TEXT("ConstructionCalls")) == 4 && Int(C, TEXT("ConstructionValue")) == 27,
                                                                TEXT("component_construction_failure_is_retryable"));
        }
    }
#endif
    auto* CDO = Class->GetDefaultObject();
    Check(Int(CDO, TEXT("ConstructionValue")) == 0 && Int(CDO, TEXT("ConstructionCalls")) == 0,
                                                          TEXT("component_CDO_untouched"));
    auto Template = R.RunConstruction(R.ScriptPath(TEXT("dsl_construction_component.das")), CDO);
    Check(!Template.bSucceeded, TEXT("component_template_rejected"));
    R.ReleaseObject(C);
    Owner->Destroy();
    Check(R.PersistentContextCount() == Before, TEXT("component_construction_preserves_existing_VMs"));
    return Good;
}
