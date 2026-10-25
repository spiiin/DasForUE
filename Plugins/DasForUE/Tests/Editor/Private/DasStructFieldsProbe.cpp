#include "DasTypeService.h"
#include "DasTypeRegistry.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "StructUtils/UserDefinedStruct.h"
#include "Kismet2/StructureEditorUtils.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_VariableGet.h"
#include "K2Node_MakeStruct.h"
#include "K2Node_BreakStruct.h"
#include "EdGraphSchema_K2.h"
#include "UObject/StructOnScope.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
bool RunDasStructNumericProbe(const FString&, UDasTypeRegistry*, UBlueprint*, UWorld*, bool, FDasRuntimeModule&);
#include "Editor.h"
bool CheckDasNestedProbe(ADasActor*, bool);
bool CheckDasArrayAccess(const FString&, ADasActor*, FDasRuntimeModule&);
bool CheckDasNestedDefaultGuard(const FString&, UDasTypeRegistry*, UBlueprint*);
void DasFailStructMigrationForTesting(int32);
namespace
{
void StructWire(UEdGraph* G, UEdGraphPin* A, UEdGraphPin* B)
{
    check(G->GetSchema()->TryCreateConnection(A, B));
}
bool SaveStructProbe(UObject* O)
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
void BuildDasStructFieldProbes(UBlueprint* BP, UScriptStruct* Type)
{
    auto MakeGraph = [&](FName Name, FEdGraphPinType ResultType)
    {
        auto* G =
            FBlueprintEditorUtils::CreateNewGraph(BP, Name, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
        FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, G, true, nullptr);
        TArray<UK2Node_FunctionEntry*> Entries;
        G->GetNodesOfClass(Entries);
        auto* Result = NewObject<UK2Node_FunctionResult>(G);
        G->AddNode(Result);
        Result->CreateNewGuid();
        Result->FunctionReference = Entries[0]->FunctionReference;
        Result->AllocateDefaultPins();
        Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue, ResultType, EGPD_Input);
        StructWire(G, Entries[0]->FindPinChecked(UEdGraphSchema_K2::PN_Then),
                   Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        return TPair<UEdGraph*, UK2Node_FunctionResult*>(G, Result);
    };
    FEdGraphPinType Int;
    Int.PinCategory = UEdGraphSchema_K2::PC_Int;
    auto Read = MakeGraph(TEXT("ReadConfigCount"), Int);
    auto* Get = NewObject<UK2Node_VariableGet>(Read.Key);
    Get->VariableReference.SetSelfMember(TEXT("Config"));
    Read.Key->AddNode(Get);
    Get->CreateNewGuid();
    Get->AllocateDefaultPins();
    auto* Break = NewObject<UK2Node_BreakStruct>(Read.Key);
    Break->StructType = Type;
    Read.Key->AddNode(Break);
    Break->CreateNewGuid();
    Break->AllocateDefaultPins();
    StructWire(Read.Key, Get->GetValuePin(), Break->FindPinChecked(Type->GetFName()));
    StructWire(Read.Key, Break->FindPinChecked(FindDasStructField(Type, TEXT("Count"))->GetFName()),
                                               Read.Value->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
    FEdGraphPinType Struct;
    Struct.PinCategory = UEdGraphSchema_K2::PC_Struct;
    Struct.PinSubCategoryObject = Type;
    auto Write = MakeGraph(TEXT("MakeConfig"), Struct);
    auto* Make = NewObject<UK2Node_MakeStruct>(Write.Key);
    Make->StructType = Type;
    Write.Key->AddNode(Make);
    Make->CreateNewGuid();
    Make->AllocateDefaultPins();
    Make->FindPinChecked(FindDasStructField(Type, TEXT("Count"))->GetFName())->DefaultValue = TEXT("88");
    Make->FindPinChecked(FindDasStructField(Type, TEXT("Title"))->GetFName())->DefaultValue = TEXT("Graph 世界");
    StructWire(Write.Key, Make->FindPinChecked(Type->GetFName()),
               Write.Value->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
    FEdGraphPinType FromInputType;
    FromInputType.PinCategory = UEdGraphSchema_K2::PC_Struct;
    FromInputType.PinSubCategoryObject = Type;
    auto FromInput = MakeGraph(TEXT("MakeConfigFromInput"), FromInputType);
    TArray<UK2Node_FunctionEntry*> FromEntries;
    FromInput.Key->GetNodesOfClass(FromEntries);
    FromEntries[0]->CreateUserDefinedPin(TEXT("Input"), Int, EGPD_Output);
    auto* FromMake = NewObject<UK2Node_MakeStruct>(FromInput.Key);
    FromMake->StructType = Type;
    FromInput.Key->AddNode(FromMake);
    FromMake->CreateNewGuid();
    FromMake->AllocateDefaultPins();
    StructWire(FromInput.Key, FromEntries[0]->FindPinChecked(
                                  TEXT("Input")),
                                  FromMake->FindPinChecked(FindDasStructField(Type, TEXT("Count"))->GetFName()));
    StructWire(FromInput.Key, FromMake->FindPinChecked(Type->GetFName()),
               FromInput.Value->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
    FKismetEditorUtilities::CompileBlueprint(BP);
}
bool RunDasStructFieldProbe(const FString& Script, UDasTypeRegistry* Registry, UBlueprint* BP, UWorld* World,
                            bool Create, FDasRuntimeModule& R, bool Drop)
{
    bool Pass = true;
    FString Error;
    auto Check = [&](bool V, const TCHAR* Name)
    {
        Pass &= V;
        UE_LOG(LogTemp, Display, TEXT("DAS_STRUCT_FIELDS %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), Name);
        if (!V)
            UE_LOG(LogTemp, Display, TEXT("Struct detail: %s"), *Error);
    };
    auto* Type = CastChecked<UUserDefinedStruct>(Registry->Resolve(TEXT("direct.Settings"), false, Error));
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
    if (Create)
        Check(CheckDasArrayAccess(Script, Actor, R), TEXT("struct_array_token_access_before_migration"));
    auto RunRead = [&]()
    {
        auto* F = Actor->FindFunction(TEXT("ReadConfigCount"));
        FStructOnScope A(F);
        {
            FEditorScriptExecutionGuard G;
            Actor->ProcessEvent(F, A.GetStructMemory());
        }
        return CastFieldChecked<FIntProperty>(F->GetReturnProperty())
            ->GetPropertyValue_InContainer(A.GetStructMemory());
    };
    auto Read = [&](const TCHAR* Field)
    {
        auto* P = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Config"));
        return CastFieldChecked<FIntProperty>(FindDasStructField(Type, Field))
            ->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Actor));
    };
    const bool Numeric = FParse::Param(FCommandLine::Get(), TEXT("StructNumeric"));
    if (!Create && Numeric)
        return RunDasStructNumericProbe(Script, Registry, BP, World, false, R) &&
               CheckDasArrayAccess(Script, Actor, R) && CheckDasNestedProbe(Actor, true);
    if (Create)
    {
        FKismetEditorUtilities::CompileBlueprint(BP);
        Refresh();
        FString Before;
        FFileHelper::LoadFileToString(Before, *R.ScriptPath(Script));
        const FString After = Before.Replace(
            TEXT("Count : int = 15"),
                TEXT("Score : int = 15"))
                    .Replace(TEXT("Items.0.Inner.Count"),
                        TEXT("Items.0.Inner.Score"))
                            .Replace(TEXT("Items.99.Inner.Count"),
                                TEXT("Items.99.Inner.Score"))
                                    .Replace(TEXT("result.Count"),
                                        TEXT("result.Score"))
                                            .Replace(TEXT("self.Config.Count"),
                                                TEXT("self.Config.Score"))
                                                    .Replace(TEXT("Score : int = 15"),
                                                        TEXT("@ue_id=\"direct.Settings.Count\"\n    Score : int = 15\n    @ue_property=(EditAnywhere, BlueprintReadWrite)\n    Enabled : bool = true"));
        const FGuid Guid = Type->GetCustomGuid(), FieldGuid = FStructureEditorUtils::GetVarDesc(Type)[0].VarGuid;
        Check(FFileHelper::SaveStringToFile(After, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_add_and_rename_candidate"));
        Check(CheckDasNestedDefaultGuard(Script, Registry, BP), TEXT("nested_default_mismatch_guard"));
        const bool Dirty = World->GetOutermost()->IsDirty();
        for (int32 Stage = 1; Stage <= 4; ++Stage)
        {
            DasFailStructMigrationForTesting(Stage);
            const bool Failed = !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error);
            DasFailStructMigrationForTesting(0);
            Refresh();
            Check(Failed &&
                  Error.StartsWith(
                      TEXT("Struct migration rolled back:")) && Type->GetCustomGuid() == Guid &&
                      FindDasStructField(Type, TEXT("Count")) &&
                                         !FindDasStructField(
                                             Type, TEXT("Score")) &&
                                             Read(TEXT("Count")) == 24 && RunRead() == 24 &&
                                                  World->GetOutermost()->IsDirty() == Dirty &&
                                                  CheckDasNestedProbe(Actor, false),
                                                  TEXT("rollback_restores_layout_values_graph_and_dirty_flags"));
        }
        Check(FFileHelper::SaveStringToFile(
            After.Replace(TEXT("@ue_id=\"direct.Settings.Count\""), TEXT("@ue_id=\"wrong.score\"")),
                          *R.ScriptPath(Script), FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
            !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error) &&
            FindDasStructField(Type, TEXT("Count")), TEXT("changed_field_ID_refused"));
        Check(FFileHelper::SaveStringToFile(
            After.Replace(TEXT("Score : int = 15"), TEXT("Score : float = 15.0")), *R.ScriptPath(Script),
                              FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                          !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error),
            TEXT("field_type_change_refused"));
        Check(FFileHelper::SaveStringToFile(After, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("restore_valid_struct_candidate"));
        Check(FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error), TEXT("add_rename_success"));
        Refresh();
        auto* Config = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Config"));
        auto* Enabled = CastField<FBoolProperty>(FindDasStructField(Type, TEXT("Enabled")));
        Check(Type->GetCustomGuid() == Guid &&
              FStructureEditorUtils::GetVarDesc(Type).ContainsByPredicate(
                  [&](const auto& V)
                  {
                      return V.FriendlyName == TEXT("Score") && V.VarGuid == FieldGuid &&
                                                    V.MetaData.FindRef(TEXT("DasMemberId")) ==
                                                                       TEXT("direct.Settings.Count");
                  }) &&
              Read(TEXT("Score")) == 24 && RunRead() == 24 && Enabled &&
                   Enabled->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor)),
                   TEXT("stable_type_field_GUID_ID_old_value_and_new_default"));
        Check(CastFieldChecked<FStrProperty>(
            FindDasStructField(Type, TEXT("Title")))
                ->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor)) ==
            TEXT("Saved 世界") && CastFieldChecked<FIntProperty>(
                                      FindDasStructField(Type, TEXT("Score")))
                                              ->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(
                                                  BP->GeneratedClass->GetDefaultObject())) == 15,
                                      TEXT("Unicode_override_and_CDO_old_default_preserved"));
        auto* Make = Actor->FindFunction(TEXT("MakeConfig"));
        FStructOnScope A(Make);
        {
            FEditorScriptExecutionGuard G;
            Actor->ProcessEvent(Make, A.GetStructMemory());
        }
        auto* Ret = CastFieldChecked<FStructProperty>(Make->GetReturnProperty());
        Check(CastFieldChecked<FIntProperty>(
            FindDasStructField(Type, TEXT("Score")))
                    ->GetPropertyValue_InContainer(Ret->ContainerPtrToValuePtr<void>(A.GetStructMemory())) == 88,
            TEXT("MakeStruct_default_preserved_after_rename"));
        FDasScalarValue Amount;
        Amount.Kind = 0;
        Amount.IntValue = 3;
        const auto Bump = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_Bump"), {Amount},
                                                                               EDasMethodSignature::Int, Actor);
        Error = Bump.Error;
        Check(Bump.bSucceeded && Bump.Value == 27, TEXT("updated_direct_struct_DSL_callback"));
        R.ReleaseObject(Actor);
        Check(FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error),
              TEXT("struct_migration_idempotent"));
        if (Drop)
        {
            const FString Rules = FPaths::GetBaseFilename(Script) + TEXT("_values.json");
            auto WriteSource = [&](const FString& Text)
            {
                return FFileHelper::SaveStringToFile(Text, *R.ScriptPath(Script),
                                                     FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
            };
            auto WriteRule = [&](const FString& Id)
            {
                return FFileHelper::SaveStringToFile(TEXT("{\"schema_version\":1,\"rules\":[{\"field_id\":\"")+ Id +
                                                          TEXT("\",\"operation\":\"drop\"}]}"), *R.ScriptPath(Rules),
                                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
            };
            const FString Removed = After.Replace(
                TEXT("\r\n"),
                     TEXT("\n"))
                         .Replace(
                             TEXT("    @ue_property=(EditAnywhere, BlueprintReadWrite)\n    Enabled : bool = true\n"),
                                  TEXT(""));
            Check(Removed != After && WriteSource(Removed), TEXT("write_drop_candidate"));
            Check(!FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error) &&
                  FindDasStructField(Type, TEXT("Enabled")), TEXT("drop_without_rule_refused"));
            Check(WriteRule(TEXT("unknown.field")) &&
                            !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules),
                            TEXT("unknown_drop_rule_refused"));
            for (
                const auto& Invalid : TArray<FString>{TEXT("{\"schema_version\":2,\"rules\":[]}"),
                    TEXT("{\"schema_version\":1,\"rules\":[{\"field_id\":\"direct.Settings.Enabled\",\"operation\":\"archive\"}]}"),
                        TEXT("{\"schema_version\":1,\"rules\":[{\"field_id\":\"direct.Settings.Enabled\",\"operation\":\"drop\"},{\"field_id\":\"direct.Settings.Enabled\",\"operation\":\"drop\"}]}"),
                            TEXT("{\"schema_version\":1,\"rules\":[],\"ignore_links\":true}"),
                                 TEXT("{\"schema_version\":1,\"rules\":[null]}")})
                Check(FFileHelper::SaveStringToFile(Invalid, *R.ScriptPath(Rules),
                                                    FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                      !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
                      Type->GetCustomGuid() == Guid &&
                      Read(TEXT("Score")) == 27, TEXT("invalid_drop_schema_refused_before_mutation"));
            Check(!FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, TEXT("../rules.json")),
                                                        TEXT("nonlocal_drop_rules_refused"));
            Check(WriteRule(TEXT("direct.Settings.Enabled")), TEXT("write_explicit_drop_rule"));
            auto FindMakePin = [&](const TCHAR* Name) -> UEdGraphPin*
            {
                for (UEdGraph* G : BP->FunctionGraphs)
                    for (UEdGraphNode* N : G->Nodes)
                        if (auto* M = Cast<UK2Node_MakeStruct>(N); M && G->GetFName() == TEXT("MakeConfig"))
                            if (auto* P = FindDasStructField(Type, Name))
                                return M->FindPin(P->GetFName());
                return nullptr;
            };
            for (UEdGraph* G : BP->FunctionGraphs)
                for (UEdGraphNode* N : G->Nodes)
                    if (auto* M = Cast<UK2Node_MakeStruct>(N); M && G->GetFName() == TEXT("MakeConfig"))
                    {
                        for (auto& Optional : M->ShowPinForProperties)
                            if (Optional.PropertyName == FindDasStructField(Type, TEXT("Enabled"))->GetFName())
                                Optional.bShowPin = true;
                        M->ReconstructNode();
                    }
            FKismetEditorUtilities::CompileBlueprint(BP);
            Refresh();
            auto* RemovedPin = FindMakePin(TEXT("Enabled"));
            Check(RemovedPin != nullptr, TEXT("MakeStruct_added_pin_exists"));
            if (RemovedPin)
            {
                const FString Default = RemovedPin->DefaultValue;
                RemovedPin->DefaultValue = TEXT("false");
                Check(!FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
                      Error.Contains(TEXT("overridden")), TEXT("drop_overridden_MakeStruct_default_refused"));
                RemovedPin->DefaultValue = Default;
            }
            const FString RemovedTitle = After.Replace(
                TEXT("\r\n"),
                     TEXT("\n"))
                         .Replace(TEXT("    @ue_property=(EditAnywhere, BlueprintReadWrite)\n    Title : string = \"Initial\"\n"),
                             TEXT("")).Replace(TEXT("self.Config.Title = value"), TEXT("let ignored = value"));
            auto* TitlePin = FindMakePin(TEXT("Title"));
            Check(TitlePin != nullptr, TEXT("MakeStruct_Title_pin_exists"));
            if (TitlePin)
            {
                const FString Default = TitlePin->DefaultValue;
                TitlePin->DefaultValue = TEXT("");
                Check(WriteSource(RemovedTitle) &&
                      WriteRule(TEXT("direct.Settings.Title")) &&
                                !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
                                Error.Contains(TEXT("overridden")), TEXT("drop_empty_string_override_refused"));
                TitlePin->DefaultValue = Default;
            }
            const FString LinkedRemoval = After.Replace(
                TEXT("\r\n"),
                    TEXT("\n"))
                        .Replace(TEXT("    @ue_property=(EditAnywhere, BlueprintReadWrite)\n    @ue_id=\"direct.Settings.Count\"\n    Score : int = 15\n"),
                            TEXT(""))
                                .Replace(TEXT("result.Score += int(mode)"),
                                              TEXT("result.Title = \"checked\""))
                                                  .Replace(TEXT("self.Config.Score += amount"),
                                                                TEXT("let ignored = amount"))
                                                                    .Replace(TEXT("return self.Config.Score"),
                                                                                  TEXT("return 0"));
            Check(WriteSource(LinkedRemoval) &&
                  WriteRule(TEXT("direct.Settings.Count")) &&
                            !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
                            Error.Contains(TEXT("Blueprint links")), TEXT("drop_linked_BreakStruct_field_refused"));
            Check(WriteSource(Removed) && WriteRule(TEXT("direct.Settings.Enabled")), TEXT("restore_drop_candidate"));
            const bool BeforeDirty = World->GetOutermost()->IsDirty();
            for (int32 Stage = 1; Stage <= 4; ++Stage)
            {
                DasFailStructMigrationForTesting(Stage);
                const bool Failed =
                    !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules);
                DasFailStructMigrationForTesting(0);
                Refresh();
                Check(Failed &&
                      Error.StartsWith(
                          TEXT("Struct migration rolled back:")) &&
                          FindDasStructField(
                              Type, TEXT("Enabled")) &&
                              Read(TEXT("Score")) == 27 && RunRead() == 27 &&
                                   FindMakePin(TEXT("Enabled")) &&
                                               FindMakePin(TEXT("Enabled"))->DefaultValue.ToBool() &&
                                                           World->GetOutermost()->IsDirty() == BeforeDirty &&
                                                           CheckDasNestedProbe(Actor, true),
                                                           TEXT("drop_rollback_layout_values_graph_and_dirty_flags"));
            }
            const FName RemovedName = FindDasStructField(Type, TEXT("Enabled"))->GetFName();
            auto* Bool = CastFieldChecked<FBoolProperty>(FindDasStructField(Type, TEXT("Enabled")));
            Bool->SetPropertyValue_InContainer(FindFProperty<FStructProperty>(
                Actor->GetClass(), TEXT("Config"))->ContainerPtrToValuePtr<void>(Actor), false);
            Check(FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules),
                  TEXT("explicit_drop_success"));
            Refresh();
            Check(!FindDasStructField(Type, TEXT("Enabled")) && Type->GetCustomGuid() == Guid &&
                                      Read(TEXT("Score")) == 27 && RunRead() == 27,
                                           TEXT("drop_keeps_surviving_identity_values_and_graph"));
            bool NoOrphans = true;
            for (UEdGraph* G : BP->FunctionGraphs)
                for (UEdGraphNode* N : G->Nodes)
                    if (auto* Op = Cast<UK2Node_StructOperation>(N); Op && Op->StructType == Type)
                        for (auto* Pin : Op->Pins)
                            NoOrphans &= Pin && Pin->PinName != RemovedName && !Pin->bOrphanedPin;
            Check(NoOrphans, TEXT("drop_removes_obsolete_member_pins_without_orphans"));
            Check(!FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
                  Error.Contains(TEXT("Unused")), TEXT("stale_drop_rule_refused"));
        }
        if (Numeric)
        {
            Pass &= RunDasStructNumericProbe(Script, Registry, BP, World, true, R);
            Refresh();
        }
        Check(CheckDasArrayAccess(Script, Actor, R), TEXT("struct_array_token_access_after_migration"));
        Check(CheckDasNestedProbe(Actor, true), TEXT("nested_struct_values_after_migration"));
        for (const auto& B : Registry->Bindings)
            Check(SaveStructProbe(B.Type), TEXT("save_struct_model"));
        if (auto* Bundle = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Bundle")))
        {
            for (TFieldIterator<FStructProperty> P(Bundle->Struct); P; ++P)
                Check(SaveStructProbe(P->Struct), TEXT("save_nested_envelope"));
            Check(SaveStructProbe(Bundle->Struct), TEXT("save_nested_bundle"));
        }
        Check(SaveStructProbe(BP) && SaveStructProbe(World), TEXT("save_struct_blueprint_and_map"));
    }
    else
    {
        Check(CheckDasArrayAccess(Script, Actor, R), TEXT("reopened_struct_array_tokens"));
        Check(CheckDasNestedProbe(Actor, true), TEXT("reopened_nested_struct_data"));
        Check(FindDasStructField(Type, TEXT("Score")) &&
                                 (Drop                       ? !FindDasStructField(Type, TEXT("Enabled"))
                                                             : FindDasStructField(Type, TEXT("Enabled")) != nullptr) &&
                                                             Read(TEXT("Score")) == 27 && RunRead() == 27,
                                                                  TEXT("reopened_struct_values_and_BreakStruct"));
        R.ReleaseObject(Actor);
        Check(FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error),
              TEXT("reopened_struct_idempotent"));
    }
    return Pass;
}
