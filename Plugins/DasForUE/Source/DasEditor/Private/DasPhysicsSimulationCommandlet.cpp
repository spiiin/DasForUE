#include "DasPhysicsSimulationCommandlet.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/WorldSettings.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/ScopeExit.h"

UDasPhysicsSimulationCommandlet::UDasPhysicsSimulationCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasPhysicsSimulationCommandlet::Main(const FString& Params){
 FString Probe;if(!FParse::Value(*Params,TEXT("Probe="),Probe))return 1;
 const auto IVS=UWorld::InitializationValues().CreatePhysicsScene(true).ShouldSimulatePhysics(true).AllowAudioPlayback(false).CreateNavigation(false).CreateAISystem(false);
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&IVS));
 if(!World.IsValid() || !World->GetPhysicsScene())return 1;
 GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
 World->InitializeActorsForPlay(FURL());
 World->GetWorldSettings()->bGlobalGravitySet=true;World->GetWorldSettings()->GlobalGravityZ=-980.f;
 auto* Actor=World->SpawnActor<AActor>();
 auto* Box=NewObject<UBoxComponent>(Actor,TEXT("SimulatedBox"));
 Actor->SetRootComponent(Box);Actor->AddInstanceComponent(Box);Box->SetMobility(EComponentMobility::Movable);
 Box->SetBoxExtent(FVector(20));Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
 Box->SetCollisionObjectType(ECC_PhysicsBody);Box->SetCollisionResponseToAllChannels(ECR_Ignore);Box->RegisterComponent();
 auto* Handle=NewObject<UPhysicsHandleComponent>(Actor,TEXT("ProbeHandle"));Actor->AddInstanceComponent(Handle);Handle->RegisterComponent();
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 ON_SCOPE_EXIT{Handle->ReleaseComponent();Runtime.ReleaseObject(Actor);GEngine->DestroyWorldContext(World.Get());World->DestroyWorld(false);};
 World->BeginPlay();
 // This transient world has no GameMode to route StartPlay to actors.
 World->GetWorldSettings()->NotifyBeginPlay();
 if(!Actor->HasActorBegunPlay() || !Handle->HasBegunPlay() || !Handle->IsComponentTickEnabled()){UE_LOG(LogTemp,Error,TEXT("Physics fixture did not enter BeginPlay/tick"));return 1;}
 constexpr float Dt=1.f/60.f;
 auto Step=[&](int32 Frames){for(int32 I=0;I<Frames;++I){++GFrameCounter;World->Tick(LEVELTICK_All,Dt);}};
 auto Run=[&](int32 Phase){const auto R=Runtime.RunFile(Probe,Phase,false,Actor);if(!R.bSucceeded || R.Value!=42){UE_LOG(LogTemp,Error,TEXT("DAS_PHYSICS_PHASE_FAIL phase=%d value=%d error=%s"),Phase,R.Value,*R.Error);return false;}return true;};
 auto Check=[&](bool Good,const TCHAR* Name){UE_LOG(LogTemp,Display,TEXT("DAS_PHYSICS_%s %s position=%s velocity=%s angular=%s"),Good?TEXT("PASS"):TEXT("FAIL"),Name,*Box->GetComponentLocation().ToString(),*Box->GetPhysicsLinearVelocity().ToString(),*Box->GetPhysicsAngularVelocityInDegrees().ToString());return Good;};
 if(!Run(0))return 1;Step(30);
 if(!Check(Box->IsSimulatingPhysics() && FMath::IsNearlyEqual(Box->GetMass(),10.f,0.1f) && Box->GetComponentLocation().X>40 && Box->GetComponentLocation().X<60 && FMath::Abs(Box->GetComponentRotation().Yaw)>20,TEXT("linear_angular_velocity_and_mass")))return 1;
 FVector Before=Box->GetComponentLocation();if(!Run(1))return 1;Step(30);
 if(!Check(Box->GetPhysicsLinearVelocity().X>90 && Box->GetPhysicsLinearVelocity().X<110 && Box->GetComponentLocation().X-Before.X>40,TEXT("mass_scaled_impulse")))return 1;
 Before=Box->GetComponentLocation();
 for(int32 I=0;I<30;++I){if(!Run(I==0?2:3))return 1;Step(1);}
 if(!Check(Box->GetPhysicsLinearVelocity().X>40 && Box->GetPhysicsLinearVelocity().X<60 && Box->GetComponentLocation().X-Before.X>8 && Box->GetComponentLocation().X-Before.X<20,TEXT("force_over_real_physics_steps")))return 1;
 if(!Run(4))return 1;Step(60);
 if(!Check(Box->GetPhysicsLinearVelocity().Size()<10 && Box->GetPhysicsAngularVelocityInDegrees().Size()<10,TEXT("linear_and_angular_damping")))return 1;
 Before=Box->GetComponentLocation();if(!Run(5))return 1;Step(30);
 if(!Check(Box->GetPhysicsLinearVelocity().Z<-300 && Box->GetPhysicsLinearVelocity().Z>-650 && Box->GetComponentLocation().Z-Before.Z<-60,TEXT("gravity_changes_velocity_and_position")))return 1;
 const FQuat GrabbedRotation=Box->GetComponentQuat();
 if(!Run(6))return 1;FVector Target;FRotator Rotation;Handle->GetTargetLocationAndRotation(Target,Rotation);Before=Box->GetComponentLocation();Step(120);
 if(!Check(Handle->GetGrabbedComponent()==Box && FVector::Dist(Box->GetComponentLocation(),Target)<20 && Box->GetComponentLocation().X-Before.X>60,TEXT("handle_grab_and_translate")))return 1;
 if(!Run(7))return 1;Handle->GetTargetLocationAndRotation(Target,Rotation);Step(120);
 if(!Check(FVector::Dist(Box->GetComponentLocation(),Target)<20 && Box->GetComponentQuat().AngularDistance(Rotation.Quaternion()*GrabbedRotation)<0.4,TEXT("handle_target_location_and_rotation")))return 1;
 if(!Run(8))return 1;Before=Box->GetComponentLocation();Step(30);
 if(!Check(Handle->GetGrabbedComponent()==nullptr && Box->GetComponentLocation().X-Before.X>20 && Box->GetComponentLocation().X-Before.X<40,TEXT("handle_release_restores_free_motion")))return 1;
 if(!Run(9))return 1;
 UE_LOG(LogTemp,Display,TEXT("DAS_PHYSICS_SIMULATION=PASS fixed_dt=%g"),Dt);return 0;
}
