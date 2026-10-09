#pragma once
#include "DasRuntime.h"
#include "DasProbeInterface.h"
#include "DasContainerTypes.h"
#include "UObject/Script.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Engine/Level.h"
inline void RunDasGeneratedInterfaceProbe(FDasRuntimeModule& R,UObject* Actor,UObject* Receiver,TFunctionRef<void(bool,const TCHAR*)> Check){
 const FString Path=R.ScriptPath(TEXT("generated_interface_calls.das"));
 const TArray<int32> Expected={123,30,1,375,1,31,1,1};
 for(int32 Index=0;Index<Expected.Num();++Index){auto Result=R.RunFile(Path,Index,false,Receiver);if(!Result.bSucceeded)UE_LOG(LogTemp,Error,TEXT("Generated interface: %s"),*Result.Error);Check(Result.bSucceeded && Result.Value==Expected[Index],*(TEXT("generated_interface_case_")+FString::FromInt(Index)));if(Index==6 || Index==7){auto* Field=FindFProperty<FIntProperty>(Receiver->GetClass(),TEXT("EventCount"));Check(Field && Field->GetPropertyValue_InContainer(Receiver)==(Index==6?17:0),Index==6?TEXT("generated_interface_void_event_input"):TEXT("generated_interface_void_event_no_input"));}}
 auto Stale=R.RunFile(Path,12,false,Receiver);Check(!Stale.bSucceeded && Stale.Error.Contains(TEXT("signature changed")),TEXT("generated_interface_stale_declaration_rejected"));
 auto Reentry=R.RunFile(Path,0,false,Actor);Check(!Reentry.bSucceeded && Reentry.Error.Contains(TEXT("reentrant")),TEXT("generated_interface_VM_reentry_remains_rejected"));
}
inline void RunDasTypedInterfaceProbe(FDasRuntimeModule& R,UObject* Actor,UObject* Receiver,const FString& Script,TFunctionRef<void(bool,const TCHAR*)> Check){
 RunDasGeneratedInterfaceProbe(R,Actor,Receiver,Check);
 FDasScalarValue Target,Input;Target.Kind=8;Target.ObjectClass=TEXT("/Script/CoreUObject.Object");Target.ObjectValue=Receiver;Input.Kind=0;Input.IntValue=5;
 auto Query=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_TypedQuery"),{Target,Input},EDasMethodSignature::Int,Actor);Check(Query.bSucceeded && Query.Value==123,TEXT("typed_interface_scalar_frame"));
 auto Weights=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_TypedWeights"),{Target},EDasMethodSignature::Double,Actor);Check(Weights.bSucceeded && Weights.DoubleValue==2.5+1.23456789012345+2,TEXT("typed_interface_array_double_roundtrip"));
 auto Stats=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_TypedStats"),{Target},EDasMethodSignature::Int,Actor);Check(Stats.bSucceeded && Stats.Value==30,TEXT("typed_interface_multiple_out_ref"));
 auto Missing=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_TypedMissingInput"),{Target},EDasMethodSignature::Int,Actor);Check(!Missing.bSucceeded && Missing.Error.Contains(TEXT("missing input")),TEXT("typed_interface_missing_input_rejected"));
 auto Wrong=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_TypedWrongScalar"),{Target},EDasMethodSignature::Int,Actor);Check(!Wrong.bSucceeded,TEXT("typed_interface_wrong_scalar_rejected"));
 auto Released=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_TypedReleased"),{Target},EDasMethodSignature::Int,Actor);Check(!Released.bSucceeded && Released.Error.Contains(TEXT("released")),TEXT("typed_interface_released_handle_rejected"));
 auto Map=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_TypedMap"),{Target},EDasMethodSignature::String,Actor);Check(Map.bSucceeded && Map.StringValue==TEXT("Map 世界"),TEXT("typed_interface_map_Unicode_owned_return"));
 auto Set=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_TypedSet"),{Target},EDasMethodSignature::Int,Actor);Check(Set.bSucceeded && Set.Value==31,TEXT("typed_interface_set_owned_return"));
 R.ReleaseObject(Actor);
}
inline bool RunDasInterfaceProbe(FDasRuntimeModule& R,AActor* Actor,const FString& Script,TFunctionRef<void(bool,const TCHAR*)> Check,bool Editor){
 bool Pass=true;auto Expect=[&](bool V,const TCHAR* Name){Pass &= V;Check(V,Name);};
 auto Native=[&](){Expect(Actor->GetClass()->ImplementsInterface(UDasProbeInterface::StaticClass()),TEXT("interface_native_membership"));Expect(IDasProbeInterface::Execute_Query(Actor,5)==12,TEXT("interface_Execute_Query_dispatch"));Expect(IDasProbeInterface::Execute_Label(Actor)==TEXT("Interface 世界"),TEXT("interface_Execute_Label_Unicode"));TArray<double> Input={2.5};auto Output=IDasProbeInterface::Execute_Weights(Actor,Input);Expect(Output==TArray<double>{2.5,1.23456789012345} && Input.Num()==1,TEXT("interface_Execute_array_owned_return"));int32 Counter=10,Total=777;IDasProbeInterface::Execute_Stats(Actor,Counter,Total);Expect(Counter==13 && Total==26,TEXT("interface_Execute_out_ref"));};
#if WITH_EDITOR
 if(Editor){FEditorScriptExecutionGuard Guard;Native();}else
#endif
 {Native();}
 R.ReleaseObject(Actor);
 FDasScalarValue Target;Target.Kind=8;Target.ObjectClass=TEXT("/Script/CoreUObject.Object");Target.ObjectValue=Actor;
 auto Membership=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_Implements"),{Target},EDasMethodSignature::Bool,Actor);Expect(Membership.bSucceeded && Membership.BoolValue,TEXT("interface_script_membership"));
 Target.ObjectValue=nullptr;auto Null=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_Implements"),{Target},EDasMethodSignature::Bool,Actor);Expect(Null.bSucceeded && !Null.BoolValue,TEXT("interface_null_membership_false"));
 Target.ObjectValue=NewObject<USphereComponent>();auto Other=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_Implements"),{Target},EDasMethodSignature::Bool,Actor);Expect(Other.bSucceeded && !Other.BoolValue,TEXT("interface_nonimplementer_membership_false"));auto BadClass=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_BadInterface"),{Target},EDasMethodSignature::Bool,Actor);Expect(!BadClass.bSucceeded && BadClass.Error.Contains(TEXT("UInterface")),TEXT("interface_wrong_class_rejected"));FDasScalarValue Value;Value.Kind=0;Value.IntValue=5;auto BadTarget=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_InterfaceQuery"),{Target,Value},EDasMethodSignature::Int,Actor);Expect(!BadTarget.bSucceeded && BadTarget.Error.Contains(TEXT("does not implement")),TEXT("interface_wrong_target_dispatch_rejected"));R.ReleaseObject(Actor);
 if(!Editor){AActor* Receiver=nullptr;for(const auto& Candidate:Actor->GetWorld()->PersistentLevel->Actors)if(Candidate && Candidate->GetFName()==TEXT("InterfaceReceiver"))Receiver=Candidate.Get();Expect(Receiver!=nullptr,TEXT("interface_cooked_plain_receiver_loaded"));if(Receiver){Target.ObjectValue=Receiver;auto Query=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_InterfaceQuery"),{Target,Value},EDasMethodSignature::Int,Actor);Expect(Query.bSucceeded && Query.Value==123,TEXT("interface_cooked_das_to_Blueprint_int"));auto Label=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_NestedActor_InterfaceLabel"),{Target},EDasMethodSignature::String,Actor);Expect(Label.bSucceeded && Label.StringValue==TEXT("Blueprint 世界"),TEXT("interface_cooked_das_to_Blueprint_Unicode"));RunDasTypedInterfaceProbe(R,Actor,Receiver,Script,Check);R.ReleaseObject(Actor);}}
 return Pass;
}
