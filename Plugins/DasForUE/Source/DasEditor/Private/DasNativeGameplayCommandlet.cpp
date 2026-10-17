#include "DasNativeGameplayCommandlet.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
UDasNativeGameplayCommandlet::UDasNativeGameplayCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasNativeGameplayCommandlet::Main(const FString& Params){FString Output,Error;if(!FParse::Value(*Params,TEXT("Output="),Output))return 1;if(!FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).ExportNativeGameplay(Output,Error)){UE_LOG(LogTemp,Error,TEXT("DAS_NATIVE_GAMEPLAY_EXPORT=FAIL %s"),*Error);return 1;}UE_LOG(LogTemp,Display,TEXT("DAS_NATIVE_GAMEPLAY_EXPORT=PASS"));return 0;}
