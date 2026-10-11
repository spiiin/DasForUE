#include "DasDSLService.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EngineUtils.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace {
int32 LifeValue(UObject* O,const TCHAR* Name){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),Name):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;}
struct FLifeState {TStrongObjectPtr<UBlueprint> Owner,Component;TStrongObjectPtr<UDasScriptComponent> Instance;int32 EndNotifications=0;};
class FLifeCommand:public IAutomationLatentCommand {
 TSharedRef<FLifeState> State;FAutomationTestBase* Test;int Phase=0;double Started=FPlatformTime::Seconds();
public:
 FLifeCommand(TSharedRef<FLifeState> S,FAutomationTestBase* T):State(S),Test(T){}
 bool Update()override {
  auto* W=GEditor->PlayWorld.Get();auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  if(Phase==2){if(W && FPlatformTime::Seconds()-Started<30)return false;Test->TestTrue(TEXT("PIE ended and sessions released"),!W && R.PersistentContextCount()==0);return true;}
  ADasProbeActor* Actor=nullptr;if(W)for(TActorIterator<ADasProbeActor> I(W);I;++I)if(I->GetFName()==TEXT("DynamicOwner"))Actor=*I;
  if(!Actor || !W->HasBegunPlay()){if(FPlatformTime::Seconds()-Started<45)return false;Test->AddError(TEXT("PIE startup timeout"));GEditor->RequestEndPlayMap();Phase=2;Started=FPlatformTime::Seconds();return false;}
  if(Phase==0){
   auto Result=R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_dynamic_owner.das")),TEXT("ue_DynamicOwner_Create"),false,Actor);
   Test->TestTrue(TEXT("nested BeginPlay completed before create returned"),Result.bSucceeded && Result.BoolValue);if(!Result.bSucceeded)Test->AddError(Result.Error);
   for(auto* C:Actor->GetComponents())if(C && C->GetFName()==TEXT("DynamicLife"))State->Instance.Reset(Cast<UDasScriptComponent>(C));
   if(!State->Instance.IsValid()){Test->AddError(TEXT("dynamic component missing"));GEditor->RequestEndPlayMap();Phase=2;Started=FPlatformTime::Seconds();return false;}
   State->Instance->OnScriptEnded.AddLambda([S=State](UDasScriptComponent*,EEndPlayReason::Type){++S->EndNotifications;});
   Phase=1;Started=FPlatformTime::Seconds();return false;
  }
  auto* C=State->Instance.Get();if(LifeValue(C,TEXT("Ticks"))<2 && FPlatformTime::Seconds()-Started<15)return false;
  Test->TestTrue(TEXT("dynamic component ticks"),LifeValue(C,TEXT("Ticks"))>=2);
  Test->TestTrue(TEXT("BeginPlay once"),LifeValue(C,TEXT("Begins"))==1);
  const int32 Before=LifeValue(C,TEXT("Ticks"));
  auto Result=R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_dynamic_owner.das")),TEXT("ue_DynamicOwner_Remove"),false,Actor);
  Test->TestTrue(TEXT("script-owned destruction"),Result.bSucceeded && Result.BoolValue);if(!Result.bSucceeded)Test->AddError(Result.Error);
  Test->TestTrue(TEXT("EndPlay once during destruction"),LifeValue(C,TEXT("Ends"))==1 && State->EndNotifications==1);
  Test->TestTrue(TEXT("destruction disables ticking and unregisters"),!C->IsRegistered() && !C->IsComponentTickEnabled() && LifeValue(C,TEXT("Ticks"))==Before);
  Test->TestTrue(TEXT("component lifecycle has no error"),C->LastDasError.IsEmpty());
  C->OnScriptEnded.Clear();State->Instance.Reset();GEditor->RequestEndPlayMap();Phase=2;Started=FPlatformTime::Seconds();return false;
 }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasDynamicComponentPIE,"DasForUE.Components.DynamicPIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FDasDynamicComponentPIE::RunTest(const FString&){
 auto S=MakeShared<FLifeState>();FString Error;UBlueprint* Component=nullptr;UBlueprint* Owner=nullptr;
 const FString Root=TEXT("/Game/DasDynamicPIE/")+FGuid::NewGuid().ToString(EGuidFormats::Digits);
 if(!FDasDSLService::Apply(TEXT("dsl_dynamic_lifecycle.das"),Root+TEXT("/BP_Component"),TEXT("dynamiclife.DynamicLife"),Component,Error) || !FDasDSLService::Apply(TEXT("dsl_dynamic_owner.das"),Root+TEXT("/BP_Owner"),TEXT("dynamicowner.DynamicOwner"),Owner,Error)){AddError(Error);return false;}
 S->Component.Reset(Component);S->Owner.Reset(Owner);
 auto* W=FAutomationEditorCommonUtils::CreateNewMap();FActorSpawnParameters P;P.Name=TEXT("DynamicOwner");auto* Actor=W->SpawnActor<ADasProbeActor>(Owner->GeneratedClass,FTransform::Identity,P);
 auto* Field=FindFProperty<FStrProperty>(Owner->GeneratedClass,TEXT("ComponentClass"));if(!Actor || !Field){AddError(TEXT("owner setup failed"));return false;}
 Field->SetPropertyValue_InContainer(Actor,Component->GeneratedClass->GetPathName());
 ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FLifeCommand>(S,this));return true;
}
#endif
