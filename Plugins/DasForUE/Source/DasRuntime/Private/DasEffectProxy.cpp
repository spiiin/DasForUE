#include "DasEffectProxy.h"
#include "DasRuntime.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"
#include "Containers/Ticker.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleManager.h"
bool UDasEffectProxy::IsLive()const {
 auto* O=Owner.Get();auto* W=IsValid(O)?O->GetWorld():nullptr;
 if(!Active || !W || !W->IsGameWorld() || W->bIsTearingDown)return false;
 if(auto* A=Cast<AActor>(O))return !A->IsActorBeingDestroyed();
 if(auto* C=Cast<UActorComponent>(O))return !C->IsBeingDestroyed() && IsValid(C->GetOwner()) && !C->GetOwner()->IsActorBeingDestroyed();
 return false;
}
void UDasEffectProxy::Deactivate(){
 if(!Active)return;Active=false;
 if(IsValid(Audio)){Audio->OnAudioFinished.RemoveDynamic(this,&UDasEffectProxy::AudioFinished);Audio->Stop();if(auto* A=Audio->GetOwner())A->RemoveInstanceComponent(Audio);Audio->DestroyComponent();}Audio=nullptr;
 if(IsValid(Niagara)){Niagara->OnSystemFinished.RemoveDynamic(this,&UDasEffectProxy::NiagaraFinished);Niagara->DeactivateImmediate();if(auto* A=Niagara->GetOwner())A->RemoveInstanceComponent(Niagara);Niagara->DestroyComponent();}Niagara=nullptr;
}
void UDasEffectProxy::BeginDestroy(){Deactivate();Super::BeginDestroy();}
void UDasEffectProxy::AudioFinished(){AudioDone=true;CompleteIfReady();}
void UDasEffectProxy::NiagaraFinished(UNiagaraComponent* Component){if(Component!=Niagara)return;NiagaraDone=true;CompleteIfReady();}
void UDasEffectProxy::CompleteIfReady(){
 if(Starting || Queued || !AudioDone || !NiagaraDone || !IsLive())return;Queued=true;
 const TWeakObjectPtr<UDasEffectProxy> Weak(this);
 FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Weak](float){
  auto* P=Weak.Get();if(!P || !P->IsLive())return false;
  if(auto* R=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"))){FDasScalarValue V;V.Kind=int32(EDasMethodSignature::Void);R->DispatchScalarEvent(P->SubscriptionId,MoveTemp(V));}
  return false;
 }),0.f);
}
