#pragma once
#include "DasDSLService.h"
#include "DasInterfaceService.h"
#include "DasGameplayWidgets.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "DasInterfaceService.h"
#include "DasGameplayWidgets.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "DasPlayableFixtures.h"
#include "Engine/World.h"
#include "Misc/ScopeExit.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "GameFramework/Character.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
inline bool SaveDasPlayableDemo(const FString& Root,UClass* CharacterClass,FString& Error,UClass* PickupClass=nullptr,UClass* WidgetClass=nullptr){
 auto* World=UWorld::CreateWorld(EWorldType::Editor,false,TEXT("PlayableMap"),CreatePackage(*(Root+TEXT("/PlayableMap"))));World->SetFlags(RF_Public|RF_Standalone);World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
 ON_SCOPE_EXIT {World->DestroyWorld(false);};
 auto* Cube=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));if(!Cube){Error=TEXT("Missing Engine cube for playable demo");return false;}
 auto Block=[&](FVector Location,FVector Scale){auto* A=World->SpawnActor<AStaticMeshActor>();A->GetStaticMeshComponent()->SetStaticMesh(Cube);A->SetActorLocation(Location);A->SetActorScale3D(Scale);A->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);A->GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECR_Block);return A;};
 Block(FVector(0,0,-50),FVector(100,100,1));Block(FVector(600,0,100),FVector(2,2,2));Block(FVector(600,500,50),FVector(2,2,1));Block(FVector(1000,500,150),FVector(2,2,3));
 auto* Light=World->SpawnActor<ADirectionalLight>();Light->SetActorRotation(FRotator(-45,-30,0));Light->GetLightComponent()->SetIntensity(3.f);
 auto* Sky=World->SpawnActor<ASkyLight>();Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);Sky->GetLightComponent()->Intensity=1.f;
 FActorSpawnParameters Spawn;Spawn.Name=TEXT("DAS_Player");Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;auto* Player=World->SpawnActor<ACharacter>(CharacterClass,FTransform(FVector(0,0,100)),Spawn);if(!Player){Error=TEXT("Cannot spawn playable Character");return false;}Player->AutoPossessPlayer=EAutoReceiveInput::Player0;Player->AutoPossessAI=EAutoPossessAI::Disabled;
 if(WidgetClass){auto* P=FindFProperty<FClassProperty>(CharacterClass,TEXT("HUDClass"));if(!P){Error=TEXT("HUD class property missing");return false;}P->SetObjectPropertyValue_InContainer(Player,WidgetClass);}
 if(PickupClass){
  Spawn.Name=TEXT("DAS_Pickup");auto* Pickup=World->SpawnActor<AActor>(PickupClass,FTransform(FVector(300,0,150)),Spawn);
  auto* Collector=FindFProperty<FObjectPropertyBase>(PickupClass,TEXT("Collector"));
  if(!Pickup || !Collector){Error=TEXT("Cannot create pickup/collector in playable map");return false;}
  Collector->SetObjectPropertyValue_InContainer(Pickup,Player);
  // Separate package: the floor's Cube is already resident when the game starts.
  // Only the soft property points at this copy; Visual stays empty in the map/CDO.
  auto* MeshProperty=FindFProperty<FSoftObjectProperty>(PickupClass,TEXT("MeshAsset"));
  if(!MeshProperty){Error=TEXT("Pickup mesh must be a typed soft object property");return false;}
  const FString MeshPackage=Root+TEXT("/PickupMesh");
  auto* PickupMesh=DuplicateObject<UStaticMesh>(Cube,CreatePackage(*MeshPackage),TEXT("PickupMesh"));
  if(!PickupMesh){Error=TEXT("Cannot create standalone pickup mesh");return false;}
  PickupMesh->SetFlags(RF_Public|RF_Standalone);
  const FString MeshFile=FPackageName::LongPackageNameToFilename(MeshPackage,FPackageName::GetAssetPackageExtension());
  FSavePackageArgs MeshArgs;MeshArgs.TopLevelFlags=RF_Public|RF_Standalone;
  if(!UPackage::SavePackage(PickupMesh->GetOutermost(),PickupMesh,*MeshFile,MeshArgs)){Error=TEXT("Cannot save pickup mesh");return false;}
  MeshProperty->SetPropertyValue_InContainer(Pickup,FSoftObjectPtr(FSoftObjectPath(PickupMesh->GetPathName())));
  Player->GetCapsuleComponent()->SetGenerateOverlapEvents(true);
 }
 const FString File=FPackageName::LongPackageNameToFilename(Root+TEXT("/PlayableMap"),FPackageName::GetMapPackageExtension());FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(World->GetOutermost(),World,*File,Args)){Error=TEXT("Cannot save playable map");return false;}
 UE_LOG(LogTemp,Display,TEXT("DAS_PLAYABLE_CHARACTER_MAP=%s/PlayableMap"),*Root);return true;
}

// User-facing example creation does not depend on the regression fixture exporter.
inline bool CreateDasStandalonePlayableDemo(const FString& Root,FString& Error){
 TArray<UBlueprint*> Interfaces;
 if(!FDasInterfaceService::Apply(TEXT("ue_interaction_contract.das"),TEXT("/Game/DasExamples/Shared"),Interfaces,Error))return false;
 UBlueprint* Character=nullptr;
 if(!FDasDSLService::Apply(TEXT("dsl_playable_character.das"),Root+TEXT("/BP_PlayableCharacter"),TEXT("dsl_playable_character.PlayableCharacter"),Character,Error))return false;
 if(!PrepareDasPlayableInputAssets(Root,Character,Error,true))return false;
 const FString File=FPackageName::LongPackageNameToFilename(Character->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());
 IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);
 FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;
 for(auto* Interface:Interfaces){const FString InterfaceFile=FPackageName::LongPackageNameToFilename(Interface->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(InterfaceFile),true);if(!UPackage::SavePackage(Interface->GetOutermost(),Interface,*InterfaceFile,Args)){Error=TEXT("Cannot save shared interaction interface");return false;}}
 if(!UPackage::SavePackage(Character->GetOutermost(),Character,*File,Args)){Error=TEXT("Cannot save playable Character example");return false;}
 UBlueprint* Pickup=nullptr;
 if(!FDasDSLService::Apply(TEXT("dsl_playable_pickup.das"),Root+TEXT("/BP_PlayablePickup"),TEXT("dsl_playable_pickup.PlayablePickup"),Pickup,Error))return false;
 const FString PickupFile=FPackageName::LongPackageNameToFilename(Pickup->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());
 if(!UPackage::SavePackage(Pickup->GetOutermost(),Pickup,*PickupFile,Args)){Error=TEXT("Cannot save playable pickup example");return false;}
 auto* HUD=FKismetEditorUtilities::CreateBlueprint(UDasGameplayHUD::StaticClass(),CreatePackage(*(Root+TEXT("/WBP_HUD"))),TEXT("WBP_HUD"),BPTYPE_Normal,UWidgetBlueprint::StaticClass(),UWidgetBlueprintGeneratedClass::StaticClass());
 if(!HUD){Error=TEXT("Cannot create HUD Widget Blueprint");return false;}
 FKismetEditorUtilities::CompileBlueprint(HUD);
 if(HUD->Status==BS_Error){Error=TEXT("Cannot compile HUD Widget Blueprint");return false;}
 // Save generated component/interface dependencies, as well as the two actor Blueprints.
 for(TObjectIterator<UBlueprint> It;It;++It){auto* B=*It;if(B!=Character && B!=Pickup && B->GetOutermost()->GetName().StartsWith(Root+TEXT("/"))){
  const FString DependencyFile=FPackageName::LongPackageNameToFilename(B->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());
  if(!UPackage::SavePackage(B->GetOutermost(),B,*DependencyFile,Args)){Error=TEXT("Cannot save playable dependency: ")+B->GetPathName();return false;}
 }}
 return SaveDasPlayableDemo(Root,Character->GeneratedClass,Error,Pickup->GeneratedClass,HUD->GeneratedClass);

}
