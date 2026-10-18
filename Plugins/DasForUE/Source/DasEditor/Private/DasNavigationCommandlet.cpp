#include "DasNavigationCommandlet.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TargetPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BrushComponent.h"
#include "GameFramework/WorldSettings.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NavigationData.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Builders/CubeBuilder.h"
#include "Model.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/ScopeExit.h"
#include "AssetCompilingManager.h"
#include "Containers/Ticker.h"
UDasNavigationCommandlet::UDasNavigationCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasNavigationCommandlet::Main(const FString& Params){
 FString Probe;if(!FParse::Value(*Params,TEXT("Probe="),Probe))return 1;
 const auto IVS=UWorld::InitializationValues().CreatePhysicsScene(true).ShouldSimulatePhysics(false).AllowAudioPlayback(false).CreateNavigation(false).CreateAISystem(false);
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Editor,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&IVS));if(!World.IsValid())return 1;
 GEngine->CreateNewWorldContext(EWorldType::Editor).SetCurrentWorld(World.Get());
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 ATargetPoint* Owner=nullptr;
 ON_SCOPE_EXIT{if(World->HasBegunPlay())World->EndPlay(EEndPlayReason::Quit);if(Owner)Runtime.ReleaseObject(Owner);GEngine->DestroyWorldContext(World.Get());World->DestroyWorld(false);};
 auto Check=[](bool Good,const TCHAR* Name){UE_LOG(LogTemp,Display,TEXT("DAS_NAVIGATION_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);return Good;};
 auto* Cube=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));if(!Cube)return 1;
 auto Block=[&](FVector Location,FVector Scale){
  auto* A=World->SpawnActor<AStaticMeshActor>();
  A->GetStaticMeshComponent()->SetStaticMesh(Cube);A->SetActorLocation(Location);A->SetActorScale3D(Scale);
  A->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);A->GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECR_Block);
 };
 Block(FVector(0,0,-50),FVector(40,40,1));
 Block(FVector(0,0,150),FVector(2,6,3));
 Block(FVector(3000,0,-50),FVector(8,8,1));
 auto* Bounds=World->SpawnActor<ANavMeshBoundsVolume>();Bounds->SetActorLocation(FVector(1000,0,200));
 Bounds->Brush=NewObject<UModel>(Bounds);Bounds->Brush->Initialize(nullptr,true);Bounds->GetBrushComponent()->Brush=Bounds->Brush;
 auto* Builder=NewObject<UCubeBuilder>();Builder->X=8000;Builder->Y=4400;Builder->Z=1000;
 if(!Builder->Build(World.Get(),Bounds))return 1;Bounds->GetBrushComponent()->UpdateBounds();
 FNavigationSystem::AddNavigationSystemToWorld(*World,FNavigationSystemRunMode::EditorMode);
 auto* Nav=FNavigationSystem::GetCurrent<UNavigationSystemV1>(World.Get());if(!Nav)return 1;
 // Commandlets have no editor frame loop: finish assets and service UE's delayed unlock.
 FAssetCompilingManager::Get().FinishAllCompilation();
 for(int32 Frame=0;Frame<64 && Nav->IsNavigationBuildingLocked();++Frame)FTSTicker::GetCoreTicker().Tick(0.125f);
 if(!Check(!Nav->IsNavigationBuildingLocked(),TEXT("asset_compilation_navigation_unlock")))return 1;
 Nav->OnNavigationBoundsUpdated(Bounds);Nav->Build();
 auto* Data=Nav->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);if(!Data)return 1;Data->EnsureBuildCompletion();
 const FVector Start(-600,0,50),End(600,0,50),Island(3000,0,50),Extent(100,100,200);
 FNavLocation PStart,PEnd,PIsland;
 if(!Check(Nav->ProjectPointToNavigation(Start,PStart,Extent) && Nav->ProjectPointToNavigation(End,PEnd,Extent) && Nav->ProjectPointToNavigation(Island,PIsland,Extent),TEXT("recast_covers_start_goal_and_disconnected_island")))return 1;
 // Build in EditorMode, then execute generated calls with a real live Game world context.
 World->WorldType=EWorldType::Game;GEngine->GetWorldContextFromWorldChecked(World.Get()).WorldType=EWorldType::Game;
 World->InitializeActorsForPlay(FURL());Owner=World->SpawnActor<ATargetPoint>(Start,FRotator::ZeroRotator);if(!Owner)return 1;
 World->BeginPlay();World->GetWorldSettings()->NotifyBeginPlay();
 TStrongObjectPtr<UNavigationPath> NativePath(UNavigationSystemV1::FindPathToLocationSynchronously(Owner,Start,End,Owner));
 double Cost=0,Length=0;
 const auto CostResult=UNavigationSystemV1::GetPathCost(Owner,Start,End,Cost,nullptr,nullptr);
 const auto LengthResult=UNavigationSystemV1::GetPathLength(Owner,Start,End,Length,nullptr,nullptr);
 if(!Check(NativePath.IsValid() && NativePath->IsValid() && !NativePath->IsPartial() && NativePath->GetPathLength()>1200 && CostResult==ENavigationQueryResult::Success && LengthResult==ENavigationQueryResult::Success && Cost>0 && Length>1200,TEXT("native_route_detours_around_obstacle")))return 1;
 UE_LOG(LogTemp,Display,TEXT("DAS_NAVIGATION_METRICS path_length=%.3f query_length=%.3f cost=%.3f"),NativePath->GetPathLength(),Length,Cost);
 for(int32 Phase=0;Phase<3;++Phase){
  const auto Result=Runtime.RunFile(Probe,Phase,false,Owner);
  if(!Check(Result.bSucceeded && Result.Value==42,Phase==0?TEXT("generated_project_random_and_raycast"):Phase==1?TEXT("generated_synchronous_paths_cost_and_length"):TEXT("generated_disconnected_island_and_projection_failure"))){UE_LOG(LogTemp,Error,TEXT("DAS_NAVIGATION_SCRIPT_ERROR %s"),*Result.Error);return 1;}
 }
 UE_LOG(LogTemp,Display,TEXT("DAS_NAVIGATION_RUNTIME=PASS"));return 0;
}
