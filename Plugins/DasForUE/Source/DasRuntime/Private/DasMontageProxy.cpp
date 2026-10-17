#include "DasMontageProxy.h"
#include "DasRuntime.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Containers/Ticker.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleManager.h"

bool UDasMontageProxy::IsLive()const {
 auto* O=Owner.Get();auto* M=Mesh.Get();auto* A=Anim.Get();auto* W=IsValid(O)?O->GetWorld():nullptr;
 if(!Active || !W || !W->IsGameWorld() || W->bIsTearingDown || !IsValid(M) || M->IsBeingDestroyed() || M->GetWorld()!=W || !IsValid(A) || M->GetAnimInstance()!=A)return false;
 if(auto* Actor=Cast<AActor>(O);Actor && Actor->IsActorBeingDestroyed())return false;
 if(auto* C=Cast<UActorComponent>(O);C && (C->IsBeingDestroyed() || !IsValid(C->GetOwner()) || C->GetOwner()->IsActorBeingDestroyed()))return false;
 return IsValid(M->GetOwner()) && !M->GetOwner()->IsActorBeingDestroyed();
}
bool UDasMontageProxy::IsPlaying()const {auto* A=Anim.Get();auto* I=A?A->GetMontageInstanceForID(InstanceId):nullptr;return IsLive() && !Finished && I && I->IsActive();}
void UDasMontageProxy::Attach(){
 auto* A=Anim.Get();auto* I=A?A->GetMontageInstanceForID(InstanceId):nullptr;check(I);
 I->OnMontageBlendingOutStarted.BindUObject(this,&UDasMontageProxy::BlendingOut);
 I->OnMontageEnded.BindUObject(this,&UDasMontageProxy::Ended);
 A->OnPlayMontageNotifyBegin.AddDynamic(this,&UDasMontageProxy::NotifyBegin);
 A->OnPlayMontageNotifyEnd.AddDynamic(this,&UDasMontageProxy::NotifyEnd);
}
void UDasMontageProxy::Disconnect(){if(auto* A=Anim.Get()){
 A->OnPlayMontageNotifyBegin.RemoveDynamic(this,&UDasMontageProxy::NotifyBegin);
 A->OnPlayMontageNotifyEnd.RemoveDynamic(this,&UDasMontageProxy::NotifyEnd);
 if(auto* I=A->GetMontageInstanceForID(InstanceId)){if(I->OnMontageEnded.IsBoundToObject(this))I->OnMontageEnded.Unbind();if(I->OnMontageBlendingOutStarted.IsBoundToObject(this))I->OnMontageBlendingOutStarted.Unbind();}
}}
void UDasMontageProxy::Stop(float BlendOut){if(!Finished)if(auto* A=Anim.Get())if(auto* I=A->GetMontageInstanceForID(InstanceId))I->Stop(FAlphaBlend(BlendOut),true);}
void UDasMontageProxy::Deactivate(){if(!Active)return;Active=false;Disconnect();Stop(0.f);}
void UDasMontageProxy::BeginDestroy(){Deactivate();Super::BeginDestroy();}
void UDasMontageProxy::BlendingOut(UAnimMontage*,bool Interrupted){if(!Interrupted)Queue(EDasMontageEventKind::BlendOut);}
void UDasMontageProxy::Ended(UAnimMontage*,bool Interrupted){if(Finished)return;Finished=true;Disconnect();Queue(Interrupted?EDasMontageEventKind::Interrupted:EDasMontageEventKind::Completed);}
void UDasMontageProxy::NotifyBegin(FName N,const FBranchingPointNotifyPayload& P){if(P.MontageInstanceID==InstanceId)Queue(EDasMontageEventKind::NotifyBegin,N);}
void UDasMontageProxy::NotifyEnd(FName N,const FBranchingPointNotifyPayload& P){if(P.MontageInstanceID==InstanceId)Queue(EDasMontageEventKind::NotifyEnd,N);}
void UDasMontageProxy::Queue(EDasMontageEventKind K,FName N){
 if(!IsLive())return;const TWeakObjectPtr<UDasMontageProxy> Weak(this);
 FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Weak,K,N](float){
  auto* P=Weak.Get();if(!P || !P->IsLive())return false;
  if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=int32(EDasMethodSignature::Struct);V.ObjectClass=FDasMontageEvent::StaticStruct()->GetPathName();FDasMontageEvent E;E.Kind=K;E.NotifyName=N;V.StructValue=FInstancedStruct::Make<FDasMontageEvent>(E);R->DispatchScalarEvent(P->SubscriptionId,MoveTemp(V));}
  return false;
 }),0.f);
}
