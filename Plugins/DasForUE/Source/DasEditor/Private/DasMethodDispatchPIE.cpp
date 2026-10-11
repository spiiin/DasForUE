#include "DasDSLService.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_CallParentFunction.h"
#include "K2Node_CallFunction.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/MetaData.h"
#include "UObject/UnrealType.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace {
struct FDispatchPIEState {TStrongObjectPtr<UBlueprint> Base,Child,Replace;bool Passed=true;};
int32 DispatchScore(UObject* O){auto* P=FindFProperty<FIntProperty>(O->GetClass(),TEXT("Score"));return P?P->GetPropertyValue_InContainer(O):MIN_int32;}
int32 DispatchCall(ADasProbeActor* O,const TCHAR* Name,int32 Input){struct FArgs{int32 Input,Result=0;};FArgs Args{Input};if(auto* F=O->FindFunction(Name))O->ProcessEvent(F,&Args);else return MIN_int32;return Args.Result;}
bool CreateOverride(UBlueprint* BP,UFunction* Parent,bool CallParent){
    auto* G=FBlueprintEditorUtils::CreateNewGraph(BP,Parent->GetFName(),UEdGraph::StaticClass(),UEdGraphSchema_K2::StaticClass());
    // The UClass overload creates a genuine override and its parent-call node.
    // The UFunction overload creates a new, independent function signature.
    FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP,G,false,Parent->GetOwnerClass());
    TArray<UK2Node_FunctionEntry*> Entries;TArray<UK2Node_FunctionResult*> Results;TArray<UK2Node_CallParentFunction*> Parents;
    G->GetNodesOfClass(Entries);G->GetNodesOfClass(Results);G->GetNodesOfClass(Parents);
    if(Entries.Num()!=1 || Results.Num()!=1 || Parents.Num()!=1)return false;
    auto* Entry=Entries[0];auto* Result=Results[0];auto* ParentCall=Parents[0];auto* Schema=G->GetSchema();
    auto* Return=Result->FindPin(UEdGraphSchema_K2::PN_ReturnValue);if(!Return)return false;
    Schema->BreakPinLinks(*Return,true);
    if(!CallParent){
        ParentCall->BreakAllNodeLinks();G->RemoveNode(ParentCall);
        const bool Connected=Schema->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        Schema->TrySetDefaultValue(*Return,TEXT("200"));return Connected;
    }
    auto* Add=NewObject<UK2Node_CallFunction>(G);Add->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary,Add_IntInt)));G->AddNode(Add,false,false);Add->CreateNewGuid();Add->PostPlacedNewNode();Add->AllocateDefaultPins();
    bool Good=Schema->TryCreateConnection(ParentCall->GetReturnValuePin(),Add->FindPinChecked(TEXT("A")));
    Schema->TrySetDefaultValue(*Add->FindPinChecked(TEXT("B")),TEXT("100"));
    Good &= Schema->TryCreateConnection(Add->GetReturnValuePin(),Return);
    return Good;
}
class FDispatchPIECommand:public IAutomationLatentCommand {
    TSharedRef<FDispatchPIEState> State;FAutomationTestBase* Test;bool Ending=false;double Started=FPlatformTime::Seconds();
    void Check(bool Good,const TCHAR* Name){State->Passed &= Test->TestTrue(Name,Good);UE_LOG(LogTemp,Display,TEXT("DAS_METHOD_DISPATCH %s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);}
public:
    FDispatchPIECommand(TSharedRef<FDispatchPIEState> S,FAutomationTestBase* T):State(S),Test(T){}
    bool Update()override{
        auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto* W=GEditor->PlayWorld.Get();
        if(Ending){if(W && FPlatformTime::Seconds()-Started<30)return false;Check(!W && R.PersistentContextCount()==0,TEXT("EndPIE_releases_sessions"));UE_LOG(LogTemp,Display,TEXT("DAS_METHOD_DISPATCH_PIE_RESULT=%s"),State->Passed?TEXT("PASS"):TEXT("FAIL"));return true;}
        ADasProbeActor* Base=nullptr;ADasProbeActor* Child=nullptr;ADasProbeActor* Replace=nullptr;
        if(W)for(TActorIterator<ADasProbeActor> It(W);It;++It){if(It->GetFName()==TEXT("DispatchBase"))Base=*It;if(It->GetFName()==TEXT("DispatchChild"))Child=*It;if(It->GetFName()==TEXT("DispatchReplace"))Replace=*It;}
        if(!W || !W->HasBegunPlay() || !Base || !Child || !Replace){if(FPlatformTime::Seconds()-Started<45)return false;Check(false,TEXT("PIE_startup"));GEditor->RequestEndPlayMap();Ending=true;Started=FPlatformTime::Seconds();return false;}
        Check(DispatchScore(Base)==3 && DispatchScore(Child)==3 && DispatchScore(Replace)==3,TEXT("lifecycle_calls_script_only_helper"));
        Check(!Base->FindFunction(TEXT("Initialize")) && !Base->FindFunction(TEXT("Add")) && !Base->FindFunction(TEXT("Sum")),TEXT("helpers_are_not_reflected"));
        Check(DispatchCall(Base,TEXT("Exercise"),5)==18 && DispatchCall(Child,TEXT("Exercise"),5)==18,TEXT("self_calls_ref_recursion_container_and_callable_method"));
        Check(DispatchCall(Base,TEXT("Query"),2)==10,TEXT("event_default_body_without_override"));
        Check(DispatchCall(Child,TEXT("Query"),2)==110 && DispatchScore(Child)==10,TEXT("DAS_event_dispatch_to_child_BP_and_parent_body_once"));
        Check(DispatchCall(Replace,TEXT("Query"),2)==200 && DispatchScore(Replace)==3,TEXT("BP_override_without_parent_skips_DAS_default_body"));
        Check(Base->LastDasError.IsEmpty() && Child->LastDasError.IsEmpty() && Replace->LastDasError.IsEmpty(),TEXT("no_runtime_errors"));
        GEditor->RequestEndPlayMap();Ending=true;Started=FPlatformTime::Seconds();return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasMethodDispatchPIETest,"DasForUE.MethodDispatch.PIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FDasMethodDispatchPIETest::RunTest(const FString&){
    auto State=MakeShared<FDispatchPIEState>();FString Error;UBlueprint* BP=nullptr;
    const FString Root=TEXT("/Game/DasMethodDispatch/PIE_")+FGuid::NewGuid().ToString(EGuidFormats::Digits);
    if(!TestTrue(TEXT("Import method dispatch DSL"),FDasDSLService::Apply(TEXT("dsl_method_dispatch_probe.das"),Root+TEXT("/BP_Base"),TEXT("dispatchprobe.DispatchProbe"),BP,Error))){AddError(Error);return false;}
    FKismetEditorUtilities::CompileBlueprint(BP);
    State->Base.Reset(BP);auto* Hook=BP->GeneratedClass->FindFunctionByName(TEXT("Hook"));auto* Query=BP->GeneratedClass->FindFunctionByName(TEXT("Query"));
    UE_LOG(LogTemp,Display,TEXT("DISPATCH_FLAGS Hook=%x Query=%x"),Hook?uint32(Hook->FunctionFlags):0,Query?uint32(Query->FunctionFlags):0);
    State->Passed &= TestTrue(TEXT("Explicit event is overridable; ordinary callable is final"),Hook && Hook->HasAnyFunctionFlags(FUNC_BlueprintEvent) && !Hook->HasAnyFunctionFlags(FUNC_Final) && Query && Query->HasAnyFunctionFlags(FUNC_Final));
    {
        auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        FString Source;const FString File=TEXT("dsl_dispatch_guard_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT(".das");const FString Path=Runtime.ScriptPath(File);
        ON_SCOPE_EXIT{IFileManager::Get().Delete(*Path);};
        UBlueprint* Guard=nullptr;
        if(!FFileHelper::LoadFileToString(Source,*Runtime.ScriptPath(TEXT("dsl_method_dispatch_probe.das"))) || !FFileHelper::SaveStringToFile(Source,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return false;
        if(!TestTrue(TEXT("Import guard fixture"),FDasDSLService::Apply(File,Root+TEXT("/BP_Guard"),TEXT("dispatchprobe.DispatchProbe"),Guard,Error))){AddError(Error);return false;}
        const FString Before=Guard->GetOutermost()->GetMetaData().GetValue(Guard,TEXT("DasDSLModel"));auto* Function=Guard->GeneratedClass->FindFunctionByName(TEXT("Hook"));
        Source=Source.Replace(TEXT("[ue_function(BlueprintEvent)]"),TEXT("[ue_function(BlueprintCallable)]"));
        if(!FFileHelper::SaveStringToFile(Source,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return false;
        State->Passed &= TestTrue(TEXT("Role change rejected before asset mutation"),!FDasDSLService::Reimport(Guard,Error) && Error.Contains(TEXT("dependency migration")) && Function==Guard->GeneratedClass->FindFunctionByName(TEXT("Hook")) && Before==Guard->GetOutermost()->GetMetaData().GetValue(Guard,TEXT("DasDSLModel")));
    }
    for(bool Parent:{true,false}){
        auto* Package=CreatePackage(*(Root+(Parent?TEXT("/BP_Child"):TEXT("/BP_Replace"))));
        auto* Child=FKismetEditorUtilities::CreateBlueprint(BP->GeneratedClass,Package,Parent?TEXT("BP_Child"):TEXT("BP_Replace"),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass());
        if(!TestTrue(TEXT("Create child override graph"),Child && CreateOverride(Child,Hook,Parent)))return false;
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Child);FKismetEditorUtilities::CompileBlueprint(Child);
        if(!TestTrue(TEXT("Child Blueprint compiles"),Child->Status!=BS_Error))return false;
        if(Parent)State->Child.Reset(Child);else State->Replace.Reset(Child);
    }
    auto* W=FAutomationEditorCommonUtils::CreateNewMap();
    auto Spawn=[&](UBlueprint* Blueprint,const TCHAR* Name){FActorSpawnParameters P;P.Name=Name;return W->SpawnActor<ADasProbeActor>(Blueprint->GeneratedClass,FTransform::Identity,P);};
    if(!TestTrue(TEXT("Spawn all dispatch actors"),Spawn(State->Base.Get(),TEXT("DispatchBase")) && Spawn(State->Child.Get(),TEXT("DispatchChild")) && Spawn(State->Replace.Get(),TEXT("DispatchReplace"))))return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FDispatchPIECommand>(State,this));return true;
}
#endif
