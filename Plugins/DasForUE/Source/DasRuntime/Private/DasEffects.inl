bool ScriptEffectPlay(const char* Name,const FDasObjectHandle& SoundHandle,const FDasObjectHandle& SystemHandle,const FTransform& Transform,const char* Handler,float Volume,float Pitch,das::Context* C){
 auto* S=ManagedOwnerSession(C);auto* W=GameplayWorld(C);const FString Key=TimerName(Name,C),Entry=Handler?UTF8_TO_TCHAR(Handler):TEXT("");
 if(Transform.ContainsNaN() || !Transform.GetRotation().IsNormalized() || !FMath::IsFinite(Volume) || Volume<0.f || Volume>16.f || !FMath::IsFinite(Pitch) || Pitch<=0.f || Pitch>4.f)C->throw_error("effects require a finite normalized transform, volume 0..16 and pitch (0,4]");
 auto* Sound=SoundHandle.Token?Cast<USoundBase>(ResolveObject(SoundHandle,C)):nullptr;auto* System=SystemHandle.Token?Cast<UNiagaraSystem>(ResolveObject(SystemHandle,C)):nullptr;
 if((SoundHandle.Token && !Sound) || (SystemHandle.Token && !System))C->throw_error("effect assets must be SoundBase and NiagaraSystem");
 const auto Contract=FDasCallContract::Scalar(EDasMethodSignature::Void);if(!Entry.IsEmpty() && !VerifyContract(S->Live->Program,Entry,Contract))C->throw_error("effect completion requires exported void handler()");
 if((!Sound && !System) || W->GetNetMode()==NM_DedicatedServer || (System && !System->IsReadyToRun()))return false;
 auto* Actor=Cast<AActor>(S->Owner.Get());if(!Actor)if(auto* Component=Cast<UActorComponent>(S->Owner.Get()))Actor=Component->GetOwner();if(!IsValid(Actor) || Actor->IsActorBeingDestroyed())C->throw_error("effect owner must be a live Actor or ActorComponent");
 int32 Count=0;bool Replacing=false;for(const auto& B:S->Bindings)if(B->EffectProxy.IsValid()){++Count;Replacing|=B->EffectName==Key;}if(!Replacing && Count>=64)C->throw_error("effect subscription limit exceeded");
 auto B=MakeUnique<FManagedSubscription>();B->Id=NextSubscriptionId++;B->Handler=Entry;B->Kind=EDasMethodSignature::Void;B->EffectName=Key;B->EffectProxy.Reset(NewObject<UDasEffectProxy>());auto* P=B->EffectProxy.Get();P->SubscriptionId=B->Id;P->Owner=S->Owner;B->Source=P;
 if(Sound){auto* A=NewObject<UAudioComponent>(Actor,NAME_None,RF_Transient);P->Audio=A;A->bAutoActivate=false;A->bAutoDestroy=false;A->bStopWhenOwnerDestroyed=true;A->bAllowSpatialization=true;A->SetSound(Sound);A->SetVolumeMultiplier(Volume);A->SetPitchMultiplier(Pitch);Actor->AddInstanceComponent(A);A->RegisterComponentWithWorld(W);A->SetWorldTransform(Transform);A->OnAudioFinished.AddDynamic(P,&UDasEffectProxy::AudioFinished);P->AudioDone=false;}
 if(System){auto* N=NewObject<UNiagaraComponent>(Actor,NAME_None,RF_Transient);P->Niagara=N;N->SetAutoActivate(false);N->SetAutoDestroy(false);N->SetAsset(System);Actor->AddInstanceComponent(N);N->RegisterComponentWithWorld(W);N->SetWorldTransform(Transform);N->OnSystemFinished.AddDynamic(P,&UDasEffectProxy::NiagaraFinished);P->NiagaraDone=false;}
 // The new components are configured before revoking the previous cue.
 S->Bindings.RemoveAll([&](const auto& Old){return Old->EffectProxy.IsValid() && Old->EffectName==Key;});S->Bindings.Add(MoveTemp(B));if(!Entry.IsEmpty())S->RequiredEntries.Add(Entry,Contract);
 if(P->Audio){P->Audio->Play();if(!P->Audio->IsPlaying())P->AudioDone=true;}
 if(P->Niagara)P->Niagara->Activate(true);
 P->Starting=false;P->CompleteIfReady();return true;
}
bool ScriptEffectStop(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);const FString Key=TimerName(Name,C);return S->Bindings.RemoveAll([&](const auto& B){return B->EffectProxy.IsValid() && B->EffectName==Key;})>0;}
int32 ScriptEffectStopAll(das::Context* C){auto* S=ManagedOwnerSession(C);return S->Bindings.RemoveAll([](const auto& B){return B->EffectProxy.IsValid();});}
bool ScriptEffectActive(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);const FString Key=TimerName(Name,C);for(const auto& B:S->Bindings)if(B->EffectProxy.IsValid() && B->EffectName==Key)return B->EffectProxy->IsLive() && !B->EffectProxy->Queued;return false;}
