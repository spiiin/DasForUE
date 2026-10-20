#include "DasScriptComponent.h"
#include "DasLibraryTestTypes.h"
#include "DasInstanceDefaultsGameFixtures.h"
#include "DasPawnCharacterFixtures.h"
#include "DasStaticLibraryProbe.h"
#include "DasDSLService.h"
#include "DasPureFunctionTestGraph.h"
#include "DasProbeActor.h"
#include "DasComponentConstructionProbe.h"
#include "DasRuntime.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EngineUtils.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"

namespace
{
struct FGameplayPIEState
{
    TArray<TStrongObjectPtr<UBlueprint>> InstanceDefaultBlueprints;
    TStrongObjectPtr<UBlueprint> Blueprint, SingleBlueprint, InheritedBlueprint, LifeBlueprint, HelperBlueprint,
        ConstructionBlueprint, PureBlueprint, LibraryBlueprint, LibraryConsumerBlueprint, ValuesBlueprint,
        WorldConsumerBlueprint, DefaultConsumerBlueprint;
    TWeakObjectPtr<ADasProbeActor> First, Second;
    TWeakObjectPtr<ADasStaticLibraryProbe> LibraryCarrier;
    TStrongObjectPtr<ADasProbeActor> DestroyedOwner;
    int32 DestroyedPulses = 0;
    bool Passed = true;
};
int32 ProbeInt(UObject* O, const TCHAR* Name)
{
    auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), Name) : nullptr;
    return P ? P->GetPropertyValue_InContainer(O) : MIN_int32;
}
bool ProbeBool(UObject* O, const TCHAR* Name)
{
    auto* P = O ? FindFProperty<FBoolProperty>(O->GetClass(), Name) : nullptr;
    return P && P->GetPropertyValue_InContainer(O);
}
class FGameplayPIECommand : public IAutomationLatentCommand
{
    TSharedRef<FGameplayPIEState> State;
    FAutomationTestBase* Test;
    int32 Phase = 0, StoppedPulses = 0;
    double Started = FPlatformTime::Seconds();
    float StopTime = 0;
    void Check(bool Good, const TCHAR* Name)
    {
        State->Passed &= Test->TestTrue(Name, Good);
        UE_LOG(LogTemp, Display, TEXT("DAS_GAMEPLAY_PIE %s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
    }
    void End()
    {
        State->DestroyedOwner.Reset();
        GEditor->RequestEndPlayMap();
        Phase = 2;
        Started = FPlatformTime::Seconds();
    }

public:
    FGameplayPIECommand(TSharedRef<FGameplayPIEState> S, FAutomationTestBase* T) : State(S), Test(T) {}
    bool Update() override
    {
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        auto* World = GEditor->PlayWorld.Get();
        if (Phase == 2)
        {
            if (World && FPlatformTime::Seconds() - Started < 30)
                return false;
            Check(!World && R.PersistentContextCount() == 0 && R.SubscriptionCount() == 0,
                  TEXT("EndPIE_releases_contexts_and_subscriptions"));
            UE_LOG(LogTemp, Display,
                   TEXT("DAS_GAMEPLAY_RUNTIME_PIE_RESULT=%s"), State->Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        if (World && !State->First.IsValid())
            for (TActorIterator<ADasProbeActor> It(World); It; ++It)
            {
                if (It->GetFName() == TEXT("GameplayFirst"))
                    State->First = *It;
                if (It->GetFName() == TEXT("GameplaySecond"))
                    State->Second = *It;
            }
        auto* A = State->First.Get();
        auto* B = State->Second.Get();
        if (Phase == 0)
        {
            const bool Ready =
                World && World->HasBegunPlay() && A && B &&
                ProbeInt(A, TEXT("Pulses")) >= 2 && ProbeInt(A, TEXT("Loads")) >= 1 &&
                                                             ProbeInt(A, TEXT("Delays")) >= 1 &&
                                                                      ProbeInt(A, TEXT("SnapshotSum")) != 0 &&
                                                                               DasTextByteTimersReady(World);
            if (!Ready)
            {
                if (FPlatformTime::Seconds() - Started < 30 && (!A || A->LastDasError.IsEmpty()))
                    return false;
                Check(false, TEXT("callbacks_finish_before_timeout"));
                if (A && !A->LastDasError.IsEmpty())
                    Test->AddError(A->LastDasError);
                End();
                return false;
            }
            ADasProbeActor* Single = nullptr;
            for (TActorIterator<ADasProbeActor> It(World); It; ++It)
                if (It->GetFName() == TEXT("SingleDelegateOwner"))
                    Single = *It;
            Check(Single != nullptr, TEXT("single_delegate_actor_duplicated_into_PIE"));
            if (Single)
            {
                auto Invoke = [&](const TCHAR* Name, int32 Input = 0)
                {
                    struct FArgs
                    {
                        int32 Value, ReturnValue = 0;
                    };
                    FArgs Args{Input};
                    auto* F = Single->FindFunction(Name);
                    if (F)
                        Single->ProcessEvent(F, F->NumParms ? &Args : nullptr);
                    return Args.ReturnValue;
                };
                Invoke(TEXT("Bind"));
                Check(Invoke(TEXT("Invoke"), 3) == 3, TEXT("single_delegate_field_executes_in_PIE"));
                Invoke(TEXT("Clear"));
                auto* Field = FindFProperty<FDelegateProperty>(Single->GetClass(), TEXT("Callback"));
                Check(Field && !Field->GetPropertyValue_InContainer(Single).IsBound() &&
                      Invoke(TEXT("InvokeCopy"), 4) == 7, TEXT("single_delegate_copy_survives_clearing_field"));
                auto Result = R.RunPersistent(R.ScriptPath(TEXT("single_delegate_reference_probe.das")), 5, Single);
                Check(Result.bSucceeded && Result.Value == 12 && Single->LastDasError.IsEmpty(),
                      TEXT("single_delegate_foreign_accessors_in_PIE"));
                auto Methods = R.RunNamedPersistent(R.ScriptPath(TEXT("single_delegate_reference_probe.das")),
                                                                 TEXT("method_probe"), 2, Single);
                Check(Methods.bSucceeded && Methods.Value == 14 && Single->LastDasError.IsEmpty(),
                      TEXT("single_delegate_method_parameter_and_result_in_PIE"));
                int32 RefValue = 0;
                if (auto* Fn = Single->FindFunction(TEXT("ProbeRefOut")))
                    Single->ProcessEvent(Fn, &RefValue);
                Check(RefValue == 30 && Single->LastDasError.IsEmpty(),
                      TEXT("single_delegate_ref_out_and_multicast_ref_in_PIE"));
                int32 ArrayRef = 0;
                if (auto* Fn = Single->FindFunction(TEXT("ProbeRefArray")))
                    Single->ProcessEvent(Fn, &ArrayRef);
                Check(ArrayRef == 1, TEXT("delegate_array_ref_out_in_PIE"));
                ADasProbeActor* Inherited = nullptr;
                for (TActorIterator<ADasProbeActor> It(World); It; ++It)
                    if (It->GetFName() == TEXT("InheritedOwner"))
                        Inherited = *It;
                struct FInheritanceArgs
                {
                    int32 Input = 2, Result = 0;
                };
                FInheritanceArgs IA;
                if (Inherited)
                    if (auto* Fn = Inherited->FindFunction(TEXT("Probe")))
                        Inherited->ProcessEvent(Fn, &IA);
                Check(Inherited && IA.Result == 27 && Inherited->LastDasError.IsEmpty(),
                      TEXT("authored_inheritance_override_Super_in_PIE"));
                if (Inherited)
                    Inherited->Destroy();
                Single->Destroy();
                Check(R.SubscriptionCount(Single) == 0, TEXT("single_delegate_owner_destroy_cleanup"));
            }
            ADasProbeActor* Life = nullptr;
            for (TActorIterator<ADasProbeActor> It(World); It; ++It)
                if (It->GetFName() == TEXT("LifeOwner"))
                    Life = *It;
            Check(Life != nullptr, TEXT("inherited_lifecycle_actor_in_PIE"));
            if (Life)
            {
                auto* BehaviorField = FindFProperty<FObjectProperty>(Life->GetClass(), TEXT("Behavior"));
                auto* PivotField = FindFProperty<FObjectProperty>(Life->GetClass(), TEXT("Pivot"));
                TStrongObjectPtr<UActorComponent> Behavior(
                    BehaviorField ? Cast<UActorComponent>(BehaviorField->GetObjectPropertyValue_InContainer(Life))
                                  : nullptr);
                TStrongObjectPtr<ADasProbeActor> KeepLife(Life);
                Check(Life->GetRootComponent() && Life->GetRootComponent()->bHiddenInGame,
                      TEXT("inherited_native_template_override"));
                Check(ProbeInt(Behavior.Get(), TEXT("ConstructionValue")) == 113 &&
                               ProbeInt(Behavior.Get(), TEXT("ConstructionTrace")) == 12 &&
                                        ProbeInt(Life, TEXT("ConstructionObserved")) == 113,
                                                 TEXT("component_construction_before_actor_survives_PIE"));
                int32 PureOutput = 0;
                auto* Snapshot = Behavior.IsValid() ? Behavior->FindFunction(TEXT("Snapshot")) : nullptr;
                if (Snapshot)
                    Behavior->ProcessEvent(Snapshot, &PureOutput);
                Check(Snapshot && Snapshot->HasAnyFunctionFlags(FUNC_BlueprintPure) && PureOutput == 55,
                      TEXT("component_pure_void_out_in_PIE"));
                Check(ProbeInt(Behavior.Get(), TEXT("Bonus")) == 55,
                               TEXT("inherited_component_editable_template_default"));
                Check(Behavior.IsValid() && PivotField &&
                          PivotField->GetObjectPropertyValue_InContainer(Life) == Life->GetRootComponent(),
                      TEXT("inherited_SCS_fields_and_root"));
                Check(ProbeInt(Life, TEXT("BaseBegins")) == 1 &&
                               ProbeInt(Life, TEXT("ChildBegins")) == 1 &&
                                        ProbeInt(Life, TEXT("BaseTicks")) > 0,
                                                 TEXT("explicit_parent_BeginPlay_and_inherited_Tick"));
                Check(ProbeInt(Behavior.Get(), TEXT("BaseBegins")) == 1 &&
                               ProbeInt(Behavior.Get(), TEXT("ChildBegins")) == 1 &&
                                        ProbeInt(Behavior.Get(), TEXT("BaseTicks")) > 0 &&
                                                 ProbeInt(Behavior.Get(), TEXT("BaseTicks")) ==
                                                          ProbeInt(Behavior.Get(), TEXT("ChildTicks")),
                                                                   TEXT("authored_component_parent_lifecycle_once"));
                auto ComponentField = [&](const TCHAR* Name)
                {
                    auto* P = FindFProperty<FObjectProperty>(Life->GetClass(), Name);
                    return P ? P->GetObjectPropertyValue_InContainer(Life) : nullptr;
                };
                auto* Extension = Cast<USceneComponent>(ComponentField(TEXT("Extension")));
                auto* Marker = Cast<USceneComponent>(ComponentField(TEXT("Marker")));
                TStrongObjectPtr<UObject> ExtraBehavior(ComponentField(TEXT("ExtraBehavior")));
                Check(Extension && Marker && Extension->GetAttachParent() == Life->GetRootComponent() &&
                          Marker->GetAttachParent() == Extension &&
                          Extension->GetRelativeLocation().Equals(FVector(0, 0, 25)),
                      TEXT("derived_scene_components_attach_to_inherited_root_and_owned_node"));
                Check(ProbeInt(ExtraBehavior.Get(), TEXT("Bonus")) == 77 &&
                               ProbeInt(ExtraBehavior.Get(), TEXT("BaseBegins")) == 1 &&
                                        ProbeInt(ExtraBehavior.Get(), TEXT("ChildBegins")) == 1,
                                                 TEXT("derived_authored_component_defaults_and_lifecycle"));
                Life->EmitDasSignal(4);
                Check(ProbeInt(
                    Life, TEXT("BaseSignals")) == 4 &&
                    ProbeInt(Life, TEXT("ChildSignals")) == 40 &&
                             ProbeInt(Behavior.Get(), TEXT("BaseSignals")) == 4 &&
                                      ProbeInt(Behavior.Get(), TEXT("ChildSignals")) == 40 &&
                                               ProbeInt(
                                                   ExtraBehavior.Get(), TEXT("BaseSignals")) == 4 &&
                                                   ProbeInt(
                                                       ExtraBehavior.Get(), TEXT("ChildSignals")) == 40,
                                                       TEXT("inherited_and_owned_actor_component_event_subscriptions"));
                Life->EmitDasSignal(1);
                Check(ProbeInt(Life, TEXT("BaseSignals")) == 5 &&
                               ProbeInt(Life, TEXT("ChildSignals")) == 50,
                                        TEXT("inherited_event_delivered_once_per_handler"));
                struct FLifeArgs
                {
                    int32 Input = 0, Result = 0;
                };
                FLifeArgs LA;
                if (auto* Fn = Life->FindFunction(TEXT("Probe")))
                    Life->ProcessEvent(Fn, &LA);
                Check(LA.Result == 3 && Life->LastDasError.IsEmpty(), TEXT("inherited_component_typed_access"));
                Life->Destroy();
                Check(R.SubscriptionCount(Life) == 0 && R.SubscriptionCount(Behavior.Get()) == 0 &&
                          R.SubscriptionCount(ExtraBehavior.Get()) == 0,
                      TEXT("inherited_event_subscriptions_released_on_Destroy"));
                Check(ProbeInt(ExtraBehavior.Get(), TEXT("BaseEnds")) == 1 &&
                               ProbeInt(ExtraBehavior.Get(), TEXT("ChildEnds")) == 1,
                                        TEXT("derived_component_EndPlay_once"));
                Check(ProbeInt(Life, TEXT("BaseEnds")) == 1 &&
                               ProbeInt(Life, TEXT("ChildEnds")) == 1 &&
                                        ProbeInt(Behavior.Get(), TEXT("BaseEnds")) == 1 &&
                                                 ProbeInt(Behavior.Get(), TEXT("ChildEnds")) == 1,
                                                          TEXT("parent_EndPlay_actor_and_component_once"));
            }
            AActor* StandaloneOwner = nullptr;
            for (TActorIterator<AActor> It(World); It; ++It)
                if (It->ActorHasTag(TEXT("StandaloneConstructionOwner")))
                    StandaloneOwner = *It;
            auto* Standalone = StandaloneOwner ? StandaloneOwner->FindComponentByClass<UDasScriptComponent>() : nullptr;
            if (Standalone)
                UE_LOG(LogTemp, Display,
                       TEXT("DAS_COMPONENT_PIE_STATE Score=%d Value=%d Calls=%d Init=%d BeginSaw=%d Error=%s"),
                            ProbeInt(Standalone, TEXT("Score")),
                                     ProbeInt(Standalone, TEXT("ConstructionValue")),
                                              ProbeInt(Standalone, TEXT("ConstructionCalls")),
                                                       ProbeInt(Standalone, TEXT("InitializeCalls")),
                                                                ProbeInt(Standalone, TEXT("BeginSaw")),
                                                                         *Standalone->LastDasError);
            Check(
                Standalone &&
                ProbeInt(Standalone, TEXT("Score")) == 8 &&
                         ProbeInt(Standalone, TEXT("ConstructionValue")) == 21 &&
                                  ProbeInt(Standalone, TEXT("ConstructionCalls")) == 1 &&
                                           ProbeInt(Standalone, TEXT("InitializeCalls")) == 1 &&
                                                    ProbeInt(
                                                        Standalone, TEXT("BeginSaw")) == 21 &&
                                                        Standalone->LastDasError.IsEmpty(),
                                                        TEXT("native_owner_component_construction_values_survive_PIE"));
            FString ConstructionReport;
            Check(Standalone && RunDasComponentConstructionProbe(Standalone->GetClass(), World, ConstructionReport),
                  TEXT("dynamic_native_component_construction"));
            UE_LOG(LogTemp, Display, TEXT("%s"), *ConstructionReport);
            if (StandaloneOwner)
                StandaloneOwner->Destroy();
            ADasProbeActor* LibraryCaller = nullptr;
            for (TActorIterator<ADasProbeActor> It(World); It; ++It)
                if (It->ActorHasTag(TEXT("LibraryConsumer")))
                    LibraryCaller = *It;
            int32 LibrarySum = 0;
            if (LibraryCaller)
                if (auto* Fn = LibraryCaller->FindFunction(TEXT("EvaluatePure")))
                    LibraryCaller->ProcessEvent(Fn, &LibrarySum);
            Check(LibraryCaller && LibrarySum == 12, TEXT("static_library_Blueprint_consumer"));
            if (LibraryCaller)
                LibraryCaller->Destroy();
            ADasProbeActor* DefaultCaller = nullptr;
            for (TActorIterator<ADasProbeActor> It(World); It; ++It)
                if (It->ActorHasTag(TEXT("DefaultLibraryConsumer")))
                    DefaultCaller = *It;
            int32 DefaultSum = 0;
            if (DefaultCaller)
                if (auto* Fn = DefaultCaller->FindFunction(TEXT("EvaluatePure")))
                    DefaultCaller->ProcessEvent(Fn, &DefaultSum);
            Check(DefaultCaller && DefaultSum == 5, TEXT("library_Blueprint_omitted_arguments_use_defaults"));
            if (DefaultCaller)
                DefaultCaller->Destroy();
            ADasProbeActor* WorldCaller = nullptr;
            for (TActorIterator<ADasProbeActor> It(World); It; ++It)
                if (It->ActorHasTag(TEXT("WorldLibraryConsumer")))
                    WorldCaller = *It;
            double WorldTime = -1;
            if (WorldCaller)
                if (auto* Fn = WorldCaller->FindFunction(TEXT("EvaluatePure")))
                    WorldCaller->ProcessEvent(Fn, &WorldTime);
            Check(WorldCaller && WorldTime == World->GetTimeSeconds() + 1.,
                  TEXT("library_Blueprint_automatic_WorldContext"));
            if (WorldCaller)
            {
                FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).ReleaseObject(WorldCaller);
                WorldCaller->Destroy();
            }
            ADasStaticLibraryProbe* LibraryCarrier = nullptr;
            for (TActorIterator<ADasStaticLibraryProbe> It(World); It; ++It)
                LibraryCarrier = *It;
            FString LibraryReport;
            Check(LibraryCarrier && RunDasStaticLibraryProbe(LibraryCarrier->LibraryClass, LibraryReport),
                  TEXT("static_library_in_game"));
            UE_LOG(LogTemp, Display, TEXT("%s"), *LibraryReport);
            FString ValuesReport;
            Check(LibraryCarrier && RunDasLibraryValuesProbe(LibraryCarrier->ValuesClass, LibraryCarrier, ValuesReport),
                  TEXT("library_complex_values_and_WorldContext"));
            UE_LOG(LogTemp, Display, TEXT("%s"), *ValuesReport);
            FString InstanceReport;
            Check(LibraryCarrier && RunDasInstanceDefaultsProbe(LibraryCarrier, InstanceReport),
                  TEXT("instance_defaults_in_game"));
            UE_LOG(LogTemp, Display, TEXT("%s"), *InstanceReport);
            FString PawnReport;
            Check(LibraryCarrier && RunDasPawnCharacterProbe(LibraryCarrier, PawnReport),
                  TEXT("pawn_character_in_game"));
            UE_LOG(LogTemp, Display, TEXT("%s"), *PawnReport);
            FString PlayableReport;
            Check(LibraryCarrier && RunDasPlayableCharacterProbe(LibraryCarrier, PlayableReport),
                  TEXT("playable_character_in_game"));
            UE_LOG(LogTemp, Display, TEXT("%s"), *PlayableReport);
            State->LibraryCarrier = LibraryCarrier;
            ADasProbeActor* PureOwner = nullptr;
            for (TActorIterator<ADasProbeActor> It(World); It; ++It)
                if (It->ActorHasTag(TEXT("PureOwner")))
                    PureOwner = *It;
            int32 PureResult = 0;
            if (PureOwner)
                if (auto* Fn = PureOwner->FindFunction(TEXT("EvaluatePure")))
                    PureOwner->ProcessEvent(Fn, &PureResult);
            Check(PureOwner && PureResult == 15 &&
                  PureOwner->FindFunction(TEXT("Query"))->HasAnyFunctionFlags(FUNC_BlueprintPure) &&
                                          PureOwner->LastDasError.IsEmpty(),
                                          TEXT("pure_Blueprint_data_node_override_Super_in_PIE"));
            if (PureOwner)
                PureOwner->Destroy();
            ADasProbeActor* HelperOwner = nullptr;
            for (TActorIterator<ADasProbeActor> It(World); It; ++It)
                if (It->GetFName() == TEXT("HelperOwner"))
                    HelperOwner = *It;
            struct FHelperPIEArgs
            {
                int32 Input = 2, Result = 0;
            };
            FHelperPIEArgs HelperArgs;
            if (HelperOwner)
                if (auto* Fn = HelperOwner->FindFunction(TEXT("LocalProbe")))
                    HelperOwner->ProcessEvent(Fn, &HelperArgs);
            Check(HelperOwner && HelperArgs.Result == 21 &&
                  ProbeInt(HelperOwner, TEXT("Score")) == 12 && HelperOwner->LastDasError.IsEmpty(),
                           TEXT("three_level_local_helpers_self_override_Super"));
            Check(HelperOwner && ProbeInt(HelperOwner, TEXT("ConstructionValue")) == 23 &&
                                          ProbeInt(HelperOwner, TEXT("ConstructionTrace")) == 12,
                                                   TEXT("construction_values_survive_PIE"));
            if (HelperOwner)
                HelperOwner->Destroy();
            Check(A->LastDasError.IsEmpty() && B->LastDasError.IsEmpty(), TEXT("no_script_runtime_errors"));
            Check(ProbeInt(A, TEXT("SnapshotSum")) == 7 && ProbeInt(A, TEXT("NextSum")) == 13,
                                                                    TEXT("owned_array_capture_and_Vector2D_next_tick"));
            Check(ProbeInt(A, TEXT("Delays")) == 1 && ProbeInt(A, TEXT("Loads")) == 1 &&
                                                               ProbeBool(A, TEXT("LoadOK")),
                                                                         TEXT("latent_and_async_completion_once"));
            Check(ProbeInt(A, TEXT("CancelledCalls")) == 0, TEXT("cancelled_next_tick_and_latent_do_not_dispatch"));
            Check(ProbeBool(A, TEXT("SceneOK")), TEXT("scene_create_attach_transform_detach_destroy"));
            A->EmitDasFloatSignal(2.5f);
            A->EmitDasVoidSignal();
            A->EmitDasStringSignal(TEXT("PIE 世界"));
            A->EmitDasArrayIntSignal(TArray<int32>{11, 12});
            A->EmitDasVector2DRefSignal(FVector2D(9, 10));
            auto* FP = FindFProperty<FFloatProperty>(A->GetClass(), TEXT("Floats"));
            auto* TP = FindFProperty<FStrProperty>(A->GetClass(), TEXT("Text"));
            auto* VP = FindFProperty<FStructProperty>(A->GetClass(), TEXT("Position"));
            Check(FP && FP->GetPropertyValue_InContainer(A) == 2.5f &&
                  ProbeInt(A, TEXT("Voids")) == 1 && TP &&
                           TP->GetPropertyValue_InContainer(A) ==
                               TEXT("PIE 世界") &&
                                    ProbeInt(A, TEXT("ArraySum")) == 23 && VP &&
                                             *VP->ContainerPtrToValuePtr<FVector2D>(A) == FVector2D(9, 10),
                                             TEXT("typed_events_deliver_scalar_string_array_and_constref_Vector2D"));
            Check(ProbeInt(B, TEXT("ArraySum")) == 0 && ProbeInt(B, TEXT("Voids")) == 0,
                                                                 TEXT("events_are_owner_isolated"));
            if (auto* Fn = A->FindFunction(TEXT("Stop")))
                A->ProcessEvent(Fn, nullptr);
            else
                Check(false, TEXT("Stop_method_exists"));
            StoppedPulses = ProbeInt(A, TEXT("Pulses"));
            StopTime = World->GetTimeSeconds();
            if (auto* Fn = B->FindFunction(TEXT("ArmPending")))
                B->ProcessEvent(Fn, nullptr);
            State->DestroyedOwner.Reset(B);
            State->DestroyedPulses = ProbeInt(B, TEXT("Pulses"));
            B->Destroy();
            Check(R.PersistentContextCount(World) ==
                          1 + (LibraryCarrier && LibraryCarrier->ObjectCallbackActor ? 1 : 0) +
                              (LibraryCarrier && LibraryCarrier->DestroyedObjectCallbackActor ? 1 : 0) +
                              (LibraryCarrier && LibraryCarrier->ContainerCallbackActor ? 1 : 0) +
                              (LibraryCarrier && LibraryCarrier->DestroyedContainerCallbackActor ? 1 : 0) &&
                      R.SubscriptionCount(B) == 0,
                  TEXT("Destroy_revokes_second_owner"));
            Phase = 1;
            return false;
        }
        if (!World || !A)
        {
            Check(false, TEXT("surviving_owner_remains_valid"));
            End();
            return false;
        }
        if (World->GetTimeSeconds() - StopTime < 0.15f && FPlatformTime::Seconds() - Started < 30)
            return false;
        Check(CheckDasTextByteTimerCleanup(World), TEXT("text_byte_timer_release_prevents_late_delivery"));
        if (auto* Carrier = State->LibraryCarrier.Get())
            Carrier->Destroy();
        Check(ProbeInt(A, TEXT("Pulses")) == StoppedPulses &&
                       ProbeInt(A, TEXT("Delays")) == 1 &&
                                ProbeInt(A, TEXT("Loads")) == 1 && A->LastDasError.IsEmpty(),
                                         TEXT("cancelled_repeat_stays_stopped_and_completions_do_not_repeat"));
        Check(State->DestroyedOwner.IsValid() &&
              ProbeInt(State->DestroyedOwner.Get(), TEXT("CancelledCalls")) == 0 &&
                       ProbeInt(State->DestroyedOwner.Get(), TEXT("Pulses")) == State->DestroyedPulses,
                                TEXT("Destroy_cancels_pending_timer_latent_and_repeat"));
        End();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasGameplayRuntimePIETest, "DasForUE.Gameplay.RuntimePIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasGameplayRuntimePIETest::RunTest(const FString&)
{
    auto State = MakeShared<FGameplayPIEState>();
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    FString Error;
    UBlueprint* BP = nullptr;
    const FString Root = TEXT("/Game/DasGameplayProbe/PIE_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits);
    if (!TestTrue(TEXT("Create gameplay DSL actor"),
                       FDasDSLService::Apply(TEXT("dsl_gameplay_runtime_probe.das"),
                                                  Root + TEXT("/BP_Probe"), TEXT("gameplayprobe.GameplayRuntimeProbe"),
                                                                                 BP, Error)))
    {
        AddError(Error);
        return false;
    }
    State->Blueprint.Reset(BP);
    for (auto* Name : {TEXT("GameplayFirst"), TEXT("GameplaySecond")})
    {
        FActorSpawnParameters Spawn;
        Spawn.Name = Name;
        if (!TestNotNull(TEXT("Spawn probe actor"),
                              World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn)))
            return false;
    }
    if (!TestTrue(TEXT("Create single delegate DSL actor"),
                       FDasDSLService::Apply(TEXT("dsl_single_delegate_actor.das"),
                                                  Root + TEXT("/BP_SingleDelegate"),
                                                              TEXT("singledelegate.SingleDelegateActor"), BP, Error)))
    {
        AddError(Error);
        return false;
    }
    State->SingleBlueprint.Reset(BP);
    FActorSpawnParameters SingleSpawn;
    SingleSpawn.Name = TEXT("SingleDelegateOwner");
    if (!TestNotNull(TEXT("Spawn single delegate owner"),
                          World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, SingleSpawn)))
        return false;
    if (!TestTrue(TEXT("Create authored child"),
                       FDasDSLService::Apply(TEXT("dsl_inheritance_child.das"),
                                                  Root + TEXT("/BP_ChildCounter"), TEXT("inheritance.ChildCounter"), BP,
                                                                                        Error)))
    {
        AddError(Error);
        return false;
    }
    State->InheritedBlueprint.Reset(BP);
    FActorSpawnParameters InheritSpawn;
    InheritSpawn.Name = TEXT("InheritedOwner");
    if (!World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, InheritSpawn))
        return false;
    if (!TestTrue(TEXT("Create inherited lifecycle actor"),
                       FDasDSLService::Apply(TEXT("dsl_life_actor_child.das"),
                                                  Root + TEXT("/BP_ChildLifeActor"), TEXT("lifefamily.ChildLifeActor"),
                                                                                          BP, Error)))
    {
        AddError(Error);
        return false;
    }
    auto* ParentLifeBP = Cast<UBlueprint>(BP->ParentClass->ClassGeneratedBy);
    USCS_Node* ParentBehavior = nullptr;
    USCS_Node* ParentPivot = nullptr;
    if (ParentLifeBP && ParentLifeBP->SimpleConstructionScript)
        for (auto* Node : ParentLifeBP->SimpleConstructionScript->GetAllNodes())
        {
            if (Node->GetVariableName() == TEXT("Behavior"))
                ParentBehavior = Node;
            if (Node->GetVariableName() == TEXT("Pivot"))
                ParentPivot = Node;
        }
    if (!TestTrue(TEXT("Parent component templates unchanged"),
                       ParentBehavior &&
                           ProbeInt(ParentBehavior->ComponentTemplate, TEXT("Bonus")) == 42 && ParentPivot &&
                                    !CastChecked<USceneComponent>(ParentPivot->ComponentTemplate)->bHiddenInGame))
        return false;
    State->LifeBlueprint.Reset(BP);
    if (!TestTrue(TEXT("Child owns only its three new SCS nodes"),
                       BP->SimpleConstructionScript && BP->SimpleConstructionScript->GetAllNodes().Num() == 3))
        return false;
    FActorSpawnParameters LifeSpawn;
    LifeSpawn.Name = TEXT("LifeOwner");
    if (!World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, LifeSpawn))
        return false;
    if (!TestTrue(TEXT("Create grandchild helper actor"),
                       FDasDSLService::Apply(TEXT("dsl_inheritance_grandchild.das"),
                                                  Root + TEXT("/BP_GrandCounter"), TEXT("inheritance.GrandCounter"), BP,
                                                                                        Error)))
    {
        AddError(Error);
        return false;
    }
    State->HelperBlueprint.Reset(BP);
    FActorSpawnParameters HelperSpawn;
    HelperSpawn.Name = TEXT("HelperOwner");
    if (!World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, HelperSpawn))
        return false;
    UBlueprint* LibraryBP = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_math_library.das"),
                                    Root + TEXT("/BP_GameMath"), TEXT("dsl_math_library.GameMath"), LibraryBP, Error))
    {
        AddError(Error);
        return false;
    }
    State->LibraryBlueprint.Reset(LibraryBP);
    auto* LibraryCarrier = World->SpawnActor<ADasStaticLibraryProbe>();
    if (!LibraryCarrier)
        return false;
    LibraryCarrier->LibraryClass = LibraryBP->GeneratedClass;
    TArray<UBlueprint*> DefaultAssets;
    if (!PrepareDasInstanceDefaultGameFixtures(Root, LibraryCarrier, DefaultAssets, Error, false))
    {
        AddError(Error);
        return false;
    }
    if (!PrepareDasPawnCharacterFixtures(Root, LibraryCarrier, DefaultAssets, Error, false))
    {
        AddError(Error);
        return false;
    }
    for (auto* Asset : DefaultAssets)
        State->InstanceDefaultBlueprints.Emplace(Asset);
    if (!PrepareDasLibraryTestTypes(Error))
    {
        AddError(Error);
        return false;
    }
    UBlueprint* Values = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_library_values.das"),
                                    Root + TEXT("/BP_GameValues"), TEXT("dsl_library_values.GameValues"), Values,
                                                                        Error))
    {
        AddError(Error);
        return false;
    }
    State->ValuesBlueprint.Reset(Values);
    LibraryCarrier->ValuesClass = Values->GeneratedClass;
    auto* DefaultConsumer = CreateDasPureConsumer(
        LibraryBP->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_DefaultConsumer"))),
                                                 TEXT("BP_DefaultConsumer"), Error, TEXT("DefaultAdd"), true);
    if (!DefaultConsumer)
    {
        AddError(Error);
        return false;
    }
    State->DefaultConsumerBlueprint.Reset(DefaultConsumer);
    auto* DefaultCaller = World->SpawnActor<ADasProbeActor>(DefaultConsumer->GeneratedClass);
    if (!DefaultCaller)
        return false;
    DefaultCaller->Tags.Add(TEXT("DefaultLibraryConsumer"));
    auto* WorldConsumer = CreateDasPureConsumer(
        Values->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_WorldConsumer"))), TEXT("BP_WorldConsumer"), Error);
    if (!WorldConsumer)
    {
        AddError(Error);
        return false;
    }
    State->WorldConsumerBlueprint.Reset(WorldConsumer);
    auto* WorldCaller = World->SpawnActor<ADasProbeActor>(WorldConsumer->GeneratedClass);
    if (!WorldCaller)
        return false;
    WorldCaller->Tags.Add(TEXT("WorldLibraryConsumer"));
    auto* LibraryConsumer =
        CreateDasPureConsumer(LibraryBP->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_LibraryConsumer"))),
                                                                       TEXT("BP_LibraryConsumer"), Error);
    if (!LibraryConsumer)
    {
        AddError(Error);
        return false;
    }
    State->LibraryConsumerBlueprint.Reset(LibraryConsumer);
    auto* LibraryCaller = World->SpawnActor<ADasProbeActor>(LibraryConsumer->GeneratedClass);
    if (!LibraryCaller)
        return false;
    LibraryCaller->Tags.Add(TEXT("LibraryConsumer"));
    auto* PureBP = CreateDasPureConsumer(BP->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_PureConsumer"))),
                                                                           TEXT("BP_PureConsumer"), Error);
    if (!PureBP)
    {
        AddError(Error);
        return false;
    }
    State->PureBlueprint.Reset(PureBP);
    auto* PureActor = World->SpawnActor<ADasProbeActor>(PureBP->GeneratedClass);
    if (!PureActor)
        return false;
    PureActor->Tags.Add(TEXT("PureOwner"));
    if (!TestTrue(TEXT("Create standalone construction component"),
                       FDasDSLService::Apply(TEXT("dsl_construction_component_leaf.das"),
                                                  Root + TEXT("/BP_LeafConstruction"),
                                                              TEXT("construction.LeafConstruction"), BP, Error)))
    {
        AddError(Error);
        return false;
    }
    State->ConstructionBlueprint.Reset(BP);
    auto* StandaloneOwner = World->SpawnActor<AActor>();
    StandaloneOwner->Tags.Add(TEXT("StandaloneConstructionOwner"));
    auto* Standalone = NewObject<UDasScriptComponent>(StandaloneOwner, BP->GeneratedClass,
                                                      TEXT("StandaloneConstruction"), RF_Transactional);
    StandaloneOwner->AddInstanceComponent(Standalone);
    Standalone->RegisterComponent();
    FindFProperty<FIntProperty>(Standalone->GetClass(), TEXT("Score"))->SetPropertyValue_InContainer(Standalone, 8);
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FGameplayPIECommand>(State, this));
    return true;
}
#endif
