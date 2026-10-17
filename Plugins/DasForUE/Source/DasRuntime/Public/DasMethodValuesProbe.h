#pragma once
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Curves/CurveFloat.h"
#include "Engine/World.h"
#include "Engine/EngineBaseTypes.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "DasSchema.h"
#include "DasContainerTypes.h"
#include "UObject/StructOnScope.h"
inline bool RunDasMethodValuesProbe(UClass* DriverClass,UClass* TargetClass,FString& Report){
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false));World->InitializeActorsForPlay(FURL());auto* Driver=World->SpawnActor<ADasProbeActor>(DriverClass);auto* Target=World->SpawnActor<ADasProbeActor>(TargetClass);if(!Driver || !Target)return false;
 struct FArguments{UObject* Target;int32 Result=0;};FArguments Args{Target};auto* Function=Driver->FindFunction(TEXT("Exercise"));if(Function)Driver->ProcessEvent(Function,&Args);bool Pass=Function && Args.Result==1 && Driver->LastDasError.IsEmpty();
 if(Pass){auto* Partner=FindFProperty<FObjectPropertyBase>(TargetClass,TEXT("Partner"));auto* Reject=Driver->FindFunction(TEXT("RejectNativeAuthoredField"));if(!Partner || !Reject)Pass=false;else {Partner->SetObjectPropertyValue_InContainer(Target,Driver);Args.Result=0;Driver->ProcessEvent(Reject,&Args);Pass=Args.Result==1 && Driver->LastDasError.IsEmpty() && !Partner->GetObjectPropertyValue_InContainer(Target);}Report+=FString::Printf(TEXT("%s native_assigned_wrong_authored_field_rejected_and_cleared\n"),Pass?TEXT("PASS"):TEXT("FAIL"));}
 if(Pass){auto* Partners=FindFProperty<FArrayProperty>(TargetClass,TEXT("DirectPartners"));auto* Reject=Driver->FindFunction(TEXT("RejectNativeAuthoredArrays"));if(!Partners || !Reject)Pass=false;else {FScriptArrayHelper Values(Partners,Partners->ContainerPtrToValuePtr<void>(Target));const int32 Element=Values.AddValue();if(!DasReferenceSet(Partners->Inner,Values.GetRawPtr(Element),Driver))Pass=false;else {Args.Result=0;Driver->ProcessEvent(Reject,&Args);Pass=Args.Result==1 && Driver->LastDasError.IsEmpty() && Values.Num()==0;}}Report+=FString::Printf(TEXT("%s native_wrong_authored_array_id_rejected_and_cleared\n"),Pass?TEXT("PASS"):TEXT("FAIL"));}
 if(Pass){auto* Ephemeral=World->SpawnActor<AActor>();auto* Property=FindFProperty<FObjectPropertyBase>(TargetClass,TEXT("NativeActor"));auto* Expired=Driver->FindFunction(TEXT("ExpiredNativeFields"));if(!Ephemeral || !Property || !Expired)Pass=false;else {Property->SetObjectPropertyValue_InContainer(Target,Ephemeral);auto* Settings=FindFProperty<FStructProperty>(TargetClass,TEXT("Settings"));auto* Point=Settings?CastField<FStructProperty>(FindDasStructField(Settings->Struct,TEXT("Point"))):nullptr;auto* Owner=Point?CastField<FObjectPropertyBase>(FindDasStructField(Point->Struct,TEXT("Owner"))):nullptr;if(!Owner)return false;Owner->SetObjectPropertyValue_InContainer(Point->ContainerPtrToValuePtr<void>(Settings->ContainerPtrToValuePtr<void>(Target)),Ephemeral);auto* Array=FindFProperty<FArrayProperty>(TargetClass,TEXT("DirectActors"));if(!Array)return false;FScriptArrayHelper Objects(Array,Array->ContainerPtrToValuePtr<void>(Target));const int32 Element=Objects.AddValue();if(!DasReferenceSet(Array->Inner,Objects.GetRawPtr(Element),Ephemeral))return false;Ephemeral->Destroy();Args.Result=0;Driver->ProcessEvent(Expired,&Args);Pass=Args.Result==1 && Driver->LastDasError.IsEmpty();}Report+=FString::Printf(TEXT("%s destroyed_native_reference_property_is_null\n"),Pass?TEXT("PASS"):TEXT("FAIL"));}Report+=FString::Printf(TEXT("%s natural_enum_nested_struct_result_ref_out_free_function_and_recovery\n"),Pass?TEXT("PASS"):TEXT("FAIL"));if(!Pass)Report+=FString::Printf(TEXT("result=%d error=%s\n"),Args.Result,*Driver->LastDasError);
 if(Pass){
  auto* Map=FindFProperty<FMapProperty>(DriverClass,TEXT("NameScores"));auto* Set=FindFProperty<FSetProperty>(DriverClass,TEXT("NameTags"));
  auto* Copy=DriverClass->FindFunctionByName(TEXT("CopyNameScores"));auto* CopySet=DriverClass->FindFunctionByName(TEXT("CopyNameTags"));
  auto* Input=Copy?FindFProperty<FMapProperty>(Copy,TEXT("values")):nullptr;auto* Output=Copy?CastField<FMapProperty>(Copy->GetReturnProperty()):nullptr;
  auto* SetInput=CopySet?FindFProperty<FSetProperty>(CopySet,TEXT("values")):nullptr;auto* SetOutput=CopySet?CastField<FSetProperty>(CopySet->GetReturnProperty()):nullptr;
  Pass=Map && Set && Input && Output && SetInput && SetOutput && CastField<FNameProperty>(Map->KeyProp) && CastField<FNameProperty>(Set->ElementProp) && Map->SameType(Input) && Map->SameType(Output) && Set->SameType(SetInput) && Set->SameType(SetOutput);
  if(Pass){FScriptMapHelper M(Map,Map->ContainerPtrToValuePtr<void>(Driver));FScriptSetHelper S(Set,Set->ContainerPtrToValuePtr<void>(Driver));FName Alias(TEXT("uNIT_3")),None(NAME_None);Pass=M.Num()==4 && M.FindMapIndexWithKey(&Alias)!=INDEX_NONE && M.FindMapIndexWithKey(&None)!=INDEX_NONE && S.Num()==3 && S.FindElementIndex(&Alias)!=INDEX_NONE;}
  Report+=FString::Printf(TEXT("%s authored_name_map_set_native_properties_signatures_and_equality\n"),Pass?TEXT("PASS"):TEXT("FAIL"));
 }
 if(Pass){
  auto* States=FindFProperty<FSetProperty>(DriverClass,TEXT("EnumStates"));auto* Native=FindFProperty<FSetProperty>(DriverClass,TEXT("NativeStates"));auto* Enum=States?DasEnumPropertyType(States->ElementProp):nullptr;
  Pass=States && Native && Enum && Enum->GetValueByNameString(TEXT("Idle"))==7 && Enum->GetValueByNameString(TEXT("Active"))==42 && DasEnumPropertyType(Native->ElementProp)==StaticEnum<ESpawnActorCollisionHandlingMethod>();
  for(const auto& Pair:TArray<TPair<FString,FSetProperty*>>{{TEXT("CopyEnumSet"),States},{TEXT("CopyNativeEnumSet"),Native}}){auto* Fn=DriverClass->FindFunctionByName(*Pair.Key);auto* Input=Fn?FindFProperty<FSetProperty>(Fn,TEXT("values")):nullptr;auto* Output=Fn?CastField<FSetProperty>(Fn->GetReturnProperty()):nullptr;Pass &= Pair.Value && Input && Output && Pair.Value->SameType(Input) && Pair.Value->SameType(Output);}
  if(Pass){FScriptSetHelper Values(States,States->ContainerPtrToValuePtr<void>(Driver));uint8 Active=42;Pass=Values.Num()==1 && Values.FindElementIndex(&Active)!=INDEX_NONE;
   auto* Carrier=DasContainerCarrierForProperty(States);FInstancedStruct Candidate;Candidate.InitializeAs(Carrier);auto* Set=Carrier?CastField<FSetProperty>(Carrier->FindPropertyByName(TEXT("Values"))):nullptr;if(!Set)Pass=false;else {FScriptSetHelper Items(Set,Set->ContainerPtrToValuePtr<void>(Candidate.GetMutableMemory()));FString Error;uint8 Invalid=0;Items.AddElement(&Invalid);Pass &= !ValidateDasStructSnapshot(Candidate,Error);Items.EmptyElements();Items.AddElement(&Active);Pass &= ValidateDasStructSnapshot(Candidate,Error);Items.EmptyElements();Invalid=255;Items.AddElement(&Invalid);Pass &= !ValidateDasStructSnapshot(Candidate,Error);}}
  Report+=FString::Printf(TEXT("%s enum_set_properties_signatures_sparse_values_and_snapshot_validation\n"),Pass?TEXT("PASS"):TEXT("FAIL"));
 }
 if(Pass){
  auto* Set=FindFProperty<FSetProperty>(DriverClass,TEXT("ActorKeys"));auto* Map=FindFProperty<FMapProperty>(DriverClass,TEXT("ObjectScores"));
  Pass=Set && Map && IsDasObjectHashProperty(Set->ElementProp) && IsDasObjectHashProperty(Map->KeyProp) && DasReferencePropertyClass(Set->ElementProp)==AActor::StaticClass() && DasReferencePropertyClass(Map->KeyProp)==AActor::StaticClass();
  for(const auto& Pair:TArray<TPair<FString,FProperty*>>{{TEXT("CopyObjectSet"),Set},{TEXT("CopyObjectMap"),Map}}){auto* Fn=DriverClass->FindFunctionByName(*Pair.Key);auto* Input=Fn?Fn->FindPropertyByName(TEXT("values")):nullptr;auto* Output=Fn?Fn->GetReturnProperty():nullptr;Pass &= Pair.Value && Input && Output && Pair.Value->SameType(Input) && Pair.Value->SameType(Output);}
  if(Pass){FScriptSetHelper Values(Set,Set->ContainerPtrToValuePtr<void>(Driver));FScriptMapHelper Scores(Map,Map->ContainerPtrToValuePtr<void>(Driver));TObjectPtr<UObject> SelfKey=Driver,NullKey=nullptr;Pass=Values.Num()==1 && Values.FindElementIndex(&SelfKey)!=INDEX_NONE && Scores.Num()==2 && Scores.FindMapIndexWithKey(&NullKey)!=INDEX_NONE;}
  if(Pass){FString Error;for(auto* Field:TArray<FProperty*>{Set,Map}){auto* Carrier=DasContainerCarrierForProperty(Field);FInstancedStruct Candidate;Candidate.InitializeAs(Carrier);auto* Values=Carrier->FindPropertyByName(TEXT("Values"));if(auto* NativeSet=CastField<FSetProperty>(Values)){FScriptSetHelper Items(NativeSet,NativeSet->ContainerPtrToValuePtr<void>(Candidate.GetMutableMemory()));Items.AddDefaultValue_Invalid_NeedsRehash();Items.AddDefaultValue_Invalid_NeedsRehash();Items.Rehash();}else {auto* NativeMap=CastFieldChecked<FMapProperty>(Values);FScriptMapHelper Items(NativeMap,NativeMap->ContainerPtrToValuePtr<void>(Candidate.GetMutableMemory()));Items.AddDefaultValue_Invalid_NeedsRehash();Items.AddDefaultValue_Invalid_NeedsRehash();Items.Rehash();}Pass &= !ValidateDasStructSnapshot(Candidate,Error);}}
  Report+=FString::Printf(TEXT("%s object_hash_native_properties_signatures_identity_and_null\n"),Pass?TEXT("PASS"):TEXT("FAIL"));
 }
 if(Pass){auto* Owned=NewObject<UCurveFloat>(GetTransientPackage());TWeakObjectPtr<UObject> Weak=Owned;
  auto Invoke=[&](const TCHAR* Name,UObject* Value){auto* Fn=Driver->FindFunction(Name);if(!Fn)return false;FStructOnScope Memory(Fn);if(auto* Arg=FindFProperty<FObjectPropertyBase>(Fn,TEXT("value")))Arg->SetObjectPropertyValue_InContainer(Memory.GetStructMemory(),Value);Driver->LastDasError.Reset();Driver->ProcessEvent(Fn,Memory.GetStructMemory());return Driver->LastDasError.IsEmpty();};
  const bool Hold=Invoke(TEXT("HoldHashObject"),Owned);CollectGarbage(RF_NoFlags);Pass=Hold && Weak.IsValid();
  const bool Clear=Invoke(TEXT("ClearHashObject"),nullptr);CollectGarbage(RF_NoFlags);Pass &= Clear && !Weak.IsValid();
  Report+=FString::Printf(TEXT("%s object_hash_UPROPERTY_pins_and_VM_keys_do_not_leak_GC_roots error=%s\n"),Pass?TEXT("PASS"):TEXT("FAIL"),*Driver->LastDasError);
 }
 World->DestroyWorld(false);Report+=FString::Printf(TEXT("DAS_METHOD_VALUES_RESULT=%s\n"),Pass?TEXT("PASS"):TEXT("FAIL"));return Pass;
}
