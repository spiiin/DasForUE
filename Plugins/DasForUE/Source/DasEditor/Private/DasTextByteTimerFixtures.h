#pragma once
#include "DasDSLService.h"
#include "DasTypeService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasStaticLibraryProbe.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Modules/ModuleManager.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
inline bool PrepareDasTextByteTimerFixtures(const FString& Root,ADasStaticLibraryProbe* Carrier,TArray<UBlueprint*>& Assets,FString& Error,bool SaveAssets){
 TArray<UObject*> ValueTypes;if(!FDasTypeService::Apply(TEXT("ue_callback_value_types.das"),TEXT("/Game/DasCallbackObjects/Types"),ValueTypes,Error))return false;
 if(SaveAssets)for(auto* Asset:ValueTypes){FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;const FString File=FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);if(!UPackage::SavePackage(Asset->GetOutermost(),Asset,*File,Save)){Error=TEXT("Cannot save callback value types");return false;}}
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));for(const auto* File:{TEXT("dsl_text_byte_timers_bad_ref.das"),TEXT("dsl_text_byte_timers_bad_array.das"),TEXT("dsl_multi_callbacks_bad_ref.das"),TEXT("dsl_multi_callbacks_bad_count.das"),TEXT("dsl_multi_callbacks_bad_default.das")}){FDasAuthoringModule Model;FString Issue;if(R.ReadAuthoringScript(R.ScriptPath(File),Model,Issue) || Issue.IsEmpty()){Error=TEXT("Invalid timer callback accepted: ")+FString(File);return false;}}
 UBlueprint* ContainerBP=nullptr;if(!FDasDSLService::Apply(TEXT("dsl_callback_containers.das"),Root+TEXT("/BP_ContainerCallbacks"),TEXT("dsl_callback_containers.ContainerCallbackActor"),ContainerBP,Error))return false;Assets.Add(ContainerBP);
 FActorSpawnParameters ContainerSpawn;ContainerSpawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;Carrier->ContainerCallbackActor=Carrier->GetWorld()->SpawnActor<AActor>(ContainerBP->GeneratedClass,FTransform::Identity,ContainerSpawn);if(!Carrier->ContainerCallbackActor)return false;
 if(SaveAssets){FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;const FString File=FPackageName::LongPackageNameToFilename(ContainerBP->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);if(!UPackage::SavePackage(ContainerBP->GetOutermost(),ContainerBP,*File,Save))return false;}
 UBlueprint* BP=nullptr;if(!FDasDSLService::Apply(TEXT("dsl_text_byte_timers.das"),Root+TEXT("/BP_TextByteTimers"),TEXT("dsl_text_byte_timers.TextByteTimerActor"),BP,Error))return false;Assets.Add(BP);
 FActorSpawnParameters Spawn;Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;Carrier->TextByteTimerActor=Carrier->GetWorld()->SpawnActor<AActor>(BP->GeneratedClass,FTransform::Identity,Spawn);if(!Carrier->TextByteTimerActor){Error=TEXT("Cannot create timer actor");return false;}
 if(SaveAssets){FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;const FString File=FPackageName::LongPackageNameToFilename(BP->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);if(!UPackage::SavePackage(BP->GetOutermost(),BP,*File,Save)){Error=TEXT("Cannot save timer class");return false;}}return true;
}
