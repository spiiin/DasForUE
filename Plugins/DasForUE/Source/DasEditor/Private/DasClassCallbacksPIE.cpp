#include "DasDSLService.h"
#include "DasInterfaceService.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace {
struct FClassCallbackState {
 TArray<TStrongObjectPtr<UBlueprint>> Interfaces;
 TStrongObjectPtr<UBlueprint> Blueprint;
 bool Passed=true;
};
int32 ClassCount(UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;}
class FClassCallbackCommand:public IAutomationLatentCommand {
 TSharedRef<FClassCallbackState> State;FAutomationTestBase* Test;int32 Phase=0;double Start=FPlatformTime::Seconds();
 void Check(bool Good,const TCHAR* Name){State->Passed &= Test->TestTrue(Name,Good);UE_LOG(LogTemp,Display,TEXT("DAS_CLASS_CALLBACKS %s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);}
public:
 FClassCallbackCommand(TSharedRef<FClassCallbackState> S,FAutomationTestBase* T):State(S),Test(T){}
 bool Update() override {
  auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto* W=GEditor->PlayWorld.Get();
  if(Phase==1){if(W && FPlatformTime::Seconds()-Start<30)return false;Check(!W,TEXT("PIE ended"));UE_LOG(LogTemp,Display,TEXT("DAS_CLASS_CALLBACKS_RESULT=%s"),State->Passed?TEXT("PASS"):TEXT("FAIL"));return true;}
  ADasProbeActor* Owner=nullptr;if(W)for(TActorIterator<ADasProbeActor> I(W);I;++I)if(I->GetFName()==TEXT("ClassCallbacks"))Owner=*I;
  if(!Owner || ClassCount(Owner,TEXT("Repeats"))<3 || ClassCount(Owner,TEXT("MixedNext"))<1){
   if(FPlatformTime::Seconds()-Start<30 && (!Owner || Owner->LastDasError.IsEmpty()))return false;
   Check(false,TEXT("class callbacks completed"));if(Owner && !Owner->LastDasError.IsEmpty())Test->AddError(Owner->LastDasError);
  }else{
   Check(Owner->LastDasError.IsEmpty() && ClassCount(Owner,TEXT("Errors"))==0,TEXT("typed class conversions have no errors"));
   Check(ClassCount(Owner,TEXT("Calls"))==2,TEXT("single delegate class result and nullable input"));
   Check(ClassCount(Owner,TEXT("NullEvents"))==1 && ClassCount(Owner,TEXT("Events"))==1 && ClassCount(Owner,TEXT("MixedEvents"))==1,TEXT("scalar and mixed multicast events"));
   Check(ClassCount(Owner,TEXT("Next"))==1 && ClassCount(Owner,TEXT("Nulls"))==1 && ClassCount(Owner,TEXT("MixedNext"))==1,TEXT("single null and mixed next-tick captures"));
   Check(ClassCount(Owner,TEXT("Repeats"))==3 && ClassCount(Owner,TEXT("Rejected"))==1 && R.ScriptTimerCount(Owner)==0,TEXT("repeat cancellation and rejected replacement preserve timer"));
   auto* Function=Owner->FindFunction(TEXT("ChangeClass"));FStructOnScope Memory(Function);
   auto* Input=Function?FindFProperty<FClassProperty>(Function,TEXT("value")):nullptr;
   auto* Previous=Function?FindFProperty<FClassProperty>(Function,TEXT("previous")):nullptr;
   auto* Result=Function?CastField<FClassProperty>(Function->GetReturnProperty()):nullptr;
   Check(Input && Previous && Result && Input->MetaClass==AActor::StaticClass() && Result->MetaClass==AActor::StaticClass(),TEXT("authored interface retains class pin base"));
   if(Input && Previous && Result){Input->SetObjectPropertyValue_InContainer(Memory.GetStructMemory(),AStaticMeshActor::StaticClass());Owner->ProcessEvent(Function,Memory.GetStructMemory());Check(!Input->GetObjectPropertyValue_InContainer(Memory.GetStructMemory()) && Previous->GetObjectPropertyValue_InContainer(Memory.GetStructMemory())==AStaticMeshActor::StaticClass() && Result->GetObjectPropertyValue_InContainer(Memory.GetStructMemory())==AStaticMeshActor::StaticClass(),TEXT("Blueprint interface class return ref and out roundtrip"));}
  }
  GEditor->RequestEndPlayMap();Phase=1;Start=FPlatformTime::Seconds();return false;
 }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasClassCallbacksPIE,"DasForUE.Callbacks.ClassReferencesPIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FDasClassCallbacksPIE::RunTest(const FString&){
 auto State=MakeShared<FClassCallbackState>();FString Error;TArray<UBlueprint*> Interfaces;
 if(!FDasInterfaceService::Apply(TEXT("dsl_class_callback_interface.das"),TEXT("/Game/DasClassCallbacks/Types"),Interfaces,Error)){AddError(Error);return false;}
 for(auto* Interface:Interfaces)State->Interfaces.Emplace(Interface);
 UBlueprint* BP=nullptr;
 if(!FDasDSLService::Apply(TEXT("dsl_class_callbacks.das"),TEXT("/Game/DasClassCallbacks/BP_ClassCallbacks"),TEXT("dsl_class_callbacks.ClassCallbackActor"),BP,Error)){AddError(Error);return false;}
 State->Blueprint.Reset(BP);auto* World=FAutomationEditorCommonUtils::CreateNewMap();if(!World)return false;
 FActorSpawnParameters Spawn;Spawn.Name=TEXT("ClassCallbacks");if(!World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,Spawn))return false;
 ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));ADD_LATENT_AUTOMATION_COMMAND(FClassCallbackCommand(State,this));return true;
}
#endif
