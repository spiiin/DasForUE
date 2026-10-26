#include "DasNativeGameplayCommandlet.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/StrongObjectPtr.h"
UDasNativeGameplayCommandlet::UDasNativeGameplayCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasNativeGameplayCommandlet::Main(const FString& Params)
{
    if (FParse::Param(*Params, TEXT("Coverage")))
    {
        FString Output, Profile, Error;
        if (!FParse::Value(*Params, TEXT("Output="), Output) ||
                           !FParse::Value(*Params, TEXT("Profile="), Profile) ||
                                          !FModuleManager::LoadModuleChecked<FDasRuntimeModule>(
                                              TEXT("DasRuntime")).ExportNativeGameplayCoverage(Output, Profile, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_API_COVERAGE_EXPORT=FAIL %s"), *Error);
            return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("DAS_API_COVERAGE_EXPORT=PASS"));
        return 0;
    }
#if DAS_WITH_EDITOR_TESTS
    if (FParse::Param(*Params, TEXT("Probe")))
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        auto* World = UWorld::CreateWorld(EWorldType::Game, false);
        if (!World)
            return 1;
        TStrongObjectPtr<UWorld> Keep(World);
        auto* Class = FindObject<UClass>(nullptr, TEXT("/Script/Engine.Character"));
        auto* Actor = Class ? World->SpawnActor<AActor>(Class) : nullptr;
        if (!Actor)
        {
            World->DestroyWorld(false);
            return 1;
        }
        const auto Result = Runtime.RunFile(Runtime.ScriptPath(TEXT("native_discovery_probe.das")), 0, false, Actor);
        Runtime.ReleaseObject(Actor);
        World->DestroyWorld(false);
        if (!Result.bSucceeded || Result.Value != 1)
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_NATIVE_DISCOVERY_RUNTIME=FAIL %s"), *Result.Error);
            return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_DISCOVERY_RUNTIME=PASS"));
        return 0;
    }
#endif
    FString Output, Requests, Error;
    if (!FParse::Value(*Params, TEXT("Output="), Output) || !FParse::Value(*Params, TEXT("Requests="), Requests))
    {
        UE_LOG(LogTemp, Error, TEXT("DAS_NATIVE_GAMEPLAY_EXPORT=FAIL Output= and Requests= are required"));
        return 1;
    }
    if (!FModuleManager::LoadModuleChecked<FDasRuntimeModule>(
            TEXT("DasRuntime")).ExportNativeGameplay(Output, Requests, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("DAS_NATIVE_GAMEPLAY_EXPORT=FAIL %s"), *Error);
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_GAMEPLAY_EXPORT=PASS"));
    return 0;
}
