#pragma once
#include "Engine/World.h"
#include "Misc/ScopeExit.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/StaticMeshComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "GameFramework/Character.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
inline bool SaveDasPlayableDemo(const FString& Root,UClass* CharacterClass,FString& Error){
 auto* World=UWorld::CreateWorld(EWorldType::Editor,false,TEXT("PlayableMap"),CreatePackage(*(Root+TEXT("/PlayableMap"))));World->SetFlags(RF_Public|RF_Standalone);World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
 ON_SCOPE_EXIT {World->DestroyWorld(false);};
 auto* Cube=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));if(!Cube){Error=TEXT("Missing Engine cube for playable demo");return false;}
 auto Block=[&](FVector Location,FVector Scale){auto* A=World->SpawnActor<AStaticMeshActor>();A->GetStaticMeshComponent()->SetStaticMesh(Cube);A->SetActorLocation(Location);A->SetActorScale3D(Scale);A->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);A->GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECR_Block);return A;};
 Block(FVector(0,0,-50),FVector(100,100,1));Block(FVector(600,0,100),FVector(2,2,2));Block(FVector(600,500,50),FVector(2,2,1));Block(FVector(1000,500,150),FVector(2,2,3));
 auto* Light=World->SpawnActor<ADirectionalLight>();Light->SetActorRotation(FRotator(-45,-30,0));Light->GetLightComponent()->SetIntensity(3.f);
 auto* Sky=World->SpawnActor<ASkyLight>();Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);Sky->GetLightComponent()->Intensity=1.f;
 FActorSpawnParameters Spawn;Spawn.Name=TEXT("DAS_Player");Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;auto* Player=World->SpawnActor<ACharacter>(CharacterClass,FTransform(FVector(0,0,100)),Spawn);if(!Player){Error=TEXT("Cannot spawn playable Character");return false;}Player->AutoPossessPlayer=EAutoReceiveInput::Player0;Player->AutoPossessAI=EAutoPossessAI::Disabled;
 const FString File=FPackageName::LongPackageNameToFilename(Root+TEXT("/PlayableMap"),FPackageName::GetMapPackageExtension());FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(World->GetOutermost(),World,*File,Args)){Error=TEXT("Cannot save playable map");return false;}
 UE_LOG(LogTemp,Display,TEXT("DAS_PLAYABLE_CHARACTER_MAP=%s/PlayableMap"),*Root);return true;
}
