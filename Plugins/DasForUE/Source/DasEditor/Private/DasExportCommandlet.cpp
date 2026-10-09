#include "DasExportCommandlet.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "Misc/Parse.h"
#include "DasAuthoredInterfaceClass.h"
#include "Engine/Blueprint.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "BlueprintCompilationManager.h"
UDasExportCommandlet::UDasExportCommandlet() { IsClient = false; IsServer = false; IsEditor = true; LogToConsole = true; }
int32 UDasExportCommandlet::Main(const FString& Params)
{
    FString Selection, Output, Error;
    if (!FParse::Value(*Params, TEXT("Selection="), Selection) || !FParse::Value(*Params, TEXT("Output="), Output)) return 1;
    FString Json;TSharedPtr<FJsonObject> SelectionObject;
    if(FFileHelper::LoadFileToString(Json,*Selection) && FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json),SelectionObject) && SelectionObject){
        TSet<FString> Prepared;for(const auto* Key:{TEXT("functions"),TEXT("class_rules")}){const TArray<TSharedPtr<FJsonValue>>* Items=nullptr;if(!SelectionObject->TryGetArrayField(Key,Items))continue;for(const auto& Item:*Items){const TSharedPtr<FJsonObject>* Entry=nullptr;FString ClassPath;if(!Item->TryGetObject(Entry) || !(*Entry)->TryGetStringField(TEXT("class"),ClassPath) || !ClassPath.StartsWith(TEXT("/Game/")) || !ClassPath.EndsWith(TEXT("_C")) || Prepared.Contains(ClassPath))continue;Prepared.Add(ClassPath);
            auto* BP=LoadObject<UDasAuthoredInterfaceBlueprint>(nullptr,*ClassPath.LeftChop(2),nullptr,LOAD_NoWarn);if(!BP || BP->StableId.IsEmpty()){UE_LOG(LogTemp,Error,TEXT("DAS_EXPORT_FAILED: Expected authored interface Blueprint"));return 1;}
            // BPI signature UFunctions are reconstructed by the editor compiler on load.
            FKismetEditorUtilities::CompileBlueprint(BP);FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();if(BP->Status==BS_Error){UE_LOG(LogTemp,Error,TEXT("DAS_EXPORT_FAILED: Authored interface compilation failed"));return 1;}
        }}
    }
    if (!FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).ExportBindings(Selection, Output, Error))
    { UE_LOG(LogTemp, Error, TEXT("DAS_EXPORT_FAILED: %s"), *Error); return 1; }
    UE_LOG(LogTemp, Display, TEXT("DAS_EXPORT_RESULT=PASS")); return 0;
}
