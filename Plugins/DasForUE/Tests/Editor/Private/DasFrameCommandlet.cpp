#include "DasFrameCommandlet.h"
#include "DasDSLService.h"
#include "DasBlueprintService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "BlueprintCompilationManager.h"
#include "K2Node_VariableGet.h"
#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "StructUtils/InstancedStruct.h"
#include "DasActor.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
#include "UObject/SavePackage.h"
#include "UObject/MetaData.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/NoExportTypes.h"
UDasFrameCommandlet::UDasFrameCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 RunDasRegisteredTypesProbe(const FString& Params);
int32 RunDasAuthoredValuesProbe(const FString& Params);
int32 RunDasTypeProbe(const FString& Params);
int32 RunDasObjectFieldsProbe(const FString& Params);
int32 RunDasStructuralRollbackProbe(const FString& Params);
int32 RunDasArchiveProbe(const FString& Params);
int32 RunDasNumericConvertProbe(const FString& Params);
int32 RunDasNumericWidenProbe(const FString& Params);
int32 RunDasRenameScopeProbe(const FString& Params);
int32 UDasFrameCommandlet::Main(const FString& Params)
{
    if (FParse::Param(*Params, TEXT("RegisteredTypes")))
        return RunDasRegisteredTypesProbe(Params);
    if (FParse::Param(*Params, TEXT("AuthoredValues")))
        return RunDasAuthoredValuesProbe(Params);
    if (FParse::Param(*Params, TEXT("TypeDeclarations")))
        return RunDasTypeProbe(Params);
    if (FParse::Param(*Params, TEXT("ObjectFields")))
        return RunDasObjectFieldsProbe(Params);
    if (FParse::Param(*Params, TEXT("StructuralRollback")))
        return RunDasStructuralRollbackProbe(Params);
    if (FParse::Param(*Params, TEXT("ArchiveFields")))
        return RunDasArchiveProbe(Params);
    if (FParse::Param(*Params, TEXT("NumericConvert")))
        return RunDasNumericConvertProbe(Params);
    if (FParse::Param(*Params, TEXT("NumericWiden")))
        return RunDasNumericWidenProbe(Params);
    if (FParse::Param(*Params, TEXT("RenameScope")))
        return RunDasRenameScopeProbe(Params);
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
        return 1;
    FString Id, Phase, Error;
    if (!FParse::Value(*Params, TEXT("ProbeId="), Id) || Id.Len() != 32 ||
                       !FParse::Value(*Params, TEXT("Phase="), Phase))
        return 1;
    for (TCHAR Ch : Id)
        if (!FChar::IsHexDigit(Ch))
            return 1;
    const bool RenameMode = FParse::Param(*Params, TEXT("FieldRename"));
    const bool MigrationMode = FParse::Param(*Params, TEXT("DefaultMigration"));
    const bool FieldMode = FParse::Param(*Params, TEXT("StructFields"));
    const bool EnumMode = FParse::Param(*Params, TEXT("Enums"));
    const bool StructMode = FParse::Param(*Params, TEXT("Struct"));
    const bool MathMode = FParse::Param(*Params, TEXT("Math"));
    const bool ObjectMode = FParse::Param(*Params, TEXT("Objects"));
    const bool TextMode = FParse::Param(*Params, TEXT("Text"));
    const FString Script = RenameMode ? TEXT("dsl_renamefields_")+ Id +
                                             TEXT(".das")
                                             : MigrationMode
                                             ? TEXT("dsl_migration_")+ Id +
                                                    TEXT(".das")
                                                    : FieldMode
                                                    ? TEXT("dsl_struct_fields.das")
                                                    : EnumMode
                                                    ? TEXT("dsl_struct_enum_methods.das")
                                                    : StructMode
                                                    ? TEXT("dsl_reflected_methods.das")
                                                    : MathMode
                                                    ? TEXT("dsl_math_methods.das")
                                                    : ObjectMode
                                                    ? TEXT("dsl_object_methods.das")
                                                    : TextMode
                                                    ? TEXT("dsl_text_methods.das")
                                                    : TEXT("dsl_mixed_methods.das");
    const FString ClassId = RenameMode              ? TEXT("rename.RenameFields")
                                                    : (FieldMode || MigrationMode)
        ? TEXT("structfields.StructFields")
        : EnumMode
        ? TEXT("enumvalues.EnumStructBridge")
        : StructMode
        ? TEXT("reflected.StructBridge")
        : MathMode
        ? TEXT("math.MathBridge")
        : ObjectMode
        ? TEXT("objects.ObjectBridge")
        : TextMode
        ? TEXT("text.TextCounter")
        : TEXT("mixed.MixedCounter");
    const FString Package = TEXT("/Game/DasFrameAssetProbe/Run_")+ Id + TEXT("/BP_Mixed");
    UBlueprint* BP = nullptr;
    bool Passed = true;
    auto Check = [&](bool V, const TCHAR* N)
    {
        Passed &= V;
        UE_LOG(LogTemp, Display, TEXT("DAS_FRAME_ASSET %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
    };
    if (RenameMode && Phase == TEXT("Create"))
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        FString Source;
        if (!FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_rename_fields.das"))) ||
                                           !FFileHelper::SaveStringToFile(
                                               Source, *Runtime.ScriptPath(Script),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return 1;
    }
    if (MigrationMode && Phase == TEXT("Create"))
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        FString Source;
        if (!FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_struct_fields.das"))))
            return 1;
        Source = Source.Replace(TEXT("class StructFields {"),
            TEXT("class StructFields {\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Score : int = 5\n"));
        if (!FFileHelper::SaveStringToFile(Source, *Runtime.ScriptPath(Script),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return 1;
    }
    if (Phase == TEXT("Create"))
        Check(FDasDSLService::Apply(Script, Package, ClassId, BP, Error), TEXT("create_frame_graphs"));
    else if (Phase == TEXT("Reopen"))
    {
        BP = LoadObject<UBlueprint>(nullptr, *(Package + TEXT(".BP_Mixed")));
        Check(BP && BP->GeneratedClass, TEXT("reopen_frame_graphs"));
    }
    else
        return 1;
    if (!Passed || !BP)
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Editor, false));
    FActorSpawnParameters Spawn;
    auto* Actor = World->SpawnActor<ADasActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    Check(Actor != nullptr, TEXT("spawn_saved_frame_class"));
    if (!Actor)
        return 1;
    if (RenameMode)
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        const FString Path = Runtime.ScriptPath(Script);
        FString Original;
        Check(FFileHelper::LoadFileToString(Original, *Path), TEXT("rename_source_available"));
        auto ScoreOf = [](UObject* Object, FName Name)
        {
            auto* Property = Object ? FindFProperty<FIntProperty>(Object->GetClass(), Name) : nullptr;
            return Property ? Property->GetPropertyValue_InContainer(Object) : -1;
        };
        auto CountOf = [](UObject* Object, FName Name)
        {
            auto* Property = Object ? FindFProperty<FStructProperty>(Object->GetClass(), Name) : nullptr;
            auto* Count = Property ? FindFProperty<FIntProperty>(Property->Struct, TEXT("Count")) : nullptr;
            return Count ? Count->GetPropertyValue_InContainer(Property->ContainerPtrToValuePtr<void>(Object)) : -1;
        };
        if (Phase == TEXT("Create"))
        {
            auto* Score = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score"));
            auto* Payload = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Payload"));
            Score->SetPropertyValue_InContainer(Actor, 77);
            FindFProperty<FIntProperty>(
                Payload->Struct,
                TEXT("Count"))->SetPropertyValue_InContainer(Payload->ContainerPtrToValuePtr<void>(Actor), 88);
            FindFProperty<FStrProperty>(Payload->Struct,
                                        TEXT("Label"))
                                            ->SetPropertyValue_InContainer(Payload->ContainerPtrToValuePtr<void>(Actor),
                                                                           TEXT("Override 世界"));
            const FName ActorName = Actor->GetFName();
            auto* Untouched = World->SpawnActor<ADasActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
            const FName UntouchedName = Untouched->GetFName();
            auto* Variable = BP->NewVariables.FindByPredicate([](const auto& V) { return V.VarName == TEXT("Score"); });
            const FGuid Guid = Variable->VarGuid;
            auto* Graph = BP->UbergraphPages[0].Get();
            auto* Getter = NewObject<UK2Node_VariableGet>(Graph);
            Getter->VariableReference.SetSelfMember(TEXT("Score"));
            Graph->AddNode(Getter);
            Getter->CreateNewGuid();
            Getter->AllocateDefaultPins();
            const FString Changed = Original.Replace(
                TEXT("Score :"),
                     TEXT("Total :"))
                         .Replace(TEXT("self.Score"),
                                       TEXT("self.Total"))
                                           .Replace(
                                               TEXT("Payload :"),
                                                    TEXT("Data :")).Replace(TEXT("self.Payload"), TEXT("self.Data"));
            Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_rename_with_stable_IDs"));
            UClass* Before = BP->GeneratedClass;
            Check(!FDasDSLService::Reimport(BP, Error) && BP->GeneratedClass == Before &&
                  ScoreOf(Actor, TEXT("Score")) == 77, TEXT("implicit_rename_refused_without_mutation"));
            Check(FFileHelper::SaveStringToFile(
                Changed.Replace(TEXT("Total :"), TEXT("Payload :"))
                                                     .Replace(TEXT("self.Total"), TEXT("self.Payload")), *Path,
                                                                  FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                                                              !FDasDSLService::ReimportRenamedFields(BP, Error) &&
                                                              BP->GeneratedClass == Before,
                                     TEXT("rename_occupied_destination_refused"));
            const FString BadId = Changed.Replace(TEXT("@ue_id=\"rename.RenameFields.Score\""), TEXT(""));
            Check(FFileHelper::SaveStringToFile(BadId, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                      !FDasDSLService::ReimportRenamedFields(BP, Error) && BP->GeneratedClass == Before,
                  TEXT("auto_ID_rename_refused"));
            Check(FFileHelper::SaveStringToFile(
                Changed.Replace(TEXT("Total : int = 5"), TEXT("Total : int = 6")), *Path,
                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                !FDasDSLService::ReimportRenamedFields(BP, Error) &&
                ScoreOf(Actor, TEXT("Score")) == 77, TEXT("rename_combined_default_change_refused"));
            Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                      FDasDSLService::ReimportRenamedFields(BP, Error),
                  TEXT("rename_stable_ID_migration"));
            if (!Error.IsEmpty())
                UE_LOG(LogTemp, Error, TEXT("Rename: %s"), *Error);
            Actor = nullptr;
            Untouched = nullptr;
            for (TActorIterator<ADasActor> It(World.Get()); It; ++It)
            {
                if (It->GetFName() == ActorName)
                    Actor = *It;
                if (It->GetFName() == UntouchedName)
                    Untouched = *It;
            }
            Check(ScoreOf(Actor, TEXT("Total")) == 77 &&
                          CountOf(Actor, TEXT("Data")) == 88 &&
                                  ScoreOf(Untouched, TEXT("Total")) == 5 &&
                                          CountOf(Untouched, TEXT("Data")) == 10,
                                                  TEXT("rename_preserves_overrides_and_untouched_values"));
            auto* Data = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Data"));
            Check(Actor && Data &&
                  FindFProperty<FStrProperty>(
                      Data->Struct,
                      TEXT("Label"))->GetPropertyValue_InContainer(Data->ContainerPtrToValuePtr<void>(Actor)) ==
                      TEXT("Override 世界"), TEXT("rename_preserves_owned_struct_string"));
            Variable = BP->NewVariables.FindByPredicate([](const auto& V) { return V.VarName == TEXT("Total"); });
            Check(Variable && Variable->VarGuid == Guid &&
                  Getter->VariableReference.GetMemberName() ==
                      TEXT("Total"), TEXT("rename_preserves_variable_GUID_and_graph_reference"));
            auto* Child = FKismetEditorUtilities::CreateBlueprint(
                BP->GeneratedClass, GetTransientPackage(),
                *FString(TEXT("RenameChild_" )+ Id), BPTYPE_Normal, UBlueprint::StaticClass(),
                         UBlueprintGeneratedClass::StaticClass(), TEXT("DasRenameProbe"));
            Check(Child != nullptr, TEXT("create_transient_dependent_rename_probe"));
            Before = BP->GeneratedClass;
            Check(FFileHelper::SaveStringToFile(Changed.Replace(
                TEXT("Total :"), TEXT("GrandTotal :"))
                                     .Replace(TEXT("self.Total"), TEXT("self.GrandTotal")), *Path,
                                              FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                     !FDasDSLService::ReimportRenamedFields(BP, Error) && BP->GeneratedClass == Before &&
                     ScoreOf(Actor, TEXT("Total")) == 77, TEXT("rename_dependent_child_refused_before_mutation"));
            Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("restore_successful_rename_source"));
        }
        Check(ScoreOf(BP->GeneratedClass->GetDefaultObject(), TEXT("Total")) == 5 &&
                      CountOf(BP->GeneratedClass->GetDefaultObject(), TEXT("Data")) == 10 &&
                              !BP->GeneratedClass->FindPropertyByName(
                                  TEXT("Score")) &&
                                  !BP->GeneratedClass->FindPropertyByName(TEXT("Payload")),
                                                                          TEXT("renamed_fields_defaults_survive_save"));
        TArray<UK2Node_VariableGet*> Getters;
        FBlueprintEditorUtils::GetAllNodesOfClass(BP, Getters);
        Check(Getters.ContainsByPredicate([](const auto* Node)
                                          { return Node->VariableReference.GetMemberName() == TEXT("Total"); }) &&
                  !Getters.ContainsByPredicate([](const auto* Node)
                                               { return Node->VariableReference.GetMemberName() == TEXT("Score"); }),
              TEXT("renamed_graph_reference_persists_on_reopen"));
        FString StableId;
        FBlueprintEditorUtils::GetBlueprintVariableMetaData(BP, TEXT("Total"), nullptr, TEXT("DasMemberId"), StableId);
        Check(StableId == TEXT("rename.RenameFields.Score"), TEXT("rename_stable_identity_persists"));
        if (Actor)
        {
            auto* Function = Actor->FindFunction(TEXT("Add"));
            FStructOnScope Frame(Function);
            FindFProperty<FIntProperty>(Function,
                                        TEXT("Input"))->SetPropertyValue_InContainer(Frame.GetStructMemory(), 3);
            FEditorScriptExecutionGuard Guard;
            Actor->ProcessEvent(Function, Frame.GetStructMemory());
            Check(CastField<FIntProperty>(Function->GetReturnProperty())
                      ->GetPropertyValue_InContainer(Frame.GetStructMemory()) ==
                  (Phase == TEXT("Create")? 171 : 21), TEXT("renamed_self_access_ProcessEvent"));
        }
        UClass* Before = BP->GeneratedClass;
        Check(FDasDSLService::ReimportRenamedFields(BP, Error) && BP->GeneratedClass == Before,
              TEXT("rename_repeat_is_idempotent"));
    }
    else if (MigrationMode)
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        const FString Path = Runtime.ScriptPath(Script);
        FString Original;
        Check(FFileHelper::LoadFileToString(Original, *Path), TEXT("migration_source_available"));
        auto CountOf = [](UObject* Object)
        {
            if (!Object)
                return -1;
            auto* Property = FindFProperty<FStructProperty>(Object->GetClass(), TEXT("Payload"));
            auto* Count = Property ? FindFProperty<FIntProperty>(Property->Struct, TEXT("Count")) : nullptr;
            return Count ? Count->GetPropertyValue_InContainer(Property->ContainerPtrToValuePtr<void>(Object)) : -1;
        };
        auto ScoreOf = [](UObject* Object)
        {
            auto* Score = Object ? FindFProperty<FIntProperty>(Object->GetClass(), TEXT("Score")) : nullptr;
            return Score ? Score->GetPropertyValue_InContainer(Object) : -1;
        };
        Check(CountOf(Actor) == (Phase == TEXT("Create")? 10 : 11) &&
                                 ScoreOf(Actor) == (Phase == TEXT("Create")? 5 : 8),
                                                    TEXT("migration_defaults_persist_across_reopen"));
        if (Phase == TEXT("Create"))
        {
            auto* Untouched = World->SpawnActor<ADasActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
            auto* Property = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Payload"));
            FindFProperty<FIntProperty>(
                Property->Struct,
                TEXT("Count"))->SetPropertyValue_InContainer(Property->ContainerPtrToValuePtr<void>(Actor), 77);
            UClass* Before = BP->GeneratedClass;
            const FName ActorName = Actor->GetFName();
            const FName UntouchedName = Untouched->GetFName();
            const FString Changed = Original.Replace(
                TEXT("Count=10"), TEXT("Count=11")).Replace(TEXT("Score : int = 5"), TEXT("Score : int = 8"));
            Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_new_defaults"));
            Check(!FDasDSLService::Reimport(BP, Error, EDasDefaultMigrationPolicy::Refuse) &&
                      BP->GeneratedClass == Before && CountOf(BP->GeneratedClass->GetDefaultObject()) == 10,
                  TEXT("explicit_refuse_default_policy"));
            Check(FDasDSLService::Reimport(BP, Error, EDasDefaultMigrationPolicy::PreserveInstances) &&
                      BP->GeneratedClass == Before,
                  TEXT("explicit_migration_updates_CDO_without_reinstance"));
            if (!Error.IsEmpty())
                UE_LOG(LogTemp, Error, TEXT("Migration: %s"), *Error);
            Check(CountOf(Actor) == 77 && CountOf(Untouched) == 10 && ScoreOf(Actor) == 5 && ScoreOf(Untouched) == 5,
                  TEXT("migration_preserves_overridden_and_unmodified_existing_instances"));
            auto* Fresh = World->SpawnActor<ADasActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
            Check(CountOf(Fresh) == 11 && ScoreOf(Fresh) == 8, TEXT("new_instances_use_migrated_defaults"));
            Check(FFileHelper::SaveStringToFile(Changed.Replace(
                TEXT("Count=11"), TEXT("Unknown=11")), *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                !FDasDSLService::Reimport(BP, Error, EDasDefaultMigrationPolicy::PreserveInstances) &&
                CountOf(BP->GeneratedClass->GetDefaultObject()) == 11,
                                                TEXT("invalid_default_migration_has_no_partial_write"));
            Check(FFileHelper::SaveStringToFile(Changed.Replace(
                TEXT("class StructFields {"),
                    TEXT("class StructFields {\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Extra : int = 3\n")),
                *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) &&
                                                !FDasDSLService::Reimport(
                                                    BP, Error, EDasDefaultMigrationPolicy::PreserveInstances) &&
                                                !BP->GeneratedClass->FindPropertyByName(
                                                    TEXT("Extra")), TEXT("mixed_structural_migration_refused"));
            Check(FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("restore_valid_migrated_source"));
            FKismetEditorUtilities::CompileBlueprint(BP);
            FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();
            Actor = nullptr;
            Untouched = nullptr;
            for (TActorIterator<ADasActor> It(World.Get()); It; ++It)
            {
                if (It->GetFName() == ActorName)
                    Actor = *It;
                if (It->GetFName() == UntouchedName)
                    Untouched = *It;
            }
            Check(BP->Status != BS_Error && CountOf(BP->GeneratedClass->GetDefaultObject()) == 11 &&
                      ScoreOf(BP->GeneratedClass->GetDefaultObject()) == 8 && CountOf(Actor) == 77 &&
                      CountOf(Untouched) == 10,
                  TEXT("migrated_defaults_survive_later_compile_and_preserve_instances"));
        }
        Check(FDasDSLService::Reimport(BP, Error, EDasDefaultMigrationPolicy::PreserveInstances),
              TEXT("migration_repeat_is_idempotent"));
    }
    else if (FieldMode)
    {
        auto* Property = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Payload"));
        auto* Count = Property ? FindFProperty<FIntProperty>(Property->Struct, TEXT("Count")) : nullptr;
        auto* Mode = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Mode"));
        Check(Property &&
              Property->Struct->GetPathName() ==
                  TEXT("/Script/DasRuntime.DasCallPayload") && Count &&
                       Count->GetPropertyValue_InContainer(Property->ContainerPtrToValuePtr<void>(Actor)) == 10 &&
                       Mode && Mode->HasAnyPropertyFlags(CPF_BlueprintReadOnly),
                       TEXT("struct_field_defaults_and_flags_survive_save"));
        if (Count)
        {
            Count->SetPropertyValue_InContainer(Property->ContainerPtrToValuePtr<void>(Actor), 77);
            auto* Function = Actor->FindFunction(TEXT("Add"));
            Check(Function != nullptr, TEXT("struct_field_add_method"));
            if (Function)
            {
                FStructOnScope Arguments(Function);
                FindFProperty<FIntProperty>(
                    Function, TEXT("Input"))->SetPropertyValue_InContainer(Arguments.GetStructMemory(), 3);
                FEditorScriptExecutionGuard Guard;
                Actor->ProcessEvent(Function, Arguments.GetStructMemory());
                Check(CastField<FIntProperty>(Function->GetReturnProperty())
                                  ->GetPropertyValue_InContainer(Arguments.GetStructMemory()) == 80 &&
                          Count->GetPropertyValue_InContainer(Property->ContainerPtrToValuePtr<void>(Actor)) == 80,
                      TEXT("struct_self_read_write_ProcessEvent"));
            }
            auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
            FDasAuthoringModule Model;
            Check(Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script), Model, Error),
                  TEXT("struct_field_authoring_model"));
            FDasClassSchema Schema;
            Check(Model.ToActorSchema(ClassId, Schema, Error), TEXT("struct_field_actor_schema"));
            auto Changed = Schema;
            Changed.Fields.FindByPredicate([](const auto& F) { return F.Name == TEXT("Payload"); })->StructDefault =
                TEXT("(Count=11)");
            Check(!FDasBlueprintService::Apply(BP, Changed, Error),
                  TEXT("struct_default_change_refused_before_mutation"));
            const auto Wrong = Runtime.RunNamedFramePersistent(
                Runtime.ScriptPath(Script), TEXT("ue_StructFields_Wrong"), {}, EDasMethodSignature::Void, Actor);
            Check(!Wrong.bSucceeded &&
                      Count->GetPropertyValue_InContainer(Property->ContainerPtrToValuePtr<void>(Actor)) == 80,
                  TEXT("struct_self_wrong_type_preserves_field"));
            if (Phase == TEXT("Create"))
            {
                FDasFieldSchema Extra;
                Extra.Name = TEXT("AddedProbe");
                Extra.StableId = ClassId + TEXT(".AddedProbe");
                Extra.IntDefault = 5;
                Schema.Fields.Add(Extra);
                TStrongObjectPtr<ADasActor> HeldActor(Actor);
                Check(FDasBlueprintService::Apply(BP, Schema, Error), TEXT("struct_field_reinstance_add_variable"));
                // Locate the replacement actor in the same world after Blueprint compilation.
                Actor = nullptr;
                for (TActorIterator<ADasActor> It(World.Get()); It; ++It)
                    if (It->GetClass() == BP->GeneratedClass)
                    {
                        Actor = *It;
                        break;
                    }
                Property = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Payload"));
                Count = Property ? FindFProperty<FIntProperty>(Property->Struct, TEXT("Count")) : nullptr;
                Check(Actor && Count &&
                          Count->GetPropertyValue_InContainer(Property->ContainerPtrToValuePtr<void>(Actor)) == 80 &&
                          Count->GetPropertyValue_InContainer(
                              Property->ContainerPtrToValuePtr<void>(BP->GeneratedClass->GetDefaultObject())) == 10,
                      TEXT("struct_reinstance_preserves_override_and_CDO"));
                // The temporary added field is deliberately retained; the asset is a disposable probe.
            }
        }
    }
    else if (EnumMode)
    {
        auto* F = Actor->FindFunction(TEXT("SetMode"));
        Check(F && F->NumParms == 3, TEXT("enum_struct_signature_survives_save"));
        if (F)
        {
            FStructOnScope Memory(F);
            auto* A = FindFProperty<FStructProperty>(F, TEXT("value"));
            auto* Symbol = FindFProperty<FStrProperty>(F, TEXT("symbol"));
            auto* Result = CastField<FStructProperty>(F->GetReturnProperty());
            if (A && Symbol && Result && A->Struct == Result->Struct)
            {
                auto* Mode = FindFProperty<FEnumProperty>(A->Struct, TEXT("Mode"));
                Symbol->SetPropertyValue_InContainer(Memory.GetStructMemory(), TEXT("Active"));
                FEditorScriptExecutionGuard Guard;
                Actor->ProcessEvent(F, Memory.GetStructMemory());
                uint8* Output = Result->ContainerPtrToValuePtr<uint8>(Memory.GetStructMemory());
                Check(Mode && Mode->GetUnderlyingProperty()->GetSignedIntPropertyValue(
                                  Mode->ContainerPtrToValuePtr<uint8>(Output)) == 7,
                      TEXT("saved_enum_struct_ProcessEvent"));
            }
            else
                Check(false, TEXT("enum_struct_parameters"));
        }
        Check(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasParameterId.SetMode.value")) ==
                                                         TEXT("enumvalues.EnumStructBridge.SetMode.value"),
                                                              TEXT("enum_struct_parameter_identity_persisted"));
    }
    else if (StructMode)
    {
        auto* F = Actor->FindFunction(TEXT("Edit"));
        Check(F && F->NumParms == 3, TEXT("reflected_signature_survives_save"));
        if (F)
        {
            FStructOnScope Memory(F);
            auto* A = FindFProperty<FStructProperty>(F, TEXT("value"));
            auto* Amount = FindFProperty<FIntProperty>(F, TEXT("amount"));
            auto* Result = CastField<FStructProperty>(F->GetReturnProperty());
            if (A && Amount && Result && A->Struct == Result->Struct)
            {
                uint8* Input = A->ContainerPtrToValuePtr<uint8>(Memory.GetStructMemory());
                auto* Count = FindFProperty<FIntProperty>(A->Struct, TEXT("Count"));
                auto* Label = FindFProperty<FStrProperty>(A->Struct, TEXT("Label"));
                if (Count && Label)
                {
                    Count->SetPropertyValue_InContainer(Input, 7);
                    Label->SetPropertyValue_InContainer(Input, TEXT("Привет 世界"));
                    Amount->SetPropertyValue_InContainer(Memory.GetStructMemory(), 3);
                    FEditorScriptExecutionGuard Guard;
                    Actor->ProcessEvent(F, Memory.GetStructMemory());
                    uint8* Output = Result->ContainerPtrToValuePtr<uint8>(Memory.GetStructMemory());
                    Check(Count->GetPropertyValue_InContainer(Output) == 10 &&
                          Label->GetPropertyValue_InContainer(Output) ==
                              TEXT("Привет 世界 !"), TEXT("saved_reflected_wildcard_ProcessEvent"));
                }
                else
                    Check(false, TEXT("reflected_leaf_fields"));
            }
            else
                Check(false, TEXT("reflected_parameter_struct_types"));
        }
        Check(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasParameterId.Edit.value")) ==
                                                         TEXT("reflected.StructBridge.Edit.value"),
                                                              TEXT("reflected_parameter_identity_persisted"));
    }
    else if (MathMode)
    {
        auto* F = Actor->FindFunction(TEXT("PassTransform"));
        Check(F && F->NumParms == 2, TEXT("math_signature_survives_save"));
        if (F)
        {
            FStructOnScope Memory(F);
            auto* A = FindFProperty<FStructProperty>(F, TEXT("value"));
            auto* Result = CastField<FStructProperty>(F->GetReturnProperty());
            if (A && Result && A->Struct == TBaseStructure<FTransform>::Get() &&
                Result->Struct == TBaseStructure<FTransform>::Get())
            {
                const FTransform Value(FRotator(0, 90, 0), FVector(10, 20, 30), FVector(2, 3, 4));
                *A->ContainerPtrToValuePtr<FTransform>(Memory.GetStructMemory()) = Value;
                FEditorScriptExecutionGuard Guard;
                Actor->ProcessEvent(F, Memory.GetStructMemory());
                Check(Result->ContainerPtrToValuePtr<FTransform>(Memory.GetStructMemory())->Equals(Value, 1e-12),
                      TEXT("saved_native_transform_ProcessEvent"));
            }
            else
                Check(false, TEXT("math_parameter_struct_types"));
        }
        Check(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasParameterId.PassTransform.value")) ==
                                                         TEXT("math.MathBridge.PassTransform.value"),
                                                              TEXT("math_parameter_identity_persisted"));
    }
    else if (ObjectMode)
    {
        auto* F = Actor->FindFunction(TEXT("ActorIdentity"));
        Check(F && F->NumParms == 2, TEXT("object_signature_survives_save"));
        auto* Target = World->SpawnActor<ADasActor>(ADasActor::StaticClass(), FTransform::Identity, Spawn);
        if (F && Target)
        {
            FStructOnScope Memory(F);
            auto* A = FindFProperty<FObjectProperty>(F, TEXT("actor"));
            auto* Result = CastField<FObjectProperty>(F->GetReturnProperty());
            if (A && Result && A->PropertyClass == AActor::StaticClass() &&
                Result->PropertyClass == AActor::StaticClass())
            {
                A->SetObjectPropertyValue_InContainer(Memory.GetStructMemory(), Target);
                FEditorScriptExecutionGuard Guard;
                Actor->ProcessEvent(F, Memory.GetStructMemory());
                Check(Result->GetObjectPropertyValue_InContainer(Memory.GetStructMemory()) == Target,
                      TEXT("saved_typed_Actor_ProcessEvent"));
            }
            else
                Check(false, TEXT("object_parameter_classes"));
        }
        else
            Check(false, TEXT("object_target_spawn"));
        Check(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasParameterId.ActorIdentity.actor")) ==
                                                         TEXT("objects.ObjectBridge.ActorIdentity.actor"),
                                                              TEXT("object_parameter_identity_persisted"));
    }
    else if (TextMode)
    {
        auto* F = Actor->FindFunction(TEXT("Echo"));
        Check(F && F->NumParms == 3, TEXT("text_signature_survives_save"));
        if (F)
        {
            FStructOnScope Memory(F);
            auto* A = FindFProperty<FStrProperty>(F, TEXT("value"));
            auto* B = FindFProperty<FStrProperty>(F, TEXT("suffix"));
            auto* Result = CastField<FStrProperty>(F->GetReturnProperty());
            if (A && B && Result)
            {
                A->SetPropertyValue_InContainer(Memory.GetStructMemory(), TEXT("Привет 世界"));
                B->SetPropertyValue_InContainer(Memory.GetStructMemory(), TEXT(" !"));
                FEditorScriptExecutionGuard Guard;
                Actor->ProcessEvent(F, Memory.GetStructMemory());
                Check(Result->GetPropertyValue_InContainer(Memory.GetStructMemory()) ==
                      TEXT("Привет 世界 !"), TEXT("saved_text_ProcessEvent"));
            }
            else
                Check(false, TEXT("text_parameters"));
        }
        auto* D = FindFProperty<FDoubleProperty>(Actor->GetClass(), TEXT("DSLPrecise"));
        auto* N = FindFProperty<FNameProperty>(Actor->GetClass(), TEXT("DSLKey"));
        Check(D && D->GetPropertyValue_InContainer(Actor) == 1.23456789012345 && N &&
              N->GetPropertyValue_InContainer(Actor) == FName(TEXT("Initial_7")), TEXT("saved_double_name_defaults"));
        Check(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasParameterId.Echo.value")) ==
                                                         TEXT("text.TextCounter.Echo.value"),
                                                              TEXT("text_parameter_identity_persisted"));
    }
    else
    {
        auto* F = Actor->FindFunction(TEXT("Compute"));
        Check(F && F->NumParms == 4, TEXT("mixed_signature_survives_save"));
        if (F)
        {
            FStructOnScope Memory(F);
            auto* A = FindFProperty<FIntProperty>(F, TEXT("count"));
            auto* B = FindFProperty<FFloatProperty>(F, TEXT("factor"));
            auto* C = FindFProperty<FBoolProperty>(F, TEXT("apply"));
            auto* R = CastField<FFloatProperty>(F->GetReturnProperty());
            if (A && B && C && R)
            {
                A->SetPropertyValue_InContainer(Memory.GetStructMemory(), 2);
                B->SetPropertyValue_InContainer(Memory.GetStructMemory(), 0.5f);
                C->SetPropertyValue_InContainer(Memory.GetStructMemory(), true);
                FEditorScriptExecutionGuard Guard;
                Actor->ProcessEvent(F, Memory.GetStructMemory());
                Check(R->GetPropertyValue_InContainer(Memory.GetStructMemory()) == 24.f,
                      TEXT("saved_frame_graph_ProcessEvent"));
            }
            else
                Check(false, TEXT("saved_parameter_types"));
        }
        Check(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasParameterId.Compute.count")) ==
                                                         TEXT("mixed.MixedCounter.Compute.count"),
                                                              TEXT("frame_parameter_identity_persisted"));
    }
    UClass* Before = BP->GeneratedClass;
    if (!FieldMode && !MigrationMode && !RenameMode)
        Check(FDasDSLService::Reimport(BP, Error) && BP->GeneratedClass == Before,
              TEXT("saved_frame_reimport_without_reinstance"));
    if (Phase == TEXT("Create"))
    {
        BP->SetFlags(RF_Public | RF_Standalone);
        const FString File = FPackageName::LongPackageNameToFilename(Package, FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        Args.SaveFlags = SAVE_NoError;
        Check(UPackage::SavePackage(BP->GetOutermost(), BP, *File, Args), TEXT("save_frame_blueprint"));
    }
    World->DestroyWorld(false);
    UE_LOG(LogTemp, Display, TEXT("DAS_FRAME_ASSET_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
    return Passed ? 0 : 1;
}
