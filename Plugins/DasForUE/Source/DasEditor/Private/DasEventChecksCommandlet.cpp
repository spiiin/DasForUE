#include "DasEventChecksCommandlet.h"
#include "DasAuthoring.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Script.h"
#include "Containers/Ticker.h"
UDasEventChecksCommandlet::UDasEventChecksCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasEventChecksCommandlet::Main(const FString& Params)
{
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* Name, const FString& Error = FString())
    {
        ++Checks;
        if (!OK)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_EVENT_CHECK_%s %s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name, *Error);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    FString Source, Error;
    if (!FFileHelper::LoadFileToString(Source, *(Repo / TEXT("Tests/EventChecks/probe.das"))))
        return 2;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  File = TEXT("event_checks_")+ Id + TEXT(".das"), Path = R.ScriptPath(File);
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path, false, true);
    };
    auto Write = [&](const FString& S)
    {
        FFileHelper::SaveStringToFile(S, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    auto Compile = [&](const FString& S)
    {
        Write(S);
        FDasAuthoringModule M;
        return R.ReadAuthoringScript(Path, M, Error);
    };
    Check(Compile(Source), TEXT("native_and_authored_bindings_without_strings_compile"), Error);
    auto Bad = [&](const FString& S, const TCHAR* Name, const TCHAR* Hint)
    {
        const bool OK = Compile(S);
        Check(!OK && Error.Contains(Hint) &&
              !Error.Contains(TEXT("syntax error")) && !Error.Contains(TEXT("not a handled type annotation")), Name,
                                                                       Error);
    };
    Bad(Source.Replace(TEXT("def OnSignal(value:int)"), TEXT("def OnSignal(value:float)")),
                       TEXT("native_same_arity_wrong_type_rejected"), TEXT("argument 0"));
    Bad(Source.Replace(TEXT("self.OnDasSignal +="), TEXT("self.Unknown +=")),
                       TEXT("unknown_native_event_rejected"), TEXT("Unknown"));
    Bad(Source.Replace(TEXT("right:float):void {self.Total"), TEXT("right:int):void {self.Total")),
                       TEXT("authored_same_arity_wrong_type_rejected"), TEXT("argument 1"));
    Bad(Source.Replace(TEXT("self.Changed += self.OnChanged"), TEXT("self.Changed += self.Missing")),
                       TEXT("unknown_handler_rejected"), TEXT("Missing"));
    Bad(Source.Replace(TEXT("self.OnDasSignal += self.OnSignal"), TEXT("if(true){self.OnDasSignal += self.OnSignal}")),
                       TEXT("conditional_declaration_rejected"), TEXT("BindEvents"));
    const FString Owner = TEXT("options gen2\nrequire ue_dsl\nrequire ue_object_types\nrequire ue_probe_actor\n[ue_class(parent=ActorComponent)]\nclass Listener { [ue_event(source=owner,source_type=ProbeActorRef,name=OnDasSignal)]\ndef Signal(value:int):void{} }\n");
    Check(Compile(Owner), TEXT("typed_owner_contract_compiles"), Error);
    Bad(Owner.Replace(TEXT("source_type=ProbeActorRef,"), TEXT("")),
                      TEXT("untyped_owner_rejected"), TEXT("source_type"));
    Bad(Owner.Replace(TEXT("value:int"), TEXT("value:float")),
                      TEXT("typed_owner_wrong_payload_rejected"), TEXT("argument 0"));
    const FString NativeComponent = TEXT("options gen2\nrequire ue_scene_components\nrequire ue_component_types\n[ue_class] class Listener { components=ue_components(){var Scene:SceneComponentRef}\n[ue_event(source=Scene,name=OnComponentActivated)]\ndef Activated(component:ObjectRef;reset:bool):void{} }\n");
    Check(Compile(NativeComponent), TEXT("native_component_full_signature_compiles"), Error);
    Bad(NativeComponent.Replace(TEXT("reset:bool"), TEXT("reset:int")),
                                TEXT("native_component_wrong_type_rejected"), TEXT("argument 1"));
    const FString AuthoredComponent = TEXT("options gen2\nrequire ue_health_refs\n[ue_class] class Listener {components=ue_components(){var Health:HealthComponentRef}\n[ue_event(source=Health,name=Changed)]\ndef Changed(current:float;maximum:float):void{} }\n");
    Check(Compile(AuthoredComponent), TEXT("authored_component_checked_from_source"), Error);
    Bad(AuthoredComponent.Replace(TEXT("maximum:float"), TEXT("maximum:int")),
                                  TEXT("authored_component_wrong_type_rejected"), TEXT("argument 1"));
    Bad(AuthoredComponent.Replace(TEXT("name=Changed"), TEXT("name=Typo")),
                                  TEXT("authored_component_unknown_event_rejected"), TEXT("Typo"));
    for (const TCHAR* Example :
         {TEXT("dsl_scalar_events.das"), TEXT("dsl_array_events.das"), TEXT("dsl_health_component.das"),
                                                                            TEXT("dsl_component_behavior.das")})
    {
        FDasAuthoringModule M;
        Check(R.ReadAuthoringScript(R.ScriptPath(Example), M, Error), Example, Error);
    }
    Write(Source);
    UBlueprint* BP = nullptr;
    Check(FDasDSLService::Apply(File, TEXT("/Game/DasEventChecks/")+ Id +
                                           TEXT("/BP_Events"), TEXT("eventchecks.EventActor"), BP, Error),
                                           TEXT("Apply_validated_event_contracts"), Error);
    if (!BP || !BP->GeneratedClass)
        return 1;
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
    FEditorScriptExecutionGuard AllowScript;
    auto Value = [&]()
    {
        return FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Total"))->GetPropertyValue_InContainer(Actor);
    };
    auto Pump = [&]()
    {
        FTSTicker::GetCoreTicker().Tick(.01f);
    };
    const auto First = R.RunLifecycle(Path, EDasCallback::Tick, Actor, .01f);
    Check(First.bSucceeded && R.SubscriptionCount(Actor) == 3, TEXT("declarations_register_managed_subscriptions"),
                                                                    First.Error);
    Actor->OnDasSignal.Broadcast(7);
    Actor->OnDasFloatSignal.Broadcast(2.0f);
    Pump();
    Check(Value() == 9, TEXT("native_int_and_float_callbacks_execute"), Actor->LastDasError);
    Actor->ProcessEvent(Actor->FindFunctionChecked(TEXT("Fire")), nullptr);
    Pump();
    Check(Value() == 14, TEXT("authored_multicast_tuple_callback_executes"), Actor->LastDasError);
    Write(Source.Replace(TEXT("// RELOAD_MARKER"), TEXT("// changed\n")) + TEXT("\n"));
    const auto Reload = R.RunLifecycle(Path, EDasCallback::Tick, Actor, .01f);
    Actor->OnDasSignal.Broadcast(1);
    Pump();
    Check(Reload.bSucceeded && Reload.ReloadError.IsEmpty() && Reload.Revision > First.Revision &&
              R.SubscriptionCount(Actor) == 3 && Value() == 15,
          TEXT("reload_rebinds_without_duplicate_events"), Reload.Error + Reload.ReloadError);
    Write(Source.Replace(TEXT("def OnSignal(value:int)"), TEXT("def OnSignal(value:float)")));
    const auto Rejected = R.RunLifecycle(Path, EDasCallback::Tick, Actor, .01f);
    Actor->OnDasSignal.Broadcast(1);
    Pump();
    Check(Rejected.bSucceeded && !Rejected.ReloadError.IsEmpty() && Rejected.Revision == Reload.Revision &&
              Value() == 16,
          TEXT("bad_event_reload_preserves_working_handlers"), Rejected.ReloadError);
    R.ReleaseObject(Actor);
    Actor->OnDasSignal.Broadcast(100);
    Pump();
    Check(R.SubscriptionCount(Actor) == 0 && Value() == 16, TEXT("release_removes_managed_event_handlers"));
    Write(Owner.Replace(TEXT("class Listener {"), TEXT("class Listener {def Tick(delta:float):void{}\n")));
    auto* Component = NewObject<UDasScriptComponent>(Actor);
    Actor->AddInstanceComponent(Component);
    Component->RegisterComponent();
    const auto OwnerResult = R.RunLifecycle(Path, EDasCallback::Tick, Component, .01f);
    Check(OwnerResult.bSucceeded && R.SubscriptionCount(Component) == 1, TEXT("typed_owner_runtime_binding"),
                                                                              OwnerResult.Error);
    R.ReleaseObject(Component);
    auto* WrongOwner = World->SpawnActor<AActor>();
    auto* WrongComponent = NewObject<UDasScriptComponent>(WrongOwner);
    WrongOwner->AddInstanceComponent(WrongComponent);
    WrongComponent->RegisterComponent();
    const auto WrongResult = R.RunLifecycle(Path, EDasCallback::Tick, WrongComponent, .01f);
    Check(!WrongResult.bSucceeded && !WrongResult.Error.IsEmpty() && R.SubscriptionCount(WrongComponent) == 0,
          TEXT("wrong_actual_owner_rejected_without_subscription"), WrongResult.Error);
    R.ReleaseObject(WrongComponent);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_EVENT_CHECK_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
