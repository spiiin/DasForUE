#include "DasExportCommandlet.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "Misc/Parse.h"
UDasExportCommandlet::UDasExportCommandlet() { IsClient = false; IsServer = false; IsEditor = true; LogToConsole = true; }
int32 UDasExportCommandlet::Main(const FString& Params)
{
    FString Selection, Output, Error;
    if (!FParse::Value(*Params, TEXT("Selection="), Selection) || !FParse::Value(*Params, TEXT("Output="), Output)) return 1;
    if (!FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).ExportBindings(Selection, Output, Error))
    { UE_LOG(LogTemp, Error, TEXT("DAS_EXPORT_FAILED: %s"), *Error); return 1; }
    UE_LOG(LogTemp, Display, TEXT("DAS_EXPORT_RESULT=PASS")); return 0;
}
