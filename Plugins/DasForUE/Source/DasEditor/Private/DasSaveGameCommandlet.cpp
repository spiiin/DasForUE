#include "DasSaveGameCommandlet.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasSaveGame.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "UObject/UnrealType.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Async/TaskGraphInterfaces.h"
#include "Containers/Ticker.h"

UDasSaveGameCommandlet::UDasSaveGameCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasSaveGameCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Ok, const TCHAR* Label)
    {
        ++Checks;
        if (!Ok)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_SAVEGAME_%s %s"), Ok ? TEXT("PASS"): TEXT("FAIL"), Label);
        return Ok;
    };
    FString Error;
    FDasAuthoringModule Model;
    if (!Check(R.ReadAuthoringScript(R.ScriptPath(TEXT("savegame_profile.das")), Model, Error),
                                     TEXT("author_savegame_inheritance_and_methods")))
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Error);
        return 1;
    }
    const FString Tag = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Base = TEXT("DasSaveGame_")+ Tag;
    const TArray<FString> Slots = {Base + TEXT("_sync"), Base + TEXT("_async"), Base + TEXT("_missing"),
                                                                                            Base + TEXT("_cancel")};
    UBlueprint* BP = nullptr;
    if (!Check(Model.Classes.Num() == 1 &&
               FDasDSLService::Apply(TEXT("savegame_profile.das"),
                                          TEXT("/Game/Tests/DasSaveGame/")+ Base, Model.Classes[0].Id, BP, Error),
                                     TEXT("import_authored_savegame_blueprint")))
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Error);
        return 1;
    }
    TStrongObjectPtr<UBlueprint> Keep(BP);
    auto* Field = FindFProperty<FIntProperty>(BP->GeneratedClass, TEXT("Score"));
    Check(BP->GeneratedClass->IsChildOf(UDasSaveGame::StaticClass()) && Field &&
              Field->HasAnyPropertyFlags(CPF_SaveGame),
          TEXT("savegame_host_and_property_flags"));
    // The same adapter supports classes with data only, without a dummy method.
    auto DataOnly = Model;
    DataOnly.Classes[0].Methods.Reset();
    FDasClassSchema Schema;
    Check(DataOnly.ToActorSchema(DataOnly.Classes[0].Id, Schema, Error) && Schema.EffectiveMethods().IsEmpty(),
          TEXT("data_only_schema"));
    FString Template;
    FString Fixture;
    if (!FParse::Value(*Params, TEXT("Fixture="), Fixture) || !FFileHelper::LoadFileToString(Template, *Fixture))
        return 2;
    Template.ReplaceInline(TEXT("__CLASS__"), *BP->GeneratedClass->GetPathName());
    for (int32 I = 0; I < Slots.Num(); ++I)
        Template.ReplaceInline(*FString::Printf(TEXT("__SLOT_%s__"), I == 0 ? TEXT("SYNC"): I == 1 ? TEXT("ASYNC"): I == 2 ? TEXT("MISSING"): TEXT("CANCEL")),
            *Slots[I]);
    const FString Script = TEXT("savegame_probe_")+ Tag + TEXT(".das"), Path = R.ScriptPath(Script);
    if (!FFileHelper::SaveStringToFile(Template, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path);
        for (const auto& Slot : Slots)
            if (UGameplayStatics::DoesSaveGameExist(Slot, 0))
                UGameplayStatics::DeleteGameInSlot(Slot, 0);
    };
    TStrongObjectPtr<UWorld> W(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(W.Get());
    TStrongObjectPtr<AActor> Owner(W->SpawnActor<AActor>());
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(Owner.Get());
        GEngine->DestroyWorldContext(W.Get());
        W->DestroyWorld(false);
    };
    auto Call = [&](const TCHAR* Entry)
    {
        auto V = R.RunNamedPersistent(Path, Entry, 0, Owner.Get());
        if (!V.bSucceeded)
            UE_LOG(LogTemp, Display, TEXT("SaveGame VM: %s"), *V.Error);
        return V;
    };
    auto Pump = [&](TFunctionRef<bool()> Done)
    {
        const double End = FPlatformTime::Seconds() + 5.;
        while (FPlatformTime::Seconds() < End)
        {
            FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
            FTSTicker::GetCoreTicker().Tick(.01f);
            if (Done())
                return true;
            FPlatformProcess::Sleep(.01f);
        }
        return false;
    };
    const auto Start = Call(TEXT("start"));
    if (!Check(Start.bSucceeded && Start.Value == 1,
               TEXT("create_methods_sync_save_load_exists_delete_and_async_start")))
        return 1;
    Check(Pump(
              [&]
              {
                  const auto V = Call(TEXT("status"));
                  return V.bSucceeded && V.Value == 1;
              }),
          TEXT("async_save_then_load_callback_and_pre_mutation_snapshot"));
    CollectGarbage(RF_NoFlags);
    const auto Read = Call(TEXT("retained_after_gc"));
    Check(Read.bSucceeded && Read.Value == 1, TEXT("loaded_object_survives_callback_and_gc"));
    const auto Missing = Call(TEXT("load_missing"));
    Check(Missing.bSucceeded && Missing.Value == 1 &&
              Pump(
                  [&]
                  {
                      const auto V = Call(TEXT("status"));
                      return V.bSucceeded && V.Value == 2;
                  }),
          TEXT("missing_slot_delivers_null_handle"));
    const auto Cancel = Call(TEXT("cancel_write"));
    Check(Cancel.bSucceeded && Cancel.Value == 1, TEXT("cancel_removes_delivery_registration"));
    Check(Pump([&] { return UGameplayStatics::DoesSaveGameExist(Slots[3], 0); }),
          TEXT("cancelled_delivery_does_not_cancel_native_write"));
    // Flush native completion before cleaning up our unique slot.
    FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
    const auto Final = Call(TEXT("status"));
    Check(Final.bSucceeded && Final.Value == 2, TEXT("cancelled_callback_is_not_invoked"));
    UE_LOG(LogTemp, Display, TEXT("DAS_SAVEGAME_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
