#include "DasSaveGameCompatibilityCommandlet.h"
#include "DasAuthoring.h"
#include "Dom/JsonObject.h"
#include "Misc/Parse.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasSaveGame.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Blueprint.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/GarbageCollection.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
template <class T> T* CheckedProperty(UClass* Class, FName Name)
{
    auto* P = FindFProperty<T>(Class, Name);
    checkf(P, TEXT("Expected authored SaveGame property %s"), *Name.ToString());
    return P;
}

FString Field(const FString& Name, const FString& Type, const FString& Value, const FString& Id = FString())
{
    return TEXT("    @ue_property=(EditAnywhere,BlueprintReadWrite)\n")+
                (Id.IsEmpty() ? FString() : TEXT("    @ue_id=\"")+ Id + TEXT("\"\n")) +
                     TEXT("    ")+ Name + TEXT(":")+ Type + TEXT("=")+ Value + TEXT("\n");
}
FString RuntimeSource(int32 Version)
{
    FString Body = Field(TEXT("Kept"), TEXT("int"), TEXT("1"));
    if (Version < 3)
        Body += Field(TEXT("Removed"), TEXT("int"), TEXT("11"));
    Body += Field(TEXT("DefaultOnly"), TEXT("int"), Version >= 4 ? TEXT("20"): TEXT("10"));
    Body += Field(TEXT("Overridden"), TEXT("int"), Version >= 4 ? TEXT("20"): TEXT("10"));
    Body += Field(TEXT("Widen"), Version >= 5 ? TEXT("double"): TEXT("int"), Version >= 5 ? TEXT("0.0lf"): TEXT("0"));
    Body += Field(TEXT("Narrow"), Version >= 6 ? TEXT("int"): TEXT("double"), Version >= 6 ? TEXT("0"): TEXT("0.0lf"));
    if (Version >= 2)
        Body = Field(TEXT("Added"), TEXT("int"), TEXT("88")) + Body;
    return TEXT("options gen2\nrequire ue_savegame\n[ue_class]\nclass RuntimeSave:SaveGame {\n")+ Body +
                TEXT("    [ue_function(BlueprintCallable)]\n    def Read(input:int):int {return self.Kept}\n}\n");
}
FString PackageSource(const FString& Probe, bool Renamed)
{
    const FString Name = Renamed ? TEXT("Points"): TEXT("Score");
    return TEXT("options gen2\nrequire ue_savegame\n[ue_class]\nclass CompatibilitySave:SaveGame {\n")+
        Field(
            Name,
            TEXT("int"), Renamed
                 ? TEXT("20")
                 : TEXT("10"), TEXT("compat.")+ Probe + TEXT(".score")) +
                        Field(TEXT("Control"),
                            TEXT("int"), TEXT("1")) +
                                 TEXT("    [ue_function(BlueprintCallable)]\n    def Read(input:int):int {return self.")+
                                     Name + TEXT("}\n}\n");
}
FGuid Guid(UBlueprint* BP, FName Name)
{
    for (const auto& V : BP->NewVariables)
        if (V.VarName == Name)
            return V.VarGuid;
    return {};
}
bool SaveAsset(UBlueprint* BP)
{
    const FString File = FPackageName::LongPackageNameToFilename(BP->GetOutermost()->GetName(),
                                                                 FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    return UPackage::SavePackage(BP->GetOutermost(), BP, *File, Args);
}
bool Json(const TSharedRef<FJsonObject>& Value, const FString& File)
{
    FString Text;
    auto Writer = TJsonWriterFactory<>::Create(&Text);
    return FJsonSerializer::Serialize(Value, Writer) &&
           FFileHelper::SaveStringToFile(Text, *File, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}
}
UDasSaveGameCompatibilityCommandlet::UDasSaveGameCompatibilityCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasSaveGameCompatibilityCommandlet::Main(const FString& Params)
{
    FString Probe, Phase, Output;
    if (!FParse::Value(*Params, TEXT("Probe="), Probe) || Probe.Len() != 32 ||
                       !FParse::Value(*Params, TEXT("Phase="), Phase) ||
                                      !FParse::Value(*Params, TEXT("Output="), Output))
        return 2;
    for (TCHAR C : Probe)
        if (!FChar::IsHexDigit(C))
            return 2;
    if (Phase != TEXT("Prepare")&& Phase != TEXT("Upgrade"))
        return 2;
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Root = TEXT("/Game/Tests/DasSaveGameCompatibility/")+ Probe;
    const FString Package = Root + TEXT("/BP_Profile"), AssetPath = Package + TEXT(".BP_Profile");
    const FString Script = TEXT("savegame_compatibility_")+ Probe + TEXT(".das"), Path = R.ScriptPath(Script);
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Name, const FString& Error = FString())
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display,
               TEXT("DAS_SAVEGAME_COMPAT_%s %s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name, Good ? TEXT(""): *Error);
        return Good;
    };
    auto Report = MakeShared<FJsonObject>();
    Report->SetStringField(TEXT("probe"), Probe);
    Report->SetStringField(TEXT("class"), Package + TEXT(".BP_Profile_C"));
    Report->SetStringField(TEXT("script"), Script);
    Report->SetStringField(TEXT("cook_directory"), FPackageName::LongPackageNameToFilename(Root));
    FString Error;
    UBlueprint* BP = nullptr;
    auto Import = [&](const FString& File, const FString& PackageName, UBlueprint*& Out)
    {
        FDasAuthoringModule Model;
        return R.ReadAuthoringScript(R.ScriptPath(File), Model, Error) && Model.Classes.Num() == 1 &&
               FDasDSLService::Apply(File, PackageName, Model.Classes[0].Id, Out, Error);
    };
    if (Phase == TEXT("Prepare"))
    {
        const FString RuntimeFile =
                          TEXT("savegame_runtime_")+ Probe + TEXT(".das"), RuntimePath = R.ScriptPath(RuntimeFile);
        UBlueprint* RuntimeBP = nullptr;
        ON_SCOPE_EXIT
        {
            IFileManager::Get().Delete(*RuntimePath, false, true);
            if (RuntimeBP)
            {
                FAssetRegistryModule::AssetDeleted(RuntimeBP);
                RuntimeBP->GetOutermost()->SetDirtyFlag(false);
                RuntimeBP->ClearFlags(RF_Public | RF_Standalone);
            }
        };
        if (!FFileHelper::SaveStringToFile(RuntimeSource(1), *RuntimePath,
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            !Check(Import(RuntimeFile, Root + TEXT("/BP_Runtime"), RuntimeBP),
                          TEXT("import_authored_runtime_v1"), Error))
            return 1;
        TStrongObjectPtr<UBlueprint> KeepRuntime(RuntimeBP);
        TArray<uint8> Bytes;
        {
            TStrongObjectPtr<USaveGame> Original(
                UGameplayStatics::CreateSaveGameObject(RuntimeBP->GeneratedClass.Get()));
            auto SetInt = [&](FName Name, int32 Value)
            {
                CheckedProperty<FIntProperty>(Original->GetClass(), Name)
                    ->SetPropertyValue_InContainer(Original.Get(), Value);
            };
            SetInt(TEXT("Kept"), 99);
            SetInt(TEXT("DefaultOnly"), 10); // Explicitly assigning the old default still compares equal to the CDO.
            SetInt(TEXT("Removed"), 77);
            SetInt(TEXT("Overridden"), 42);
            SetInt(TEXT("Widen"), 123);
            CheckedProperty<FDoubleProperty>(Original->GetClass(),
                                             TEXT("Narrow"))->SetPropertyValue_InContainer(Original.Get(), 12.75);
            if (!Check(UGameplayStatics::SaveGameToMemory(Original.Get(), Bytes) && !Bytes.IsEmpty(),
                       TEXT("standard_savegame_v1_bytes")))
                return 1;
        }
        CollectGarbage(RF_NoFlags);
        for (int32 Version = 2; Version <= 6; ++Version)
        {
            if (!FFileHelper::SaveStringToFile(RuntimeSource(Version), *RuntimePath,
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                !Check(FDasDSLService::Reimport(RuntimeBP, Error, EDasDefaultMigrationPolicy::Automatic),
                       *FString::Printf(TEXT("publish_schema_v%d"), Version), Error))
                return 1;
            {
                TStrongObjectPtr<USaveGame> Loaded(UGameplayStatics::LoadGameFromMemory(Bytes));
                if (!Check(Loaded.IsValid() && Loaded->GetClass() == RuntimeBP->GeneratedClass,
                           TEXT("old_bytes_load_as_current_authored_class")))
                    return 1;
                auto Int = [&](FName Name)
                {
                    const auto* P = FindFProperty<FIntProperty>(Loaded->GetClass(), Name);
                    return P ? P->GetPropertyValue_InContainer(Loaded.Get()) : MIN_int32;
                };
                Check(Int(TEXT("Kept")) == 99, TEXT("unchanged_field_survives_schema_changes"));
                if (Version == 2)
                    Check(Int(TEXT("Added")) == 88 && Int(TEXT("Removed")) == 77,
                                                          TEXT("new_field_uses_current_default"));
                if (Version == 3)
                    Check(!FindFProperty<FProperty>(Loaded->GetClass(), TEXT("Removed")) &&
                                                    Int(TEXT("Added")) == 88,
                                                        TEXT("removed_tag_skipped_without_stream_desync"));
                if (Version == 4)
                {
                    Check(Int(TEXT("DefaultOnly")) == 20, TEXT("omitted_old_default_uses_new_default"));
                    Check(Int(TEXT("Overridden")) == 42, TEXT("stored_override_beats_new_default"));
                    Report->SetNumberField(TEXT("default_equal_before"), 10);
                    Report->SetNumberField(TEXT("default_equal_loaded"), Int(TEXT("DefaultOnly")));
                    Report->SetNumberField(TEXT("override_loaded"), Int(TEXT("Overridden")));
                }
                if (Version == 5)
                {
                    const auto* P = FindFProperty<FDoubleProperty>(Loaded->GetClass(), TEXT("Widen"));
                    const double V = P ? P->GetPropertyValue_InContainer(Loaded.Get()) : -1.;
                    Check(V == 123., TEXT("native_int_to_double_load_conversion"));
                    Report->SetNumberField(TEXT("widen_loaded"), V);
                }
                if (Version == 6)
                {
                    Check(Int(TEXT("Narrow")) == 12, TEXT("native_double_to_int_load_truncates_fraction"));
                    Report->SetNumberField(TEXT("narrow_before"), 12.75);
                    Report->SetNumberField(TEXT("narrow_loaded"), Int(TEXT("Narrow")));
                    const auto Value = R.RunNamedPersistent(RuntimePath, TEXT("ue_RuntimeSave_Read"), 0, Loaded.Get());
                    Check(Value.bSucceeded && Value.Value == 99, TEXT("loaded_data_read_by_authored_DAS_method"),
                                                                      Value.Error);
                    R.ReleaseObject(Loaded.Get());
                }
            }
            CollectGarbage(RF_NoFlags);
        }
        if (!Check(!FPackageName::DoesPackageExist(Package), TEXT("unique_export_package")))
            return 1;
        if (!FFileHelper::SaveStringToFile(PackageSource(Probe, false), *Path,
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            !Check(Import(Script, Package, BP), TEXT("import_packaged_v1"), Error))
            return 1;
        Check(Guid(BP, TEXT("Score")).IsValid() && SaveAsset(BP), TEXT("save_original_blueprint_with_guid"));
        Report->SetStringField(TEXT("variable_guid"), Guid(BP, TEXT("Score")).ToString());
    }
    else
    {
        BP = LoadObject<UBlueprint>(nullptr, *AssetPath);
        if (!Check(BP != nullptr, TEXT("reopen_original_asset_in_fresh_process")))
            return 1;
        TStrongObjectPtr<UBlueprint> Keep(BP);
        const FGuid Before = Guid(BP, TEXT("Score"));
        Report->SetBoolField(TEXT("editor_guids_available"), BP->GeneratedClass->ArePropertyGuidsAvailable());
        Report->SetStringField(TEXT("compiled_guid_before"),
                                    BP->GeneratedClass->FindPropertyGuidFromName(TEXT("Score")).ToString());
        // Bytes contain a non-default value under the old name. No instance stays alive during rename.
        TArray<uint8> EditorBytes;
        {
            TStrongObjectPtr<USaveGame> Original(UGameplayStatics::CreateSaveGameObject(BP->GeneratedClass.Get()));
            CheckedProperty<FIntProperty>(BP->GeneratedClass,
                                          TEXT("Score"))->SetPropertyValue_InContainer(Original.Get(), 73);
            Check(UGameplayStatics::SaveGameToMemory(Original.Get(), EditorBytes),
                  TEXT("editor_original_rename_bytes"));
        }
        CollectGarbage(RF_NoFlags);
        if (!FFileHelper::SaveStringToFile(PackageSource(Probe, true), *Path,
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            !Check(FDasDSLService::Reimport(BP, Error, EDasDefaultMigrationPolicy::Automatic),
                   TEXT("rename_by_stable_id"), Error))
            return 1;
        Check(Before.IsValid() &&
              Before == Guid(BP, TEXT("Points")) &&
                             !FindFProperty<FProperty>(BP->GeneratedClass, TEXT("Score")),
                                                       TEXT("rename_preserves_blueprint_variable_guid"));
        {
            TStrongObjectPtr<USaveGame> Restored(UGameplayStatics::LoadGameFromMemory(EditorBytes));
            const auto* P = FindFProperty<FIntProperty>(BP->GeneratedClass, TEXT("Points"));
            const int32 Value = Restored.IsValid() && P ? P->GetPropertyValue_InContainer(Restored.Get()) : MIN_int32;
            Check(Value == 73, TEXT("editor_savegame_guid_redirect_preserves_renamed_value"));
            Report->SetNumberField(TEXT("editor_renamed_loaded"), Value);
        }
        Check(SaveAsset(BP), TEXT("save_renamed_blueprint_same_package"));
        Report->SetStringField(TEXT("variable_guid"), Guid(BP, TEXT("Points")).ToString());
        Report->SetStringField(TEXT("compiled_guid_after"),
                                    BP->GeneratedClass->FindPropertyGuidFromName(TEXT("Points")).ToString());
    }
    Report->SetNumberField(TEXT("checks"), Checks);
    Report->SetStringField(TEXT("state"), Failures ? TEXT("FAIL"): TEXT("PASS"));
    if (!Json(Report, Output))
        return 2;
    UE_LOG(LogTemp, Display,
           TEXT("DAS_SAVEGAME_COMPAT_%s=%s checks=%d"), *Phase.ToUpper(),
                Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
