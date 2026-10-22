#include "DasInlineCallbacksCommandlet.h"
#include "DasAuthoring.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Engine/Engine.h"
#include "Engine/Blueprint.h"
#include "Misc/Paths.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InputAction.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Script.h"
UDasInlineCallbacksCommandlet::UDasInlineCallbacksCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasInlineCallbacksCommandlet::Main(const FString& Params)
{
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool V, const TCHAR* Name, const FString& Detail = FString())
    {
        ++Checks;
        if (!V)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_INLINE_CALLBACK_%s %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), Name, *Detail);
    };
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    FString Source, Error;
    if (!FFileHelper::LoadFileToString(Source, *(Repo / TEXT("Tests/InlineCallbacks/probe.template.das"))))
        return 2;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString File = TEXT("inline_callbacks_")+ Id + TEXT(".das"), Path = Runtime.ScriptPath(File);
    const FString Other = Runtime.ScriptPath(TEXT("inline_callbacks_other_")+ Id + TEXT(".das"));
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path, false, true);
        IFileManager::Get().Delete(*Other, false, true);
    };
    auto Write = [&](const FString& S)
    {
        FFileHelper::SaveStringToFile(S, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    Write(Source);
    auto Names = [](const FDasAuthoringModule& M)
    {
        TArray<FString> N;
        for (const auto& C : M.Classes)
            for (const auto& F : C.Methods)
                if (F.Name.StartsWith(TEXT("__ue_cb_")))
                    N.Add(F.Name);
        N.Sort();
        return N;
    };
    FDasAuthoringModule Model;
    const bool Compiled = Runtime.ReadAuthoringScript(Path, Model, Error);
    Check(Compiled, TEXT("timer_and_EnhancedInput_blocks_compile"), Error);
    const auto InitialNames = Names(Model);
    Check(InitialNames.Num() == 4 && InitialNames[0] != InitialNames[1], TEXT("distinct_stable_generated_methods"));
    FFileHelper::SaveStringToFile(TEXT("// moved source\n\n")+ Source, *Other);
    FDasAuthoringModule M;
    Check(Runtime.ReadAuthoringScript(Other, M, Error) && Names(M) == InitialNames,
          TEXT("names_independent_of_file_path_and_line_shift"), Error);
    Write(Source.Replace(TEXT("self.TimerHits+=1"), TEXT("self.TimerHits+=2")));
    Check(Runtime.ReadAuthoringScript(Path, M, Error) && Names(M) != InitialNames,
          TEXT("body_change_changes_callback_identity"), Error);
    Write(Source.Replace(TEXT("self.TimerHits+=1"), TEXT("let message=\"А\";self.TimerHits+=1")));
    const bool UnicodeA = Runtime.ReadAuthoringScript(Path, M, Error);
    const auto UnicodeNames = Names(M);
    Write(Source.Replace(TEXT("self.TimerHits+=1"), TEXT("let message=\"Б\";self.TimerHits+=1")));
    Check(UnicodeA && Runtime.ReadAuthoringScript(Path, M, Error) && Names(M) != UnicodeNames,
          TEXT("UTF8_body_hash_distinguishes_unicode_literals"), Error);
    const FString Capture = Source.Replace(
        TEXT("ue_timer_start(\"repeat\""), TEXT("let captured=3;ue_timer_start(\"repeat\""))
                                               .Replace(TEXT("self.TimerHits+=1"), TEXT("self.TimerHits+=captured"));
    Write(Capture);
    Check(!Runtime.ReadAuthoringScript(Path, M, Error) &&
          Error.Contains(TEXT("captured")) && Error.Contains(TEXT("timer payload")),
                                                             TEXT("outer_local_capture_rejected_with_payload_hint"),
                                                                  Error);
    Write(Capture.Replace(TEXT("let captured=3"), TEXT("var captured=3")));
    Check(!Runtime.ReadAuthoringScript(Path, M, Error) &&
          Error.Contains(TEXT("timer payload")), TEXT("mutable_outer_local_capture_rejected"), Error);
    Write(Source.Replace(TEXT("def Start():void"),
                              TEXT("def Start(captured:int):void"))
                                  .Replace(TEXT("self.TimerHits+=1"), TEXT("self.TimerHits+=captured")));
    Check(!Runtime.ReadAuthoringScript(Path, M, Error) &&
          Error.Contains(TEXT("timer payload")), TEXT("outer_parameter_capture_rejected"), Error);
    Write(Source.Replace(TEXT("ue_timer_start(\"repeat\",0.05f,$ { self.TimerHits+=1 },true)"),
        TEXT("for(captured in range(2)) { ue_timer_start(\"repeat\",0.05f,$ { self.TimerHits+=captured },true) }")));
    Check(!Runtime.ReadAuthoringScript(Path, M, Error) && Error.Contains(TEXT("timer payload")),
                                                                         TEXT("loop_iterator_capture_rejected"), Error);
    Write(Source.Replace(TEXT("self.TimerHits+=1"), TEXT("let own=2;self.TimerHits+=own")));
    Check(Runtime.ReadAuthoringScript(Path, M, Error), TEXT("callback_local_variables_allowed"), Error);
    Write(Source.Replace(TEXT("self.TimerHits+=1"), TEXT("return 7")));
    Check(!Runtime.ReadAuthoringScript(Path, M, Error), TEXT("non_void_callback_rejected"), Error);
    Write(Source);
    UBlueprint* BP = nullptr;
    const bool Applied = Compiled && FDasDSLService::Apply(
                                         File, TEXT("/Game/DasInlineCallbacks/")+ Id +
                                                    TEXT("/BP_Inline"), TEXT("inlinecallbacks.InlineActor"), BP, Error);
    Check(Applied, TEXT("Apply_generated_callback_methods"), Error);
    if (!BP)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_INLINE_CALLBACK_RESULT=FAIL checks=%d"), Checks);
        return 1;
    }
    const auto IVS = UWorld::InitializationValues()
                         .CreatePhysicsScene(false)
                         .CreateNavigation(false)
                         .CreateAISystem(false)
                         .AllowAudioPlayback(false);
    TStrongObjectPtr<UWorld> World(
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &IVS));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->InitializeActorsForPlay(FURL());
    auto* Actor = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass);
    TStrongObjectPtr<ADasProbeActor> Keep(Actor);
    auto* Component = NewObject<UEnhancedInputComponent>(Actor);
    Actor->InputComponent = Component;
    Component->RegisterComponent();
    auto* PC = World->SpawnActor<APlayerController>();
    TStrongObjectPtr<UEnhancedPlayerInput> Input(NewObject<UEnhancedPlayerInput>(PC));
    PC->PlayerInput = Input.Get();
    TStrongObjectPtr<UInputAction> Action(NewObject<UInputAction>());
    Action->ValueType = EInputActionValueType::Axis1D;
    FindFProperty<FObjectPropertyBase>(Actor->GetClass(),
                                       TEXT("Action"))->SetObjectPropertyValue_InContainer(Actor, Action.Get());
    FEditorScriptExecutionGuard AllowScript;
    auto Value = [&](const TCHAR* N)
    {
        auto* P = FindFProperty<FIntProperty>(Actor->GetClass(), N);
        return P ? P->GetPropertyValue_InContainer(Actor) : -1;
    };
    auto Step = [&]()
    {
        ++GFrameCounter;
        FWorldDelegates::OnWorldTickStart.Broadcast(World.Get(), LEVELTICK_All, .06f);
        World->GetTimerManager().Tick(.06f);
        FTSTicker::GetCoreTicker().Tick(.01f);
    };
    auto Inject = [&]()
    {
        TArray<UInputComponent*> Stack{Component};
        Input->InjectInputForAction(Action.Get(), FInputActionValue(2.f));
        Input->ProcessInputStack(Stack, .016f, false);
        FTSTicker::GetCoreTicker().Tick(.01f);
    };
    Actor->ProcessEvent(Actor->FindFunctionChecked(TEXT("Start")), nullptr);
    Check(Actor->LastDasError.IsEmpty() && Runtime.ScriptTimerCount(Actor) == 1 &&
              Runtime.SubscriptionCount(Actor) == 1,
          TEXT("managed_timer_and_input_registered"), Actor->LastDasError);
    Step();
    Step();
    Check(Value(TEXT("TimerHits")) > 0, TEXT("inline_timer_uses_implicit_self"), Actor->LastDasError);
    Inject();
    Check(Value(TEXT("InputHits")) == 2, TEXT("inline_EnhancedInput_receives_typed_argument"), Actor->LastDasError);
    Actor->ProcessEvent(Actor->FindFunctionChecked(TEXT("StartNamed")), nullptr);
    Check(Runtime.ScriptTimerCount(Actor) == 2, TEXT("legacy_named_timer_registered_for_reload_regression"));
    const auto Before = Runtime.RunLifecycle(Path, EDasCallback::Tick, Actor, .016f);
    Write(TEXT("\n\n")+ Source.Replace(TEXT("return self.TimerHits } // RELOAD_MARKER"),
                                            TEXT("return self.TimerHits+0 } // RELOAD_MARKER")));
    const auto Reload = Runtime.RunLifecycle(Path, EDasCallback::Tick, Actor, .016f);
    Check(Reload.bSucceeded && Reload.ReloadError.IsEmpty() && Reload.Revision > Before.Revision,
          TEXT("hot_reload_accepts_stable_callback_methods"), Reload.Error + Reload.ReloadError);
    // Input recipes are normally reconciled at the next world tick, never while a callback is active.
    FWorldDelegates::OnWorldTickStart.Broadcast(World.Get(), LEVELTICK_All, .016f);
    Check(Runtime.ScriptTimerCount(Actor) == 1 && Runtime.SubscriptionCount(Actor) == 1,
          TEXT("hot_reload_keeps_one_timer_and_input_subscription"));
    const int32 TimerBefore = Value(TEXT("TimerHits")), InputBefore = Value(TEXT("InputHits"));
    Step();
    Step();
    Inject();
    Check(Value(TEXT("TimerHits")) > TimerBefore && Value(TEXT("InputHits")) == InputBefore + 2,
                                                          TEXT("timer_and_EnhancedInput_fire_after_reload"),
                                                               Actor->LastDasError);
    Write(Capture);
    const auto Rejected = Runtime.RunLifecycle(Path, EDasCallback::Tick, Actor, .016f);
    Check(Rejected.bSucceeded && Rejected.Revision == Reload.Revision &&
          Rejected.ReloadError.Contains(TEXT("timer payload")),
                                        TEXT("capture_reload_rejected_preserving_working_revision"),
                                             Rejected.ReloadError);
    const int32 StillTimer = Value(TEXT("TimerHits")), StillInput = Value(TEXT("InputHits"));
    Step();
    Step();
    Inject();
    Check(Value(TEXT("TimerHits")) > StillTimer && Value(TEXT("InputHits")) == StillInput + 2,
                                                         TEXT("callbacks_survive_rejected_reload"));
    Write(Source.Replace(TEXT("return self.TimerHits } // RELOAD_MARKER"),
                              TEXT("return self.TimerHits+0+0 } // RELOAD_MARKER")));
    const auto Repair = Runtime.RunLifecycle(Path, EDasCallback::Tick, Actor, .016f);
    Check(Repair.bSucceeded && Repair.ReloadError.IsEmpty() && Repair.Revision > Reload.Revision,
          TEXT("repaired_reload_accepts_same_callback_identity"), Repair.ReloadError);
    const int32 PayloadBefore = Value(TEXT("TimerHits"));
    Actor->ProcessEvent(Actor->FindFunctionChecked(TEXT("StartPayload")), nullptr);
    Step();
    Step();
    Check(Actor->LastDasError.IsEmpty() &&
          Value(TEXT("TimerHits")) >= PayloadBefore + 3 && Runtime.ScriptTimerCount(Actor) == 1,
                TEXT("explicit_timer_payload_replaces_forbidden_capture"), Actor->LastDasError);
    Actor->ProcessEvent(Actor->FindFunctionChecked(TEXT("StartMultiPayload")), nullptr);
    const int32 MultiBefore = Value(TEXT("TimerHits"));
    Step();
    Step();
    Check(Actor->LastDasError.IsEmpty() && Value(TEXT("TimerHits")) >= MultiBefore + 5,
                                                 TEXT("multiple_inline_timer_payload_values_delivered"),
                                                      Actor->LastDasError);
    Write(Source.Replace(TEXT("return self.TimerHits } // RELOAD_MARKER"),
                              TEXT("return self.TimerHits+0+0+0 } // RELOAD_MARKER")));
    const auto MultiReload = Runtime.RunLifecycle(Path, EDasCallback::Tick, Actor, .016f);
    const int32 MultiReloadBefore = Value(TEXT("TimerHits"));
    Step();
    Step();
    Check(MultiReload.bSucceeded && MultiReload.ReloadError.IsEmpty() && MultiReload.Revision > Repair.Revision &&
          Runtime.ScriptTimerCount(Actor) == 1 &&
          Value(TEXT("TimerHits")) >= MultiReloadBefore + 5, TEXT("multi_payload_inline_timer_survives_reload"),
                                                                  MultiReload.Error + MultiReload.ReloadError);
    Runtime.ReleaseObject(Actor);
    Check(Runtime.ScriptTimerCount(Actor) == 0 && Runtime.SubscriptionCount(Actor) == 0 &&
              Component->GetActionEventBindings().Num() == 0,
          TEXT("context_release_cleans_lifted_callbacks"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_INLINE_CALLBACK_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
