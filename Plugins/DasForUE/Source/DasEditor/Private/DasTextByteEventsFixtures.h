#pragma once
#include "DasInterfaceService.h"
#include "DasDSLService.h"
#include "DasStaticLibraryProbe.h"
#include "Engine/Blueprint.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
inline bool PrepareDasTextByteEventsFixtures(const FString& Root,ADasStaticLibraryProbe* Carrier,TArray<UBlueprint*>& Assets,FString& Error,bool SaveAssets){
 TArray<UBlueprint*> Interfaces;if(!FDasInterfaceService::Apply(TEXT("dsl_text_byte_interface.das"),TEXT("/Game/DasTextByteEvents/Types"),Interfaces,Error))return false;
 UBlueprint* BP=nullptr;if(!FDasDSLService::Apply(TEXT("dsl_text_byte_events.das"),Root+TEXT("/BP_TextByteEvents"),TEXT("dsl_text_byte_events.TextByteEvents"),BP,Error) || !FDasDSLService::Reimport(BP,Error))return false;
 Assets.Append(Interfaces);Assets.Add(BP);Carrier->TextByteEventsClass=BP->GeneratedClass;
 if(SaveAssets){Interfaces.Add(BP);for(auto* Asset:Interfaces){FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;const FString File=FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);if(!UPackage::SavePackage(Asset->GetOutermost(),Asset,*File,Save)){Error=TEXT("Cannot save text/byte events fixture");return false;}}}return true;
}
