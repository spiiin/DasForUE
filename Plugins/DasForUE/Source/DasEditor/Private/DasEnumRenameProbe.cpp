#include "DasTypeService.h"
#include "DasTypeRegistry.h"
#include "DasDefinedEnum.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasActor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "K2Node_SwitchEnum.h"
#include "K2Node_Select.h"
#include "K2Node_EnumLiteral.h"
#include "K2Node_FunctionEntry.h"
#include "EdGraphSchema_K2.h"
#include "UObject/StructOnScope.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Editor.h"
void DasFailEnumAppendForTesting(int32);
bool RunDasEnumNodeProbes(ADasActor*, bool);
namespace
{
bool SaveRename(UObject* O)
{
    const FString Path = FPackageName::LongPackageNameToFilename(
        O->GetOutermost()->GetName(),
        O->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
    FSavePackageArgs A;
    A.TopLevelFlags = RF_Public | RF_Standalone;
    A.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(O->GetOutermost(), O, *Path, A);
}
}
bool RunDasEnumRenameProbe(const FString& Script, UDasTypeRegistry* Registry, UBlueprint* BP, UWorld* World,
                           bool Create, FDasRuntimeModule& R)
{
    bool Pass = true;
    FString Error;
    auto Check = [&](bool V, const TCHAR* Name)
    {
        Pass &= V;
        UE_LOG(LogTemp, Display, TEXT("DAS_ENUM_RENAME %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), Name);
        if (!V && !Error.IsEmpty())
            UE_LOG(LogTemp, Display, TEXT("Rename detail: %s"), *Error);
    };
    auto* Enum = CastChecked<UDasDefinedEnum>(Registry->Resolve(TEXT("direct.State"), true, Error));
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
    auto GraphState = [&](FName Name)
    {
        TArray<UEdGraph*> Graphs;
        BP->GetAllGraphs(Graphs);
        bool Switch = false, Select = false, Literal = false, Input = false;
        for (auto* G : Graphs)
            for (UEdGraphNode* N : G->Nodes)
            {
                if (auto* S = Cast<UK2Node_SwitchEnum>(N))
                    if (S->Enum == Enum)
                    {
                        auto* Pin = S->FindPin(Name);
                        Switch |= Pin && Pin->LinkedTo.Num() == 1;
                    }
                if (auto* S = Cast<UK2Node_Select>(N))
                    if (S->GetEnum() == Enum)
                    {
                        auto* Pin = S->FindPin(Name);
                        Select |= Pin && Pin->DefaultValue == TEXT("420");
                    }
                if (auto* L = Cast<UK2Node_EnumLiteral>(N))
                    if (L->Enum == Enum)
                        Literal |= L->FindPinChecked(UK2Node_EnumLiteral::GetEnumInputPinName())->DefaultValue ==
                                   Name.ToString();
                if (auto* E = Cast<UK2Node_FunctionEntry>(N))
                    if (G->GetFName() == TEXT("EnumSelect"))
                        Input |= E->FindPinChecked(TEXT("Input"))->DefaultValue == Name.ToString();
            }
        return Switch && Select && Literal && Input;
    };
    if (Create)
    {
        FString Before;
        FFileHelper::LoadFileToString(Before, *R.ScriptPath(Script));
        const FString After = Before.Replace(
            TEXT("ue_enum(id=\"direct.State\","),
                 TEXT("ue_enum(member_id_Running=\"direct.State.Active\", id=\"direct.State\","))
                     .Replace(TEXT("Active = 42"),
                                   TEXT("Running = 42")).Replace(TEXT("PlayState.Active"), TEXT("PlayState.Running"));
        Check(FFileHelper::SaveStringToFile(After, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_rename_candidate"));
        const bool EnumDirty = Enum->GetOutermost()->IsDirty(), BPDirty = BP->GetOutermost()->IsDirty(),
                   WorldDirty = World->GetOutermost()->IsDirty();
        const FString ClassWire = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
        for (int32 Stage : {1, 2, 3, 4})
        {
            DasFailEnumAppendForTesting(Stage);
            const bool Failed = !FDasTypeService::RenameEnumMembers(Script, Registry->GetPathName(), Error);
            DasFailEnumAppendForTesting(0);
            Refresh();
            Check(
                Failed &&
                Error.StartsWith(
                    TEXT("Enum append rolled back:")) &&
                    Enum->GetValueByNameString(
                        TEXT("Active")) == 42 &&
                        Enum->GetIndexByNameString(
                            TEXT("Running")) == INDEX_NONE &&
                            Enum->MemberIds.FindRef(42) ==
                                TEXT("direct.State.Active") &&
                                     GraphState(
                                         TEXT("Active")) && RunDasEnumNodeProbes(Actor, true) &&
                                         FindFProperty<FByteProperty>(
                                             Actor->GetClass(),
                                             TEXT("Mode"))->GetPropertyValue_InContainer(Actor) == 42 &&
                                             BP->GetOutermost()->GetMetaData().GetValue(
                                                 BP, TEXT("DasDSLModel")) == ClassWire &&
                                                 Enum->GetOutermost()->IsDirty() == EnumDirty &&
                                                 BP->GetOutermost()->IsDirty() == BPDirty &&
                                                 World->GetOutermost()->IsDirty() == WorldDirty,
                                                 TEXT("rollback_restores_names_defaults_links_values_and_dirty_flags"));
        }
        Check(FFileHelper::SaveStringToFile(
            After.Replace(TEXT("member_id_Running=\"direct.State.Active\""),
                               TEXT("member_id_Running=\"wrong.running\"")),
                          *R.ScriptPath(Script), FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
            !FDasTypeService::RenameEnumMembers(Script, Registry->GetPathName(), Error) &&
            Enum->GetValueByNameString(TEXT("Active")) == 42, TEXT("changed_member_ID_refused"));
        Check(FFileHelper::SaveStringToFile(
            After.Replace(TEXT("Running = 42"), TEXT("Running = 41")), *R.ScriptPath(Script),
                              FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                          !FDasTypeService::RenameEnumMembers(Script, Registry->GetPathName(), Error),
            TEXT("renumber_refused"));
        Check(FFileHelper::SaveStringToFile(After, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("restore_valid_candidate"));
        Check(FDasTypeService::RenameEnumMembers(Script, Registry->GetPathName(), Error), TEXT("rename_success"));
        Refresh();
        Check(GraphState(TEXT("Running")) && RunDasEnumNodeProbes(Actor, true),
                         TEXT("renamed_branch_select_literal_input_defaults_execute_same_values"));
        Check(Enum->GetValueByNameString(
            TEXT("Running")) == 42 &&
            Enum->GetIndexByNameString(
                TEXT("Active")) == INDEX_NONE &&
                Enum->MemberIds.FindRef(42) ==
                    TEXT("direct.State.Active") &&
                         FindFProperty<FByteProperty>(Actor->GetClass(),
                                                      TEXT("Mode"))->GetPropertyValue_InContainer(Actor) == 42,
                                                      TEXT("same_number_ID_and_instance_data"));
        FDasAuthoringModule RenamedClass;
        Check(FDasAuthoringModule::FromJson(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel")),
                                                                                       RenamedClass, Error),
                                            TEXT("read_renamed_class_model"));
        Check(FindFProperty<FByteProperty>(
            BP->GeneratedClass,
            TEXT("Mode"))->GetPropertyValue_InContainer(BP->GeneratedClass->GetDefaultObject()) == 42 &&
            RenamedClass.Classes[0].Properties.ContainsByPredicate(
                [](const auto& P) { return P.Name == TEXT("Mode") && P.Default.String == TEXT("Running"); }),
            TEXT("class_default_symbol_changed_number_preserved"));
        R.ReleaseObject(Actor);
        Check(FDasTypeService::RenameEnumMembers(Script, Registry->GetPathName(), Error), TEXT("rename_idempotent"));
        for (const auto& B : Registry->Bindings)
            Check(SaveRename(B.Type), TEXT("save_renamed_type_model"));
        Check(SaveRename(BP), TEXT("save_renamed_graphs"));
    }
    else
    {
        Check(Enum->GetValueByNameString(TEXT("Running")) == 42 &&
                                         Enum->MemberIds.FindRef(42) ==
                                             TEXT("direct.State.Active") &&
                                                  GraphState(TEXT("Running")) && RunDasEnumNodeProbes(Actor, true),
                                                             TEXT("reopened_rename_graphs_defaults_and_member_ID"));
        R.ReleaseObject(Actor);
        Check(FDasTypeService::RenameEnumMembers(Script, Registry->GetPathName(), Error),
              TEXT("reopened_rename_idempotent"));
    }
    return Pass;
}
