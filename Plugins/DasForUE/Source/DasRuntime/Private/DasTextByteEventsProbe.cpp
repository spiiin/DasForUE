#include "DasStaticLibraryProbe.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "DasSchema.h"
#include "Engine/World.h"
#include "Modules/ModuleManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/TextProperty.h"
bool RunDasTextByteEventsProbe(ADasStaticLibraryProbe* Carrier,FString& Report){
 bool Good=true;auto Check=[&](bool V,const TCHAR* N){Good &= V;Report+=FString(V?TEXT("PASS "):TEXT("FAIL "))+N+TEXT("\n");};
 auto* Class=Carrier?Carrier->TextByteEventsClass.Get():nullptr;if(!Class){Check(false,TEXT("text_byte_events_cooked_class"));return false;}
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));FActorSpawnParameters Spawn;Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;auto* A=Carrier->GetWorld()->SpawnActor<ADasProbeActor>(Class,FTransform::Identity,Spawn);if(!A)return false;TStrongObjectPtr<ADasProbeActor> Keep(A);
 auto* Interface=LoadObject<UClass>(nullptr,TEXT("/Game/DasTextByteEvents/Types/TextByteContract.TextByteContract_C"));Check(Interface && Class->ImplementsInterface(Interface),TEXT("text_byte_authored_interface_cooked_identity"));
 for(int32 I=0;I<2;++I){
  auto* Probe=A->FindFunction(TEXT("Probe"));bool Result=false;if(Probe)A->ProcessEvent(Probe,&Result);Check(Probe && Result && A->LastDasError.IsEmpty(),TEXT("text_byte_delegates_return_ref_out_multicast_arrays"));if(!A->LastDasError.IsEmpty())Report+=A->LastDasError+TEXT("\n");
  const auto Call=R.RunNamedFramePersistent(R.ScriptPath(TEXT("dsl_text_byte_interface_calls.das")),TEXT("main"),{},EDasMethodSignature::Bool,A);Check(Call.bSucceeded && Call.BoolValue,TEXT("text_byte_interface_message_return_ref_out"));if(!Call.bSucceeded)Report+=Call.Error+TEXT("\n");
  const FText Original=FText::AsLocalizable_Advanced_LocText(TEXT("TextByteEvents"),TEXT("Source"),TEXT("Original text"));
  auto* Peer=Carrier->GetWorld()->SpawnActor<ADasProbeActor>(Class,FTransform::Identity,Spawn);if(!Peer)return false;TStrongObjectPtr<ADasProbeActor> KeepPeer(Peer);
  for(const auto* Name:{TEXT("RunText"),TEXT("RunByte")}){auto* F=Peer->FindFunction(Name);FStructOnScope Args(F);auto* Callback=F?FindFProperty<FDelegateProperty>(F,TEXT("callback")):nullptr;bool Pass=Callback!=nullptr;
   if(Callback){FScriptDelegate Binding;Binding.BindUFunction(A,Name==FString(TEXT("RunText"))?TEXT("EchoText"):TEXT("EchoByte"));Callback->SetPropertyValue_InContainer(Args.GetStructMemory(),Binding);
    if(auto* Input=FindFProperty<FTextProperty>(F,TEXT("value"))){Input->SetPropertyValue_InContainer(Args.GetStructMemory(),Original);Peer->ProcessEvent(F,Args.GetStructMemory());auto* Output=CastField<FTextProperty>(F->GetReturnProperty());Pass=Output && DasTextDefaultsEqual(Output->GetPropertyValue_InContainer(Args.GetStructMemory()),Original);}
    else {auto* ByteInput=FindFProperty<FByteProperty>(F,TEXT("value"));auto* ByteOutput=CastField<FByteProperty>(F->GetReturnProperty());Pass=ByteInput && ByteOutput && !ByteInput->Enum && !ByteOutput->Enum;if(Pass){ByteInput->SetPropertyValue_InContainer(Args.GetStructMemory(),255);Peer->ProcessEvent(F,Args.GetStructMemory());Pass=ByteOutput->GetPropertyValue_InContainer(Args.GetStructMemory())==255;}}
   }Check(Pass && Peer->LastDasError.IsEmpty(),TEXT("text_byte_delegate_parameter_native_cross_context"));if(!Peer->LastDasError.IsEmpty())Report+=Peer->LastDasError+TEXT("\n");}
  R.ReleaseObject(Peer);Peer->Destroy();
  auto* Seen=FindFProperty<FIntProperty>(Class,TEXT("Seen"));auto* Byte=FindFProperty<FByteProperty>(Class,TEXT("LastByte"));auto* Text=FindFProperty<FTextProperty>(Class,TEXT("LastText"));const int32 Before=Seen?Seen->GetPropertyValue_InContainer(A):0;
  A->OnDasByteSignal.Broadcast(255);A->OnDasTextSignal.Broadcast(Original);A->OnDasTextRefSignal.Broadcast(Original);A->OnDasArrayByteSignal.Broadcast({0,255});A->OnDasArrayTextSignal.Broadcast({Original,FText::AsCultureInvariant(TEXT("Invariant"))});
  Check(Seen && Seen->GetPropertyValue_InContainer(A)==Before+5 && Byte && Byte->GetPropertyValue_InContainer(A)==255 && Text && DasTextDefaultsEqual(Text->GetPropertyValue_InContainer(A),Original) && A->LastDasError.IsEmpty(),TEXT("text_byte_native_event_payloads_identity_arrays"));
  R.ReleaseObject(A);Check(R.SubscriptionCount(A)==0,TEXT("text_byte_event_release_detaches_listeners"));
 }
 A->Destroy();R.ReleaseObject(A);return Good;
}
