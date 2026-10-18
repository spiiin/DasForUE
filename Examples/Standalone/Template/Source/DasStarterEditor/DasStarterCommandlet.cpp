#include "DasStarterCommandlet.h"
#include "DasDSLService.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Engine/DirectionalLight.h"
#include "Components/LightComponent.h"
#include "UObject/SavePackage.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectIterator.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
UDasStarterCommandlet::UDasStarterCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasStarterCommandlet::Main(const FString& Params){
 FString Error;UBlueprint *Logic=nullptr,*Character=nullptr,*Pickup=nullptr;
 auto Fail=[&](){UE_LOG(LogTemp,Error,TEXT("DAS_STARTER_EXPORT=FAIL %s"),*Error);return 1;};
 // Upgrade lighting in an existing map without recreating the authored scene.
 if(FParse::Param(*Params,TEXT("LightingOnly"))){
  auto* Package=LoadPackage(nullptr,TEXT("/Game/Starter/StarterMap"),LOAD_None);
  auto* World=Package?UWorld::FindWorldInPackage(Package):nullptr;
  if(!World || !World->PersistentLevel){Error=TEXT("Cannot load existing StarterMap");return Fail();}
  Package->FullyLoad();int32 Changed=0;
  for(const auto& Actor:World->PersistentLevel->Actors)if(auto* Light=Cast<ADirectionalLight>(Actor)){
   Light->GetLightComponent()->SetMobility(EComponentMobility::Movable);++Changed;
  }
  if(!Changed){Error=TEXT("StarterMap has no directional light");return Fail();}
  const FString File=FPackageName::LongPackageNameToFilename(Package->GetName(),FPackageName::GetMapPackageExtension());
  FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;
  if(!UPackage::SavePackage(Package,World,*File,Args)){Error=TEXT("Cannot save lighting update");return Fail();}
  UE_LOG(LogTemp,Display,TEXT("DAS_STARTER_LIGHTING_UPDATE=PASS lights=%d"),Changed);return 0;
 }
 if(!FDasDSLService::Apply(TEXT("starter_component.das"),TEXT("/Game/Starter/BP_StarterLogic"),TEXT("starter_component.StarterLogic"),Logic,Error) ||
    !FDasDSLService::Apply(TEXT("starter_character.das"),TEXT("/Game/Starter/BP_Character"),TEXT("starter_character.StarterCharacter"),Character,Error) ||
    !FDasDSLService::Apply(TEXT("starter_pickup.das"),TEXT("/Game/Starter/BP_Pickup"),TEXT("starter_pickup.StarterPickup"),Pickup,Error))return Fail();
 auto Save=[](UObject* O,bool Map){const FString File=FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),Map?FPackageName::GetMapPackageExtension():FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;return UPackage::SavePackage(O->GetOutermost(),O,*File,Args);};
 for(TObjectIterator<UBlueprint> It;It;++It)if(It->GetOutermost()->GetName().StartsWith(TEXT("/Game/Starter/")) && !Save(*It,false)){Error=TEXT("Cannot save Blueprint");return Fail();}
 auto* W=UWorld::CreateWorld(EWorldType::Editor,false,TEXT("StarterMap"),CreatePackage(TEXT("/Game/Starter/StarterMap")));if(!W){Error=TEXT("Cannot create world");return Fail();}W->SetFlags(RF_Public|RF_Standalone);W->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
 auto* Cube=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));auto* Floor=W->SpawnActor<AStaticMeshActor>();Floor->GetStaticMeshComponent()->SetStaticMesh(Cube);Floor->SetActorLocation(FVector(0,0,-50));Floor->SetActorScale3D(FVector(30,30,1));Floor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
 // This generated demo uses dynamic lighting and needs no baked lightmaps.
 auto* Light=W->SpawnActor<ADirectionalLight>();Light->GetLightComponent()->SetMobility(EComponentMobility::Movable);Light->SetActorRotation(FRotator(-45,-30,0));
 FActorSpawnParameters Spawn;Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;Spawn.Name=TEXT("Starter_Player");auto* Player=W->SpawnActor<ACharacter>(Character->GeneratedClass,FTransform(FVector(0,0,100)),Spawn);
 if(!Player){W->DestroyWorld(false);Error=TEXT("Cannot spawn Character");return Fail();}Player->AutoPossessPlayer=EAutoReceiveInput::Player0;Player->AutoPossessAI=EAutoPossessAI::Disabled;Player->GetCapsuleComponent()->SetGenerateOverlapEvents(true);
 Spawn.Name=TEXT("Starter_Pickup");auto* Item=W->SpawnActor<AActor>(Pickup->GeneratedClass,FTransform(FVector(300,0,100)),Spawn);auto* P=FindFProperty<FObjectPropertyBase>(Pickup->GeneratedClass,TEXT("Collector"));
 if(Item && P)P->SetObjectPropertyValue_InContainer(Item,Player);
 const bool Good=Item && P && Save(W,true);W->DestroyWorld(false);if(!Good){Error=TEXT("Cannot save starter map");return Fail();}
 UE_LOG(LogTemp,Display,TEXT("DAS_STARTER_EXPORT=PASS map=/Game/Starter/StarterMap"));return 0;
}
