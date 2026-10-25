#include "DasActor.h"
#include "Editor.h"
#include "DasProbeActor.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasScriptHosts.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasActorHostIsolationTest, "DasForUE.Hosts.ActorIsolation",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasActorHostIsolationTest::RunTest(const FString& Parameters)
{
    TestTrue(TEXT("Actor resolves to production host even in test-enabled Editor"),
                  DasNativeScriptHost(TEXT("Actor")) == ADasActor::StaticClass() &&
                                      DasNativeScriptHost(TEXT("/Script/Engine.Actor")) == ADasActor::StaticClass());
    TestTrue(TEXT("Probe is an explicit child of production host"),
                  ADasProbeActor::StaticClass()->GetSuperClass() == ADasActor::StaticClass());
    for (const auto* Name : {TEXT("Peer"), TEXT("OnDasSignal"), TEXT("DasNativeArrayVectors")})
    {
        TestNull(FString(TEXT("No test field on production host: ")) + Name,
                         ADasActor::StaticClass()->FindPropertyByName(Name));
        TestNotNull(FString(TEXT("Test field on probe host: ")) + Name,
                            ADasProbeActor::StaticClass()->FindPropertyByName(Name));
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Demo;
    FString DemoError;
    if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_gameplay_actor.das")), Demo, DemoError))
    {
        AddError(TEXT("Production gameplay sample: ") + DemoError);
        return false;
    }
    const auto* Gameplay = Demo.Classes.FindByPredicate([](const auto& C) { return C.Name == TEXT("GameplayActor"); });
    TestTrue(TEXT("Gameplay sample declares its own signal on an ordinary Actor"),
                  Gameplay && Gameplay->NativeParent == TEXT("/Script/Engine.Actor"));
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString File = TEXT("actor_host_") + Id + TEXT(".das"), Path = Runtime.ScriptPath(File);
    const FString Package = TEXT("/Game/Tests/ActorHost/") + Id;
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path);
    };
    const FString Source = TEXT(R"DAS(options gen2
require ue_dsl
require ue_probe_actor
[ue_class(module_id="actorhost", Blueprintable)]
class CleanActor : Actor {
    @ue_property=(BlueprintReadWrite)
    Score:int=10
    [ue_function(BlueprintCallable)]
    def Add(input:int):int { self.Score+=input; return self.Score }
}
[ue_class(module_id="actorhost", Blueprintable)]
class CleanChild : CleanActor {}
[ue_class(module_id="actorhost", Blueprintable)]
class FixtureActor : ProbeActor {
    [ue_function(BlueprintCallable)]
    def Read(input:int):int { return input }
}
)DAS");
    if (!TestTrue(
            TEXT("Write isolation declarations"),
                 FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)))
        return false;
    UBlueprint *Base = nullptr, *Child = nullptr, *Fixture = nullptr;
    FString Error;
    auto Apply = [&](const TCHAR* Name, UBlueprint*& BP)
    {
        const bool Result =
            FDasDSLService::Apply(File, Package + TEXT("/BP_") + Name, FString(TEXT("actorhost.")) + Name, BP, Error);
        if (!Result)
            AddError(Error);
        return Result;
    };
    if (!Apply(TEXT("CleanActor"), Base) || !Apply(TEXT("CleanChild"), Child) || !Apply(TEXT("FixtureActor"), Fixture))
        return false;
    TestTrue(TEXT("Default DSL class has production native parent"), Base->ParentClass == ADasActor::StaticClass());
    TestTrue(TEXT("Authored inheritance keeps production host"),
                  Child->ParentClass == Base->GeneratedClass &&
                      !Child->GeneratedClass->IsChildOf(ADasProbeActor::StaticClass()));
    TestTrue(TEXT("Fixture DSL opts into probe host"), Fixture->ParentClass == ADasProbeActor::StaticClass());
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    auto* Actor = World->SpawnActor<ADasActor>(Child->GeneratedClass);
    if (!TestNotNull(TEXT("Spawn clean authored child"), Actor))
        return false;
    ON_SCOPE_EXIT
    {
        Runtime.ReleaseObject(Actor);
        Actor->Destroy();
    };
    struct FArgs
    {
        int32 Input = 5;
        int32 Result = 0;
    } Args;
    auto* Function = Actor->FindFunction(TEXT("Add"));
    if (!TestNotNull(TEXT("Inherited reflected function"), Function))
        return false;
    FEditorScriptExecutionGuard ScriptExecution;
    Actor->ProcessEvent(Function, &Args);
    TestEqual(TEXT("Unreal dispatch to daScript on clean host"), Args.Result, 15);
    TestTrue(TEXT("No runtime error"), Actor->LastDasError.IsEmpty());
    TestNull(TEXT("No fixture storage on authored child"), Actor->GetClass()->FindPropertyByName(TEXT("Peer")));
    return !HasAnyErrors();
}
