#include "CoreMinimal.h"
#include "HAL/PlatformProperties.h"
#include "DasRuntime.h"
#include "DasSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

void StartDasSaveGameCompatibilityValidation()
{
    FString Phase, ClassPath, File, Script, Observation;
    const TCHAR* Cmd = FCommandLine::Get();
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Name)
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_SAVEGAME_COMPAT_PACKAGED_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
        return Good;
    };
    bool Args = FParse::Value(
        Cmd, TEXT("CompatibilityPhase="), Phase) &&
        FParse::Value(Cmd, TEXT("CompatibilityClass="), ClassPath) &&
                      FParse::Value(Cmd, TEXT("CompatibilityFile="), File) &&
                                    FParse::Value(Cmd, TEXT("CompatibilityScript="), Script) &&
                                                  FParse::Value(Cmd, TEXT("CompatibilityObservation="), Observation);
    UClass* Class = Args ? LoadObject<UClass>(nullptr, *ClassPath) : nullptr;
    if (!Check(Class && Class->IsChildOf(UDasSaveGame::StaticClass()) && FPlatformProperties::RequiresCookedData(),
               TEXT("real_cooked_authored_savegame_class")))
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_SAVEGAME_COMPAT_PACKAGED_RESULT=FAIL checks=%d"), Checks);
        FPlatformMisc::RequestExitWithStatus(false, 1);
        return;
    }
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Value = MIN_int32, Control = MIN_int32;
    TArray<uint8> Bytes;
    if (Phase == TEXT("Write"))
    {
        const auto* Score = FindFProperty<FIntProperty>(Class, TEXT("Score"));
        const auto* Keep = FindFProperty<FIntProperty>(Class, TEXT("Control"));
        if (Check(Score && Keep && !FindFProperty<FProperty>(Class, TEXT("Points")), TEXT("old_cooked_schema")))
        {
            TStrongObjectPtr<USaveGame> Game(UGameplayStatics::CreateSaveGameObject(Class));
            Score->SetPropertyValue_InContainer(Game.Get(), 73);
            Keep->SetPropertyValue_InContainer(Game.Get(), 123);
            const auto Read =
                R.RunNamedPersistent(R.ScriptPath(Script), TEXT("ue_CompatibilitySave_Read"), 0, Game.Get());
            Check(Read.bSucceeded && Read.Value == 73, TEXT("old_packaged_DAS_reads_original_field"));
            R.ReleaseObject(Game.Get());
            Check(UGameplayStatics::SaveGameToMemory(Game.Get(), Bytes) && FFileHelper::SaveArrayToFile(Bytes, *File),
                  TEXT("write_standard_GVAS_bytes"));
            Value = Score->GetPropertyValue_InContainer(Game.Get());
            Control = Keep->GetPropertyValue_InContainer(Game.Get());
        }
    }
    else if (Phase == TEXT("Read"))
    {
        const auto* Points = FindFProperty<FIntProperty>(Class, TEXT("Points"));
        const auto* Keep = FindFProperty<FIntProperty>(Class, TEXT("Control"));
        if (Check(Points && Keep &&
                  !FindFProperty<FProperty>(Class, TEXT("Score")), TEXT("new_cooked_schema_same_class_path")) &&
                                            Check(FFileHelper::LoadFileToArray(Bytes, *File) && !Bytes.IsEmpty(),
                                                  TEXT("read_original_package_save_bytes")))
        {
            TStrongObjectPtr<USaveGame> Loaded(UGameplayStatics::LoadGameFromMemory(Bytes));
            if (Check(Loaded.IsValid() && Loaded->GetClass() == Class, TEXT("old_save_loads_new_cooked_class")))
            {
                Value = Points->GetPropertyValue_InContainer(Loaded.Get());
                Control = Keep->GetPropertyValue_InContainer(Loaded.Get());
                Check(Control == 123, TEXT("unchanged_control_value_survives"));
                // Characterize the stock serializer, not a new migration policy: its archive
                // doesn't set ArIsSaveGame, so cooked tagged loading doesn't use Blueprint GUIDs.
                Check(Value == 20, TEXT("stock_cooked_rename_uses_new_default_instead_of_old_73"));
                const auto Read =
                    R.RunNamedPersistent(R.ScriptPath(Script), TEXT("ue_CompatibilitySave_Read"), 0, Loaded.Get());
                Check(Read.bSucceeded && Read.Value == 20, TEXT("new_packaged_DAS_reads_renamed_field"));
                R.ReleaseObject(Loaded.Get());
            }
        }
    }
    else
        Check(false, TEXT("known_phase"));
    const FString Json = FString::Printf(TEXT("{\"phase\":\"%s\",\"state\":\"%s\",\"checks\":%d,\"value\":%d,\"control\":%d,\"requires_cooked_data\":true,\"property_guids_available\":%s}"),
        *Phase,
        Failures
        ? TEXT("FAIL")
        : TEXT("PASS"), Checks + 1, Value, Control, Class->ArePropertyGuidsAvailable() ? TEXT("true"): TEXT("false"));
    Check(FFileHelper::SaveStringToFile(Json, *Observation, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
          TEXT("persist_observations"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_SAVEGAME_COMPAT_PACKAGED_OBSERVED phase=%s value=%d control=%d guids=%d"), *Phase, Value, Control,
                Class->ArePropertyGuidsAvailable());
    UE_LOG(LogTemp, Display,
           TEXT("DAS_SAVEGAME_COMPAT_PACKAGED_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    FPlatformMisc::RequestExitWithStatus(false, Failures ? 1 : 0);
}
