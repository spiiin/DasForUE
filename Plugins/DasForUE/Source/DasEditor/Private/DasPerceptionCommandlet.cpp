#include "DasPerceptionCommandlet.h"
#include "DasRuntime.h"
#include "DasCallbackFrames.h"
#include "DasPerceptionTypes.h"
#include "UObject/UnrealType.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/WorldSettings.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
void UDasPerceptionObserver::OnTarget(AActor* Actor,FAIStimulus S){
 if(Actor!=Expected.Get()){++Errors;return;}
 if(S.Type==UAISense::GetSenseID<UAISense_Sight>()){
  if(S.WasSuccessfullySensed())++SightOn;else ++SightOff;
 }else if(S.Type==UAISense::GetSenseID<UAISense_Hearing>() && S.WasSuccessfullySensed()){
  ++HearingOn;if(S.Tag!=TEXT("DasNoise") || !FMath::IsNearlyEqual(S.Strength,2.f) || !S.StimulusLocation.Equals(FVector(2000,0,64)))++Errors;
 }else if(S.Type!=UAISense::GetSenseID<UAISense_Hearing>())++Errors;
}
UDasPerceptionCommandlet::UDasPerceptionCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasPerceptionCommandlet::Main(const FString& Params){
 FString Template;if(!FParse::Value(*Params,TEXT("Probe="),Template))return 1;
 auto Check=[](bool Good,const TCHAR* Name){UE_LOG(LogTemp,Display,TEXT("DAS_PERCEPTION_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);return Good;};
 const auto IVS=UWorld::InitializationValues().CreatePhysicsScene(true).ShouldSimulatePhysics(false).AllowAudioPlayback(false).CreateNavigation(false).CreateAISystem(true);
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&IVS));if(!World.IsValid())return 1;
 GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());World->InitializeActorsForPlay(FURL());
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));UAIPerceptionComponent* Perception=nullptr;AAIController* ScriptOwner=nullptr;
 ON_SCOPE_EXIT{if(World->HasBegunPlay())World->EndPlay(EEndPlayReason::Quit);if(ScriptOwner)Runtime.ReleaseObject(ScriptOwner);GEngine->DestroyWorldContext(World.Get());World->DestroyWorld(false);};
 auto* Controller=World->SpawnActor<ADasPerceptionHost>();auto* Pawn=World->SpawnActor<APawn>();
 auto* Target=World->SpawnActor<ATargetPoint>(FVector(300,0,64),FRotator::ZeroRotator);if(!Controller || !Pawn || !Target)return 1;
 ScriptOwner=Controller;Controller->Possess(Pawn);Controller->SetControlRotation(FRotator::ZeroRotator);
 Perception=NewObject<UAIPerceptionComponent>(Controller);Controller->Perception=Perception;Controller->AddInstanceComponent(Perception);Controller->SetPerceptionComponent(*Perception);
 auto* Sight=NewObject<UAISenseConfig_Sight>(Perception);Sight->SightRadius=1000;Sight->LoseSightRadius=1200;Sight->PeripheralVisionAngleDegrees=90;Sight->SetMaxAge(0.5f);Sight->DetectionByAffiliation=FAISenseAffiliationFilter(true,true,true);
 auto* Hearing=NewObject<UAISenseConfig_Hearing>(Perception);Hearing->HearingRange=3000;Hearing->SetMaxAge(0.5f);Hearing->DetectionByAffiliation=FAISenseAffiliationFilter(true,true,true);
 Perception->ConfigureSense(*Sight);Perception->ConfigureSense(*Hearing);Perception->SetDominantSense(UAISense_Sight::StaticClass());Perception->RegisterComponent();
 auto* System=UAIPerceptionSystem::GetCurrent(World.Get());if(!Check(System!=nullptr,TEXT("real_perception_system")))return 1;
 TStrongObjectPtr<UDasPerceptionObserver> Observer(NewObject<UDasPerceptionObserver>());Observer->Expected=Target;
 Perception->OnTargetPerceptionUpdated.AddDynamic(Observer.Get(),&UDasPerceptionObserver::OnTarget);
 World->BeginPlay();World->GetWorldSettings()->NotifyBeginPlay();
 // The DSL compiler creates the tuple and binder from the ordinary ue_event method.
 const FString Probe=Template;FString Error;FDasAuthoringModule Authoring;
 if(!Runtime.ReadAuthoringScript(Probe,Authoring,Error)){UE_LOG(LogTemp,Error,TEXT("DAS_PERCEPTION_DSL %s"),*Error);return 1;}
 FDasAIStimulus InvalidView;
 if(!Check(ProjectDasAIStimulus(FAIStimulus(),InvalidView,Error) && InvalidView.Sense==EDasAISense::Invalid && !InvalidView.Valid,TEXT("invalid_native_stimulus_is_not_sight")))return 1;
 FAIStimulus OtherStimulus;OtherStimulus.Type=System->RegisterSenseClass(UAISense_Damage::StaticClass());FDasAIStimulus OtherView;
 if(!Check(OtherStimulus.Type!=FAISenseID::InvalidID() && ProjectDasAIStimulus(OtherStimulus,OtherView,Error) && OtherView.Sense==EDasAISense::Other,TEXT("other_native_sense_is_not_sight_or_hearing")))return 1;
 // Only the exact native source/view pair may cross the one-way callback boundary.
 auto* Event=FindFProperty<FMulticastDelegateProperty>(Perception->GetClass(),TEXT("OnTargetPerceptionUpdated"));
 FDasDeclaredType ActorType;ActorType.Kind=TEXT("object");ActorType.TypePath=AActor::StaticClass()->GetPathName();
 FDasDeclaredType WrongType;WrongType.Kind=TEXT("struct");WrongType.TypePath=TBaseStructure<FVector>::Get()->GetPathName();
 auto* Wrong=DasCallbackFrame({ActorType,WrongType},Error);
 if(!Check(Event && Wrong && !ValidateDasCallbackFrameSource(Event->SignatureFunction,Wrong,Error),TEXT("unrelated_callback_struct_is_rejected")))return 1;
 FDasDeclaredType RawType;RawType.Kind=TEXT("struct");RawType.TypePath=FAIStimulus::StaticStruct()->GetPathName();
 auto* RawFrame=DasCallbackFrame({ActorType,RawType},Error);
 if(!Check(RawFrame && ValidateDasCallbackFrameSource(Event->SignatureFunction,RawFrame,Error),TEXT("raw_stimulus_callback_remains_compatible")))return 1;
 auto Run=[&](int32 Phase){auto R=Runtime.RunPersistent(Probe,Phase,ScriptOwner);if(!R.bSucceeded || R.Value!=42){UE_LOG(LogTemp,Error,TEXT("DAS_PERCEPTION_SCRIPT phase=%d value=%d error=%s"),Phase,R.Value,*R.Error);return false;}return true;};
 auto Tick=[&](){++GFrameCounter;World->Tick(LEVELTICK_All,0.05f);};
 auto Until=[&](auto Predicate){for(int32 I=0;I<120 && !Predicate();++I)Tick();return Predicate();};
 if(!Run(0) || !Check(Runtime.SubscriptionCount(ScriptOwner)==1,TEXT("actor_stimulus_callback_bound")))return 1;
 System->RegisterSourceForSenseClass(UAISense_Sight::StaticClass(),*Target);
 if(!Check(Until([&](){return Observer->SightOn>=1;}) && Observer->Errors==0,TEXT("native_sight_detects_source")) || !Run(1))return 1;
 Target->SetActorLocation(FVector(2000,0,64));
 if(!Check(Until([&](){return Observer->SightOff>=1;}),TEXT("native_sight_loses_out_of_range_source")) || !Run(2))return 1;
 UAISense_Hearing::ReportNoiseEvent(World.Get(),Target->GetActorLocation(),2.f,Target,3000.f,TEXT("DasNoise"));
 if(!Check(Until([&](){return Observer->HearingOn>=1;}) && Observer->Errors==0,TEXT("native_hearing_receives_real_noise")) || !Run(3))return 1;
 for(int32 I=0;I<60;++I)Tick();
 if(!Run(4) || !Check(Observer->Errors==0,TEXT("generated_queries_and_expired_hearing")))return 1;
 const int32 Before=Observer->HearingOn;Target->SetActorLocation(FVector(10000,0,64));
 UAISense_Hearing::ReportNoiseEvent(World.Get(),Target->GetActorLocation(),2.f,Target,1000.f,TEXT("FarNoise"));for(int32 I=0;I<10;++I)Tick();
 if(!Check(Observer->HearingOn==Before && Observer->Errors==0,TEXT("out_of_range_noise_not_delivered")) || !Run(4))return 1;
 Runtime.ReleaseObject(ScriptOwner);if(!Check(Runtime.SubscriptionCount(ScriptOwner)==0,TEXT("released_context_unsubscribed")))return 1;
 const int32 Seen=Observer->SightOn;Target->SetActorLocation(FVector(300,0,64));
 if(!Check(Until([&](){return Observer->SightOn>Seen;}) && Runtime.SubscriptionCount(ScriptOwner)==0,TEXT("native_stimuli_continue_without_released_das_callback")))return 1;
 if(!Run(5) || !Check(Runtime.SubscriptionCount(ScriptOwner)==1,TEXT("fresh_context_has_no_stale_callback_state")))return 1;
 Runtime.ReleaseObject(ScriptOwner);Perception->DestroyComponent();
 if(!Check(Runtime.SubscriptionCount(ScriptOwner)==0,TEXT("destroyed_listener_has_no_subscription")))return 1;
 UE_LOG(LogTemp,Display,TEXT("DAS_PERCEPTION_RUNTIME=PASS sight_on=%d sight_off=%d hearing_on=%d"),Observer->SightOn,Observer->SightOff,Observer->HearingOn);return 0;
}
