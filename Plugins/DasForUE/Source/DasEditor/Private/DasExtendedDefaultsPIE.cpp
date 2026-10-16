#include "DasDSLService.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "DasProbeActor.h"
#include "DasExtendedDefaultsProbe.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Editor.h"
#include "UObject/StrongObjectPtr.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace {
class FExtendedDefaultsPIECommand:public IAutomationLatentCommand{
 TStrongObjectPtr<UBlueprint> Blueprint;FAutomationTestBase* Test;bool Ending=false,Good=false;double Started=FPlatformTime::Seconds();
public:
 FExtendedDefaultsPIECommand(UBlueprint* B,FAutomationTestBase* T):Blueprint(B),Test(T){}
 bool Update()override{
  auto* World=GEditor->PlayWorld.Get();
  if(Ending){if(World && FPlatformTime::Seconds()-Started<30)return false;Good &= Test->TestTrue(TEXT("EndPIE releases defaults contexts"),!World && FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).PersistentContextCount()==0);UE_LOG(LogTemp,Display,TEXT("DAS_EXTENDED_DEFAULTS_PIE=%s"),Good?TEXT("PASS"):TEXT("FAIL"));return true;}
  if(!World || !World->HasBegunPlay()){if(FPlatformTime::Seconds()-Started<30)return false;Test->AddError(TEXT("Timed out starting defaults PIE"));return true;}
  FString Report;Good=RunDasExtendedDefaultsProbe(Blueprint->GeneratedClass,World,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);Test->TestTrue(TEXT("Extended defaults in real PIE"),Good);
  GEditor->RequestEndPlayMap();Ending=true;Started=FPlatformTime::Seconds();return false;
 }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtendedDefaultsPIETest,"DasForUE.Defaults.RuntimePIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FExtendedDefaultsPIETest::RunTest(const FString&){
 auto* World=FAutomationEditorCommonUtils::CreateNewMap();UBlueprint* BP=nullptr;FString Error;
 const FString Root=TEXT("/Game/DasExtendedDefaults/PIE_")+FGuid::NewGuid().ToString(EGuidFormats::Digits);
 if(!TestTrue(TEXT("Import defaults fixture"),FDasDSLService::Apply(TEXT("dsl_extended_defaults.das"),Root+TEXT("/BP_Defaults"),TEXT("extendeddefaults.ExtendedDefaults"),BP,Error))){AddError(Error);return false;}
 if(!World->SpawnActor<ADasProbeActor>(BP->GeneratedClass))return false;
 ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
 FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FExtendedDefaultsPIECommand>(BP,this));return true;
}
#endif
