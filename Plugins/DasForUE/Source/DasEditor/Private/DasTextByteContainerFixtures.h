#pragma once
#include "DasTextByteEventsFixtures.h"
#include "DasTypeService.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasStaticLibraryProbe.h"
#include "Engine/Blueprint.h"
#include "Modules/ModuleManager.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
inline bool PrepareDasTextByteContainerFixtures(const FString& Root,ADasStaticLibraryProbe* Carrier,TArray<UBlueprint*>& Assets,FString& Error,bool SaveAssets){
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));for(const auto* File:{TEXT("dsl_text_byte_containers_bad_text_key.das"),TEXT("dsl_text_byte_containers_bad_text_set.das"),TEXT("dsl_text_byte_containers_bad_nested.das")}){FDasAuthoringModule Model;FString Issue;if(R.ReadAuthoringScript(R.ScriptPath(File),Model,Issue) || Issue.IsEmpty()){Error=TEXT("Accepted invalid text/byte container shape: ")+FString(File);return false;}}
 TArray<UObject*> Types;if(!FDasTypeService::Apply(TEXT("ue_text_byte_container_types.das"),TEXT("/Game/DasTextByteContainers/Types"),Types,Error))return false;
 UBlueprint* BP=nullptr;if(!FDasDSLService::Apply(TEXT("dsl_text_byte_containers.das"),Root+TEXT("/BP_TextByteContainers"),TEXT("dsl_text_byte_containers.TextByteContainers"),BP,Error) || !FDasDSLService::Reimport(BP,Error))return false;Assets.Add(BP);Carrier->TextByteContainerClass=BP->GeneratedClass;
 if(SaveAssets){Types.Add(BP);for(auto* Asset:Types){FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;const FString File=FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);if(!UPackage::SavePackage(Asset->GetOutermost(),Asset,*File,Save)){Error=TEXT("Cannot save text/byte container fixture");return false;}}}
 return PrepareDasTextByteEventsFixtures(Root,Carrier,Assets,Error,SaveAssets);
}
