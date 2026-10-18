#include "DasAsyncPhysicsBridge.h"
#include "DasExecutionRolePrivate.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "PBDRigidsSolver.h"
#include "Containers/Ticker.h"
#include "Misc/ScopeExit.h"
namespace {
 TArray<TWeakObjectPtr<UDasAsyncPhysicsBridge>> Bridges;
 FTSTicker::FDelegateHandle DeferredTicker;
 uint64 NextRevision=1,NextEpoch=1;
 thread_local const FDasAsyncPhysicsStep* CurrentStep=nullptr;
 bool LiveOwner(UObject* Object,UWorld* World){
  if(!IsValid(Object) || Object->IsTemplate() || !World || !World->IsGameWorld() || World->bIsTearingDown || Object->GetWorld()!=World)return false;
  auto* Component=Cast<UActorComponent>(Object);auto* Actor=Component?Component->GetOwner():Cast<AActor>(Object);
  return IsValid(Actor) && !Actor->IsActorBeingDestroyed() && Actor->HasActorBegunPlay() && (!Component || (Component->IsRegistered() && !Component->IsBeingDestroyed() && Component->HasBegunPlay()));
 }
}
bool FDasAsyncPhysicsStep::IsCurrent()const {
 if(!IsInGameThread())return false;auto* B=Bridge.Get();return CurrentStep && CurrentStep->Epoch==Epoch && CurrentStep->Revision==Revision && CurrentStep->Bridge==Bridge && GetDasExecutionRole()==EDasExecutionRole::PhysicsPreSim && B && B->Executing && B->Revision==Revision && B->IsLiveBinding();
}
UDasAsyncPhysicsBridge* UDasAsyncPhysicsBridge::Create(UObject* Owner,FCallback InCallback,FString& Error){
 Error.Reset();if(!IsInGameThread()){Error=TEXT("async physics bridge requires game thread");return nullptr;}auto* World=IsValid(Owner)?Owner->GetWorld():nullptr;
 if(!IsInGameThread() || GetDasExecutionRole()!=EDasExecutionRole::Gameplay || IsDasFrozenPhysicsWorld(Owner) || !InCallback || !LiveOwner(Owner,World) || !World->GetPhysicsScene()) {Error=TEXT("async physics bridge requires ordinary game-thread lifecycle and a live begun-play owner");return nullptr;}
 Bridges.RemoveAll([](const auto& P){return !P.IsValid();});if(Bridges.Num()>=256){Error=TEXT("async physics binding limit reached");return nullptr;}
 auto* Actor=Cast<AActor>(Owner);if(!Actor)Actor=CastChecked<UActorComponent>(Owner)->GetOwner();
 auto* B=NewObject<UDasAsyncPhysicsBridge>(Actor);B->ScriptOwner=Owner;B->BoundWorld=World;B->Callback=MoveTemp(InCallback);B->Revision=NextRevision++;B->Active=true;
 Actor->AddInstanceComponent(B);B->RegisterComponent();B->Activate(true);
 if(!B->IsRegistered() || !B->HasBegunPlay()){Error=TEXT("async physics bridge registration failed");B->Revoke();return nullptr;}
 Bridges.Add(B);B->SetAsyncPhysicsTickEnabled(true);return B;
}
bool UDasAsyncPhysicsBridge::IsLiveBinding()const {
 return IsInGameThread() && Active && IsRegistered() && !IsBeingDestroyed() && IsActive() && LiveOwner(ScriptOwner.Get(),BoundWorld.Get()) && GetWorld()==BoundWorld.Get();
}
void UDasAsyncPhysicsBridge::QueueFinalize(){
 if(!Pending && IsRegistered() && IsDasFrozenPhysicsWorld(this))DeferRemoveAsyncPhysicsTick();
 Pending=true;
 if(!DeferredTicker.IsValid())DeferredTicker=FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float){UDasAsyncPhysicsBridge::FlushDeferred();bool Remaining=false;for(const auto& P:Bridges)if(auto* B=P.Get();B && B->Pending)Remaining=true;if(!Remaining)DeferredTicker.Reset();return Remaining;}));
}
void UDasAsyncPhysicsBridge::Revoke(){
 if(!IsInGameThread() || Finalizing)return;
 Active=false;
 if(Executing || GetDasExecutionRole()==EDasExecutionRole::PhysicsPreSim || IsDasFrozenPhysicsWorld(this)){QueueFinalize();return;}
 Finalize();
}
void UDasAsyncPhysicsBridge::Finalize(){
 if(Finalizing || Executing || GetDasExecutionRole()!=EDasExecutionRole::Gameplay || IsDasFrozenPhysicsWorld(this)){QueueFinalize();return;}
 Finalizing=true;Active=false;Pending=false;SetAsyncPhysicsTickEnabled(false);Callback=nullptr;
 if(!IsBeingDestroyed())DestroyComponent();Finalizing=false;
}
void UDasAsyncPhysicsBridge::FlushDeferred(){
 if(!IsInGameThread() || GetDasExecutionRole()!=EDasExecutionRole::Gameplay)return;
 for(const auto& P:Bridges)if(auto* B=P.Get();B && B->Pending && !B->Executing && !IsDasFrozenPhysicsWorld(B))B->Finalize();
 Bridges.RemoveAll([](const auto& P){return !P.IsValid();});
}
void UDasAsyncPhysicsBridge::RevokeOwner(const UObject* Owner){if(!IsInGameThread())return;for(const auto& P:Bridges)if(auto* B=P.Get();B && B->ScriptOwner.GetEvenIfUnreachable()==Owner)B->Revoke();}
void UDasAsyncPhysicsBridge::RevokeWorld(const UWorld* World){if(!IsInGameThread())return;for(const auto& P:Bridges)if(auto* B=P.Get();B && B->BoundWorld.GetEvenIfUnreachable()==World)B->Revoke();}
void UDasAsyncPhysicsBridge::RevokeAll(){if(!IsInGameThread())return;for(const auto& P:Bridges)if(auto* B=P.Get())B->Revoke();FlushDeferred();if(Bridges.IsEmpty() && DeferredTicker.IsValid()){FTSTicker::RemoveTicker(DeferredTicker);DeferredTicker.Reset();}}
void UDasAsyncPhysicsBridge::AsyncPhysicsTickComponent(float Dt,float SimTime){
 if(!IsLiveBinding()){Revoke();return;}
 auto* World=BoundWorld.Get();auto* Solver=World && World->GetPhysicsScene()?World->GetPhysicsScene()->GetSolver():nullptr;
 if(!IsInGameThread() || GetDasExecutionRole()!=EDasExecutionRole::Gameplay || !Solver || !Solver->IsGameThreadFrozen() || Solver->IsShuttingDown() || static_cast<const Chaos::FPhysicsSolverBase*>(Solver)->IsResimming() || !FMath::IsFinite(Dt) || Dt<=0 || !FMath::IsFinite(SimTime) || Executing){LastError=TEXT("invalid async physics phase or unsupported resimulation/reentry");Revoke();return;}
 FDasAsyncPhysicsStep Step;Step.Epoch=NextEpoch++;Step.Revision=Revision;Step.DeltaTime=Dt;Step.SimTime=SimTime;Step.Bridge=this;
 TGuardValue<bool> Busy(Executing,true);TGuardValue<const FDasAsyncPhysicsStep*> Scope(CurrentStep,&Step);FDasPhysicsExecutionScope Role;
 FString Error;bool Success=false;
 try{Success=Callback && Callback(Step,Error);}catch(const std::exception& Ex){Error=UTF8_TO_TCHAR(Ex.what());}catch(...){Error=TEXT("native async physics callback failed");}
 if(!Success){LastError=Error.IsEmpty()?TEXT("async physics callback rejected"):MoveTemp(Error);Revoke();}
}
void UDasAsyncPhysicsBridge::Deactivate(){Revoke();}
void UDasAsyncPhysicsBridge::OnUnregister(){Active=false;if(Executing)QueueFinalize();else{Pending=false;Callback=nullptr;}Super::OnUnregister();}
void UDasAsyncPhysicsBridge::EndPlay(const EEndPlayReason::Type Reason){Active=false;if(Executing)QueueFinalize();else Callback=nullptr;Super::EndPlay(Reason);}
void UDasAsyncPhysicsBridge::BeginDestroy(){Active=false;Callback=nullptr;Super::BeginDestroy();}

void UDasAsyncPhysicsBridge::RevokeOwnerExcept(const UObject* Owner,uint64 Revision){
 if(!IsInGameThread())return;for(const auto& P:Bridges)if(auto* B=P.Get();B && B->ScriptOwner.GetEvenIfUnreachable()==Owner && B->Revision!=Revision)B->Revoke();
}
