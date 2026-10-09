#pragma once
#include "DasRuntime.h"
#include "DasTypeRegistry.h"
#include "DasProbeActor.h"
#include "DasProbeInterface.h"
#include "Misc/Paths.h"
#include "DasAuthoredInterfaceClass.h"
#include "UObject/StructOnScope.h"
#include "UObject/Script.h"
#include "UObject/UnrealType.h"
inline void RunDasAuthoredInterfaceNativeValues(FDasRuntimeModule& R,UObject* Actor,const FString& Script,TFunctionRef<void(bool,const TCHAR*)> Check,bool Editor){
 auto Echo=[&](const TCHAR* Name,EDasMethodSignature Kind,const FDasScalarValue& Input){
  const auto Value=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_CounterActor_")+FString(Name),{Input},Kind,Actor,Input.ObjectClass);
  bool Match=Value.bSucceeded;
  if(Kind==EDasMethodSignature::Object)Match &= Value.ObjectValue.Get()==Input.ObjectValue.Get();
  else if(Kind==EDasMethodSignature::Vector)Match &= Value.VectorValue==Input.VectorValue;
  else if(Kind==EDasMethodSignature::Rotator)Match &= Value.RotatorValue==Input.RotatorValue;
  else if(Kind==EDasMethodSignature::Transform)Match &= Value.TransformValue.Equals(Input.TransformValue);
  else if(Kind==EDasMethodSignature::Enum)Match &= Value.Value==Input.IntValue;
  else if(Kind==EDasMethodSignature::Struct)Match &= Value.StructValue.IsValid() && Value.StructValue.GetScriptStruct()==Input.StructValue.GetScriptStruct() && Input.StructValue.GetScriptStruct()->CompareScriptStruct(Value.StructValue.GetMemory(),Input.StructValue.GetMemory(),0);
  if(!Value.bSucceeded)UE_LOG(LogTemp,Error,TEXT("Authored native signature %s: %s"),Name,*Value.Error);Check(Match,*(TEXT("authored_interface_native_frame_")+FString(Name)));
  auto* Method=Actor->FindFunction(*FString(Name));FStructOnScope Memory(Method);auto* In=Method?Method->FindPropertyByName(TEXT("value")):nullptr;auto* Out=Method?Method->GetReturnProperty():nullptr;
  if(!In || !Out){Check(false,TEXT("authored_interface_native_signature_pins_missing"));return;}
  if(Kind==EDasMethodSignature::Object)CastFieldChecked<FObjectProperty>(In)->SetObjectPropertyValue_InContainer(Memory.GetStructMemory(),Input.ObjectValue.Get());
  else if(Kind==EDasMethodSignature::Enum)SetDasEnumPropertyNumber(In,In->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()),Input.IntValue);
  else{const void* Source=Kind==EDasMethodSignature::Vector?static_cast<const void*>(&Input.VectorValue):Kind==EDasMethodSignature::Rotator?static_cast<const void*>(&Input.RotatorValue):Kind==EDasMethodSignature::Transform?static_cast<const void*>(&Input.TransformValue):Input.StructValue.GetMemory();In->CopyCompleteValue(In->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()),Source);}
#if WITH_EDITOR
  if(Editor){FEditorScriptExecutionGuard Guard;Actor->ProcessEvent(Method,Memory.GetStructMemory());}else
#endif
  {Actor->ProcessEvent(Method,Memory.GetStructMemory());}
  if(Kind==EDasMethodSignature::Object)Match=CastFieldChecked<FObjectProperty>(Out)->GetObjectPropertyValue_InContainer(Memory.GetStructMemory())==Input.ObjectValue.Get();
  else if(Kind==EDasMethodSignature::Enum)Match=DasEnumPropertyNumber(Out,Out->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()))==Input.IntValue;
  else Match=In->Identical(In->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()),Out->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()),0);
  Check(Match,*(TEXT("authored_interface_native_ProcessEvent_")+FString(Name)));
 };
 FDasScalarValue Object;Object.Kind=8;Object.ObjectClass=TEXT("/Script/Engine.Actor");Object.ObjectValue=Actor;Echo(TEXT("EchoActor"),EDasMethodSignature::Object,Object);Object.ObjectClass=TEXT("/Script/CoreUObject.Object");Echo(TEXT("EchoObject"),EDasMethodSignature::Object,Object);Object.ObjectValue=nullptr;Echo(TEXT("EchoObject"),EDasMethodSignature::Object,Object);
 FDasScalarValue Vector;Vector.Kind=9;Vector.VectorValue=FVector(1.25,-2.5,3.75);Echo(TEXT("EchoVector"),EDasMethodSignature::Vector,Vector);
 FDasScalarValue Rotator;Rotator.Kind=10;Rotator.RotatorValue=FRotator(15,30,45);Echo(TEXT("EchoRotator"),EDasMethodSignature::Rotator,Rotator);
 FDasScalarValue Transform;Transform.Kind=11;Transform.TransformValue=FTransform(FRotator(15,30,45),FVector(1.25,-2.5,3.75),FVector(2,3,4));Echo(TEXT("EchoTransform"),EDasMethodSignature::Transform,Transform);
 FDasScalarValue Mode;Mode.Kind=13;Mode.ObjectClass=TEXT("/Script/DasRuntime.EDasCallMode");Mode.IntValue=7;Echo(TEXT("EchoMode"),EDasMethodSignature::Enum,Mode);
 if(auto* Owner=Cast<ADasProbeActor>(Actor);Owner && Owner->TypeRegistry){FString RegistryError;auto* Enum=Cast<UEnum>(Owner->TypeRegistry->Resolve(TEXT("interfacevalues.State"),true,RegistryError));auto* Struct=Cast<UScriptStruct>(Owner->TypeRegistry->Resolve(TEXT("interfacevalues.Settings"),false,RegistryError));if(Enum && Struct){FDasScalarValue State;State.Kind=13;State.ObjectClass=Enum->GetPathName();State.IntValue=42;Echo(TEXT("EchoState"),EDasMethodSignature::Enum,State);FDasScalarValue Settings;Settings.Kind=12;Settings.ObjectClass=Struct->GetPathName();Settings.StructValue.InitializeAs(Struct);for(TFieldIterator<FProperty> It(Struct);It;++It){if(auto* Count=CastField<FIntProperty>(*It))Count->SetPropertyValue_InContainer(Settings.StructValue.GetMutableMemory(),33);if(auto* Title=CastField<FStrProperty>(*It))Title->SetPropertyValue_InContainer(Settings.StructValue.GetMutableMemory(),TEXT("Registry 世界"));}Echo(TEXT("EchoSettings"),EDasMethodSignature::Struct,Settings);}else Check(false,TEXT("authored_interface_registered_types_missing"));}
 auto* Payload=FindObject<UScriptStruct>(nullptr,TEXT("/Script/DasRuntime.DasCallPayload"));if(!Payload){Check(false,TEXT("authored_interface_native_payload_missing"));return;}
 FDasScalarValue Data;Data.Kind=12;Data.ObjectClass=Payload->GetPathName();Data.StructValue.InitializeAs(Payload);CastFieldChecked<FIntProperty>(Payload->FindPropertyByName(TEXT("Count")))->SetPropertyValue_InContainer(Data.StructValue.GetMutableMemory(),37);CastFieldChecked<FStrProperty>(Payload->FindPropertyByName(TEXT("Label")))->SetPropertyValue_InContainer(Data.StructValue.GetMutableMemory(),TEXT("Signature 世界"));CastFieldChecked<FDoubleProperty>(Payload->FindPropertyByName(TEXT("Weight")))->SetPropertyValue_InContainer(Data.StructValue.GetMutableMemory(),1.23456789012345);Echo(TEXT("EchoPayload"),EDasMethodSignature::Struct,Data);
}
inline bool RunDasAuthoredInterfaceValues(FDasRuntimeModule& R,UObject* Actor,UClass* Interface,const FString& Script,TFunctionRef<void(bool,const TCHAR*)> Check,bool Editor){
 bool Pass=true;auto Expect=[&](bool V,const TCHAR* Name){Pass &= V;Check(V,Name);};RunDasAuthoredInterfaceNativeValues(R,Actor,Script,Expect,Editor);auto* Identity=Cast<UDasAuthoredInterfaceClass>(Interface);Expect(Identity && Identity->StableId==TEXT("authoredinterface_")+FPaths::GetBaseFilename(Script).RightChop(15)+TEXT(".CounterContract") && !Identity->ContractJson.IsEmpty(),TEXT("authored_interface_persistent_identity"));Expect(Actor->GetClass()->ImplementsInterface(Interface),TEXT("authored_interface_native_membership"));
 auto Supports=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_CounterActor_SupportsContract"),{},EDasMethodSignature::Bool,Actor);Expect(Supports.bSucceeded && Supports.BoolValue,TEXT("body_authored_interface_type_token_membership"));
 auto SupportsNative=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_CounterActor_SupportsNative"),{},EDasMethodSignature::Bool,Actor);Expect(SupportsNative.bSucceeded && SupportsNative.BoolValue,TEXT("body_native_interface_short_string_membership"));
 FDasScalarValue Input;Input.Kind=0;Input.IntValue=5;auto Query=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_CounterActor_Query"),{Input},EDasMethodSignature::Int,Actor);Expect(Query.bSucceeded && Query.Value==12,TEXT("authored_interface_typed_query"));
 auto Event=[&](UFunction* F,void* Memory){
#if WITH_EDITOR
  if(Editor){FEditorScriptExecutionGuard Guard;Actor->ProcessEvent(F,Memory);}else
#endif
  {Actor->ProcessEvent(F,Memory);}
 };
 auto* Score=FindFProperty<FIntProperty>(Actor->GetClass(),TEXT("Score"));Expect(Score && Actor->GetClass()->ImplementsInterface(UDasEventInterface::StaticClass()),TEXT("interface_events_native_membership_and_state"));if(Score){auto* Add=Actor->FindFunction(TEXT("AddScore"));FStructOnScope EventArgs(Add);FindFProperty<FIntProperty>(Add,TEXT("amount"))->SetPropertyValue_InContainer(EventArgs.GetStructMemory(),5);Event(Add,EventArgs.GetStructMemory());Expect(Score->GetPropertyValue_InContainer(Actor)==5,TEXT("authored_interface_event_scalar_ProcessEvent"));auto* Clear=Actor->FindFunction(TEXT("ClearScore"));FStructOnScope ClearArgs(Clear);Event(Clear,ClearArgs.GetStructMemory());Expect(Score->GetPropertyValue_InContainer(Actor)==0,TEXT("authored_interface_event_no_parameters"));
auto NativeEvent=[&](bool Reset){
#if WITH_EDITOR
 if(Editor){FEditorScriptExecutionGuard Guard;if(Reset)IDasEventInterface::Execute_ResetCounter(Actor);else IDasEventInterface::Execute_Notify(Actor,9);}else
#endif
 {if(Reset)IDasEventInterface::Execute_ResetCounter(Actor);else IDasEventInterface::Execute_Notify(Actor,9);}
 };NativeEvent(false);Expect(Score->GetPropertyValue_InContainer(Actor)==9,TEXT("native_interface_event_Execute_input"));NativeEvent(true);Expect(Score->GetPropertyValue_InContainer(Actor)==0,TEXT("native_interface_event_Execute_no_parameters"));auto* Labels=Actor->FindFunction(TEXT("PushLabels"));FStructOnScope LabelArgs(Labels);auto* Items=FindFProperty<FArrayProperty>(Labels,TEXT("Values"));FScriptArrayHelper Values(Items,Items->ContainerPtrToValuePtr<void>(LabelArgs.GetStructMemory()));Values.Resize(2);CastFieldChecked<FStrProperty>(Items->Inner)->SetPropertyValue(Values.GetRawPtr(0),TEXT("One 世界"));CastFieldChecked<FStrProperty>(Items->Inner)->SetPropertyValue(Values.GetRawPtr(1),TEXT("Two"));FindFProperty<FStrProperty>(Labels,TEXT("message"))->SetPropertyValue_InContainer(LabelArgs.GetStructMemory(),TEXT("Event 世界"));Event(Labels,LabelArgs.GetStructMemory());Expect(Score->GetPropertyValue_InContainer(Actor)==2 && FindFProperty<FStrProperty>(Actor->GetClass(),TEXT("LastMessage"))->GetPropertyValue_InContainer(Actor)==TEXT("Event 世界") && Values.Num()==2,TEXT("authored_interface_event_array_Unicode_input_ownership"));Score->SetPropertyValue_InContainer(Actor,0);
 FDasScalarValue V;V.Kind=0;V.IntValue=12;auto Direct=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_CounterActor_AddScore"),{V},EDasMethodSignature::Void,Actor);Expect(Direct.bSucceeded && Score->GetPropertyValue_InContainer(Actor)==12,TEXT("interface_event_typed_void_adapter"));Score->SetPropertyValue_InContainer(Actor,0);}
 auto* Label=Actor->FindFunction(TEXT("Label"));FStructOnScope Args(Label);Event(Label,Args.GetStructMemory());Expect(CastFieldChecked<FStrProperty>(Label->GetReturnProperty())->GetPropertyValue_InContainer(Args.GetStructMemory())==TEXT("Authored 世界"),TEXT("authored_interface_ProcessEvent_Unicode"));auto* Stats=Actor->FindFunction(TEXT("Stats"));FStructOnScope Params(Stats);auto* Counter=FindFProperty<FIntProperty>(Stats,TEXT("Counter"));auto* Total=FindFProperty<FIntProperty>(Stats,TEXT("Total"));Counter->SetPropertyValue_InContainer(Params.GetStructMemory(),10);Event(Stats,Params.GetStructMemory());Expect(Counter->GetPropertyValue_InContainer(Params.GetStructMemory())==13 && Total->GetPropertyValue_InContainer(Params.GetStructMemory())==26,TEXT("authored_interface_out_ref_copyback"));auto* Weights=Actor->FindFunction(TEXT("Weights"));FStructOnScope ArrayArgs(Weights);auto* In=FindFProperty<FArrayProperty>(Weights,TEXT("Values"));auto* Out=CastFieldChecked<FArrayProperty>(Weights->GetReturnProperty());FScriptArrayHelper InputArray(In,In->ContainerPtrToValuePtr<void>(ArrayArgs.GetStructMemory()));InputArray.Resize(1);CastFieldChecked<FDoubleProperty>(In->Inner)->SetPropertyValue(InputArray.GetRawPtr(0),2.5);Event(Weights,ArrayArgs.GetStructMemory());FScriptArrayHelper ResultArray(Out,Out->ContainerPtrToValuePtr<void>(ArrayArgs.GetStructMemory()));Expect(ResultArray.Num()==2 && CastFieldChecked<FDoubleProperty>(Out->Inner)->GetPropertyValue(ResultArray.GetRawPtr(1))==1.23456789012345 && InputArray.Num()==1,TEXT("authored_interface_array_return_precision_and_ownership"));if(!Editor){auto* Owner=Cast<AActor>(Actor);for(const auto& Candidate:Owner->GetWorld()->PersistentLevel->Actors)if(Candidate && Candidate->GetFName()==TEXT("WrapperReceiver")){const FString Id=FPaths::GetBaseFilename(Script).RightChop(15);const auto Wrapped=R.RunFile(R.ScriptPath(TEXT("authored_wrapper_check_")+Id+TEXT(".das")),0,false,Candidate.Get());if(!Wrapped.bSucceeded)UE_LOG(LogTemp,Error,TEXT("Authored wrappers: %s"),*Wrapped.Error);Expect(Wrapped.bSucceeded && Wrapped.Value==1,TEXT("generated_authored_interface_calls_cooked"));}}
R.ReleaseObject(Actor);return Pass;
}
