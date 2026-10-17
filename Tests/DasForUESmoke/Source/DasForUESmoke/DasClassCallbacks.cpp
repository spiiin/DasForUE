#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "EngineUtils.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#include "UObject/GarbageCollection.h"
namespace {
int32 ClassCount(UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;}
struct FClassCallbacksCooked {
 TWeakObjectPtr<UWorld> World;TWeakObjectPtr<ADasProbeActor> Owner;
 double Started=FPlatformTime::Seconds();bool Good=true;
 void Check(bool OK,const TCHAR* Name){Good &= OK;UE_LOG(LogTemp,Display,TEXT("DAS_CLASS_CALLBACKS_PACKAGED %s %s"),OK?TEXT("PASS"):TEXT("FAIL"),Name);}
 bool Finish(){auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));if(auto* A=Owner.Get()){R.ReleaseObject(A);A->Destroy();}Check(R.PersistentContextCount(World.Get())==0 && R.ScriptTimerCount()==0,TEXT("owner_cleanup"));UE_LOG(LogTemp,Display,TEXT("DAS_CLASS_CALLBACKS_PACKAGED_RESULT=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);return false;}
 bool Tick(float){
  auto* A=Owner.Get();auto* W=World.Get();
  if(!A || !W || !W->HasBegunPlay() || ClassCount(A,TEXT("Repeats"))<3 || ClassCount(A,TEXT("MixedNext"))<1){
   if(FPlatformTime::Seconds()-Started<30 && (!A || A->LastDasError.IsEmpty()))return true;
   Check(false,TEXT("callbacks_completed"));if(A)UE_LOG(LogTemp,Error,TEXT("%s"),*A->LastDasError);return Finish();
  }
  auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  Check(A->LastDasError.IsEmpty() && ClassCount(A,TEXT("Errors"))==0,TEXT("runtime_without_editor_metadata"));
  Check(ClassCount(A,TEXT("Calls"))==2,TEXT("single_delegate_class_result_and_null"));
  Check(ClassCount(A,TEXT("Events"))==1 && ClassCount(A,TEXT("NullEvents"))==1 && ClassCount(A,TEXT("MixedEvents"))==1,TEXT("scalar_null_mixed_events"));
  Check(ClassCount(A,TEXT("Next"))==1 && ClassCount(A,TEXT("Nulls"))==1 && ClassCount(A,TEXT("MixedNext"))==1,TEXT("scalar_null_mixed_timer_captures"));
  Check(ClassCount(A,TEXT("Repeats"))==3 && ClassCount(A,TEXT("Rejected"))==1 && R.ScriptTimerCount(A)==0,TEXT("repeat_cancel_rejected_replacement"));
  CollectGarbage(RF_NoFlags);
  auto* Interface=LoadObject<UClass>(nullptr,TEXT("/Game/DasClassCallbacks/Types/ClassContract.ClassContract_C"));
  Check(Interface && Interface->HasAnyClassFlags(CLASS_Interface) && A->GetClass()->ImplementsInterface(Interface),TEXT("cooked_interface_membership_after_GC"));
  auto* Function=A->FindFunction(TEXT("ChangeClass"));auto* Declared=Interface?Interface->FindFunctionByName(TEXT("ChangeClass")):nullptr;
  Check(Function && Declared && Function->IsSignatureCompatibleWith(Declared),TEXT("cooked_interface_signature"));
  if(Function){FStructOnScope Memory(Function);auto* Input=FindFProperty<FClassProperty>(Function,TEXT("value"));auto* Previous=FindFProperty<FClassProperty>(Function,TEXT("previous"));auto* Result=CastField<FClassProperty>(Function->GetReturnProperty());
   Check(Input && Previous && Result && Input->MetaClass==AActor::StaticClass() && Previous->MetaClass==AActor::StaticClass() && Result->MetaClass==AActor::StaticClass(),TEXT("cooked_class_pin_constraints"));
   if(Input && Previous && Result)for(UClass* Class:{AStaticMeshActor::StaticClass(),A->GetClass(),static_cast<UClass*>(nullptr)}){
    Input->SetObjectPropertyValue_InContainer(Memory.GetStructMemory(),Class);Previous->SetObjectPropertyValue_InContainer(Memory.GetStructMemory(),nullptr);Result->SetObjectPropertyValue_InContainer(Memory.GetStructMemory(),nullptr);
    A->ProcessEvent(Function,Memory.GetStructMemory());Check(!Input->GetObjectPropertyValue_InContainer(Memory.GetStructMemory()) && Previous->GetObjectPropertyValue_InContainer(Memory.GetStructMemory())==Class && Result->GetObjectPropertyValue_InContainer(Memory.GetStructMemory())==Class,TEXT("interface_native_Blueprint_null_return_ref_out"));
   }
  }
  return Finish();
 }
};
}
void StartDasClassCallbacksValidation(){auto S=MakeShared<FClassCallbacksCooked>();for(const auto& C:GEngine->GetWorldContexts())if(auto* W=C.World())if(W->IsGameWorld()){S->World=W;for(TActorIterator<ADasProbeActor> I(W);I;++I)if(I->GetFName()==TEXT("ClassCallbacks"))S->Owner=*I;}FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([S](float Delta){return S->Tick(Delta);}));}
