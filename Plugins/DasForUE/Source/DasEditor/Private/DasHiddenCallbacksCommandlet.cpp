#include "DasHiddenCallbacksCommandlet.h"
#include "DasDSLService.h"
#include "DasAuthoring.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Engine/Engine.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InputAction.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Script.h"
UDasHiddenCallbacksCommandlet::UDasHiddenCallbacksCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasHiddenCallbacksCommandlet::Main(const FString& Params)
{
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* Name, const FString& Detail = FString())
    {
        ++Checks;
        if (!OK)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_HIDDEN_CALLBACK_%s %s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name, *Detail);
    };
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    const bool OperationKeys = FParse::Param(*Params, TEXT("OperationKeys"));
    FString Source, Error;
    if (!FFileHelper::LoadFileToString(Source, *(Repo / (OperationKeys ? TEXT("Tests/OperationKeys/probe.das")
                                                                       : TEXT("Tests/HiddenCallbacks/probe.das")))))
        return 2;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString File = TEXT("hidden_callbacks_")+ Id + TEXT(".das"), Path = Runtime.ScriptPath(File);
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path, false, true);
    };
    auto Write = [&](const FString& Text)
    {
        FFileHelper::SaveStringToFile(Text, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    FDasAuthoringModule Model;
    Write(Source);
    const bool Compiled = Runtime.ReadAuthoringScript(Path, Model, Error);
    Check(Compiled, TEXT("plain_and_annotation_only_callbacks_compile"), Error);
    if (!Compiled)
        return 1;
    auto Hidden = [&](const TCHAR* Name)
    {
        const auto* M = Model.Classes[0].Methods.FindByPredicate([&](const auto& F) { return F.Name == Name; });
        return M && M->Metadata.FindRef(TEXT("BlueprintInternalUseOnly")) == TEXT("true");
    };
    Check(Hidden(TEXT("Pulse")) && Hidden(TEXT("Input")) &&
                                          Hidden(TEXT("Clicked")) &&
                                                 Hidden(TEXT("Payload")) &&
                                                        Hidden(TEXT("Multi")), TEXT("named_callback_contracts_hidden"));
    Check(!Hidden(TEXT("PublicHandler")), TEXT("explicit_callable_stays_public"));
    Check(!Model.Classes[0].Methods.ContainsByPredicate([](const auto& F) { return F.Name == TEXT("Unused"); }),
          TEXT("ordinary_helper_not_reflected"));
    FString Wire;
    FDasAuthoringModule RoundTrip;
    Check(Model.ToJson(Wire, Error) && FDasAuthoringModule::FromJson(Wire, RoundTrip, Error) &&
              RoundTrip.Classes[0].Methods.Num() == Model.Classes[0].Methods.Num(),
          TEXT("hidden_contract_IR_roundtrip"), Error);
    auto Bad = [&](const FString& Text, const TCHAR* Name, const TCHAR* Hint)
    {
        Write(Text);
        FDasAuthoringModule Other;
        const bool OK = Runtime.ReadAuthoringScript(Path, Other, Error);
        Check(!OK && Error.Contains(Hint), Name, Error);
    };
    Bad(Source.Replace(TEXT("self.Pulse,true"), TEXT("self.Missing,true")),
                       TEXT("unknown_reference_rejected"), TEXT("Missing"));
    Bad(Source.Replace(TEXT("def Pulse():void {self.TimerHits+=1}"), TEXT("def Pulse():int {return 1}")),
                       TEXT("non_void_callback_rejected"), TEXT("void instance"));
    Bad(Source.Replace(TEXT("self.Pulse,true"), TEXT("self.Tick,true")),
                       TEXT("lifecycle_callback_conflict_rejected"), TEXT("lifecycle/event"));
    Bad(Source.Replace(TEXT("def Clicked():void"), TEXT("def Clicked(value:int):void")),
                       TEXT("widget_callback_signature_rejected"), TEXT("without arguments"));
    Bad(Source.Replace(TEXT("def Payload(value:int):void"), TEXT("def Payload(var value:int&):void")),
                       TEXT("reference_payload_rejected"), TEXT("callback"));
    Bad(Source.Replace(TEXT("def Pulse():void"),
                            TEXT("[ue_function(BlueprintCallable,_ue_hidden_callback)]\ndef Pulse():void")),
                       TEXT("reserved_compiler_option_rejected"), TEXT("Reserved callback"));
    FDasAuthoringModule Example;
    Check(Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_managed_timers.das")), Example, Error),
                                      TEXT("migrated_timer_example_compiles"), Error);
    Write(Source);
    UBlueprint* BP = nullptr;
    const FString Asset = TEXT("/Game/DasHiddenCallbacks/")+ Id + TEXT("/BP_Hidden");
    Check(FDasDSLService::Apply(File, Asset, TEXT("hiddencallbacks.HiddenActor"), BP, Error),
                                TEXT("Apply_hidden_entry_points"), Error);
    if (!BP || !BP->GeneratedClass)
        return 1;
    auto Visible = [&](const TCHAR* Name)
    {
        return UEdGraphSchema_K2::CanUserKismetCallFunction(BP->GeneratedClass->FindFunctionByName(Name));
    };
    Check(!Visible(TEXT("Pulse")) && !Visible(TEXT("Input")) &&
                                              !Visible(TEXT("Clicked")),
                                                       TEXT("engine_palette_filter_hides_named_callbacks"));
    bool InlineHidden = false;
    for (const auto& M : Model.Classes[0].Methods)
        if (M.Name.StartsWith(TEXT("__ue_cb_")))
            InlineHidden = M.Metadata.FindRef(TEXT("BlueprintInternalUseOnly")) == TEXT("true") && !Visible(*M.Name);
    Check(InlineHidden, TEXT("inline_callback_hidden_in_palette"));
    Check(Visible(TEXT("Start")) && Visible(TEXT("PublicHandler")), TEXT("explicit_functions_visible_in_palette"));
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
        return FindFProperty<FIntProperty>(Actor->GetClass(), N)->GetPropertyValue_InContainer(Actor);
    };
    auto Call = [&](const TCHAR* N)
    {
        Actor->ProcessEvent(Actor->FindFunctionChecked(N), nullptr);
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
    Call(TEXT("Start"));
    Step();
    Step();
    Inject();
    Check(Actor->LastDasError.IsEmpty() &&
          Value(TEXT("TimerHits")) > 0 && Value(TEXT("InputHits")) == 2,
                                                TEXT("hidden_timer_and_enhanced_input_execute"), Actor->LastDasError);
    if (OperationKeys)
    {
        struct
        {
            int32 Result = 0;
        } Output;
        Actor->ProcessEvent(Actor->FindFunctionChecked(TEXT("ExerciseKeys")), &Output);
        Check(Output.Result == 1 && Runtime.ScriptTimerCount(Actor) == 0,
              TEXT("typed_pause_resume_cancel_and_family_isolation"), Actor->LastDasError);
        Call(TEXT("Start"));
    }
    int32 Before = Value(TEXT("TimerHits"));
    Call(TEXT("Direct"));
    Check(Value(TEXT("TimerHits")) == Before + 1, TEXT("direct_call_to_promoted_method_executes"), Actor->LastDasError);
    Before = Value(TEXT("TimerHits"));
    Call(TEXT("StartPayload"));
    Call(TEXT("StartMulti"));
    Call(TEXT("StartInline"));
    Call(TEXT("StartPublic"));
    Step();
    Step();
    Check(Actor->LastDasError.IsEmpty() && Value(TEXT("TimerHits")) >= Before + 60,
                                                 TEXT("scalar_multi_inline_and_public_callbacks_execute"),
                                                      Actor->LastDasError);
    Before = Value(TEXT("TimerHits"));
    Call(TEXT("Clicked"));
    Check(Value(TEXT("TimerHits")) == Before + 10, TEXT("hidden_widget_handler_reflected_dispatch"),
                                                        Actor->LastDasError);
    const auto First = Runtime.RunLifecycle(Path, EDasCallback::Tick, Actor, .016f);
    Write(Source.Replace(TEXT("return self.TimerHits} // RELOAD_MARKER"),
                              TEXT("return self.TimerHits+0} // RELOAD_MARKER")));
    const auto Reload = Runtime.RunLifecycle(Path, EDasCallback::Tick, Actor, .016f);
    Check(Reload.bSucceeded && Reload.ReloadError.IsEmpty() && Reload.Revision > First.Revision,
          TEXT("named_hidden_callbacks_reload"), Reload.Error + Reload.ReloadError);
    Step();
    Before = Value(TEXT("InputHits"));
    Inject();
    Check(Value(TEXT("InputHits")) == Before + 2, TEXT("hidden_input_rebound_after_reload"), Actor->LastDasError);
    Runtime.ReleaseObject(Actor);
    Check(Runtime.ScriptTimerCount(Actor) == 0 && Runtime.SubscriptionCount(Actor) == 0 &&
              Component->GetActionEventBindings().Num() == 0,
          TEXT("release_cleans_hidden_callbacks"));
    Keep.Reset();
    Actor->Destroy();
    // Visibility changes must clear internal metadata on both graph entries and generated functions.
    // Structural preflight requires the existing Blueprint to be compiled before migration.
    FKismetEditorUtilities::CompileBlueprint(BP);
    Write(Source.Replace(TEXT("def Pulse():void"), TEXT("[ue_function(BlueprintCallable)]\ndef Pulse():void")));
    Check(FDasDSLService::Apply(File, Asset, TEXT("hiddencallbacks.HiddenActor"), BP, Error) &&
                                Visible(TEXT("Pulse")), TEXT("reapply_can_expose_callback_explicitly"), Error);
    FKismetEditorUtilities::CompileBlueprint(BP);
    Write(Source);
    Check(FDasDSLService::Apply(File, Asset, TEXT("hiddencallbacks.HiddenActor"), BP, Error) &&
                                !Visible(TEXT("Pulse")), TEXT("reapply_can_hide_callback_again"), Error);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_HIDDEN_CALLBACK_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
