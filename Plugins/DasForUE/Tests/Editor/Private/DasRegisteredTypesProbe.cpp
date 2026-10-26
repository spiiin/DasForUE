#include "DasTypeService.h"
#include "DasTypeRegistry.h"
#include "DasTypeSyntaxProbe.h"
#include "DasDSLService.h"
#include "DasAuthoring.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "DasDefinedEnum.h"
#include "K2Node_SwitchEnum.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphSchema_K2.h"
#include "UObject/StructOnScope.h"
#include "Editor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraph/EdGraph.h"
void DasFailEnumAppendForTesting(int32 Stage);
void BuildDasEnumNodeProbes(UBlueprint*, UEnum*);
bool BuildDasNestedWrapper(const FString&, UScriptStruct*, FString&);
void SeedDasNestedProbe(ADasActor*);
void BuildDasStructFieldProbes(UBlueprint*, UScriptStruct*);
bool RunDasStructFieldProbe(const FString&, UDasTypeRegistry*, UBlueprint*, UWorld*, bool, FDasRuntimeModule&, bool);
bool RunDasEnumValuesProbe(const FString&, UDasTypeRegistry*, UBlueprint*, UWorld*, bool, FDasRuntimeModule&);
bool RunDasEnumRenameProbe(const FString&, UDasTypeRegistry*, UBlueprint*, UWorld*, bool, FDasRuntimeModule&);
bool RunDasEnumNodeProbes(ADasActor*, bool);
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "StructUtils/UserDefinedStruct.h"
#include "Modules/ModuleManager.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "HAL/FileManager.h"
namespace
{
UK2Node_SwitchEnum* BuildEnumRoute(UBlueprint* BP, UEnum* Enum)
{
    auto* G = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("RouteEnum"), UEdGraph::StaticClass(),
                                                             UEdGraphSchema_K2::StaticClass());
    FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, G, true, nullptr);
    TArray<UK2Node_FunctionEntry*> Entries;
    G->GetNodesOfClass(Entries);
    auto* Entry = Entries[0];
    FEdGraphPinType Input;
    Input.PinCategory = UEdGraphSchema_K2::PC_Byte;
    Input.PinSubCategoryObject = Enum;
    Entry->CreateUserDefinedPin(TEXT("Input"), Input, EGPD_Output);
    auto* Switch = NewObject<UK2Node_SwitchEnum>(G);
    G->AddNode(Switch);
    Switch->CreateNewGuid();
    Switch->SetEnum(Enum);
    Switch->AllocateDefaultPins();
    G->GetSchema()->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),
                                        Switch->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
    G->GetSchema()->TryCreateConnection(Entry->FindPinChecked(TEXT("Input")),
                                                              Switch->FindPinChecked(TEXT("Selection")));
    FEdGraphPinType Int;
    Int.PinCategory = UEdGraphSchema_K2::PC_Int;
    for (const auto& Pair : TArray<TPair<FName, int32>>{{TEXT("Idle"), 70}, {TEXT("Active"), 420}})
    {
        auto* Out = NewObject<UK2Node_FunctionResult>(G);
        G->AddNode(Out);
        Out->CreateNewGuid();
        Out->FunctionReference = Entry->FunctionReference;
        Out->AllocateDefaultPins();
        Out->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue, Int, EGPD_Input);
        Out->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue)->DefaultValue = FString::FromInt(Pair.Value);
        G->GetSchema()->TryCreateConnection(Switch->FindPinChecked(Pair.Key),
                                            Out->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
    }
    FKismetEditorUtilities::CompileBlueprint(BP);
    return Switch;
}
UK2Node_SwitchEnum* RouteSwitch(UBlueprint* BP)
{
    for (UEdGraph* G : BP->FunctionGraphs)
        if (G->GetFName() == TEXT("RouteEnum"))
            for (UEdGraphNode* N : G->Nodes)
                if (auto* S = Cast<UK2Node_SwitchEnum>(N))
                    return S;
    return nullptr;
}
int32 RouteResult(ADasActor* A, uint8 Value)
{
    auto* F = A->FindFunction(TEXT("RouteEnum"));
    if (!F)
        return -1;
    FStructOnScope P(F);
    FindFProperty<FByteProperty>(F, TEXT("Input"))->SetPropertyValue_InContainer(P.GetStructMemory(), Value);
    {
        FEditorScriptExecutionGuard G;
        A->ProcessEvent(F, P.GetStructMemory());
    }
    return CastFieldChecked<FIntProperty>(F->GetReturnProperty())->GetPropertyValue_InContainer(P.GetStructMemory());
}
bool SaveRegistered(UObject* O)
{
    const FString Path = FPackageName::LongPackageNameToFilename(
        O->GetOutermost()->GetName(),
        O->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Path), true);
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    Args.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(O->GetOutermost(), O, *Path, Args);
}
}
int32 RunDasTypedObjectProbe(const FString&, const FString&);
int32 RunDasAuthoredInterfaceProbe(const FString&, const FString&);
int32 RunDasAuthoredNestedProbe(const FString&, const FString&);
int32 RunDasRegisteredTypesProbe(const FString& Params)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
        return 1;
    FString Id, Phase, Error;
    if (!FParse::Value(*Params, TEXT("ProbeId="), Id) || Id.Len() != 32 ||
                       !FParse::Value(*Params, TEXT("Phase="), Phase))
        return 1;
    for (TCHAR C : Id)
        if (!FChar::IsHexDigit(C))
            return 1;
    if (FParse::Param(*Params, TEXT("TypedObjects")))
        return RunDasTypedObjectProbe(Id, Phase);
    if (FParse::Param(*Params, TEXT("AuthoredInterface")))
        return RunDasAuthoredInterfaceProbe(Id, Phase);
    if (FParse::Param(*Params, TEXT("AuthoredNested")))
        return RunDasAuthoredNestedProbe(Id, Phase);
    const bool Append = FParse::Param(
                   *Params, TEXT("EnumAppend")),
                   RepairGraphs = FParse::Param(
                       *Params, TEXT("EnumGraphs")),
                       EnumNodes = FParse::Param(
                           *Params, TEXT("EnumNodes")),
                           RenameMembers = FParse::Param(
                               *Params, TEXT("EnumRename")),
                               ValuePolicy = FParse::Param(
                                   *Params, TEXT("EnumValues")),
                                   StructFields = FParse::Param(*Params, TEXT("StructFields")),
                                                                StructDrop = FParse::Param(*Params, TEXT("StructDrop"));
    const FString Base = TEXT("/Game/DasFrameAssetProbe/Registered_")+ Id,
                              Script = TEXT("dsl_registered_")+ Id +
                                            TEXT(".das"), RegistryPath = Base + TEXT("/TypesRegistry.TypesRegistry");
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    bool Passed = true;
    auto Check = [&](bool V, const TCHAR* Name)
    {
        Passed &= V;
        UE_LOG(LogTemp, Display, TEXT("DAS_REGISTERED_TYPES %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), Name);
    };
    UDasTypeRegistry* Registry = nullptr;
    UBlueprint* BP = nullptr;
    UWorld* World = nullptr;
    if (Phase == TEXT("Create"))
    {
        TArray<UObject*> Types;
        Check(FDasTypeService::Apply(TEXT("dsl_direct_types.das"), Base + TEXT("/Types"), Types, Error),
                                     TEXT("create_reflected_types"));
        if (Types.Num() != 2)
            return 1;
        Check(FDasTypeService::BuildRegistry(Types, Base + TEXT("/TypesRegistry"), Registry, Error),
                                             TEXT("create_runtime_ID_registry"));
        if (!Registry)
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }
        UDasTypeRegistry* Reused = nullptr;
        Check(FDasTypeService::BuildRegistry(Types, Base + TEXT("/TypesRegistry"), Reused, Error) && Reused == Registry,
                                             TEXT("idempotent_registry_identity"));
        FString Source;
        FFileHelper::LoadFileToString(Source, *R.ScriptPath(FParse::Param(*Params, TEXT("TypeSyntax"))
                                                                          ? TEXT("dsl_type_syntax_registered.das")
                                                                          : TEXT("dsl_registered_types.das")));
        Source = Source.Replace(TEXT("/Game/DasRegisteredTypes/TypesRegistry.TypesRegistry"), *RegistryPath);
        if (RenameMembers)
            Source =
                Source.Replace(TEXT("Mode : PlayState = PlayState.Idle"), TEXT("Mode : PlayState = PlayState.Active"));
        if (FParse::Param(*Params, TEXT("StructNested")))
            Check(BuildDasNestedWrapper(Base, CastChecked<UScriptStruct>(Types[1]), Source),
                  TEXT("build_nested_struct_wrapper"));
        if (FParse::Param(*Params, TEXT("StructArray")))
            Source = Source.Replace(TEXT("class TypedActor {"),
                TEXT("class TypedActor {\n    [ue_function(BlueprintCallable)]\n    def ArrayRead():int { return ue_struct_get_int(self.Bundle, \"Items.0.Inner.Count\") }\n    [ue_function(BlueprintCallable)]\n    def ArrayWrite(value:int):void { self.Bundle = ue_struct_with_int(self.Bundle, \"Items.0.Inner.Count\", value) }\n    [ue_function(BlueprintCallable)]\n    def ArrayBad():int { return ue_struct_get_int(self.Bundle, \"Items.99.Inner.Count\") }"));
        Check(FFileHelper::SaveStringToFile(Source, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("create_bound_source"));
        Check(FDasDSLService::Apply(Script, Base + TEXT("/BP_Actor"), TEXT("authoredvalues.TypedActor"), BP, Error),
                                                TEXT("compile_alias_names_through_stable_ID"));
        if (!BP)
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }
        if (RepairGraphs)
            BuildEnumRoute(BP, CastChecked<UEnum>(Registry->Bindings[0].Type));
        if (EnumNodes)
            BuildDasEnumNodeProbes(BP, CastChecked<UEnum>(Registry->Bindings[0].Type));
        if (RenameMembers)
        {
            for (UEdGraph* G : BP->FunctionGraphs)
                if (G->GetFName() == TEXT("EnumSelect"))
                {
                    TArray<UK2Node_FunctionEntry*> Entries;
                    G->GetNodesOfClass(Entries);
                    Entries[0]->FindPinChecked(TEXT("Input"))->DefaultValue = TEXT("Active");
                }
            FKismetEditorUtilities::CompileBlueprint(BP);
        }
        if (StructFields)
            BuildDasStructFieldProbes(BP, CastChecked<UScriptStruct>(Registry->Bindings[1].Type));
        World = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Base + TEXT("/Map"))));
        World->SetFlags(RF_Public | RF_Standalone);
        World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        FActorSpawnParameters P;
        P.Name = TEXT("TypedActor");
        auto* Actor = World->SpawnActor<ADasActor>(BP->GeneratedClass, FTransform::Identity, P);
        if (!Actor)
            return 1;
        Actor->TypeRegistry = Registry;
        if (FParse::Param(*Params, TEXT("StructNested")))
            SeedDasNestedProbe(Actor);
        auto* Mode = FindFProperty<FByteProperty>(Actor->GetClass(), TEXT("Mode"));
        auto* Config = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Config"));
        Mode->SetPropertyValue_InContainer(Actor, 42);
        CastFieldChecked<FIntProperty>(FindDasStructField(Config->Struct, TEXT("Count")))
                                           ->SetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor),
                                                                          21);
        CastFieldChecked<FStrProperty>(FindDasStructField(Config->Struct, TEXT("Title")))
                                           ->SetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor),
                                                                          TEXT("Saved 世界"));
        auto* Bad = NewObject<UDasTypeRegistry>();
        Bad->Bindings = Registry->Bindings;
        const FDasTypeBinding Duplicate = Bad->Bindings[0];
        Bad->Bindings.Add(Duplicate);
        Check(!Bad->Resolve(TEXT("direct.State"), true, Error), TEXT("duplicate_ID_refused"));
        Bad->Bindings = Registry->Bindings;
        Bad->Bindings[0].bEnum = false;
        Check(!Bad->Validate(Error), TEXT("wrong_asset_kind_refused"));
        Bad->Bindings = Registry->Bindings;
        Bad->Bindings[1].StructGuid = FGuid::NewGuid();
        Check(!Bad->Validate(Error), TEXT("wrong_struct_GUID_refused"));
        Check(!Registry->Resolve(TEXT("unknown.Type"), false, Error), TEXT("missing_ID_refused"));
        FString Invalid = Source.Replace(TEXT("id=\"direct.State\""), TEXT("id=\"missing.State\""));
        FDasAuthoringModule Model;
        Check(FFileHelper::SaveStringToFile(Invalid, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
              !R.ReadAuthoringScript(R.ScriptPath(Script), Model, Error) &&
              Error.Contains(TEXT("Stable type ID is not registered")), TEXT("unknown_source_ID_compile_refused"));
        Check(FFileHelper::SaveStringToFile(Source, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("restore_source"));
        if (FParse::Param(*Params, TEXT("TypeSyntax")))
        {
            const FString EnumConflict = Source.Replace(TEXT("arg_mode_enum=PlayState"),
                                                             TEXT("arg_mode_enum=\"/Script/DasRuntime.EDasCallMode\""));
            Check(FFileHelper::SaveStringToFile(EnumConflict, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                  !R.ReadAuthoringScript(R.ScriptPath(Script), Model, Error) &&
                  Error.Contains(TEXT("Conflicting type constraint")),
                                 TEXT("direct_enum_conflicting_annotation_refused"));
            const FString StructConflict = Source.Replace(TEXT("arg_data_struct=GameSettings"),
                                                               TEXT("arg_data_struct=\"/Script/CoreUObject.Vector\""));
            Check(FFileHelper::SaveStringToFile(StructConflict, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                  !R.ReadAuthoringScript(R.ScriptPath(Script), Model, Error) &&
                  Error.Contains(TEXT("Conflicting type constraint")),
                                 TEXT("direct_struct_conflicting_annotation_refused"));
            const FString Missing =
                Source.Replace(TEXT("arg_value_struct=GameSettings"), TEXT("arg_value_struct=MissingType"));
            Check(FFileHelper::SaveStringToFile(Missing, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                  !R.ReadAuthoringScript(R.ScriptPath(Script), Model, Error) &&
                  Error.Contains(TEXT("not registered")), TEXT("registered_short_name_has_no_global_fallback"));
            const FString Kind =
                Source.Replace(TEXT("return_struct=\"direct.Settings\""), TEXT("return_struct=\"direct.State\""));
            Check(FFileHelper::SaveStringToFile(Kind, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                  !R.ReadAuthoringScript(R.ScriptPath(Script), Model, Error) &&
                  Error.Contains(TEXT("kind mismatch")), TEXT("registered_identity_wrong_kind_refused"));
            const auto SavedBindings = Registry->Bindings;
            auto* SameName = NewObject<UDasDefinedEnum>(CreatePackage(*(Base + TEXT("/Other/State"))), TEXT("State"));
            SameName->StableId = TEXT("other.State");
            FDasTypeBinding Collision;
            Collision.Id = SameName->StableId;
            Collision.bEnum = true;
            Collision.Type = SameName;
            Registry->Bindings.Add(Collision);
            const FString Ambiguous =
                Source.Replace(TEXT("arg_value_enum=\"direct.State\""), TEXT("arg_value_enum=State"));
            Check(FFileHelper::SaveStringToFile(Ambiguous, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                  !R.ReadAuthoringScript(R.ScriptPath(Script), Model, Error) &&
                  Error.Contains(TEXT("Ambiguous registered")), TEXT("ambiguous_registered_short_name_refused"));
            Registry->Bindings = SavedBindings;
            Check(FFileHelper::SaveStringToFile(Source, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                      R.ReadAuthoringScript(R.ScriptPath(Script), Model, Error),
                  TEXT("restore_registered_names_source"));
        }
        for (auto* Type : Types)
            Check(SaveRegistered(Type), TEXT("save_type"));
        Check(SaveRegistered(Registry) && SaveRegistered(BP) && SaveRegistered(World),
              TEXT("save_registry_blueprint_and_map"));
    }
    else if (Phase == TEXT("Reopen"))
    {
        Registry = LoadObject<UDasTypeRegistry>(nullptr, *RegistryPath);
        BP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("/BP_Actor.BP_Actor")));
        World = LoadObject<UWorld>(nullptr, *(Base + TEXT("/Map.Map")));
        Check(Registry && BP && World, TEXT("reopen_saved_registry_blueprint_and_map"));
        if (!Registry || !BP || !World)
            return 1;
        FDasAuthoringModule Model;
        Check(R.ReadAuthoringScript(R.ScriptPath(Script), Model, Error) && Model.Classes.Num() == 1 &&
                  Model.Classes[0].Properties.ContainsByPredicate(
                      [&](const auto& P)
                      { return P.Name == TEXT("Mode") && P.Type.TypePath == Base + TEXT("/Types/State.State"); }),
              TEXT("reopened_ID_resolution"));
        Check(FDasDSLService::Reimport(BP, Error), TEXT("reopened_ID_class_reimport"));
    }
    else
        return 1;
    if (StructFields && Phase == TEXT("Reopen"))
    {
        Passed &= RunDasStructFieldProbe(Script, Registry, BP, World, false, R, StructDrop);
        UE_LOG(LogTemp, Display, TEXT("DAS_REGISTERED_TYPES_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
        return Passed ? 0 : 1;
    }
    if (ValuePolicy && Phase == TEXT("Reopen"))
    {
        Passed &= RunDasEnumValuesProbe(Script, Registry, BP, World, false, R);
        UE_LOG(LogTemp, Display, TEXT("DAS_REGISTERED_TYPES_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
        return Passed ? 0 : 1;
    }
    if (FParse::Param(*Params, TEXT("TypeSyntax")))
    {
        FDasAuthoringModule Names;
        Check(R.ReadAuthoringScript(R.ScriptPath(Script), Names, Error) &&
                  Names.Classes[0].Methods.ContainsByPredicate(
                      [&](const auto& M)
                      {
                          return M.Name == TEXT("RawConfig") &&
                                                M.ReturnType.TypePath == Base + TEXT("/Types/Settings.Settings");
                      }),
              TEXT("registered_aliases_normalize_to_same_native_identity"));
    }
    TStrongObjectPtr<UDasTypeRegistry> KeepRegistry(Registry);
    TStrongObjectPtr<UWorld> KeepWorld(World);
    Check(Registry->Validate(Error), TEXT("registry_valid"));
    auto* Enum = Cast<UDasDefinedEnum>(Registry->Resolve(TEXT("direct.State"), true, Error));
    auto* Struct = Cast<UScriptStruct>(Registry->Resolve(TEXT("direct.Settings"), false, Error));
    Check(Enum && Enum->StableId == TEXT("direct.State") && Struct &&
                                         Enum->GetName() ==
                                             TEXT("State") && Struct->GetName() ==
                                                                  TEXT("Settings"),
                                                                       TEXT("IDs_map_alias_names_to_original_assets"));
    ADasActor* Actor = nullptr;
    for (auto& V : World->PersistentLevel->Actors)
        if (V && V->GetFName() == TEXT("TypedActor"))
            Actor = Cast<ADasActor>(V.Get());
    if (!Actor)
        return 1;
    Actor->TypeRegistry = Registry;
    if (FParse::Param(*Params, TEXT("TypeSyntax")))
        RunDasTypeSyntaxValues(R, Actor, Script, Enum, Struct, Check);
    auto* Mode = FindFProperty<FByteProperty>(Actor->GetClass(), TEXT("Mode"));
    auto* Config = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Config"));
    Check(Mode->Enum == Enum && Config->Struct == Struct && Mode->GetPropertyValue_InContainer(Actor) == 42 &&
          CastFieldChecked<FIntProperty>(
              FindDasStructField(Struct, TEXT("Count")))
                      ->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor)) == 21,
              TEXT("saved_type_identity_and_values"));
    FDasScalarValue Arg;
    Arg.Kind = 0;
    Arg.IntValue = 3;
    auto Bump = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_Bump"), {Arg},
                                                                     EDasMethodSignature::Int, Actor);
    Check(Bump.bSucceeded && Bump.Value == 24, TEXT("registered_direct_method_executes"));
    if (!Bump.bSucceeded)
        UE_LOG(LogTemp, Error, TEXT("%s"), *Bump.Error);
    CollectGarbage(RF_NoFlags);
    Check(Registry->Resolve(TEXT("direct.Settings"), false, Error) == Struct,
                            TEXT("registry_hard_references_survive_GC"));
    if (Append)
    {
        if (Phase == TEXT("Create"))
        {
            FString Before;
            FFileHelper::LoadFileToString(Before, *R.ScriptPath(Script));
            const FString After = Before.Replace(TEXT("Active = 42"), TEXT("Active = 42\n    Paused = 200"));
            Check(FFileHelper::SaveStringToFile(After, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_enum_addition"));
            Check(!FDasTypeService::ExtendEnums(Script, RegistryPath, Error) &&
                  Error.Contains(TEXT("no script contexts")), TEXT("append_active_context_refused"));
            R.ReleaseObject(Actor);
            auto CompileProbe = [&]()
            {
                FKismetEditorUtilities::CompileBlueprint(BP);
                for (auto& V : World->PersistentLevel->Actors)
                    if (V && V->GetFName() == TEXT("TypedActor"))
                        Actor = Cast<ADasActor>(V.Get());
                Mode = FindFProperty<FByteProperty>(Actor->GetClass(), TEXT("Mode"));
                Config = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Config"));
            };
            CompileProbe();
            Mode->SetPropertyValue_InContainer(Actor, 43);
            Check(!FDasTypeService::ExtendEnums(Script, RegistryPath, Error, RepairGraphs) &&
                  Error.Contains(TEXT("unknown/reserved")) && Enum->NumEnums() == 3,
                                 TEXT("append_old_MAX_data_refused"));
            Mode->SetPropertyValue_InContainer(Actor, 42);
            auto* Switch = NewObject<UK2Node_SwitchEnum>(BP->UbergraphPages[0]);
            BP->UbergraphPages[0]->AddNode(Switch, false, false);
            Switch->SetEnum(Enum);
            Switch->AllocateDefaultPins();
            Check(!FDasTypeService::ExtendEnums(Script, RegistryPath, Error) &&
                  Error.Contains(TEXT("node reconstruction")) && Enum->NumEnums() == 3,
                                 TEXT("append_switch_node_refused_before_change"));
            BP->UbergraphPages[0]->RemoveNode(Switch);
            CompileProbe();
            if (EnumNodes)
                Check(RunDasEnumNodeProbes(Actor, false), TEXT("enum_nodes_before_append_sparse_semantics"));
            const bool EnumDirty = Enum->GetOutermost()->IsDirty(), StructDirty = Struct->GetOutermost()->IsDirty();
            const FString Wire = Enum->GetOutermost()->GetMetaData().GetValue(Enum, TEXT("DasTypesModel"));
            for (int32 Stage = 1; Stage <= 2; ++Stage)
            {
                DasFailEnumAppendForTesting(Stage);
                const bool Failed = !FDasTypeService::ExtendEnums(Script, RegistryPath, Error, RepairGraphs);
                DasFailEnumAppendForTesting(0);
                for (auto& V : World->PersistentLevel->Actors)
                    if (V && V->GetFName() == TEXT("TypedActor"))
                        Actor = Cast<ADasActor>(V.Get());
                Mode = FindFProperty<FByteProperty>(Actor->GetClass(), TEXT("Mode"));
                Config = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Config"));
                Check(Failed &&
                      Error.StartsWith(TEXT("Enum append rolled back:")) && Enum->NumEnums() == 3 &&
                                       Enum->GetValueByNameString(
                                           TEXT("Active")) == 42 &&
                                           Enum->GetOutermost()->GetMetaData().GetValue(
                                               Enum, TEXT("DasTypesModel")) == Wire &&
                                               Enum->GetOutermost()->IsDirty() == EnumDirty &&
                                               Struct->GetOutermost()->IsDirty() == StructDirty &&
                                               Mode->GetPropertyValue_InContainer(Actor) == 42,
                                               TEXT("enum_and_model_failure_rollback_preserves_data_dirty_flags"));
            }
            Check(FFileHelper::SaveStringToFile(After.Replace(
                RenameMembers                                ? TEXT("PlayState.Active")
                                                             : TEXT("PlayState.Idle"), RenameMembers
                                     ? TEXT("PlayState.Idle"): TEXT("PlayState.Active")), *R.ScriptPath(Script),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                !FDasTypeService::ExtendEnums(Script, RegistryPath, Error, RepairGraphs) &&
                Error.Contains(TEXT("Class declarations must remain unchanged")) && Enum->NumEnums() == 3,
                               TEXT("append_class_schema_change_rolls_back"));
            Check(FFileHelper::SaveStringToFile(After, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("restore_append_candidate"));
            auto RefreshActor = [&]()
            {
                for (auto& V : World->PersistentLevel->Actors)
                    if (V && V->GetFName() == TEXT("TypedActor"))
                        Actor = Cast<ADasActor>(V.Get());
                Mode = FindFProperty<FByteProperty>(Actor->GetClass(), TEXT("Mode"));
                Config = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Config"));
            };
            RefreshActor();
            if (RepairGraphs)
            {
                for (int32 Stage = 3; Stage <= 4; ++Stage)
                {
                    DasFailEnumAppendForTesting(Stage);
                    const bool Failed = !FDasTypeService::ExtendEnums(Script, RegistryPath, Error, true);
                    DasFailEnumAppendForTesting(0);
                    RefreshActor();
                    auto* Node = RouteSwitch(BP);
                    Check(Failed &&
                          Error.StartsWith(
                              TEXT("Enum append rolled back:")) && Enum->NumEnums() == 3 && Node &&
                              !Node->FindPin(
                                  TEXT("Paused")) &&
                                  Node->FindPinChecked(
                                      TEXT("Active"))->LinkedTo.Num() == 1 && RouteResult(Actor, 42) == 420 &&
                                      Mode->GetPropertyValue_InContainer(Actor) == 42,
                                      TEXT("switch_reconstruction_compile_rollback_preserves_links_and_values"));
                    if (EnumNodes)
                        Check(RunDasEnumNodeProbes(Actor, false),
                              TEXT("all_enum_node_semantics_restored_after_rollback"));
                }
            }
            Check(FDasTypeService::ExtendEnums(Script, RegistryPath, Error, RepairGraphs), TEXT("append_success"));
            RefreshActor();
            if (!Error.IsEmpty())
                UE_LOG(LogTemp, Display, TEXT("Append detail: %s"), *Error);
            Check(Enum->NumEnums() == 4 &&
                  Enum->GetValueByNameString(
                      TEXT("Paused")) == 200 &&
                      Registry->Resolve(
                          TEXT("direct.State"), true, Error) == Enum &&
                          Mode->GetPropertyValue_InContainer(Actor) == 42 &&
                          CastFieldChecked<FIntProperty>(
                              FindDasStructField(Struct, TEXT("Count")))
                                      ->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor)) == 24,
                              TEXT("append_preserves_asset_identity_and_existing_data"));
            Check(FDasTypeService::ExtendEnums(Script, RegistryPath, Error), TEXT("append_idempotent"));
            if (RepairGraphs)
            {
                auto* Node = RouteSwitch(BP);
                Check(Node &&
                      Node->FindPin(TEXT("Paused")) &&
                                    Node->FindPinChecked(TEXT("Active"))->LinkedTo.Num() == 1 &&
                                                         RouteResult(Actor, 7) == 70 && RouteResult(Actor, 42) == 420,
                                                         TEXT("new_switch_pin_old_links_and_executable_branches"));
                Check(SaveRegistered(BP), TEXT("save_reconstructed_switch_graph"));
            }
            Check(Enum->MemberIds.FindRef(7) == TEXT("direct.State.Idle") &&
                                                     Enum->MemberIds.FindRef(42) ==
                                                         TEXT("direct.State.Active") &&
                                                              Enum->MemberIds.FindRef(200) ==
                                                                  TEXT("direct.State.Paused"),
                                                                       TEXT("persistent_enum_member_IDs"));
            FString IDSource =
                After.Replace(TEXT("ue_enum(id=\"direct.State\","),
                                   TEXT("ue_enum(member_id_Active=\"other.active\", id=\"direct.State\","));
            FDasAuthoringModule IDModel;
            Check(FFileHelper::SaveStringToFile(IDSource, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                  !R.ReadAuthoringScript(R.ScriptPath(Script), IDModel, Error) &&
                  Error.Contains(TEXT("member identity differs")), TEXT("member_ID_drift_rejected_by_direct_adapter"));
            Check(FFileHelper::SaveStringToFile(After, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("restore_member_ID_source"));
            if (EnumNodes)
            {
                Check(RunDasEnumNodeProbes(Actor, true), TEXT("enum_nodes_after_append_count_literal_cast_select_sum"));
                Check(SaveRegistered(BP), TEXT("save_all_reconstructed_enum_nodes"));
            }
            FDasScalarValue New;
            New.Kind = 13;
            New.IntValue = 200;
            New.ObjectClass = Enum->GetPathName();
            Check(R.RunNamedFramePersistent(
                R.ScriptPath(Script),
                TEXT("ue_TypedActor_SetMode"), {New}, EDasMethodSignature::Void, Actor).bSucceeded &&
                Mode->GetPropertyValue_InContainer(Actor) == 200,
                TEXT("new_member_direct_DSL_callback"));
            R.ReleaseObject(Actor);
            Mode->SetPropertyValue_InContainer(Actor, 42);
            Check(FFileHelper::SaveStringToFile(
                After.Replace(TEXT("Active = 42"), TEXT("Active = 41")), *R.ScriptPath(Script),
                              FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                !FDasTypeService::ExtendEnums(Script, RegistryPath, Error) &&
                Enum->GetValueByNameString(TEXT("Active")) == 42, TEXT("append_renumber_refused"));
            Check(FFileHelper::SaveStringToFile(After, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("restore_migrated_source"));
            for (const auto& Binding : Registry->Bindings)
                Check(SaveRegistered(Binding.Type), TEXT("save_migrated_type_models"));
        }
        else
        {
            if (EnumNodes)
                Check(RunDasEnumNodeProbes(Actor, true), TEXT("reopened_enum_nodes_sparse_semantics"));
            if (RepairGraphs)
            {
                auto* Node = RouteSwitch(BP);
                Check(Node && Node->FindPin(TEXT("Paused")) && RouteResult(Actor, 42) == 420,
                                            TEXT("reopened_switch_graph_links_and_execution"));
            }
            Check(Enum->MemberIds.FindRef(42) == TEXT("direct.State.Active"), TEXT("reopened_enum_member_IDs"));
            Check(Enum->NumEnums() == 4 &&
                  Enum->GetValueByNameString(TEXT("Paused")) == 200 && Mode->GetPropertyValue_InContainer(Actor) == 42,
                                             TEXT("saved_append_reopened_with_old_map_values"));
            R.ReleaseObject(Actor);
            Check(FDasTypeService::ExtendEnums(Script, RegistryPath, Error), TEXT("reopened_append_idempotent"));
        }
    }
    if (RenameMembers)
        Passed &= RunDasEnumRenameProbe(Script, Registry, BP, World, Phase == TEXT("Create"), R);
    if (ValuePolicy)
        Passed &= RunDasEnumValuesProbe(Script, Registry, BP, World, Phase == TEXT("Create"), R);
    if (StructFields)
        Passed &= RunDasStructFieldProbe(Script, Registry, BP, World, Phase == TEXT("Create"), R, StructDrop);
    if (World->IsInitialized())
        World->DestroyWorld(false);
    UE_LOG(LogTemp, Display, TEXT("DAS_REGISTERED_TYPES_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
    return Passed ? 0 : 1;
}
