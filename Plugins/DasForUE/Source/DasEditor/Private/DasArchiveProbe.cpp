#include "DasDSLService.h"
#include "DasRuntime.h"
#include "Internationalization/Text.h"
#include "DasProbeActor.h"
#include "Modules/ModuleManager.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Editor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "K2Node_VariableGet.h"
#include "EdGraph/EdGraph.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"
#include "UObject/SavePackage.h"
#include "UObject/MetaData.h"
#include "UObject/StructOnScope.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#if WITH_DEV_AUTOMATION_TESTS
void DasFailArchiveAfterCompileForTesting(bool Enabled);
#endif
namespace
{
FString RetiredKey(const TCHAR* Field)
{
    return TEXT("retire.RetireFields::retire.RetireFields.")+ FString(Field);
}
int32 ArchiveScore(UObject* O)
{
    auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), TEXT("Score")) : nullptr;
    return P ? P->GetPropertyValue_InContainer(O) : -1;
}
int32 ArchiveCount(UObject* O)
{
    auto* P = O ? FindFProperty<FStructProperty>(O->GetClass(), TEXT("Payload")) : nullptr;
    auto* C = P ? FindFProperty<FIntProperty>(P->Struct, TEXT("Count")) : nullptr;
    return C ? C->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(O)) : -1;
}
void ArchiveExtraValues(UObject* O, int32 Score)
{
    FindFProperty<FByteProperty>(O->GetClass(), TEXT("Priority"))->SetPropertyValue_InContainer(O, uint8(Score));
    FindFProperty<FTextProperty>(
        O->GetClass(),
        TEXT("Title"))
            ->SetPropertyValue_InContainer(
                O, FText::ChangeKey(TEXT("ArchiveProbe"), TEXT("Override"), FText::FromString(TEXT("Retained 世界"))));
    *static_cast<TArray<int32>*>(
        O->GetClass()->FindPropertyByName(TEXT("Numbers"))->ContainerPtrToValuePtr<void>(O)) = {Score, Score + 1};
    *static_cast<TSet<int32>*>(
        O->GetClass()->FindPropertyByName(TEXT("UniqueNumbers"))->ContainerPtrToValuePtr<void>(O)) = {Score, Score + 2};
    *static_cast<TMap<FString, int32>*>(
        O->GetClass()->FindPropertyByName(TEXT("Counts"))->ContainerPtrToValuePtr<void>(O)) = {
            {TEXT("Owned 世界"), Score}};
}
bool ArchiveExtraMatches(UObject* O, int32 Score, bool Default = false)
{
    if (!O)
        return false;
    auto* Byte = FindFProperty<FByteProperty>(O->GetClass(), TEXT("Priority"));
    auto* Text = FindFProperty<FTextProperty>(O->GetClass(), TEXT("Title"));
    auto* Array = FindFProperty<FArrayProperty>(O->GetClass(), TEXT("Numbers"));
    auto* Set = FindFProperty<FSetProperty>(O->GetClass(), TEXT("UniqueNumbers"));
    auto* Map = FindFProperty<FMapProperty>(O->GetClass(), TEXT("Counts"));
    if (!Byte || Byte->Enum || !Text || !Array || !CastField<FIntProperty>(Array->Inner) || !Set ||
        !CastField<FIntProperty>(Set->ElementProp) || !Map || !CastField<FStrProperty>(Map->KeyProp) ||
        !CastField<FIntProperty>(Map->ValueProp))
        return false;
    const FText Title = Text->GetPropertyValue_InContainer(O);
    const auto Namespace = FTextInspector::GetNamespace(Title);
    const auto Key = FTextInspector::GetKey(Title);
    const auto& Numbers = *Array->ContainerPtrToValuePtr<TArray<int32>>(O);
    const auto& Tags = *Set->ContainerPtrToValuePtr<TSet<int32>>(O);
    const auto& Counts = *Map->ContainerPtrToValuePtr<TMap<FString, int32>>(O);
    const int32* Count = Counts.Find(Default ? TEXT("Initial"): TEXT("Owned 世界"));
    return Byte->GetPropertyValue_InContainer(O) == uint8(Default ? 255 : Score) && Namespace.IsSet() &&
           Namespace.GetValue() ==
               TEXT("ArchiveProbe") && Key.IsSet() &&
                    Key.GetValue() == (Default ? TEXT("Initial"): TEXT("Override")) &&
                                       Title.ToString() ==
                                           (Default ? TEXT("Initial 世界"): TEXT("Retained 世界")) &&
                                            Numbers == TArray<int32>({Default ? 1 : Score, Default ? 2 : Score + 1}) &&
                                            Tags.Num() == 2 && Tags.Contains(Default ? 3 : Score) &&
                                            Tags.Contains(Default ? 4 : Score + 2) && Counts.Num() == 1 && Count &&
                                            *Count == (Default ? 5 : Score);
}
void ArchiveValues(UObject* O, int32 Score, int32 Count)
{
    ArchiveExtraValues(O, Score);
    FindFProperty<FIntProperty>(O->GetClass(), TEXT("Score"))->SetPropertyValue_InContainer(O, Score);
    auto* P = FindFProperty<FStructProperty>(O->GetClass(), TEXT("Payload"));
    FindFProperty<FIntProperty>(P->Struct,
                                TEXT("Count"))->SetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(O), Count);
    FindFProperty<FStrProperty>(
        P->Struct, TEXT("Label"))->SetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(O), TEXT("Owned 世界"));
}
bool SaveArchiveAsset(UObject* A)
{
    if (!A)
        return false;
    const FString File = FPackageName::LongPackageNameToFilename(
        A->GetOutermost()->GetName(),
        A->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
    A->SetFlags(RF_Public | RF_Standalone);
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    Args.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(A->GetOutermost(), A, *File, Args);
}
UBlueprint* ArchiveBP(const FString& Base, const TCHAR* Name)
{
    return LoadObject<UBlueprint>(nullptr, *(Base + Name + TEXT(".")+ Name));
}
ADasProbeActor* ArchivedActor(UWorld* W, FName Name)
{
    if (W && W->PersistentLevel)
        for (auto& E : W->PersistentLevel->Actors)
            if (auto* A = Cast<ADasProbeActor>(E.Get()))
                if (A->GetFName() == Name)
                    return A;
    return nullptr;
}
}
int32 RunDasArchiveProbe(const FString& Params)
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
    const FString Base = TEXT("/Game/DasFrameAssetProbe/Archive_")+ Id +
                              TEXT("/"), Script = TEXT("dsl_archive_")+ Id + TEXT(".das");
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    bool Passed = true;
    auto Check = [&](bool V, const TCHAR* N)
    {
        Passed &= V;
        UE_LOG(LogTemp, Display, TEXT("DAS_ARCHIVE_TEST %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
    };
    UBlueprint* Parent = nullptr;
    UBlueprint* Child = nullptr;
    UWorld* World = nullptr;
    FString Original;
    FFileHelper::LoadFileToString(Original, *Runtime.ScriptPath(TEXT("dsl_retired_fields.das")));
    Original.ReplaceInline(TEXT("require ue_dsl"), TEXT("require ue_dsl\nrequire ue_text"));
    Original.ReplaceInline(TEXT("    //RetireEnd"),
        TEXT("    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Priority:uint8=uint8(255)\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Title:FText=ue_text(\"ArchiveProbe\",\"Initial\",\"Initial 世界\")\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Numbers:array<int> = [1,2]\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    UniqueNumbers:table<int> = {3,4}\n    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    Counts:table<string;int> = {\"Initial\"=>5}\n    //RetireEnd"));
    FString Removed = Original;
    const int32 A = Removed.Find(TEXT("    //RetireBegin")), B = Removed.Find(TEXT("    //RetireEnd"));
    Removed.RemoveAt(A, B - A + FString(TEXT("    //RetireEnd")).Len());
    const bool Removing = Phase == TEXT("Remove"), Restoring = Phase == TEXT("Restore");
    if (Phase == TEXT("Create"))
    {
        Check(FFileHelper::SaveStringToFile(Original, *Runtime.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("create_archive_source"));
        Check(FDasDSLService::Apply(Script, Base + TEXT("BP_Parent"), TEXT("retire.RetireFields"), Parent, Error),
                                                TEXT("create_archive_parent"));
        if (!Parent)
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }
        Check(ArchiveExtraMatches(Parent->GeneratedClass->GetDefaultObject(), 5, true),
              TEXT("initial_byte_text_native_container_defaults"));
        Child = FKismetEditorUtilities::CreateBlueprint(
            Parent->GeneratedClass, CreatePackage(*(Base + TEXT("BP_Child"))),
                                                  TEXT("BP_Child"), BPTYPE_Normal, UBlueprint::StaticClass(),
                                                       UBlueprintGeneratedClass::StaticClass(),
                                                       TEXT("DasArchiveProbe"));
        FAssetRegistryModule::AssetCreated(Child);
        ArchiveValues(Child->GeneratedClass->GetDefaultObject(), 23, 34);
        World = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Base + TEXT("Map"))));
        World->SetFlags(RF_Public | RF_Standalone);
        World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        FAssetRegistryModule::AssetCreated(World);
        FActorSpawnParameters Spawn;
        Spawn.Name = TEXT("ParentActor");
        auto* Actor = World->SpawnActor<ADasProbeActor>(Parent->GeneratedClass, FTransform::Identity, Spawn);
        ArchiveValues(Actor, 77, 88);
        FindFProperty<FStrProperty>(Actor->GetClass(),
                                    TEXT("Text"))->SetPropertyValue_InContainer(Actor, TEXT("Actor Привет 世界"));
        FindFProperty<FNameProperty>(Actor->GetClass(),
                                     TEXT("Key"))->SetPropertyValue_InContainer(Actor, FName(TEXT("Actor_19")));
        FindFProperty<FBoolProperty>(Actor->GetClass(), TEXT("Enabled"))->SetPropertyValue_InContainer(Actor, false);
        FindFProperty<FDoubleProperty>(Actor->GetClass(),
                                       TEXT("Precise"))->SetPropertyValue_InContainer(Actor, -2.34567890123456);
        FindFProperty<FFloatProperty>(Actor->GetClass(), TEXT("Rate"))->SetPropertyValue_InContainer(Actor, -2.25f);
        const auto* Variable =
            Parent->NewVariables.FindByPredicate([](const auto& V) { return V.VarName == TEXT("Score"); });
        Parent->GetOutermost()->GetMetaData().SetValue(Parent, TEXT("DasArchiveTestGuid"),
                                                                    *Variable->VarGuid.ToString(EGuidFormats::Digits));
        Spawn.Name = TEXT("ChildActor");
        ArchiveValues(World->SpawnActor<ADasProbeActor>(Child->GeneratedClass, FTransform::Identity, Spawn), 99, 100);
        Check(SaveArchiveAsset(Parent) && SaveArchiveAsset(Child) && SaveArchiveAsset(World),
              TEXT("save_original_archive_scope"));
    }
    else
    {
        Parent = ArchiveBP(Base, TEXT("BP_Parent"));
        if (!Parent)
            return 1;
        if (Removing || Restoring)
        {
            UClass* Before = Parent->GeneratedClass;
            Check(FFileHelper::SaveStringToFile(Removing ? Removed : Original, *Runtime.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_archive_declaration"));
            Check(!FDasDSLService::ReimportArchivedFields(Parent, Restoring, false, Error) &&
                      Parent->GeneratedClass == Before,
                  TEXT("isolated_archive_refuses_saved_scope"));
            if (Removing)
            {
                auto* G = Parent->UbergraphPages[0].Get();
                auto* Get = NewObject<UK2Node_VariableGet>(G);
                Get->VariableReference.SetSelfMember(TEXT("Score"));
                G->AddNode(Get);
                Get->CreateNewGuid();
                Get->AllocateDefaultPins();
                Check(!FDasDSLService::ReimportArchivedFields(Parent, false, true, Error) &&
                          Parent->GeneratedClass == Before,
                      TEXT("referenced_field_removal_refused"));
                G->RemoveNode(Get);
            }
            if (Restoring)
            {
                const FString WrongDefault = Original.Replace(TEXT("Score : int = 5"), TEXT("Score : int = 6"));
                const FString WrongType = Original.Replace(TEXT("Score : int = 5"), TEXT("Score : float = 5.0"));
                const FString WrongId = Original.Replace(
                    TEXT("Score : int = 5"), TEXT("@ue_id=\"retire.different.score\"\n    Score : int = 5"));
                for (const auto& Invalid : {WrongDefault, WrongType, WrongId})
                {
                    Check(FFileHelper::SaveStringToFile(Invalid, *Runtime.ScriptPath(Script),
                                                        FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                          TEXT("write_invalid_restore"));
                    Check(!FDasDSLService::ReimportArchivedFields(Parent, true, true, Error) &&
                              Parent->GeneratedClass == Before,
                          TEXT("changed_restore_declaration_refused_before_mutation"));
                }
                Check(FFileHelper::SaveStringToFile(Original, *Runtime.ScriptPath(Script),
                                                    FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                      TEXT("restore_exact_declaration"));
            }
            const FString OldModel = Parent->GetOutermost()->GetMetaData().GetValue(Parent, TEXT("DasDSLModel"));
#if WITH_DEV_AUTOMATION_TESTS
            DasFailArchiveAfterCompileForTesting(true);
            const bool Failed = !FDasDSLService::ReimportArchivedFields(Parent, Restoring, true, Error);
            DasFailArchiveAfterCompileForTesting(false);
            Check(Failed &&
                  Error.StartsWith(TEXT("Archive migration rolled back:")) &&
                                   Parent->GetOutermost()->GetMetaData().GetValue(
                                       Parent, TEXT("DasDSLModel")) == OldModel &&
                                       (Restoring ? !Parent->GeneratedClass->FindPropertyByName(TEXT("Score"))
                                                  : ArchiveScore(Parent->GeneratedClass->GetDefaultObject()) == 5),
                                        TEXT("archive_failure_rolls_back_layout_values_and_model"));
#endif
            Check(FDasDSLService::ReimportArchivedFields(Parent, Restoring, true, Error),
                  TEXT("archive_explicit_operation"));
            if (!Error.IsEmpty())
            {
                UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
                return 1;
            }
        }
        Child = ArchiveBP(Base, TEXT("BP_Child"));
        World = LoadObject<UWorld>(nullptr, *(Base + TEXT("Map.Map")));
        auto* Actor = ArchivedActor(World, TEXT("ParentActor"));
        auto* Other = ArchivedActor(World, TEXT("ChildActor"));
        Check(Actor && Other && Child, TEXT("load_archive_scope"));
        if (Phase == TEXT("Remove") || Phase == TEXT("ReopenRemoved"))
        {
            const auto* Score = Actor ? Actor->RetiredDasValues.Find(RetiredKey(TEXT("Score"))) : nullptr;
            const auto* Payload = Actor ? Actor->RetiredDasValues.Find(RetiredKey(TEXT("Payload"))) : nullptr;
            const auto* Text = Actor ? Actor->RetiredDasValues.Find(RetiredKey(TEXT("Text"))) : nullptr;
            const auto* Key = Actor ? Actor->RetiredDasValues.Find(RetiredKey(TEXT("Key"))) : nullptr;
            const auto* Enabled = Actor ? Actor->RetiredDasValues.Find(RetiredKey(TEXT("Enabled"))) : nullptr;
            const auto* Precise = Actor ? Actor->RetiredDasValues.Find(RetiredKey(TEXT("Precise"))) : nullptr;
            const auto* Rate = Actor ? Actor->RetiredDasValues.Find(RetiredKey(TEXT("Rate"))) : nullptr;
            Check(Key && Key->NameValue == FName(TEXT("Actor_19")) && Enabled && !Enabled->BoolValue && Precise &&
                                                 Precise->DoubleValue == -2.34567890123456 && Rate &&
                                                 Rate->FloatValue == -2.25f,
                                                 TEXT("retired_name_bool_double_float_values_persist"));
            Check(!Parent->GeneratedClass->FindPropertyByName(
                TEXT("Score")) && Score && Score->IntValue == 77 && Text &&
                Text->StringValue == TEXT("Actor Привет 世界") && Payload && Payload->StructValue.IsValid(),
                                          TEXT("retired_scalar_struct_and_string_values_persist"));
            Check(Cast<ADasProbeActor>(Parent->GeneratedClass->GetDefaultObject())
                      ->RetiredDasValues.FindChecked(
                          RetiredKey(TEXT("Score"))).IntValue == 5 &&
                          Cast<ADasProbeActor>(Child->GeneratedClass->GetDefaultObject())
                              ->RetiredDasValues.FindChecked(
                                  RetiredKey(TEXT("Score"))).IntValue == 23 &&
                                  Other->RetiredDasValues.FindChecked(
                                      RetiredKey(TEXT("Score"))).IntValue == 99,
                                      TEXT("retired_parent_child_defaults_and_actor_overrides"));
            if (Removing)
            {
                FActorSpawnParameters Spawn;
                Spawn.Name = TEXT("FreshRetired");
                auto* Fresh = World->SpawnActor<ADasProbeActor>(Child->GeneratedClass, FTransform::Identity, Spawn);
                const auto* Archived = Fresh->RetiredDasValues.Find(RetiredKey(TEXT("Score")));
                Check(Archived && Archived->IntValue == 23, TEXT("spawn_while_retired_inherits_archive_default"));
            }
        }
        else if (Phase == TEXT("Restore") || Phase == TEXT("ReopenRestored"))
        {
            const auto* Variable =
                Parent->NewVariables.FindByPredicate([](const auto& V) { return V.VarName == TEXT("Score"); });
            Check(Variable && Variable->VarGuid.ToString(EGuidFormats::Digits) ==
                                  Parent->GetOutermost()->GetMetaData().GetValue(
                                      Parent, TEXT("DasArchiveTestGuid")),
                                      TEXT("variable_GUID_survives_save_restore_reopen"));
            Check(Actor &&
                  FindFProperty<FNameProperty>(
                      Actor->GetClass(), TEXT("Key"))->GetPropertyValue_InContainer(Actor) ==
                      FName(TEXT("Actor_19")) &&
                            !FindFProperty<FBoolProperty>(
                                Actor->GetClass(), TEXT("Enabled"))->GetPropertyValue_InContainer(Actor) &&
                                FindFProperty<FDoubleProperty>(
                                    Actor->GetClass(),
                                    TEXT("Precise"))->GetPropertyValue_InContainer(Actor) == -2.34567890123456 &&
                                    FindFProperty<FFloatProperty>(
                                        Actor->GetClass(), TEXT("Rate"))->GetPropertyValue_InContainer(Actor) == -2.25f,
                                        TEXT("restored_name_bool_double_float_values"));
            auto* Fresh = ArchivedActor(World, TEXT("FreshRetired"));
            Check(ArchiveScore(Parent->GeneratedClass->GetDefaultObject()) == 5 &&
                      ArchiveScore(Child->GeneratedClass->GetDefaultObject()) == 23 && ArchiveScore(Actor) == 77 &&
                      ArchiveCount(Actor) == 88 && ArchiveScore(Other) == 99 && ArchiveCount(Other) == 100 &&
                      ArchiveScore(Fresh) == 23,
                  TEXT("restored_defaults_overrides_and_new_retired_instance"));
            Check(ArchiveExtraMatches(Parent->GeneratedClass->GetDefaultObject(), 5, true) &&
                      ArchiveExtraMatches(Child->GeneratedClass->GetDefaultObject(), 23) &&
                      ArchiveExtraMatches(Actor, 77) && ArchiveExtraMatches(Other, 99) &&
                      ArchiveExtraMatches(Fresh, 23),
                  TEXT("restored_byte_localized_text_array_set_map_defaults_and_overrides"));
            Check(FindFProperty<FStrProperty>(Actor->GetClass(), TEXT("Text"))->GetPropertyValue_InContainer(Actor) ==
                                              TEXT("Actor Привет 世界") && Actor->RetiredDasValues.IsEmpty() &&
                                                   Other->RetiredDasValues.IsEmpty(),
                                                   TEXT("restored_string_and_consumed_archive"));
        }
        else
            return 1;
        if (Removing || Restoring)
            Check(SaveArchiveAsset(Parent) && SaveArchiveAsset(Child) && SaveArchiveAsset(World),
                  TEXT("save_archive_scope_changes"));
        UClass* Before = Parent->GeneratedClass;
        Check(FDasDSLService::ReimportArchivedFields(
            Parent, Restoring || Phase == TEXT("ReopenRestored"), true, Error) && Parent->GeneratedClass == Before,
            TEXT("archive_repeat_idempotent"));
    }
    if (World && World->IsInitialized())
        World->DestroyWorld(false);
    UE_LOG(LogTemp, Display, TEXT("DAS_ARCHIVE_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
    return Passed ? 0 : 1;
}
