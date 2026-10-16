#include "DasDSLService.h"
#include "DasLifecycleTestObserver.h"
#include "DasScriptComponent.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "DasActorComponentProbe.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "EngineUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/StrongObjectPtr.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace {
struct FComponentPIEState {TStrongObjectPtr<UBlueprint> Blueprint;TStrongObjectPtr<UDasComponentLifecycleObserver> Observer{NewObject<UDasComponentLifecycleObserver>()};bool Passed=true;};
class FComponentPIECommand:public IAutomationLatentCommand {
 TSharedRef<FComponentPIEState> State;FAutomationTestBase* Test;bool Ending=false;double Started=FPlatformTime::Seconds();
public:FComponentPIECommand(TSharedRef<FComponentPIEState> S,FAutomationTestBase* T):State(S),Test(T){}
 bool Update()override{auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto Check=[&](bool Good,const TCHAR* Name){State->Passed &= Test->TestTrue(Name,Good);UE_LOG(LogTemp,Display,TEXT("DAS_COMPONENT_PIE %s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);};
 if(Ending){if(GEditor->PlayWorld && FPlatformTime::Seconds()-Started<30)return false;Check(!GEditor->PlayWorld && R.PersistentContextCount()==0 && R.SubscriptionCount()==0,TEXT("EndPIE_releases_all_contexts_and_bindings"));Check(State->Observer->Calls==1 && State->Observer->ActorEnds==1 && State->Observer->ComponentEnds==1,TEXT("remaining_actor_and_component_EndPIE_once"));UE_LOG(LogTemp,Display,TEXT("DAS_ACTOR_COMPONENT_PIE_RESULT=%s"),State->Passed?TEXT("PASS"):TEXT("FAIL"));return true;}
 auto* World=GEditor->PlayWorld.Get();ADasProbeActor* A=nullptr;ADasProbeActor* B=nullptr;if(World)for(TActorIterator<ADasProbeActor> It(World);It;++It){if(It->GetFName()==TEXT("ComponentFirst"))A=*It;if(It->GetFName()==TEXT("ComponentSecond"))B=*It;}auto* CA=A?A->FindComponentByClass<UDasScriptComponent>():nullptr;auto* CB=B?B->FindComponentByClass<UDasScriptComponent>():nullptr;
 if(!World || !World->HasBegunPlay() || !CA || !CB || DasComponentInt(CA,TEXT("TickCount"))<2 || DasComponentInt(CB,TEXT("TickCount"))<2){if(FPlatformTime::Seconds()-Started<60)return false;Check(false,TEXT("components_begin_and_tick_before_timeout"));GEditor->RequestEndPlayMap();return true;}
 Check(DasComponentInt(CA,TEXT("BeginCount"))==1 && DasComponentInt(CB,TEXT("BeginCount"))==1 && DasComponentFloat(CA,TEXT("Seconds"))>0 && DasComponentFloat(CB,TEXT("Seconds"))>0 && CA->LastDasError.IsEmpty() && CB->LastDasError.IsEmpty(),TEXT("real_PIE_component_BeginPlay_and_tick_delta"));Check(R.PersistentContextCount(World)==4 && R.PersistentContextCount(GEditor->GetEditorWorldContext().World())==0,TEXT("PIE_sessions_exclude_editor_world"));auto CallInt=[&](const TCHAR* Name,int32 Input){struct FArgs{int32 Input,Result;};FArgs Args{Input,0};if(auto* Fn=A->FindFunction(Name))A->ProcessEvent(Fn,&Args);return Args.Result;};Check(CallInt(TEXT("CallBehavior"),5)==15 && CallInt(TEXT("CallBehaviorRef"),7)==34 && CallInt(TEXT("CallBehaviorOut"),8)==18 && CallInt(TEXT("CallBehaviorHelper"),5)==15 && CallInt(TEXT("Bounce"),6)==16 && CallInt(TEXT("CallViaOwner"),5)==15 && A->LastDasError.IsEmpty(),TEXT("PIE_typed_methods_ref_out_free_function_and_reentry"));struct FReturn{int32 Result=0;};FReturn Values;if(auto* Fn=A->FindFunction(TEXT("CallValues")))A->ProcessEvent(Fn,&Values);Check(Values.Result==1 && A->LastDasError.IsEmpty(),TEXT("PIE_typed_method_values_cross_context"));A->EmitDasSignal(7);Check(DasComponentInt(CA,TEXT("SignalTotal"))==7 && DasComponentInt(CB,TEXT("SignalTotal"))==0,TEXT("PIE_owner_delegate_isolated"));CA->DestroyComponent();A->EmitDasSignal(9);Check(DasComponentInt(CA,TEXT("EndCount"))==1 && DasComponentInt(CA,TEXT("SignalTotal"))==7 && R.SubscriptionCount(CA)==0,TEXT("PIE_component_destroy_revokes_callback"));B->Destroy();Check(DasComponentInt(CB,TEXT("EndCount"))==1 && R.SubscriptionCount(CB)==0,TEXT("PIE_actor_destroy_ends_component"));auto* Remaining=World->SpawnActor<ADasProbeActor>(State->Blueprint->GeneratedClass);Remaining->OnEndPlay.AddDynamic(State->Observer.Get(),&UDasComponentLifecycleObserver::OnEnded);Remaining->FindComponentByClass<UDasScriptComponent>()->OnScriptEnded.AddUObject(State->Observer.Get(),&UDasComponentLifecycleObserver::OnComponentEnded);GEditor->RequestEndPlayMap();Ending=true;Started=FPlatformTime::Seconds();return false;}
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasActorComponentPIETest,"DasForUE.ActorComponent.PIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FDasActorComponentPIETest::RunTest(const FString&){auto State=MakeShared<FComponentPIEState>();auto* World=FAutomationEditorCommonUtils::CreateNewMap();FString Error;UBlueprint* BP=nullptr;const FString Root=TEXT("/Game/DasActorComponent/PIE_")+FGuid::NewGuid().ToString(EGuidFormats::Digits);if(!TestTrue(TEXT("Create DSL actor"),FDasDSLService::Apply(TEXT("dsl_actor_component_owner.das"),Root+TEXT("/BP_Owner"),TEXT("dsl_actor_component_owner.ComponentOwner"),BP,Error)))return false;State->Blueprint.Reset(BP);for(auto* Name:{TEXT("ComponentFirst"),TEXT("ComponentSecond")}){FActorSpawnParameters Spawn;Spawn.Name=Name;World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,Spawn);}ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FComponentPIECommand>(State,this));return true;}
#endif
