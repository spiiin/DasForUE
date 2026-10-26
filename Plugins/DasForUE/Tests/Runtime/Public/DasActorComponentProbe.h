#pragma once
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "DasScriptComponent.h"
#include "DasFrameLibrary.h"
#include "Engine/World.h"
#include "Engine/EngineBaseTypes.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
inline int32 DasComponentInt(UDasScriptComponent* C, const TCHAR* Name)
{
    auto* P = C ? FindFProperty<FIntProperty>(C->GetClass(), Name) : nullptr;
    return P ? P->GetPropertyValue_InContainer(C) : MIN_int32;
}
inline float DasComponentFloat(UDasScriptComponent* C, const TCHAR* Name)
{
    auto* P = C ? FindFProperty<FFloatProperty>(C->GetClass(), Name) : nullptr;
    return P ? P->GetPropertyValue_InContainer(C) : -1.f;
}
inline bool RunDasActorComponentProbe(UClass* Class, FString& Report)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const int32 BeforeBindings = R.SubscriptionCount();
    bool Pass = true;
    auto Check = [&](bool Good, const TCHAR* Name)
    {
        Pass &= Good;
        Report += FString::Printf(TEXT("%s %s\n"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
    };
    TStrongObjectPtr<UWorld> First(UWorld::CreateWorld(EWorldType::Game, false)),
        Second(UWorld::CreateWorld(EWorldType::Game, false));
    First->InitializeActorsForPlay(FURL());
    Second->InitializeActorsForPlay(FURL());
    auto Spawn = [&](UWorld* World)
    {
        auto* A = World->SpawnActor<ADasActor>(Class);
        A->PostInitializeComponents();
        A->DispatchBeginPlay();
        return A;
    };
    auto* A = Spawn(First.Get());
    auto* B = Spawn(Second.Get());
    auto* CA = A->FindComponentByClass<UDasScriptComponent>();
    auto* CB = B->FindComponentByClass<UDasScriptComponent>();
    Check(CA && CB &&
          DasComponentInt(CA, TEXT("BeginCount")) == 1 &&
                          DasComponentInt(CB, TEXT("BeginCount")) == 1 && CA->LastDasError.IsEmpty() &&
                                          CB->LastDasError.IsEmpty(),
                                          TEXT("component_BeginPlay_once_per_instance"));
    if (!CA || !CB)
        return false;
    auto* Bonus = FindFProperty<FIntProperty>(CA->GetClass(), TEXT("Bonus"));
    Check(Bonus && Bonus->GetPropertyValue_InContainer(CA) == 10 &&
              CA->GetClass() != UDasScriptComponent::StaticClass(),
          TEXT("DSL_component_class_and_property_default"));
    auto* Evaluate = CA->FindFunction(TEXT("Evaluate"));
    struct FEvaluate
    {
        int32 Input = 5, Result = 0;
    };
    FEvaluate Eval;
    if (Evaluate)
        CA->ProcessEvent(Evaluate, &Eval);
    Check(Evaluate && Eval.Result == 15 && CA->LastDasError.IsEmpty(), TEXT("DSL_component_Blueprint_method_frame"));
    auto* Accumulate = CA->FindFunction(TEXT("Accumulate"));
    struct FAccumulate
    {
        int32 Value = 7, Result = 0;
    };
    FAccumulate Ref;
    if (Accumulate)
        CA->ProcessEvent(Accumulate, &Ref);
    Check(Accumulate && Ref.Value == 17 && Ref.Result == 17 && CA->LastDasError.IsEmpty(),
          TEXT("DSL_component_ref_output_frame"));
    auto* Configure = A->FindFunction(TEXT("ConfigureBehavior"));
    struct FOneInt
    {
        int32 Value = 20;
    };
    FOneInt Set;
    if (Configure)
        A->ProcessEvent(Configure, &Set);
    auto* Add = A->FindFunction(TEXT("AddBehaviorBonus"));
    struct FAdd
    {
        int32 Amount = 3, Result = 0;
    };
    FAdd Added;
    if (Add)
        A->ProcessEvent(Add, &Added);
    auto* Read = A->FindFunction(TEXT("ReadBehaviorBonus"));
    struct FRead
    {
        int32 Result = 0;
    };
    FRead ReadValue;
    if (Read)
        A->ProcessEvent(Read, &ReadValue);
    Check(Configure && Add && Read && Added.Result == 23 && ReadValue.Result == 23 &&
              Bonus->GetPropertyValue_InContainer(CA) == 23 && Bonus->GetPropertyValue_InContainer(CB) == 10 &&
              A->LastDasError.IsEmpty(),
          TEXT("DSL_self_component_fields_alias_and_compound_write"));
    Report += FString::Printf(TEXT("COMPONENT_ACCESS_DIAGNOSTIC configure=%d addfn=%d readfn=%d add=%d read=%d actual=%d other=%d error=%s\n"),
        Configure != nullptr, Add != nullptr, Read != nullptr, Added.Result, ReadValue.Result,
        Bonus ? Bonus->GetPropertyValue_InContainer(CA) : -1, Bonus ? Bonus->GetPropertyValue_InContainer(CB) : -1,
        *A->LastDasError);
    Set.Value = 10;
    if (Configure)
        A->ProcessEvent(Configure, &Set);
    auto InvokeInt = [&](const TCHAR* Name, int32 Input)
    {
        struct FArgs
        {
            int32 Input, Result;
        };
        FArgs Args{Input, 0};
        if (auto* Fn = A->FindFunction(Name))
            A->ProcessEvent(Fn, &Args);
        return Args.Result;
    };
    Check(InvokeInt(TEXT("CallBehavior"), 5) == 15 && A->LastDasError.IsEmpty(), TEXT("typed_component_method_result"));
    Check(InvokeInt(TEXT("CallBehaviorRef"), 7) == 34 && A->LastDasError.IsEmpty(),
                    TEXT("typed_component_method_ref_copyback"));
    Check(InvokeInt(TEXT("CallBehaviorOut"), 8) == 18 && A->LastDasError.IsEmpty(),
                    TEXT("typed_component_method_out_copyback"));
    Check(InvokeInt(TEXT("Bounce"), 6) == 16 && A->LastDasError.IsEmpty() && CA->LastDasError.IsEmpty(),
                    TEXT("typed_method_actor_component_reentry_preserves_stack"));
    Check(InvokeInt(TEXT("CallBehaviorHelper"), 5) == 15 && A->LastDasError.IsEmpty(),
                    TEXT("typed_method_ordinary_function_and_same_context_reentry"));
    FRead Values;
    auto* ValuesFn = A->FindFunction(TEXT("CallValues"));
    if (ValuesFn)
        A->ProcessEvent(ValuesFn, &Values);
    Check(ValuesFn && Values.Result == 1 && A->LastDasError.IsEmpty(),
          TEXT("typed_method_scalar_name_enum_struct_object_marshalling"));
    FRead FailureResult;
    auto* FailureFn = A->FindFunction(TEXT("FailurePreservesRef"));
    if (FailureFn)
        A->ProcessEvent(FailureFn, &FailureResult);
    Check(FailureFn && FailureResult.Result == 7 && A->LastDasError.IsEmpty(),
          TEXT("typed_method_failure_preserves_ref_argument"));
    FRead RepeatResult;
    auto* RepeatFn = A->FindFunction(TEXT("RepeatFailure"));
    if (RepeatFn)
        A->ProcessEvent(RepeatFn, &RepeatResult);
    Check(RepeatFn && RepeatResult.Result == 1 && A->LastDasError.IsEmpty(),
          TEXT("typed_method_recovered_failures_release_call_frames"));
    FDasScalarValue BounceInput;
    BounceInput.Kind = 0;
    BounceInput.IntValue = 80;
    const auto BudgetFailure = R.RunNamedFramePersistent(R.ScriptPath(TEXT("dsl_actor_component_owner.das")),
                                                                      TEXT("ue_ComponentOwner_Bounce"), {BounceInput},
                                                                           EDasMethodSignature::Int, A);
    Check(!BudgetFailure.bSucceeded &&
          BudgetFailure.Error.Contains(TEXT("recursion limit")),
                                       TEXT("typed_method_recursion_budget_refuses_unbounded_reentry"));
    Check(InvokeInt(TEXT("CallBehavior"), 5) == 15 && A->LastDasError.IsEmpty(),
                    TEXT("typed_method_recovers_after_failed_nested_call"));
    Check(InvokeInt(TEXT("CallViaOwner"), 5) == 15 && A->LastDasError.IsEmpty(),
                    TEXT("typed_method_component_member_through_actor_reference"));
    FRead ComponentRoundtrip;
    auto* RoundtripFn = A->FindFunction(TEXT("RoundtripComponent"));
    if (RoundtripFn)
        A->ProcessEvent(RoundtripFn, &ComponentRoundtrip);
    Check(RoundtripFn && ComponentRoundtrip.Result == 10 && A->LastDasError.IsEmpty(),
          TEXT("authored_component_reference_parameter_and_result"));
    Check(R.PersistentContextCount(First.Get()) == 2 && R.PersistentContextCount(Second.Get()) == 2 &&
              R.SubscriptionCount(CA) == 1 && R.SubscriptionCount(CB) == 1,
          TEXT("actor_component_contexts_worlds_and_bindings_isolated"));
    CastChecked<ADasProbeActor>(A)->EmitDasSignal(7);
    Check(DasComponentInt(CA, TEXT("SignalTotal")) == 7 &&
                          DasComponentInt(CB, TEXT("SignalTotal")) == 0, TEXT("component_receives_only_owner_signal"));
    CA->TickComponent(0.25f, LEVELTICK_All, &CA->PrimaryComponentTick);
    Check(DasComponentInt(CA, TEXT("TickCount")) == 1 &&
                          FMath::IsNearlyEqual(DasComponentFloat(CA, TEXT("Seconds")), 0.25f) &&
                                               DasComponentInt(CB, TEXT("TickCount")) == 0,
                                                               TEXT("component_tick_delta_and_state_isolated"));
    auto* Score = FindFProperty<FIntProperty>(Class, TEXT("Score"));
    Check(Score && Score->GetPropertyValue_InContainer(A) == 101 && Score->GetPropertyValue_InContainer(B) == 100,
          TEXT("component_updates_DSL_actor_through_checked_properties"));
    TStrongObjectPtr<UDasScriptComponent> KeepRemoved(CA);
    CA->DestroyComponent();
    const int32 EndedCounter = DasComponentInt(CA, TEXT("SignalTotal"));
    CastChecked<ADasProbeActor>(A)->EmitDasSignal(9);
    Check(DasComponentInt(CA, TEXT("EndCount")) == 1 &&
                          DasComponentInt(CA, TEXT("SignalTotal")) == EndedCounter && R.SubscriptionCount(CA) == 0 &&
                                          R.PersistentContextCount(First.Get()) == 1,
                                          TEXT("component_destroy_ends_and_revokes_binding_context_once"));
    const auto RemovedMethod = R.RunNamedFramePersistent(
        R.ScriptPath(TEXT("dsl_actor_component_owner.das")), TEXT("ue_ComponentOwner_CallBehavior"),
                                                                  {UDasFrameLibrary::DefaultValue(0, TEXT(""), 0)},
                                                                   EDasMethodSignature::Int, A);
    Check(!RemovedMethod.bSucceeded && !RemovedMethod.Error.IsEmpty(),
          TEXT("typed_method_destroyed_component_refused"));
    const auto Removed = R.RunNamedFramePersistent(R.ScriptPath(TEXT("dsl_actor_component_owner.das")),
                                                                TEXT("ue_ComponentOwner_ReadBehaviorBonus"), {},
                                                                     EDasMethodSignature::Int, A);
    Check(!Removed.bSucceeded && !Removed.Error.IsEmpty(), TEXT("DSL_self_component_destroyed_reference_refused"));
    TStrongObjectPtr<UDasScriptComponent> KeepDestroyed(CB);
    B->Destroy();
    Check(DasComponentInt(CB, TEXT("EndCount")) == 1 && R.SubscriptionCount(CB) == 0 &&
                          R.PersistentContextCount(Second.Get()) == 0,
                          TEXT("actor_destroy_ends_component_and_releases_world_sessions"));
    auto* C = Spawn(First.Get());
    auto* CC = C->FindComponentByClass<UDasScriptComponent>();
    Check(CC && DasComponentInt(CC, TEXT("BeginCount")) == 1 && R.SubscriptionCount(CC) == 1,
                                TEXT("new_component_does_not_reuse_ended_session"));
    First->DestroyWorld(false);
    Second->DestroyWorld(false);
    Check(R.PersistentContextCount(First.Get()) == 0 && R.PersistentContextCount(Second.Get()) == 0 &&
              R.SubscriptionCount() == BeforeBindings,
          TEXT("world_cleanup_revokes_actor_component_sessions"));
    Report += FString::Printf(TEXT("DAS_ACTOR_COMPONENT_RESULT=%s\n"), Pass ? TEXT("PASS"): TEXT("FAIL"));
    return Pass;
}
