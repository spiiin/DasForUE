#include "DasScopedPhysicsBody.h"
#include "DasExecutionRole.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "PBDRigidsSolver.h"
namespace {
 uint64 NextBodyToken=1; // GT only; globally unique across VMs/revisions/epochs.
 bool Finite(const FVector& V){return FMath::IsFinite(V.X)&&FMath::IsFinite(V.Y)&&FMath::IsFinite(V.Z);}
 AActor* OwnerActor(UObject* O){if(auto* C=Cast<UActorComponent>(O))return C->GetOwner();return Cast<AActor>(O);}
 bool Ordinary(){return IsInGameThread() && GetDasExecutionRole()==EDasExecutionRole::Gameplay && !IsDasFrozenPhysicsWorld(nullptr);}
 bool Supported(UPrimitiveComponent* C){return (C->IsA<UBoxComponent>() || C->IsA<USphereComponent>() || C->IsA<UCapsuleComponent>() || C->IsA<UStaticMeshComponent>()) && !C->IsA<UInstancedStaticMeshComponent>();}
}
struct FDasPhysicsBodyBindings::FImpl {
 struct FBinding {TWeakObjectPtr<UPrimitiveComponent> Component;TSharedPtr<FProxyTimestampBase,ESPMode::ThreadSafe> Generation;};
 TWeakObjectPtr<UObject> Owner;TWeakObjectPtr<UWorld> World;FBinding Bodies[Capacity];
 explicit FImpl(UObject* O):Owner(O),World(IsValid(O)?O->GetWorld():nullptr){}
 // Never retain a Proxy/BI/Internal pointer. The caller borrows the current
 // member BodyInstance only after all weak lifecycle and selector checks.
 FBodyInstance* Body(UPrimitiveComponent* C,FString& Error)const{
  auto* O=Owner.Get();auto* A=IsValid(O)?OwnerActor(O):nullptr;auto* W=World.Get();
  auto* OC=Cast<UActorComponent>(O);
  if(!IsValid(O) || O->IsTemplate() || !IsValid(A) || A->IsActorBeingDestroyed() || !A->HasActorBegunPlay() || !W || W->bIsTearingDown || !W->IsGameWorld() || A->GetWorld()!=W || (OC && (!OC->IsRegistered() || OC->IsBeingDestroyed() || !OC->HasBegunPlay())) || !IsValid(C) || C->IsTemplate() || C->IsBeingDestroyed() || !C->IsRegistered() || !C->HasBegunPlay() || C->GetOwner()!=A || C->GetWorld()!=W || !C->IsPhysicsStateCreated()){
   Error=TEXT("physics body owner/component/world expired or not authorized");return nullptr;
  }
  if(!Supported(C)){Error=TEXT("physics body supports single Box/Sphere/Capsule/StaticMesh only");return nullptr;}
  auto& BI=C->BodyInstance;const auto* Welds=BI.GetCurrentWeldInfo();
  if(BI.OwnerComponent.Get()!=C || BI.WeldParent || (Welds && !Welds->IsEmpty()) || !BI.bSimulatePhysics){Error=TEXT("physics body must be simulated, single and unwelded");return nullptr;}
  return &BI;
 }
 Chaos::FRigidBodyHandle_Internal* Resolve(int32 Slot,const FDasAsyncPhysicsStep& Step,FString& Error)const{
  if(!Step.IsCurrent() || GetDasExecutionRole()!=EDasExecutionRole::PhysicsPreSim || Slot<0 || Slot>=Capacity){Error=TEXT("physics body requires current VM step and valid slot");return nullptr;}
  const auto& B=Bodies[Slot];
  // The retained timestamp, unlike a raw handle, is safe after deletion. Check
  // it before touching any freshly selected proxy (never dereference an old one).
  if(!B.Generation || B.Generation->bDeleted){Error=TEXT("physics body generation expired; bind again on ordinary game thread");return nullptr;}
  auto* BI=Body(B.Component.Get(),Error);if(!BI)return nullptr;
  auto* Proxy=BI->ActorHandle;auto* W=World.Get();auto* Scene=W?W->GetPhysicsScene():nullptr;auto* Solver=Scene?Scene->GetSolver():nullptr;
  if(!Proxy || Proxy->GetSyncTimestamp()!=B.Generation || !Solver || Proxy->GetSolverBase()!=Solver || !Solver->IsGameThreadFrozen() || Solver->IsShuttingDown() || static_cast<const Chaos::FPhysicsSolverBase*>(Solver)->IsResimming()){
   Error=TEXT("physics body generation/solver changed or phase unsupported");return nullptr;
  }
  auto* Internal=Proxy->GetPhysicsThreadAPI();
  if(!Internal || (Internal->ObjectState()!=Chaos::EObjectStateType::Dynamic && Internal->ObjectState()!=Chaos::EObjectStateType::Sleeping)) {Error=TEXT("physics body must have a dynamic or sleeping internal particle");return nullptr;}
  return Internal;
 }
};
FDasPhysicsBodyBindings::FDasPhysicsBodyBindings(UObject* Owner):Impl(MakeUnique<FImpl>(Owner)){}
FDasPhysicsBodyBindings::~FDasPhysicsBodyBindings()=default;
bool FDasPhysicsBodyBindings::Bind(int32 Slot,UPrimitiveComponent* Component,FString& Error){
 Error.Reset();if(!Ordinary() || Slot<0 || Slot>=Capacity){Error=TEXT("body binding requires ordinary game thread and slot 0..7");return false;}
 auto* BI=Impl->Body(Component,Error);if(!BI)return false;
 // Selector/override check is done on ordinary GT. Frozen access above uses
 // the known member directly rather than invoking arbitrary virtual selectors.
 if(Component->GetBodyInstance(NAME_None,false,INDEX_NONE)!=BI){Error=TEXT("custom/multiple physics body selector is unsupported");return false;}
 auto* Proxy=BI->ActorHandle;auto* W=Impl->World.Get();auto* Scene=W?W->GetPhysicsScene():nullptr;
 if(!Proxy || !Scene || Proxy->GetSolverBase()!=Scene->GetSolver()){Error=TEXT("physics body proxy/solver not ready");return false;}
 auto Generation=Proxy->GetSyncTimestamp();if(!Generation || Generation->bDeleted){Error=TEXT("physics body generation not ready");return false;}
 for(int32 I=0;I<Capacity;++I)if(I!=Slot && Impl->Bodies[I].Generation==Generation){Error=TEXT("physics body is already bound to another slot");return false;}
 Impl->Bodies[Slot]={Component,MoveTemp(Generation)};return true;
}
bool FDasPhysicsBodyBindings::Clear(int32 Slot,FString& Error){
 Error.Reset();if(!Ordinary() || Slot<0 || Slot>=Capacity){Error=TEXT("body unbinding requires ordinary game thread and slot 0..7");return false;}Impl->Bodies[Slot]={};return true;
}
uint64 FDasPhysicsBodyScope::Acquire(int32 Slot,FString& Error){
 Error.Reset();if(!Bindings.Impl->Resolve(Slot,Step,Error))return 0;
 if(!Tokens[Slot]){if(!NextBodyToken){Error=TEXT("physics body token space exhausted");return 0;}Tokens[Slot]=NextBodyToken++;}return Tokens[Slot];
}
bool FDasPhysicsBodyScope::AddForce(uint64 Token,const FVector& Force,bool AccelChange,FString& Error){
 Error.Reset();int32 Slot=INDEX_NONE;for(int32 I=0;I<FDasPhysicsBodyBindings::Capacity;++I)if(Token && Tokens[I]==Token){Slot=I;break;}
 if(Slot==INDEX_NONE){Error=TEXT("physics body token is null, expired or belongs to another VM/step");return false;}
 if(Calls>=FDasPhysicsBodyBindings::MaxCallsPerStep){Error=TEXT("physics AddForce call limit is 64 per step");return false;}
 auto* Internal=Bindings.Impl->Resolve(Slot,Step,Error);if(!Internal)return false;
 const double Mass=Internal->M(),InvMass=Internal->InvM();
 if(!Finite(Force) || !FMath::IsFinite(Mass) || Mass<=0 || !FMath::IsFinite(InvMass) || InvMass<=0){Error=TEXT("physics force and body mass/inverse mass must be finite and positive");return false;}
 const FVector Applied=AccelChange?Force*Mass:Force;const FVector Sum=Pending[Slot]+Applied;
 if(!Finite(Applied) || !Finite(Sum) || !Finite(Internal->Acceleration()+Sum*InvMass)){Error=TEXT("physics force/mass product or accumulated acceleration overflows");return false;}
 Pending[Slot]=Sum;Touched[Slot]=true;++Calls;return true;
}
bool FDasPhysicsBodyScope::Commit(FString& Error){
 Error.Reset();if(!Step.IsCurrent()){Error=TEXT("physics body scope expired before commit");return false;}
 Chaos::FRigidBodyHandle_Internal* Current[FDasPhysicsBodyBindings::Capacity]={};
 // Validate the entire bounded batch before the first write. No arbitrary UE
 // calls, virtual selectors, locks or code execution between validation/apply.
 for(int32 I=0;I<FDasPhysicsBodyBindings::Capacity;++I)if(Touched[I]){
  auto* B=Bindings.Impl->Resolve(I,Step,Error);if(!B)return false;
  const auto Accel=B->Acceleration()+Pending[I]*B->InvM();
  if(!FMath::IsFinite(B->M()) || B->M()<=0 || !FMath::IsFinite(B->InvM()) || B->InvM()<=0 || !Finite(Accel) || !Finite(B->V()+Accel*Step.DeltaTime)){Error=TEXT("physics body force commit would produce nonfinite acceleration/velocity");return false;}
  Current[I]=B;
 }
 for(int32 I=0;I<FDasPhysicsBodyBindings::Capacity;++I)if(Current[I])Current[I]->AddForce(Pending[I]);
 return true;
}

#include "DasScopedPhysicsBodyProbe.h"
namespace {
class FScopedBodyProbe final:public IDasScopedPhysicsBodyProbe {
 uint64 Previous=0;
public:
 FDasPhysicsBodyBindings A,B;
 explicit FScopedBodyProbe(UObject* Owner):A(Owner),B(Owner){}
 virtual bool VerifyIsolation(const FDasAsyncPhysicsStep& Step,FString& Error)override{
  FDasPhysicsBodyScope SA(A,Step),SB(B,Step);const auto TA=SA.Acquire(0,Error);if(!TA)return false;const auto TB=SB.Acquire(0,Error);if(!TB)return false;
  if(TA==TB || SA.AddForce(TB,FVector(1,0,0),false,Error) || SB.AddForce(TA,FVector(1,0,0),false,Error) || SA.AddForce(0,FVector(1,0,0),false,Error) || (Previous && SA.AddForce(Previous,FVector(1,0,0),false,Error))){Error=TEXT("foreign/null/expired native body capability unexpectedly accepted");return false;}
  Previous=TA;Error.Reset();return true; // no commit; no force is applied
 }
};
}
TUniquePtr<IDasScopedPhysicsBodyProbe> PrepareDasScopedPhysicsBodyProbeForTests(UObject* Owner,UPrimitiveComponent* Body,FString& Error){
 auto P=MakeUnique<FScopedBodyProbe>(Owner);if(!P->A.Bind(0,Body,Error) || !P->B.Bind(0,Body,Error))return nullptr;return P;
}
