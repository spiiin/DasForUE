#include "DasSmokeCommandlet.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"

UDasSmokeCommandlet::UDasSmokeCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}

int32 UDasSmokeCommandlet::Main(const FString& Params)
{
    FString Report;
    const bool Success = RunDasFoundationTests(Report);
    UE_LOG(LogTemp, Display, TEXT("%s"), *Report);
    return Success ? 0 : 1;
}

IMPLEMENT_MODULE(FDefaultModuleImpl, DasEditor)
