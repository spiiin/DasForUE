#include "DasInitializerProbe.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "DasScriptComponent.h"
#include "Modules/ModuleManager.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
namespace
{
int32 Value(UObject* O, const TCHAR* N)
{
    auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), N) : nullptr;
    return P ? P->GetPropertyValue_InContainer(O) : MIN_int32;
}
}
bool RunDasInitializerProbe(UObject* Actor, UObject* Component, FString& Report)
{
    bool Good = true;
    auto Check = [&](bool OK, const TCHAR* N)
    {
        Good &= OK;
        Report += FString::Printf(TEXT("DAS_INITIALIZER %s %s\n"), OK ? TEXT("PASS"): TEXT("FAIL"), N);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    Check(Value(Actor, TEXT("InitTrace")) == 123 && Value(Actor, TEXT("InitCount")) == 3 &&
                                                          Value(Actor, TEXT("BeginSawTrace")) == 123,
                                                                TEXT("three_level_actor_Initialize_before_BeginPlay"));
    Check(Value(Component, TEXT("InitTrace")) == 12 &&
                Value(Component, TEXT("InitCount")) == 2 &&
                      Value(Component, TEXT("BeginSawTrace")) == 12,
                            TEXT("component_Initialize_Super_once_before_BeginPlay"));
    Check(Value(Actor, TEXT("SeenEditable")) == Value(Actor, TEXT("EditableValue")),
                                                      TEXT("initializer_sees_instance_values"));
    auto* Ready = Actor ? FindFProperty<FBoolProperty>(Actor->GetClass(), TEXT("InitComponentsReady")) : nullptr;
    Check(Ready && Ready->GetPropertyValue_InContainer(Actor),
          TEXT("actor_initializer_sees_constructed_initialized_components"));
    auto Again = R.RunLifecycle(R.ScriptPath(TEXT("dsl_spawn_target.das")), EDasCallback::Initialize, Actor);
    auto ComponentAgain =
        R.RunLifecycle(R.ScriptPath(TEXT("dsl_dynamic_lifecycle.das")), EDasCallback::Initialize, Component);
    Check(Again.bSucceeded && ComponentAgain.bSucceeded &&
          Value(Actor, TEXT("InitCount")) == 3 && Value(Component, TEXT("InitCount")) == 2,
                                                        TEXT("repeat_initialize_noop"));
    auto Outside = R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_spawn_target.das")),
                                                         TEXT("ue_SpawnTarget_TryReinitialize"), false, Actor);
    Check(!Outside.bSucceeded && Outside.Error.Contains(TEXT("only allowed during Initialize")),
                                                        TEXT("Super_Initialize_outside_initializer_rejected"));
    return Good;
}
bool RunDasInitializerLifetimeProbe(UClass* Target, UClass* Inherited, UClass* Skipped, UClass* Failure,
                                    FString& Report)
{
    bool Good = true;
    auto Check = [&](bool OK, const TCHAR* N)
    {
        Good &= OK;
        Report += FString::Printf(TEXT("DAS_INITIALIZER %s %s\n"), OK ? TEXT("PASS"): TEXT("FAIL"), N);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    auto* A = World->SpawnActor<ADasActor>(Target);
    auto* I = World->SpawnActor<ADasActor>(Inherited);
    auto* Skip = World->SpawnActor<ADasActor>(Skipped);
    auto* F = World->SpawnActor<ADasActor>(Failure);
    if (!A || !I || !Skip || !F)
        return false;
    Check(Value(A, TEXT("InitCount")) == 0, TEXT("no_runtime_initializer_during_construction"));
    auto* Life = FindFProperty<FObjectProperty>(A->GetClass(), TEXT("Life"));
    UObject* C = Life ? Life->GetObjectPropertyValue_InContainer(A) : nullptr;
    auto FirstC = R.RunLifecycle(R.ScriptPath(TEXT("dsl_dynamic_lifecycle.das")), EDasCallback::Initialize, C);
    auto First = R.RunLifecycle(R.ScriptPath(TEXT("dsl_spawn_target.das")), EDasCallback::Initialize, A);
    R.ReleaseObject(A);
    R.ReleaseObject(C);
    auto Again = R.RunLifecycle(R.ScriptPath(TEXT("dsl_spawn_target.das")), EDasCallback::Initialize, A);
    auto AgainC = R.RunLifecycle(R.ScriptPath(TEXT("dsl_dynamic_lifecycle.das")), EDasCallback::Initialize, C);
    Check(First.bSucceeded && FirstC.bSucceeded && Again.bSucceeded && AgainC.bSucceeded &&
          Value(A, TEXT("InitCount")) == 3 && Value(C, TEXT("InitCount")) == 2,
                                                    TEXT("session_recreation_does_not_replay_initializer"));
    auto InheritedBegin =
        R.RunLifecycle(R.ScriptPath(TEXT("dsl_initialize_inherited.das")), EDasCallback::BeginPlay, I);
    Check(InheritedBegin.bSucceeded && Value(I, TEXT("InitTrace")) == 1 &&
                                             Value(I, TEXT("InitCount")) == 1 &&
                                                   Value(I, TEXT("BeginSawTrace")) == 1,
                                                         TEXT("omitted_override_forwards_parent_initializer"));
    auto SkipBegin = R.RunLifecycle(R.ScriptPath(TEXT("dsl_initialize_skipped.das")), EDasCallback::BeginPlay, Skip);
    Check(SkipBegin.bSucceeded &&
          Value(Skip, TEXT("InitTrace")) == 9 &&
                Value(Skip, TEXT("InitCount")) == 1 &&
                      Value(Skip, TEXT("BaseBegins")) == 1,
                            TEXT("override_without_Super_Initialize_skips_parent_even_with_Super_BeginPlay"));
    auto Bad = R.RunLifecycle(R.ScriptPath(TEXT("dsl_initialize_failure.das")), EDasCallback::BeginPlay, F);
    auto BadAgain = R.RunLifecycle(R.ScriptPath(TEXT("dsl_initialize_failure.das")), EDasCallback::Tick, F, 0.1f);
    R.ReleaseObject(F);
    auto BadAfterRelease = R.RunLifecycle(R.ScriptPath(TEXT("dsl_initialize_failure.das")), EDasCallback::BeginPlay, F);
    Check(!Bad.bSucceeded && !BadAgain.bSucceeded && !BadAfterRelease.bSucceeded &&
          Bad.Error.Contains(TEXT("initializer failure fixture")) &&
                             Value(F, TEXT("Attempts")) == 1 &&
                                   Value(F, TEXT("Begins")) == 0 &&
                                         Value(F, TEXT("Ticks")) == 0,
                                               TEXT("failed_initializer_latches_and_blocks_BeginPlay_Tick"));
    for (auto* O :
         {static_cast<UObject*>(A), C, static_cast<UObject*>(I), static_cast<UObject*>(Skip), static_cast<UObject*>(F)})
        if (O)
            R.ReleaseObject(O);
    World->DestroyWorld(false);
    return Good;
}
