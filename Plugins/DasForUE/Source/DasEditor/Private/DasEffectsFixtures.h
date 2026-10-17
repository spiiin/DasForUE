#pragma once
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "UObject/SavePackage.h"
#include "UObject/UnrealType.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
// Duplicate only the demo's Niagara assets; Engine/plugin templates stay untouched.
inline bool PrepareDasDemoEffects(const FString& Root,AActor* Player,AActor* Pickup,FString& Error){
 auto* Sound=LoadObject<USoundBase>(nullptr,TEXT("/Engine/EngineSounds/1kSineTonePing.1kSineTonePing"));
 auto* Template=LoadObject<UNiagaraSystem>(nullptr,TEXT("/Niagara/DefaultAssets/Templates/Systems/RadialBurst.RadialBurst"));
 if(!Sound || !Template){Error=TEXT("Missing Engine sound/Niagara demo template");return false;}
 auto Set=[](UObject* O,const TCHAR* Name,UObject* Value){auto* P=FindFProperty<FObjectPropertyBase>(O->GetClass(),Name);if(!P)return false;P->SetObjectPropertyValue_InContainer(O,Value);return true;};
 auto Make=[&](const TCHAR* Name){auto* System=DuplicateObject<UNiagaraSystem>(Template,CreatePackage(*(Root+TEXT("/")+Name)),Name);if(!System)return static_cast<UNiagaraSystem*>(nullptr);System->SetFlags(RF_Public|RF_Standalone);System->WaitForCompilationComplete(true,false);
  const FString File=FPackageName::LongPackageNameToFilename(System->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(System->GetOutermost(),System,*File,Args))return static_cast<UNiagaraSystem*>(nullptr);return System;};
 auto* PickupSystem=Make(TEXT("NS_Pickup"));auto* HurtSystem=Make(TEXT("NS_Hurt"));
 if(!PickupSystem || !HurtSystem || !Set(Pickup,TEXT("PickupSound"),Sound) || !Set(Pickup,TEXT("PickupVFX"),PickupSystem) || !Set(Player,TEXT("HurtSound"),Sound) || !Set(Player,TEXT("HurtVFX"),HurtSystem)){Error=TEXT("Cannot save/assign demo effects");return false;}return true;
}
