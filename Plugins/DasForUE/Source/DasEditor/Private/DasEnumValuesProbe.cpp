#include "DasTypeService.h"
#include "DasTypeRegistry.h"
#include "DasDefinedEnum.h"
#include "DasRuntime.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "DasActor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "K2Node_SwitchEnum.h"
#include "K2Node_Select.h"
#include "UObject/StructOnScope.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Editor.h"
void DasFailEnumAppendForTesting(int32);
bool RunDasEnumValuesProbe(const FString& Script, UDasTypeRegistry* Registry, UBlueprint* BP, UWorld* World,
                           bool Create, FDasRuntimeModule& R)
{
    bool Pass = true;
    FString Error;
    auto Check = [&](bool V, const TCHAR* Name)
    {
        Pass &= V;
        UE_LOG(LogTemp, Display, TEXT("DAS_ENUM_VALUES %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), Name);
        if (!V)
            UE_LOG(LogTemp, Display, TEXT("Values detail: %s"), *Error);
    };
    auto* E = CastChecked<UDasDefinedEnum>(Registry->Resolve(TEXT("direct.State"), true, Error));
    ADasActor* Actor = nullptr;
    auto Refresh = [&]()
    {
        for (auto& V : World->PersistentLevel->Actors)
            if (V && V->GetFName() == TEXT("TypedActor"))
                Actor = Cast<ADasActor>(V.Get());
    };
    Refresh();
    if (!Actor)
        return false;
    R.ReleaseObject(Actor);
    const FString Rules = FPaths::GetBaseFilename(Script) + TEXT("_values.json");
    auto WriteRules = [&](int32 From, int32 To)
    {
        return FFileHelper::SaveStringToFile(FString::Printf(
            TEXT("{\"schema_version\":1,\"rules\":[{\"enum_id\":\"direct.State\",\"from\":%d,\"to\":%d}]}"), From, To),
            *R.ScriptPath(Rules), FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    auto Run = [&](FName Name, int32 Input = 0)
    {
        auto* F = Actor->FindFunction(Name);
        if (!F)
            return -1;
        FStructOnScope Args(F);
        if (auto* P = FindFProperty<FByteProperty>(F, TEXT("Input")))
            P->SetPropertyValue_InContainer(Args.GetStructMemory(), uint8(Input));
        {
            FEditorScriptExecutionGuard G;
            Actor->ProcessEvent(F, Args.GetStructMemory());
        }
        auto* Out = CastField<FNumericProperty>(F->GetReturnProperty());
        return Out ? int32(Out->GetSignedIntPropertyValue(Out->ContainerPtrToValuePtr<void>(Args.GetStructMemory())))
                   : -1;
    };
    auto Save = [&](UObject* O)
    {
        const FString Path = FPackageName::LongPackageNameToFilename(
            O->GetOutermost()->GetName(),
            O->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
        FSavePackageArgs A;
        A.TopLevelFlags = RF_Public | RF_Standalone;
        A.SaveFlags = SAVE_NoError;
        return UPackage::SavePackage(O->GetOutermost(), O, *Path, A);
    };
    if (Create)
    {
        FKismetEditorUtilities::CompileBlueprint(BP);
        Refresh();
        FString Before;
        FFileHelper::LoadFileToString(Before, *R.ScriptPath(Script));
        const FString Changed = Before.Replace(TEXT("Running = 42"), TEXT("Running = 50"));
        Check(FFileHelper::SaveStringToFile(Changed, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
              WriteRules(42, 7) && !FDasTypeService::MigrateEnumValues(Script, Registry->GetPathName(), Rules, Error) &&
              E->GetValueByNameString(TEXT("Running")) == 42, TEXT("wrong_numeric_rule_refused"));
        Check(WriteRules(42, 50), TEXT("write_exact_value_rule"));
        const bool Dirty = World->GetOutermost()->IsDirty();
        for (int32 Stage : {1, 2, 3, 4})
        {
            DasFailEnumAppendForTesting(Stage);
            const bool Failed = !FDasTypeService::MigrateEnumValues(Script, Registry->GetPathName(), Rules, Error);
            DasFailEnumAppendForTesting(0);
            Refresh();
            Check(Failed && Error.StartsWith(
                                TEXT("Enum append rolled back:")) &&
                                E->GetValueByNameString(
                                    TEXT("Running")) == 42 &&
                                    FindFProperty<FByteProperty>(
                                        Actor->GetClass(), TEXT("Mode"))->GetPropertyValue_InContainer(Actor) == 42 &&
                                        World->GetOutermost()->IsDirty() == Dirty &&
                                        Run(TEXT("RouteEnum"), 42) == 420 &&
                                            Run(TEXT("EnumLiteral")) == 42,
                                                TEXT("numeric_rule_rollback_restores_actor_CDO_graph_and_dirty_flags"));
        }
        Check(FDasTypeService::MigrateEnumValues(Script, Registry->GetPathName(), Rules, Error),
              TEXT("numeric_rule_success"));
        Refresh();
        Check(E->GetValueByNameString(
            TEXT("Running")) == 50 &&
            E->MemberIds.FindRef(50) ==
                TEXT("direct.State.Active") &&
                     FindFProperty<FByteProperty>(
                         Actor->GetClass(), TEXT("Mode"))->GetPropertyValue_InContainer(Actor) == 50 &&
                         FindFProperty<FByteProperty>(
                             BP->GeneratedClass,
                             TEXT("Mode"))->GetPropertyValue_InContainer(BP->GeneratedClass->GetDefaultObject()) ==
                                 50 &&
                             Run(TEXT("RouteEnum"), 50) == 420 &&
                                 Run(TEXT("EnumLiteral")) == 50 &&
                                     Run(TEXT("EnumSum")) == 257, TEXT("numbers_and_graph_results_mapped_by_identity"));
        R.ReleaseObject(Actor);
        FindFProperty<FByteProperty>(Actor->GetClass(), TEXT("Mode"))->SetPropertyValue_InContainer(Actor, 200);
        const FString Removed = Changed.Replace(TEXT("\r\n"), TEXT("\n")).Replace(TEXT("    Paused = 200\n"), TEXT(""));
        Check(FFileHelper::SaveStringToFile(Removed.Replace(TEXT("\r\n"), TEXT("\n")), *R.ScriptPath(Script),
                                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                                                            WriteRules(200, 50),
                                            TEXT("write_removal_replacement"));
        // Paused is an unconnected Switch output / default Select option in this fixture.
        UEdGraphPin* RemovedBranch = nullptr;
        UEdGraphPin* ExistingTarget = nullptr;
        for (UEdGraph* G : BP->FunctionGraphs)
            for (UEdGraphNode* N : G->Nodes)
                if (auto* Switch = Cast<UK2Node_SwitchEnum>(N))
                {
                    if (auto* Active = Switch->FindPin(TEXT("Running")))
                        if (!Active->LinkedTo.IsEmpty())
                        {
                            RemovedBranch = Switch->FindPin(TEXT("Paused"));
                            ExistingTarget = Active->LinkedTo[0];
                        }
                }
        if (RemovedBranch && ExistingTarget)
        {
            RemovedBranch->MakeLinkTo(ExistingTarget);
            Check(!FDasTypeService::MigrateEnumValues(Script, Registry->GetPathName(), Rules, Error) &&
                  Error.Contains(TEXT("user logic")) && E->NumEnums() == 4,
                                 TEXT("linked_removed_branch_refused_before_mutation"));
            RemovedBranch->BreakLinkTo(ExistingTarget);
        }
        Check(FDasTypeService::MigrateEnumValues(Script, Registry->GetPathName(), Rules, Error),
              TEXT("removal_rule_success"));
        Refresh();
        Check(E->NumEnums() == 3 &&
              E->GetIndexByNameString(TEXT("Paused")) == INDEX_NONE &&
                                      FindFProperty<FByteProperty>(
                                          Actor->GetClass(), TEXT("Mode"))->GetPropertyValue_InContainer(Actor) == 50 &&
                                          Run(TEXT("EnumCount")) == 2 &&
                                              Run(TEXT("EnumSum")) == 57,
                                                  TEXT("removed_data_replaced_and_count_iteration_updated"));
        FDasScalarValue Mode;
        Mode.Kind = 13;
        Mode.IntValue = 50;
        Mode.ObjectClass = E->GetPathName();
        const auto Result = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_SetMode"), {Mode},
                                                                                 EDasMethodSignature::Void, Actor);
        Error = Result.Error;
        Check(Result.bSucceeded, TEXT("updated_DSL_accepts_new_number"));
        R.ReleaseObject(Actor);
        for (const auto& B : Registry->Bindings)
            Check(Save(B.Type), TEXT("save_value_migration_models"));
        Check(Save(BP) && Save(World), TEXT("save_remapped_blueprint_and_map"));
    }
    else
    {
        Check(E->NumEnums() == 3 &&
              E->GetValueByNameString(
                  TEXT("Running")) == 50 &&
                  E->MemberIds.FindRef(50) ==
                      TEXT("direct.State.Active") &&
                           FindFProperty<FByteProperty>(
                               Actor->GetClass(), TEXT("Mode"))->GetPropertyValue_InContainer(Actor) == 50 &&
                               Run(TEXT("RouteEnum"), 50) == 420 && Run(TEXT("EnumLiteral")) == 50 &&
                                                                        Run(TEXT("EnumSum")) == 57,
                                                                            TEXT("reopened_remapped_map_and_graphs"));
    }
    IFileManager::Get().Delete(*R.ScriptPath(Rules));
    return Pass;
}
