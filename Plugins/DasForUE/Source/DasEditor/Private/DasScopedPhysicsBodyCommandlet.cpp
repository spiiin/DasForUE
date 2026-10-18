#include "DasScopedPhysicsBodyCommandlet.h"
#include "DasAsyncPhysicsVM.h"
#include "DasScopedPhysicsBodyProbe.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
UDasScopedPhysicsBodyCommandlet::UDasScopedPhysicsBodyCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasScopedPhysicsBodyCommandlet::Main(const FString& Params){
 FString Path;if(!FParse::Value(*Params,TEXT("Probe="),Path))return 1;
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 int32 Failures=0,Checks=0;auto Check=[&](bool Good,const TCHAR* Name){++Checks;UE_LOG(LogTemp,Display,TEXT("DAS_SCOPED_BODY_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);if(!Good)++Failures;};
 auto* Settings=GetMutableDefault<UPhysicsSettings>();const bool OldAsync=Settings->bTickPhysicsAsync;const float OldDt=Settings->AsyncFixedTimeStepSize;
 Settings->bTickPhysicsAsync=FParse::Param(*Params,TEXT("AsyncFixed"));Settings->AsyncFixedTimeStepSize=1.f/60.f;ON_SCOPE_EXIT{Settings->bTickPhysicsAsync=OldAsync;Settings->AsyncFixedTimeStepSize=OldDt;};
 const auto IVS=UWorld::InitializationValues().CreatePhysicsScene(true).ShouldSimulatePhysics(true).AllowAudioPlayback(false).CreateNavigation(false).CreateAISystem(false);
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&IVS)),Other(UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&IVS));
 for(auto* W:{World.Get(),Other.Get()}){GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(W);W->InitializeActorsForPlay(FURL());W->BeginPlay();W->GetWorldSettings()->NotifyBeginPlay();}
 ON_SCOPE_EXIT{UDasAsyncPhysicsBridge::RevokeAll();for(auto* W:{World.Get(),Other.Get()})if(W){GEngine->DestroyWorldContext(W);W->DestroyWorld(false);}};
 auto Owner=[&](UWorld* W){auto* A=W->SpawnActor<AActor>();if(!A->HasActorBegunPlay())A->DispatchBeginPlay();return A;};
 auto BoxOn=[&](AActor* A,bool Sim=true){auto* B=NewObject<UBoxComponent>(A);A->AddInstanceComponent(B);if(!A->GetRootComponent())A->SetRootComponent(B);B->SetBoxExtent(FVector(20));B->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);B->SetCollisionResponseToAllChannels(ECR_Ignore);B->RegisterComponent();B->SetEnableGravity(false);B->SetLinearDamping(0);B->SetSimulatePhysics(Sim);if(Sim)B->SetMassOverrideInKg(NAME_None,10,true);return B;};
 auto Step=[&](int32 N){for(int32 I=0;I<N;++I){++GFrameCounter;World->Tick(LEVELTICK_All,1.f/60.f);}};
 auto* Actor=Owner(World.Get());auto* Box=BoxOn(Actor);Step(3);FString Error;
 FDasPhysicsPacket Input;
 auto Prepare=[&](AActor* A,UPrimitiveComponent* B,int32 Mode){auto V=CreateDasAsyncPhysicsVM(Path,A,Error);if(V && !V->BindBody(0,B,Error)){UE_LOG(LogTemp,Error,TEXT("bind: %s"),*Error);V->Revoke();return TSharedPtr<IDasAsyncPhysicsVM>();}Input={};Input.Values[0]=Mode==2?100:1000;Input.Values[15]=Mode;if(V && !V->Publish(Input,Error)){V->Revoke();return TSharedPtr<IDasAsyncPhysicsVM>();}return V;};
 auto V=Prepare(Actor,Box,0);Check(V.IsValid(),TEXT("prepare_VM_and_explicit_body_permission"));if(!V){UE_LOG(LogTemp,Error,TEXT("prepare: %s"),*Error);return 1;}
 Check(!V->BindBody(-1,Box,Error) && !V->BindBody(8,Box,Error),TEXT("binding_slots_bounded"));
 Check(!V->BindBody(1,Box,Error),TEXT("same_body_cannot_alias_multiple_slots"));
 auto* ForeignBox=BoxOn(Owner(World.Get()));auto* ForeignWorldBox=BoxOn(Owner(Other.Get()));
 Check(!V->BindBody(0,ForeignBox,Error) && !V->BindBody(0,ForeignWorldBox,Error),TEXT("foreign_actor_and_world_not_authorized"));
 Check(!V->BindBody(0,GetMutableDefault<UBoxComponent>(),Error),TEXT("template_body_rejected"));
 auto* Static=BoxOn(Actor,false);Check(!V->BindBody(0,Static,Error),TEXT("static_non_simulated_body_rejected"));
 auto* ISM=NewObject<UInstancedStaticMeshComponent>(Actor);Actor->AddInstanceComponent(ISM);ISM->RegisterComponent();auto* Skeletal=NewObject<USkeletalMeshComponent>(Actor);Actor->AddInstanceComponent(Skeletal);Skeletal->RegisterComponent();
 Check(!V->BindBody(0,ISM,Error) && !V->BindBody(0,Skeletal,Error),TEXT("instance_and_skeletal_selectors_rejected"));
 Input.Values[15]=1;V->Publish(Input,Error);Step(30);Input.Values[15]=0;V->Publish(Input,Error);Step(3);FDasPhysicsOutput Out;
 Check(V->Read(Out) && Out.Packet.Values[0]>0 && FMath::Abs(Box->GetPhysicsLinearVelocity().X-100*Out.Packet.Values[0])<1,TEXT("DAS_force_integrates_with_inverse_mass"));
 UE_LOG(LogTemp,Display,TEXT("DAS_SCOPED_BODY_FORCE velocity=%g seconds=%g"),Box->GetPhysicsLinearVelocity().X,Out.Packet.Values[0]);V->Revoke();
 Box->SetPhysicsLinearVelocity(FVector::ZeroVector);Step(3);Box->PutRigidBodyToSleep();Step(3);Check(!Box->IsAnyRigidBodyAwake(),TEXT("sleeping_body_fixture"));
 V=Prepare(Actor,Box,2);Check(V.IsValid(),TEXT("prepare_acceleration_mode"));Step(30);Input.Values[15]=0;V->Publish(Input,Error);Step(3);
 Check(V->Read(Out) && Out.Packet.Values[0]>0 && Box->IsAnyRigidBodyAwake() && FMath::Abs(Box->GetPhysicsLinearVelocity().X-100*Out.Packet.Values[0])<1,TEXT("accel_change_scales_by_internal_mass_and_wakes_body"));V->Revoke();
 Box->SetPhysicsLinearVelocity(FVector::ZeroVector);Step(3);V=Prepare(Actor,Box,10);Check(V.IsValid(),TEXT("opaque_token_array_clone_ABI_prepares"));Input.Values[1]=-500;Input.Values[2]=250;V->Publish(Input,Error);Step(15);Input.Values[15]=0;V->Publish(Input,Error);Step(3);
 Check(V->Read(Out) && (Box->GetPhysicsLinearVelocity()-FVector(100,-50,25)*Out.Packet.Values[0]).Size()<1,TEXT("token_copy_array_and_XYZ_force_ABI"));V->Revoke();
 // Every failure stages a force or attempts invalid access, but must leave the
 // particle unchanged: commit occurs only after successful DAS completion.
 for(int32 Mode=3;Mode<=9;++Mode){Box->SetPhysicsLinearVelocity(FVector::ZeroVector);Step(3);V=Prepare(Actor,Box,Mode);const bool Prepared=V.IsValid();Step(4);UDasAsyncPhysicsBridge::FlushDeferred();FDasPhysicsOutput NoOutput;NoOutput.StepSequence=777;
  Check(Prepared && !V->IsLive() && !V->LastError().IsEmpty() && !V->Read(NoOutput) && NoOutput.StepSequence==777 && Box->GetPhysicsLinearVelocity().IsNearlyZero(),*FString::Printf(TEXT("error_revokes_and_discards_force_and_output_%d"),Mode));}
 V=Prepare(Actor,Box,0);auto NativeProbe=PrepareDasScopedPhysicsBodyProbeForTests(Actor,Box,Error);Check(NativeProbe.IsValid(),TEXT("prepare_native_foreign_scope_probe"));int32 ProbeSteps=0;bool ProbeGood=true;FDasAsyncPhysicsStep Saved;
 auto* Peer=UDasAsyncPhysicsBridge::Create(Actor,[&](const FDasAsyncPhysicsStep& S,FString&){++ProbeSteps;Saved=S;ProbeGood &= NativeProbe && NativeProbe->VerifyIsolation(S,Error);ProbeGood &= !V->BindBody(1,Box,Error) && !V->ClearBody(0,Error);return true;},Error);Step(4);Peer->Revoke();
 Check(ProbeSteps>=2 && ProbeGood,TEXT("foreign_VM_previous_epoch_null_tokens_and_frozen_rebind_rejected"));Check(!NativeProbe->VerifyIsolation(Saved,Error),TEXT("scope_expires_on_callback_exit"));
 Check(V->ClearBody(0,Error),TEXT("ordinary_unbind_supported"));Input.Values[15]=1;V->Publish(Input,Error);Step(3);Check(!V->IsLive() && Box->GetPhysicsLinearVelocity().IsNearlyZero(),TEXT("cleared_slot_cannot_issue_token"));
 V=Prepare(Actor,Box,0);Box->RecreatePhysicsState();Step(3);Box->SetPhysicsLinearVelocity(FVector::ZeroVector);Step(3);Input.Values[15]=1;V->Publish(Input,Error);Step(3);
 Check(!V->IsLive() && V->LastError().Contains(TEXT("generation")) && Box->GetPhysicsLinearVelocity().IsNearlyZero(),TEXT("recreate_invalidates_old_generation_without_dereferencing_old_proxy"));
 V=Prepare(Actor,Box,1);Step(4);Input.Values[15]=0;V->Publish(Input,Error);Step(3);Check(V->IsLive() && Box->GetPhysicsLinearVelocity().X>0,TEXT("explicit_rebind_after_recreate_uses_new_generation"));V->Revoke();
 auto* WeldChild=BoxOn(Actor);WeldChild->AttachToComponent(Box,FAttachmentTransformRules(EAttachmentRule::KeepWorld,true));Step(3);Check(WeldChild->BodyInstance.WeldParent!=nullptr,TEXT("real_welded_body_fixture"));V=CreateDasAsyncPhysicsVM(Path,Actor,Error);
 Check(V && !V->BindBody(0,WeldChild,Error) && !V->BindBody(0,Box,Error),TEXT("welded_child_and_compound_root_rejected"));V->Revoke();WeldChild->UnWeldFromParent();WeldChild->DestroyComponent();
 auto* UnregisterBody=BoxOn(Actor);Step(3);V=Prepare(Actor,UnregisterBody,0);UnregisterBody->UnregisterComponent();Input.Values[15]=1;V->Publish(Input,Error);Step(3);Check(!V->IsLive(),TEXT("body_unregister_revokes_on_use"));
 auto* DeadBody=BoxOn(Actor);Step(3);V=Prepare(Actor,DeadBody,0);TWeakObjectPtr<UObject> WeakBody=DeadBody;DeadBody->DestroyComponent();DeadBody=nullptr;CollectGarbage(RF_NoFlags);Input.Values[15]=1;V->Publish(Input,Error);Step(3);Check(!WeakBody.IsValid() && !V->IsLive(),TEXT("destroyed_component_not_retained_or_dereferenced"));
 FString Source;FFileHelper::LoadFileToString(Source,*Path);const FString Candidate=Path+TEXT(".candidate.das");
 for(int32 Mode=0;Mode<2;++Mode){const auto Body=Mode==0?TEXT("let body=ue_physics_body(0)"):TEXT("ue_physics_add_force(default<UEPhysicsBodyToken>,1.0lf,0.0lf,0.0lf,false)");FFileHelper::SaveStringToFile(FString::Printf(TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int { %s; return input }\n"),Body),*Candidate);const auto Result=Runtime.RunFile(Candidate,0,false,Actor);Check(!Result.bSucceeded && Result.Error.Contains(TEXT("isolated VM step")),Mode==0?TEXT("gameplay_cannot_acquire_body_token"):TEXT("gameplay_cannot_apply_physics_force"));}
 FFileHelper::SaveStringToFile(TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int { var body:UEPhysicsBodyToken; return int(body) }\n"),*Candidate);
 const auto Numeric=Runtime.RunFile(Candidate,0,false,Actor);Check(!Numeric.bSucceeded,TEXT("opaque_token_has_no_numeric_conversion"));
 FFileHelper::SaveStringToFile(TEXT("options gen2\nrequire ue_foundation\n[export]\ndef physics_tick(dt:float;time:float) { let forged=UEPhysicsBodyToken(1ul); ue_physics_add_force(forged,1.0lf,0.0lf,0.0lf,false) }\n"),*Candidate);
 auto Forged=CreateDasAsyncPhysicsVM(Candidate,Actor,Error);Check(!Forged,TEXT("opaque_token_has_no_integer_constructor"));if(Forged)Forged->Revoke();
 FFileHelper::SaveStringToFile(TEXT("options gen2\nrequire ue_foundation\nvar saved=ue_physics_body(0)\n[export]\ndef physics_tick(dt:float;time:float) { ue_physics_add_force(saved,1.0lf,0.0lf,0.0lf,false) }\n"),*Candidate);
 Check(!CreateDasAsyncPhysicsVM(Candidate,Actor,Error),TEXT("global_initializer_cannot_acquire_physics_token"));
 for(int32 Mode=0;Mode<2;++Mode){const auto Body=Mode==0?TEXT("@ue_property=(BlueprintReadWrite)\n Saved:UEPhysicsBodyToken"):TEXT("[ue_function(BlueprintCallable)]\n def Echo(value:UEPhysicsBodyToken):UEPhysicsBodyToken { return value }");FFileHelper::SaveStringToFile(FString::Printf(TEXT("options gen2\nrequire ue_foundation\n[ue_class(module_id=\"scopedphysicsrejected\",Blueprintable)]\nclass Rejected {\n %s\n}\n"),Body),*Candidate);FDasAuthoringModule Model;Check(!Runtime.ReadAuthoringScript(Candidate,Model,Error) && !Error.IsEmpty(),Mode==0?TEXT("token_not_supported_in_reflected_property_IR"):TEXT("token_not_supported_in_reflected_function_IR"));}
 V=Prepare(Actor,Box,0);Runtime.RunPersistent(Path,0,Actor);FFileHelper::SaveStringToFile(Source+TEXT("\n// accepted body reload\n"),*Path);const auto Reload=Runtime.RunPersistent(Path,0,Actor);FFileHelper::SaveStringToFile(Source,*Path);Check(Reload.bSucceeded && !V->IsLive(),TEXT("accepted_reload_revokes_body_capabilities"));
 auto* Dying=Owner(World.Get());auto* DyingBox=BoxOn(Dying);Step(3);V=Prepare(Dying,DyingBox,0);TWeakObjectPtr<UObject> WeakActor=Dying,WeakDyingBox=DyingBox;Dying->Destroy();Dying=nullptr;DyingBox=nullptr;Step(3);CollectGarbage(RF_NoFlags);Check(!V->IsLive() && !WeakActor.IsValid() && !WeakDyingBox.IsValid(),TEXT("body_permission_and_VM_handle_do_not_pin_destroyed_owner"));
 UE_LOG(LogTemp,Display,TEXT("DAS_SCOPED_BODY_RESULT=%s checks=%d"),Failures?TEXT("FAIL"):TEXT("PASS"),Checks);return Failures?1:0;
}
