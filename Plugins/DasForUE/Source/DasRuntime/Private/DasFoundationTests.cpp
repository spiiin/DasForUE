#include "DasRuntime.h"
#include "DasSchema.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "DasObjectFixture.h"
#include "DasIntDelegateProxy.h"
#include "UObject/StrongObjectPtr.h"
#include "Modules/ModuleManager.h"

bool RunDasFoundationTests(FString& Report)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Failures = 0;
    auto Expect = [&](bool Condition, const TCHAR* Name, const FString& Detail = FString())
    {
        Report += FString::Printf(TEXT("%s %s %s\n"), Condition ? TEXT("PASS") : TEXT("FAIL"), Name, *Detail);
        if (!Condition) ++Failures;
    };
    auto Run = [&](const TCHAR* Name) { return Runtime.RunFile(Runtime.ScriptPath(Name), 40); };
    const auto Good = Run(TEXT("smoke.das"));
    Expect(Good.bSucceeded && Good.Value == 42, TEXT("native_roundtrip"), Good.Error);
    const auto Again = Run(TEXT("smoke.das"));
    Expect(Again.bSucceeded && Again.Value == 42, TEXT("independent_context"), Again.Error);
    const auto Syntax = Run(TEXT("invalid.das"));
    Expect(!Syntax.bSucceeded && !Syntax.Error.IsEmpty(), TEXT("compile_error_diagnostic"));
    const auto Signature = Run(TEXT("wrong_signature.das"));
    Expect(!Signature.bSucceeded && Signature.Error.Contains(TEXT("main")), TEXT("signature_rejected"));
    const auto Panic = Run(TEXT("panic.das"));
    Expect(!Panic.bSucceeded && Panic.Error.Contains(TEXT("intentional")), TEXT("runtime_exception_caught"));
    const auto Missing = Run(TEXT("not_present.das"));
    Expect(!Missing.bSucceeded && !Missing.Error.IsEmpty(), TEXT("missing_file_diagnostic"));
    const auto Schema1 = Run(TEXT("schema_v1.das"));
    Expect(Schema1.bSucceeded && Schema1.Types.Num() == 1 && Schema1.Types[0].Name == TEXT("ProbeActor") && Schema1.Types[0].Fields == TEXT("speed:float;"), TEXT("macro_schema_v1"), Schema1.Error);
    const auto Schema2 = Run(TEXT("schema_v2.das"));
    Expect(Schema2.bSucceeded && Schema2.Types.Num() == 1 && Schema2.Types[0].Fields == TEXT("speed:float;health:int;"), TEXT("macro_schema_v2_same_process"), Schema2.Error);
    const auto Aot = Runtime.RunFile(Runtime.ScriptPath(TEXT("aot_smoke.das")), 40, true);
    Expect(Aot.bSucceeded && Aot.Value == 42 && Aot.bUsedAot, TEXT("aot_native_execution"), Aot.Error);
    const auto InvalidSchema = Run(TEXT("schema_invalid.das"));
    Expect(!InvalidSchema.bSucceeded && InvalidSchema.Error.Contains(TEXT("ue_probe")), TEXT("unsupported_schema_rejected"), InvalidSchema.Error);
    const auto Unicode = Run(TEXT("unicode.das"));
    Expect(Unicode.bSucceeded && Unicode.Types.Num() == 1 && Unicode.Types[0].Name == TEXT("Тип") && Unicode.Types[0].Fields == TEXT("здоровье:int;"), TEXT("utf8_owned_after_context_destroyed"), Unicode.Error);
    const auto NoSelf = Run(TEXT("self_properties.das"));
    Expect(!NoSelf.bSucceeded && NoSelf.Error.Contains(TEXT("self is missing or expired")), TEXT("missing_self_rejected"));
    TStrongObjectPtr<UDasObjectFixture> Fixture(NewObject<UDasObjectFixture>());
    TWeakObjectPtr<UObject> WeakSelf(Fixture.Get());
    auto RunSelf = [&](const TCHAR* Name) { return Runtime.RunFile(Runtime.ScriptPath(Name), 5, false, WeakSelf); };
    const auto Access = RunSelf(TEXT("self_properties.das"));
    Expect(Access.bSucceeded && Access.Value == 105 && Fixture->health == 105 && Fixture->speed == 38.f,
        TEXT("self_scalar_read_write"), Access.Error);
    const auto WrongType = RunSelf(TEXT("self_wrong_type.das"));
    Expect(!WrongType.bSucceeded && WrongType.Error.Contains(TEXT("different scalar type")), TEXT("self_wrong_type_rejected"));
    const auto MissingField = RunSelf(TEXT("self_missing_field.das"));
    Expect(!MissingField.bSucceeded && MissingField.Error.Contains(TEXT("does not exist")), TEXT("self_missing_property_rejected"));
    const auto ReadOnly = RunSelf(TEXT("self_readonly.das"));
    Expect(!ReadOnly.bSucceeded && ReadOnly.Error.Contains(TEXT("read-only")) && Fixture->readOnly == 7,
        TEXT("self_readonly_write_rejected"));
    Fixture.Reset();
    CollectGarbage(RF_NoFlags);
    Expect(!WeakSelf.IsValid(), TEXT("self_weak_reference_does_not_pin_object"));
    const auto Expired = RunSelf(TEXT("self_properties.das"));
    Expect(!Expired.bSucceeded && Expired.Error.Contains(TEXT("self is missing or expired")), TEXT("expired_self_rejected"));
    FDasClassSchema Typed;
    FString SchemaError;
    Expect(FDasClassSchema::FromDeclaration({TEXT("ActorType"), TEXT("Actor"), TEXT("speed:float;health:int;")}, TEXT("smoke.das"), Typed, SchemaError)
        && Typed.Fields.Num() == 2 && Typed.Fields[0].Kind == EDasScalarKind::Float32 && Typed.Fields[1].Kind == EDasScalarKind::Int32,
        TEXT("typed_schema_from_macro_wire"), SchemaError);
    Expect(!FDasClassSchema::FromDeclaration({TEXT("ActorType"), TEXT("Actor"), TEXT("speed:int;SPEED:float;")}, TEXT("smoke.das"), Typed, SchemaError)
        && Typed.Fields.Num() == 2 && Typed.Fields[0].Kind == EDasScalarKind::Float32,
        TEXT("invalid_schema_preserves_previous_IR"));

    // Each run owns a unique scratch script; source plugin examples are never modified.
    const FString Scratch = FPaths::ProjectSavedDir() / TEXT("DasPersistent") / FGuid::NewGuid().ToString(EGuidFormats::Digits) / TEXT("counter.das");
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Scratch), true);
    const FString Counter = TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int {\n let n = ue_state_get_int(\"count\", 0) + input\n ue_state_set_int(\"count\", n)\n return n + OFFSET\n}\n");
    auto Write = [&](const FString& Text) { return FFileHelper::SaveStringToFile(Text, *Scratch, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM); };
    TStrongObjectPtr<UDasObjectFixture> First(NewObject<UDasObjectFixture>());
    TStrongObjectPtr<UDasObjectFixture> Second(NewObject<UDasObjectFixture>());
    auto Persistent = [&](UObject* Object, int32 Input = 1) { return Runtime.RunPersistent(Scratch, Input, Object); };
    Expect(Write(Counter.Replace(TEXT("OFFSET"), TEXT("0"))), TEXT("persistent_write_fixture"));
    auto P1 = Persistent(First.Get());
    auto P2 = Persistent(First.Get());
    auto POther = Persistent(Second.Get());
    Expect(P1.bSucceeded && P1.Value == 1 && P2.Value == 2 && P2.Revision == 1 && POther.Value == 1,
        TEXT("persistent_independent_state_and_cached_revision"), P1.Error);
    Expect(Write(Counter.Replace(TEXT("OFFSET"), TEXT("100"))), TEXT("persistent_write_revision"));
    P1 = Persistent(First.Get()); POther = Persistent(Second.Get());
    Expect(P1.bSucceeded && P1.Value == 103 && POther.Value == 102 && P1.Revision == 2,
        TEXT("persistent_reload_preserves_each_counter"), P1.Error);
    Expect(Write(TEXT("this is invalid daScript")), TEXT("persistent_write_invalid"));
    P1 = Persistent(First.Get()); P2 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 104 && !P1.ReloadError.IsEmpty() && P2.Value == 105 && P2.Revision == 2,
        TEXT("persistent_bad_source_keeps_previous_code_and_state"), P1.Error);
    Expect(Write(TEXT("options gen2\n[export]\ndef main():int { return 7 }\n")), TEXT("persistent_write_bad_signature"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 106 && P1.ReloadError.Contains(TEXT("main")), TEXT("persistent_bad_signature_fallback"));
    Expect(Write(Counter.Replace(TEXT("OFFSET"), TEXT("200"))), TEXT("persistent_repair_source"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 207 && P1.Revision == 3 && P1.ReloadError.IsEmpty(), TEXT("persistent_recovery"), P1.Error);
    const FString InitWrites = TEXT("options gen2\nrequire ue_foundation\ndef initialize():int { ue_set_self_int(\"health\", 999); return 1 }\nvar initialized = initialize()\n[export]\ndef main(input:int):int { return initialized + input }\n");
    Expect(Write(InitWrites), TEXT("persistent_write_unsafe_initializer"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 208 && !P1.ReloadError.IsEmpty() && First->health == 100 && P1.Revision == 3,
        TEXT("persistent_rejected_initializer_cannot_mutate_actor"), P1.ReloadError);
    const FString Panicking = TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int { if (input < 0) { panic(\"negative\") }; return ue_state_get_int(\"count\", 0) }\n");
    Expect(Write(Panicking), TEXT("persistent_write_runtime_failure_fixture"));
    P1 = Persistent(First.Get(), -1); P2 = Persistent(First.Get());
    Expect(!P1.bSucceeded && P1.Error.Contains(TEXT("negative")) && P2.bSucceeded && P2.Value == 8 && P1.Revision == P2.Revision,
        TEXT("persistent_context_recovers_after_runtime_exception"), P1.Error + P1.ReloadError);
    Expect(Write(Counter.Replace(TEXT("OFFSET"), TEXT("200"))), TEXT("persistent_restore_counter_fixture"));
    Runtime.ReleaseObject(First.Get());
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 201 && P1.Revision == 1, TEXT("persistent_explicit_release_resets_session"));
    const FString HelperPath = FPaths::GetPath(Scratch) / TEXT("counter_values.das");
    auto WriteHelper = [&](int32 Value) { return FFileHelper::SaveStringToFile(
        FString::Printf(TEXT("options gen2\nmodule counter_values public\ndef counter_offset():int { return %d }\n"), Value),
        *HelperPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM); };
    const FString WithDependency = Counter.Replace(TEXT("require ue_foundation"), TEXT("require ue_foundation\nrequire counter_values"))
        .Replace(TEXT("OFFSET"), TEXT("counter_offset()"));
    Expect(WriteHelper(10) && Write(WithDependency), TEXT("persistent_write_dependency"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 12, TEXT("persistent_dependency_initial"), P1.Error + P1.ReloadError);
    const uint32 DependencyRevision = P1.Revision;
    Expect(WriteHelper(20), TEXT("persistent_change_dependency_only"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 23 && P1.Revision == DependencyRevision + 1,
        TEXT("persistent_dependency_change_triggers_reload"), P1.Error + P1.ReloadError);
    const FString Globals = TEXT("options gen2\nvar calls = 0\n[export]\ndef main(input:int):int { calls += 1; return calls + input }\n");
    Expect(Write(Globals), TEXT("persistent_write_globals_fixture"));
    P1 = Persistent(First.Get(), 0); P2 = Persistent(First.Get(), 0);
    Expect(P1.bSucceeded && P1.Value == 1 && P2.bSucceeded && P2.Value == 2 && P1.Revision == P2.Revision,
        TEXT("persistent_vm_globals_reused_between_calls"), P1.Error + P1.ReloadError);
    IFileManager::Get().Delete(*HelperPath);
    Runtime.ReleaseObject(First.Get());
    FString LifecycleSource;
    Expect(FFileHelper::LoadFileToString(LifecycleSource, *Runtime.ScriptPath(TEXT("lifecycle_actor.das"))) && Write(LifecycleSource), TEXT("lifecycle_fixture"));
    auto Event = [&](EDasCallback Kind, float Delta = 0.f, int32 Reason = 0) { return Runtime.RunLifecycle(Scratch, Kind, First.Get(), Delta, Reason); };
    auto Begin = Event(EDasCallback::BeginPlay);
    auto BeginAgain = Event(EDasCallback::BeginPlay);
    Expect(Begin.bSucceeded && BeginAgain.bSucceeded && Persistent(First.Get(), 0).Value == 1 && First->health == 101,
        TEXT("lifecycle_begin_once"), Begin.Error);
    auto Tick = Event(EDasCallback::Tick, 0.25f);
    Expect(Tick.bSucceeded && First->speed == 37.25f && Persistent(First.Get()).Value == 1, TEXT("lifecycle_float_delta"), Tick.Error);
    Expect(Write(LifecycleSource.Replace(TEXT("delta_time * 1.0"), TEXT("delta_time * 10.0"))), TEXT("lifecycle_write_reload"));
    Tick = Event(EDasCallback::Tick, 0.5f);
    Expect(Tick.bSucceeded && Tick.Revision == 2 && First->speed == 42.25f && Persistent(First.Get(), 0).Value == 1 &&
        Persistent(First.Get()).Value == 2 && First->health == 101, TEXT("lifecycle_reload_does_not_repeat_begin"), Tick.Error);
    Expect(Write(TEXT("options gen2\n[export]\ndef tick(delta_time:int) {}\n")), TEXT("lifecycle_write_bad_signature"));
    Tick = Event(EDasCallback::Tick, 0.5f);
    Expect(Tick.bSucceeded && Tick.ReloadError.Contains(TEXT("tick")) && Tick.Revision == 2 && First->speed == 47.25f,
        TEXT("lifecycle_rejects_hook_signature_keeps_old_version"), Tick.ReloadError);
    Expect(Write(TEXT("options gen2\n[export]\ndef begin_play(input:int) {}\n")), TEXT("lifecycle_bad_begin_fixture"));
    Tick = Event(EDasCallback::Tick, 0.f);
    Expect(Tick.bSucceeded && Tick.ReloadError.Contains(TEXT("begin_play")) && Tick.Revision == 2,
        TEXT("lifecycle_bad_begin_signature_rejected"));
    Expect(Write(TEXT("options gen2\n[export]\ndef end_play(reason:float) {}\n")), TEXT("lifecycle_bad_end_fixture"));
    Tick = Event(EDasCallback::Tick, 0.f);
    Expect(Tick.bSucceeded && Tick.ReloadError.Contains(TEXT("end_play")) && Tick.Revision == 2,
        TEXT("lifecycle_bad_end_signature_rejected"));
    const auto End = Event(EDasCallback::EndPlay, 0.f, 2);
    const auto EndAgain = Event(EDasCallback::EndPlay, 0.f, 2);
    Expect(End.bSucceeded && EndAgain.bSucceeded && First->health == 203, TEXT("lifecycle_end_once_uses_live_code"), End.Error);
    Expect(!Event(EDasCallback::Tick, 1.f).bSucceeded, TEXT("lifecycle_tick_after_end_rejected"));
    Runtime.ReleaseObject(First.Get());
    Expect(Write(TEXT("options gen2\n[export]\ndef tick(delta_time:float) {}\n")), TEXT("lifecycle_only_script_fixture"));
    Expect(Event(EDasCallback::BeginPlay).bSucceeded && Event(EDasCallback::Tick, 0.1f).bSucceeded &&
        Event(EDasCallback::EndPlay).bSucceeded, TEXT("lifecycle_optional_hooks_without_main"));
    Runtime.ReleaseObject(First.Get());
    Expect(Write(TEXT("invalid script")), TEXT("lifecycle_initial_error_fixture"));
    Expect(!Event(EDasCallback::BeginPlay).bSucceeded, TEXT("lifecycle_initial_compile_failure"));
    First->health = 100;
    Expect(Write(LifecycleSource), TEXT("lifecycle_repair_initial_source"));
    Tick = Event(EDasCallback::Tick, 0.1f);
    Expect(Tick.bSucceeded && First->health == 101 && Persistent(First.Get(), 0).Value == 1,
        TEXT("lifecycle_first_successful_tick_delivers_missing_begin"), Tick.Error);
    Runtime.ReleaseObject(First.Get());
    Expect(Event(EDasCallback::EndPlay).bSucceeded, TEXT("lifecycle_end_does_not_create_session"));
    Expect(Write(TEXT("options gen2\nrequire ue_foundation\n[export]\ndef begin_play() { ue_set_self_int(\"health\", ue_get_self_int(\"health\") + 1); panic(\"begin failed\") }\n[export]\ndef tick(dt:float) {}\n")), TEXT("lifecycle_begin_panic_fixture"));
    First->health = 100;
    Begin = Event(EDasCallback::BeginPlay);
    Tick = Event(EDasCallback::Tick, 0.1f);
    Expect(!Begin.bSucceeded && Begin.Error.Contains(TEXT("begin failed")) && Tick.bSucceeded && First->health == 101,
        TEXT("lifecycle_begin_panic_not_replayed"), Begin.Error);
    Runtime.ReleaseObject(First.Get());

    FString DelegateSource;
    Expect(FFileHelper::LoadFileToString(DelegateSource, *Runtime.ScriptPath(TEXT("delegate_actor.das"))), TEXT("delegate_fixture_loaded"));
    DelegateSource = DelegateSource.Replace(TEXT("\"peer\""), TEXT("\"self\""));
    Expect(Write(DelegateSource), TEXT("delegate_fixture_written"));
    FScriptDelegate Foreign;
    Foreign.BindUFunction(First.Get(), GET_FUNCTION_NAME_CHECKED(UDasObjectFixture, RecordForeignSignal));
    First->OnDasSignal.AddUnique(Foreign);
    P1 = Persistent(First.Get());
    First->OnDasSignal.Broadcast(3);
    Expect(P1.bSucceeded && Persistent(First.Get()).Value == 3 && First->ForeignReceived == 3 && Runtime.SubscriptionCount(First.Get()) == 1,
        TEXT("delegate_native_broadcast_to_script"), P1.Error);
    uint64 OldSubscriptionId = 0;
    for (UObject* Bound : First->OnDasSignal.GetAllObjects())
        if (auto* Proxy = Cast<UDasIntDelegateProxy>(Bound)) OldSubscriptionId = Proxy->SubscriptionId;
    Expect(OldSubscriptionId != 0, TEXT("delegate_has_opaque_token"));
    const FString DelegateV2 = DelegateSource.Replace(TEXT("+ value)"), TEXT("+ value * 10)"));
    Expect(Write(DelegateV2), TEXT("delegate_write_new_handler"));
    P1 = Persistent(First.Get());
    First->OnDasSignal.Broadcast(2);
    Runtime.DispatchIntEvent(OldSubscriptionId, 1000);
    Expect(P1.bSucceeded && Persistent(First.Get()).Value == 23 && Runtime.SubscriptionCount(First.Get()) == 1 &&
        First->OnDasSignal.GetAllObjects().Num() == 2 && First->ForeignReceived == 5,
        TEXT("delegate_reload_replaces_once_and_revokes_old_token"), P1.ReloadError);
    const FString BadHandler = DelegateSource.Replace(TEXT("on_signal(value:int)"), TEXT("on_signal(value:float)"))
        .Replace(TEXT("+ value)"), TEXT("+ int(value))"));
    Expect(Write(BadHandler), TEXT("delegate_bad_handler_fixture"));
    P1 = Persistent(First.Get()); First->OnDasSignal.Broadcast(1);
    Expect(P1.bSucceeded && P1.ReloadError.Contains(TEXT("event handler")) && Persistent(First.Get()).Value == 33 && Runtime.SubscriptionCount(First.Get()) == 1,
        TEXT("delegate_bad_handler_keeps_old_subscription"), P1.ReloadError);
    Expect(Write(DelegateSource.Replace(TEXT("OnDasSignal"), TEXT("OnFloatSignal"))), TEXT("delegate_bad_event_fixture"));
    P1 = Persistent(First.Get()); First->OnDasSignal.Broadcast(1);
    Expect(P1.bSucceeded && !P1.ReloadError.IsEmpty() && Persistent(First.Get()).Value == 43, TEXT("delegate_rejects_wrong_UE_signature"));
    const FString SubscribeLine = TEXT("ue_subscribe_int(\"self\", \"OnDasSignal\", \"on_signal\")");
    Expect(Write(DelegateSource.Replace(*SubscribeLine, *(SubscribeLine + TEXT("\n    ") + SubscribeLine))), TEXT("delegate_duplicate_fixture"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.ReloadError.Contains(TEXT("duplicate")) && Runtime.SubscriptionCount(First.Get()) == 1,
        TEXT("delegate_duplicate_candidate_is_atomic"), P1.ReloadError);
    const int32 HealthBeforeBinding = First->health;
    Expect(Write(DelegateSource.Replace(*SubscribeLine, TEXT("ue_set_self_int(\"health\", 999)"))), TEXT("delegate_unsafe_binding_fixture"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && !P1.ReloadError.IsEmpty() && First->health == HealthBeforeBinding && Runtime.SubscriptionCount(First.Get()) == 1,
        TEXT("delegate_rejected_binding_has_no_actor_side_effects"));
    Expect(Write(DelegateV2.Replace(TEXT("def on_signal(value:int) {"), TEXT("def on_signal(value:int) {\n if (value < 0) { panic(\"event failed\") }"))), TEXT("delegate_panic_fixture"));
    P1 = Persistent(First.Get()); First->OnDasSignal.Broadcast(-1); First->OnDasSignal.Broadcast(1);
    Expect(P1.bSucceeded && Persistent(First.Get()).Value == 53 && Runtime.SubscriptionCount(First.Get()) == 1,
        TEXT("delegate_handler_recovers_after_panic"), P1.ReloadError);
    Expect(Event(EDasCallback::BeginPlay).bSucceeded && Event(EDasCallback::EndPlay).bSucceeded && Runtime.SubscriptionCount(First.Get()) == 0 &&
        First->OnDasSignal.GetAllObjects().Num() == 1, TEXT("delegate_EndPlay_removes_only_owned_bindings"));
    Runtime.ReleaseObject(First.Get());
    Expect(Write(DelegateSource), TEXT("delegate_GC_fixture"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && Runtime.SubscriptionCount(First.Get()) == 1, TEXT("delegate_before_owner_GC"));
    Runtime.ReleaseObject(First.Get());
    TStrongObjectPtr<UDasObjectFixture> ObjectTarget(NewObject<UDasObjectFixture>());
    First->Target = ObjectTarget.Get();
    auto ObjectRun = [&](const FString& Body) {
        Write(TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int {\n") + Body + TEXT("\n}\n"));
        return Runtime.RunFile(Scratch, 5, false, First.Get());
    };
    const auto ObjectExample = Runtime.RunFile(Runtime.ScriptPath(TEXT("object_calls.das")), 5, false, First.Get());
    Expect(ObjectExample.bSucceeded && ObjectExample.Value == 105 && ObjectTarget->health == 105,
        TEXT("object_native_UFunction_example"), ObjectExample.Error);
    auto Obj = ObjectRun(TEXT("let obj = ue_get_object(ue_self_object(), \"Target\")\nverify(ue_object_is_a(obj, \"/Script/CoreUObject.Object\"))\nlet scaled = ue_call_float(obj, \"ScaleSpeed\", 2.0)\nverify(scaled == 74.0)\nue_object_set_int(obj, \"health\", 17)\nreturn ue_object_get_int(obj, \"health\")"));
    Expect(Obj.bSucceeded && Obj.Value == 17, TEXT("object_scalar_properties_and_float_call"), Obj.Error);
    Obj = ObjectRun(TEXT("let obj = ue_call_object(ue_self_object(), \"GetTargetObject\")\nue_call_void_object(ue_self_object(), \"SetFixtureTarget\", obj)\nreturn ue_object_get_int(obj, \"health\")"));
    Expect(Obj.bSucceeded && Obj.Value == 17 && First->Target == ObjectTarget.Get(), TEXT("object_return_and_typed_argument"), Obj.Error);
    Obj = ObjectRun(TEXT("let obj = ue_cast_object(ue_self_object(), \"/Script/Engine.Actor\")\nreturn 0"));
    Expect(!Obj.bSucceeded && Obj.Error.Contains(TEXT("requested class")), TEXT("object_invalid_class_cast_rejected"));
    Obj = ObjectRun(TEXT("return ue_object_get_int(123, \"health\")"));
    Expect(!Obj.bSucceeded, TEXT("object_handle_is_not_an_integer_at_compile_time"));
    Obj = ObjectRun(TEXT("return ue_call_int(ue_get_object(ue_self_object(), \"Target\"), \"SetHealthValue\", 999)"));
    Expect(!Obj.bSucceeded && Obj.Error.Contains(TEXT("signature")) && ObjectTarget->health == 17,
        TEXT("object_wrong_signature_rejected_before_mutation"));
    const int32 ForeignBefore = First->ForeignReceived;
    Obj = ObjectRun(TEXT("ue_call_void_int(ue_self_object(), \"RecordForeignSignal\", 99)\nreturn 0"));
    Expect(!Obj.bSucceeded && Obj.Error.Contains(TEXT("BlueprintCallable")) && First->ForeignReceived == ForeignBefore,
        TEXT("object_non_BlueprintCallable_rejected"));
    Obj = ObjectRun(TEXT("ue_call_void_int(ue_self_object(), \"OutValue\", 99)\nreturn 0"));
    Expect(!Obj.bSucceeded && Obj.Error.Contains(TEXT("out/ref")), TEXT("object_out_parameter_rejected"));
    Obj = ObjectRun(TEXT("return ue_call_int(ue_self_object(), \"MissingFunction\", 0)"));
    Expect(!Obj.bSucceeded && Obj.Error.Contains(TEXT("BlueprintCallable")), TEXT("object_missing_function_rejected"));
    TStrongObjectPtr<UObject> WrongClass(NewObject<UDasIntDelegateProxy>());
    First->Target = WrongClass.Get();
    Obj = ObjectRun(TEXT("ue_call_void_object(ue_self_object(), \"SetFixtureTarget\", ue_get_object(ue_self_object(), \"Target\"))\nreturn 0"));
    Expect(!Obj.bSucceeded && Obj.Error.Contains(TEXT("incompatible class")) && First->Target == WrongClass.Get(),
        TEXT("object_argument_class_rejected_before_call"));
    First->Target = nullptr;
    Obj = ObjectRun(TEXT("let obj = ue_call_object(ue_self_object(), \"GetTargetObject\")\nverify(!ue_object_valid(obj))\nue_call_void_object(ue_self_object(), \"SetFixtureTarget\", obj)\nreturn 1"));
    Expect(Obj.bSucceeded && Obj.Value == 1 && !First->Target, TEXT("object_null_return_and_argument"), Obj.Error);
    First->Target = ObjectTarget.Get();
    const FString ObjectDelegate = TEXT("options gen2\nrequire ue_foundation\n[export]\ndef bind_events() { ue_subscribe_object_int(ue_get_object(ue_self_object(), \"Target\"), \"OnDasSignal\", \"on_value\") }\n[export]\ndef on_value(value:int) { ue_state_set_int(\"sum\", ue_state_get_int(\"sum\", 0) + value) }\n[export]\ndef main(input:int):int { return ue_state_get_int(\"sum\", 0) }\n");
    Expect(Write(ObjectDelegate), TEXT("object_delegate_fixture"));
    Obj = Persistent(First.Get()); ObjectTarget->OnDasSignal.Broadcast(7);
    Expect(Obj.bSucceeded && Persistent(First.Get()).Value == 7 && Runtime.SubscriptionCount(First.Get()) == 1,
        TEXT("object_arbitrary_source_subscription"), Obj.Error);
    const int32 BeforeCandidate = ObjectTarget->health;
    Expect(Write(ObjectDelegate.Replace(TEXT("ue_subscribe_object_int(ue_get_object(ue_self_object(), \"Target\"), \"OnDasSignal\", \"on_value\")"),
        TEXT("ue_call_void_int(ue_get_object(ue_self_object(), \"Target\"), \"SetHealthValue\", 999)"))), TEXT("object_call_in_candidate_fixture"));
    Obj = Persistent(First.Get());
    Expect(Obj.bSucceeded && !Obj.ReloadError.IsEmpty() && ObjectTarget->health == BeforeCandidate && Runtime.SubscriptionCount(First.Get()) == 1,
        TEXT("object_candidate_cannot_call_UFunction"));
    Runtime.ReleaseObject(First.Get());
    const FString CachedHandle = TEXT("options gen2\nrequire ue_foundation\nvar cached = ue_null_object()\n[export]\ndef main(input:int):int { if (input == 0) { cached = ue_get_object(ue_self_object(), \"Target\"); return 1 }; if (input == 1) { if (ue_object_valid(cached)) { return 1 }; return 0 }; return ue_object_get_int(cached, \"health\") }\n");
    Expect(Write(CachedHandle), TEXT("object_cached_handle_fixture"));
    Obj = Persistent(First.Get(), 0);
    Expect(Obj.bSucceeded && Persistent(First.Get(), 1).Value == 1, TEXT("object_handle_survives_repeated_calls"), Obj.Error);
    TWeakObjectPtr<UObject> WeakTarget(ObjectTarget.Get());
    First->Target = nullptr; ObjectTarget.Reset(); CollectGarbage(RF_NoFlags);
    Obj = Persistent(First.Get(), 1);
    const auto ExpiredObject = Persistent(First.Get(), 2);
    Expect(!WeakTarget.IsValid() && Obj.bSucceeded && Obj.Value == 0 && !ExpiredObject.bSucceeded && ExpiredObject.Error.Contains(TEXT("expired")),
        TEXT("object_cached_handle_is_weak_and_expires_after_GC"), ExpiredObject.Error);
    Runtime.ReleaseObject(First.Get());
    const auto Marshalled = Runtime.RunFile(Runtime.ScriptPath(TEXT("marshalling.das")), 5, false, First.Get());
    Expect(Marshalled.bSucceeded && Marshalled.Value == 12 && First->MarshalledCalls == 1, TEXT("marshal_multiarg_constref_struct_unicode"), Marshalled.Error);
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_string(c, \"Label\", \"ok\")\nue_arg_bool(c, \"bAccept\", true)\nue_invoke_call(c)\nverify(ue_result_bool(c, \"ReturnValue\"))\nue_release_call(c)\nreturn 1"));
        (void)Before;
        Expect(M.bSucceeded && M.Value == 1, TEXT("marshal_bool_return"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_string(c, \"Label\", \"ok\")\nue_invoke_call(c)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("missing input")) && First->MarshalledCalls == Before, TEXT("marshal_missing_input"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_int(c, \"Label\", 7)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before, TEXT("marshal_wrong_type"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_bool(c, \"Missing\", true)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("unknown parameter")) && First->MarshalledCalls == Before, TEXT("marshal_extra_input"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nverify(ue_result_bool(c, \"ReturnValue\"))\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("executed call")) && First->MarshalledCalls == Before, TEXT("marshal_early_result"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"OutValue\")\nue_invoke_call(c)\nverify(ue_result_int(c, \"Value\") == ue_get_self_int(\"health\"))\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before, TEXT("marshal_out_ref"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"GetTargetObject\")\nue_invoke_call(c)\nverify(!ue_object_valid(ue_result_object(c, \"ReturnValue\")))\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before, TEXT("marshal_null_object_result"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_release_call(c)\nue_invoke_call(c)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("released")) && First->MarshalledCalls == Before, TEXT("marshal_released"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformPayload\")\nue_arg_int(c, \"Payload.Count\", 7)\nue_invoke_call(c)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("missing input")) && First->MarshalledCalls == Before, TEXT("marshal_partial_struct"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("for (i in range(600)) {\n let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\n ue_arg_string(c, \"Label\", \"строка\")\n ue_arg_bool(c, \"bAccept\", true)\n ue_invoke_call(c)\n ue_release_call(c)\n}\nreturn 1"));
        (void)Before;
        Expect(M.bSucceeded && M.Value == 1, TEXT("marshal_repeat_release"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("for (i in range(257)) { let c = ue_begin_call(ue_self_object(), \"AcceptLabel\") }\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("frame limit")) && First->MarshalledCalls == Before, TEXT("marshal_budget"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_string(c, \"Label\", \"ok\")\nue_arg_bool(c, \"bAccept\", true)\nue_invoke_call(c)\nue_invoke_call(c)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("execute once")) && First->MarshalledCalls == Before + 1, TEXT("marshal_double_invoke"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_string(c, \"Label\", \"\")\nue_arg_bool(c, \"bAccept\", false)\nue_invoke_call(c)\nreturn ue_result_int(c, \"ReturnValue\")"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before + 1, TEXT("marshal_wrong_result"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nlet copy = c\nue_release_call(c)\nue_invoke_call(copy)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("released")) && First->MarshalledCalls == Before + 0, TEXT("marshal_copied_handle_revoked"), M.Error);
    }
    Expect(Write(ObjectDelegate.Replace(TEXT("ue_subscribe_object_int(ue_get_object(ue_self_object(), \"Target\"), \"OnDasSignal\", \"on_value\")"),
        TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")"))), TEXT("marshal_candidate_fixture"));
    const auto CandidateCall = Persistent(First.Get());
    Expect(!CandidateCall.bSucceeded && CandidateCall.Error.Contains(TEXT("forbidden in bind_events")), TEXT("marshal_candidate_cannot_prepare_calls"), CandidateCall.Error);
    Runtime.ReleaseObject(First.Get());
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"MutateValues\")\nue_arg_int(c, \"Count\", 7)\nue_arg_string(c, \"Text\", \"Привет\")\nue_invoke_call(c)\nverify(ue_result_int(c, \"Count\") == 10)\nverify(ue_result_int(c, \"ReturnValue\") == 20)\nverify(ue_result_bool(c, \"Accepted\"))\nlet description = ue_result_string(c, \"Description\")\nverify(ue_result_string(c, \"Text\") == description)\nue_release_call(c)\nverify(description == \"Привет 世界\")\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("inout_scalar_inout"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeObject\")\nue_arg_object(c, \"Input\", ue_self_object())\nue_arg_object(c, \"InOut\", ue_null_object())\nue_invoke_call(c)\nverify(ue_object_valid(ue_result_object(c, \"InOut\")))\nverify(ue_object_valid(ue_result_object(c, \"ReturnValue\")))\nverify(!ue_object_valid(ue_result_object(c, \"Output\")))\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("inout_object_inout"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"MutateValues\"); ue_arg_int(c, \"Count\", 1); ue_invoke_call(c); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("missing input")) && First->MarshalledCalls == Before + 0, TEXT("inout_inout_missing"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"OutValue\"); ue_arg_int(c, \"Value\", 1); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("direction")) && First->MarshalledCalls == Before + 0, TEXT("inout_out_cannot_set"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\"); ue_arg_string(c, \"Label\", \"ok\"); ue_arg_bool(c, \"bAccept\", true); ue_invoke_call(c); let text = ue_result_string(c, \"Label\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("direction")) && First->MarshalledCalls == Before + 1, TEXT("inout_constref_not_output"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"SetFixtureTarget\"); ue_arg_int(c, \"Value\", 1); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before + 0, TEXT("inout_object_wrong_scalar"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let made = ue_begin_call(ue_self_object(), \"NewFixture\")\nue_invoke_call(made)\nlet obj = ue_result_object(made, \"ReturnValue\")\nlet gc1 = ue_begin_call(ue_self_object(), \"ForceTestGC\"); ue_invoke_call(gc1); ue_release_call(gc1)\nverify(ue_object_valid(obj))\nlet next = ue_begin_call(ue_self_object(), \"ExchangeObject\")\nue_arg_object(next, \"Input\", obj)\nue_arg_object(next, \"InOut\", ue_null_object())\nue_release_call(made)\n{ let gc2 = ue_begin_call(ue_self_object(), \"ForceTestGC\"); ue_invoke_call(gc2); ue_release_call(gc2) }\nverify(ue_object_valid(obj))\nue_invoke_call(next)\nverify(ue_object_valid(ue_result_object(next, \"InOut\")))\nue_release_call(next)\n{ let gc3 = ue_begin_call(ue_self_object(), \"ForceTestGC\"); ue_invoke_call(gc3); ue_release_call(gc3) }\nverify(!ue_object_valid(obj))\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 2, TEXT("inout_pins_during_gc"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let made = ue_begin_call(ue_self_object(), \"NewFixture\")\nue_invoke_call(made)\nlet obj = ue_result_object(made, \"ReturnValue\")\nue_release_call(made)\nlet gc4 = ue_begin_call(ue_self_object(), \"ForceTestGC\"); ue_invoke_call(gc4); ue_release_call(gc4)\nlet next = ue_begin_call(ue_self_object(), \"SetFixtureTarget\")\nue_arg_object(next, \"Value\", obj)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("expired")) && First->MarshalledCalls == Before + 1, TEXT("inout_expired_argument"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"SetFixtureTarget\"); ue_arg_object(c, \"Value\", ue_null_object()); ue_invoke_call(c); return 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 0, TEXT("inout_null_argument"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"MutatePayload\")\nue_arg_int(c, \"Payload.Count\", 2)\nue_arg_string(c, \"Payload.Label\", \"ok\")\nue_arg_bool(c, \"Payload.bEnabled\", true)\nue_arg_double(c, \"Payload.Weight\", double(2))\nue_arg_float(c, \"Payload.Metrics.Ratio\", 3.0)\nue_invoke_call(c)\nverify(ue_result_int(c, \"Payload.Count\") == 6)\nverify(ue_result_int(c, \"Copy.Count\") == 6)\nverify(ue_result_string(c, \"Copy.Label\") == \"ok!\")\nverify(ue_result_float(c, \"Copy.Metrics.Ratio\") == 3.0)\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("inout_struct_inout"), M.Error);
    }
    First->Target = WrongClass.Get();
    const auto WrongObjectInput = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"SetFixtureTarget\"); ue_arg_object(c, \"Value\", ue_get_object(ue_self_object(), \"Target\")); return 1"));
    Expect(!WrongObjectInput.bSucceeded && WrongObjectInput.Error.Contains(TEXT("incompatible class")) && First->Target == WrongClass.Get(), TEXT("inout_object_class_checked_before_call"), WrongObjectInput.Error);
    First->Target = nullptr;
    const auto InOutExample = Runtime.RunFile(Runtime.ScriptPath(TEXT("inout_values.das")), 0, false, First.Get());
    Expect(InOutExample.bSucceeded && InOutExample.Value == 1, TEXT("inout_shipped_example"), InOutExample.Error);
    Expect(Write(TEXT("options gen2\nrequire ue_foundation\nvar cached = ue_null_object()\n[export]\ndef main(input:int):int {\n    if (input == 0) {\n        let c = ue_begin_call(ue_self_object(), \"NewFixture\")\n        ue_invoke_call(c)\n        cached = ue_result_object(c, \"ReturnValue\")\n        return 1\n    }\n    if (ue_object_valid(cached)) { return 1 }\n    return 0\n}\n")), TEXT("inout_automatic_cleanup_fixture"));
    const auto BeforeAutomaticCleanup = Persistent(First.Get(), 0);
    CollectGarbage(RF_NoFlags);
    const auto AfterAutomaticCleanup = Persistent(First.Get(), 1);
    Expect(BeforeAutomaticCleanup.bSucceeded && AfterAutomaticCleanup.bSucceeded && AfterAutomaticCleanup.Value == 0,
        TEXT("inout_frame_pins_released_at_callback_exit"), AfterAutomaticCleanup.Error);
    Runtime.ReleaseObject(First.Get());
    const auto EnumNameExample = Runtime.RunFile(Runtime.ScriptPath(TEXT("enum_names.das")), 0, false, First.Get());
    Expect(EnumNameExample.bSucceeded && EnumNameExample.Value == 1, TEXT("enum_name_example_inputs_returns_out_inout"), EnumNameExample.Error);
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoLegacy\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasLegacyMode\", \"DasLegacyActive\"); ue_invoke_call(c); verify(ue_result_enum(c, \"ReturnValue\", \"/Script/DasRuntime.EDasLegacyMode\") == \"DasLegacyActive\"); return 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("enum_name_legacy"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeNamedMode\")\nue_arg_enum(c, \"Value.Mode\", \"/Script/DasRuntime.EDasCallMode\", \"Paused\")\nue_arg_name(c, \"Value.Key\", \"Nested_23\")\nue_invoke_call(c)\nverify(ue_result_enum(c, \"Copy.Mode\", \"/Script/DasRuntime.EDasCallMode\") == \"Active\")\nverify(ue_result_name(c, \"Copy.Key\") == \"Nested_23\")\nverify(ue_result_enum(c, \"Value.Mode\", \"/Script/DasRuntime.EDasCallMode\") == \"Active\")\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("enum_name_struct"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EqualNames\"); ue_arg_name(c, \"A\", \"MiXeD_17\"); ue_arg_name(c, \"B\", \"mixed_17\"); ue_invoke_call(c); verify(ue_result_bool(c, \"ReturnValue\")); return 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("enum_name_name_case"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EqualNames\"); ue_arg_name(c, \"A\", \"\"); ue_arg_name(c, \"B\", \"None\"); ue_invoke_call(c); verify(ue_result_bool(c, \"ReturnValue\")); return 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("enum_name_name_none"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasLegacyMode\", \"DasLegacyIdle\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type path mismatch")) && First->MarshalledCalls == Before + 0, TEXT("enum_name_wrong_enum_type"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasCallMode\", \"Unknown\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("unknown or reserved")) && First->MarshalledCalls == Before + 0, TEXT("enum_name_missing_enum_member"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasCallMode\", \"7\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("unknown or reserved")) && First->MarshalledCalls == Before + 0, TEXT("enum_name_numeric_enum_text"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasCallMode\", \"EDasCallMode_MAX\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("unknown or reserved")) && First->MarshalledCalls == Before + 0, TEXT("enum_name_enum_max"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_int(c, \"Input\", 7); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before + 0, TEXT("enum_name_enum_as_int"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeName\"); ue_arg_string(c, \"Input\", \"Wrong\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before + 0, TEXT("enum_name_string_as_name"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\"); ue_arg_name(c, \"Label\", \"Wrong\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before + 0, TEXT("enum_name_name_as_string"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EqualNames\"); ue_arg_name(c, \"A\", \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("too long")) && First->MarshalledCalls == Before + 0, TEXT("enum_name_long_name"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"InvalidMode\"); ue_invoke_call(c); let value = ue_result_enum(c, \"ReturnValue\", \"/Script/DasRuntime.EDasCallMode\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("unknown or reserved enum value")) && First->MarshalledCalls == Before + 1, TEXT("enum_name_invalid_enum_result"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"InvalidMode\"); ue_invoke_call(c); let value = ue_result_enum(c, \"ReturnValue\", \"/Script/DasRuntime.EDasLegacyMode\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type path mismatch")) && First->MarshalledCalls == Before + 1, TEXT("enum_name_wrong_enum_result_type"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasCallMode\", \"Idle\"); ue_invoke_call(c); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("missing input")) && First->MarshalledCalls == Before + 0, TEXT("enum_name_enum_inout_required"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeName\"); ue_arg_name(c, \"Previous\", \"Wrong\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("direction")) && First->MarshalledCalls == Before + 0, TEXT("enum_name_name_out_direction"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasCallMode\", \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("enum member is too long")) && First->MarshalledCalls == Before, TEXT("enum_name_long_enum_member"), M.Error);
    }
    const auto Generated = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_calls.das")), 0, false, First.Get());
    Expect(Generated.bSucceeded && Generated.Value == 10, TEXT("generated_wrappers_execute_typed_inputs_results_structs"), Generated.Error);
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_bad_name.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: actor_ActorHasTag")), TEXT("generated_bad_name"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_bad_enum.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: fixture_ExchangeMode")), TEXT("generated_bad_enum"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_wrong_enum.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: fixture_ExchangeMode")), TEXT("generated_wrong_enum"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_bad_receiver.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("requested class")), TEXT("generated_bad_receiver"), G.Error); }
    const int32 GuardHealth = First->health;
    const auto StaleBinding = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AddHealth\"); ue_expect_signature(c, \"old signature\"); ue_arg_int(c, \"Value\", 999); ue_invoke_call(c); return 1"));
    Expect(!StaleBinding.bSucceeded && StaleBinding.Error.Contains(TEXT("regenerate bindings")) && First->health == GuardHealth,
        TEXT("generated_signature_guard_prevents_side_effects"), StaleBinding.Error);
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_bad_struct.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: fixture_TransformPayload")), TEXT("generated_bad_struct"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_fake_vector.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: actor_SetActorRelativeScale3D")), TEXT("generated_fake_vector"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_flat_struct.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: fixture_TransformPayload")), TEXT("generated_flat_struct"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_roundtrip.das")), 0, false, First.Get());
      Expect(G.bSucceeded && G.Value == 1, TEXT("class_roundtrip"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_bad_static_receiver.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: actor_GetOwner")), TEXT("class_bad_static_receiver"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_bad_static_argument.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: fixture_SetFixtureTarget")), TEXT("class_bad_static_argument"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_raw_receiver.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: actor_GetOwner")), TEXT("class_raw_receiver"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_forged_receiver.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("requested class")), TEXT("class_forged_receiver"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_null_receiver.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("null")), TEXT("class_null_receiver"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_weak_gc.das")), 0, false, First.Get());
      Expect(G.bSucceeded && G.Value == 1, TEXT("class_weak_gc"), G.Error); }
    { const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_expired_cast.das")), 0, false, First.Get());
      Expect(!G.bSucceeded && G.Error.Contains(TEXT("expired")), TEXT("class_expired_cast"), G.Error); }
    Runtime.ReleaseObject(First.Get());
    {
        const FString Path = Runtime.ScriptPath(TEXT("class_persistent.das"));
        const auto Created = Runtime.RunPersistent(Path, 0, First.Get());
        Expect(Created.bSucceeded && Created.Value == 1, TEXT("class_persistent_nullable_global_and_factory"), Created.Error);
        CollectGarbage(RF_NoFlags);
        const auto TypedExpired = Runtime.RunPersistent(Path, 1, First.Get());
        Expect(TypedExpired.bSucceeded && TypedExpired.Value == 0, TEXT("class_persistent_weak_after_callback"), TypedExpired.Error);
        const auto Rejected = Runtime.RunPersistent(Path, 2, First.Get());
        Expect(!Rejected.bSucceeded && Rejected.Error.Contains(TEXT("expired")), TEXT("class_persistent_expired_call_rejected"), Rejected.Error);
    }
    Runtime.ReleaseObject(First.Get());
    {
        const int32 BeforeDefaults = First->MarshalledCalls;
        const auto Defaults = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_defaults.das")), 0, false, First.Get());
        Expect(Defaults.bSucceeded && Defaults.Value == 1 && First->MarshalledCalls == BeforeDefaults + 4, TEXT("generated_defaults_omitted_overridden_and_repeated"), Defaults.Error);
        const auto BadDefault = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_defaults_bad_type.das")), 0, false, First.Get());
        Expect(!BadDefault.bSucceeded && BadDefault.Error.Contains(TEXT("no matching functions or generics: fixture_PolicyDefault")), TEXT("generated_defaults_explicit_type_rejected"), BadDefault.Error);
    }
    {
        const int32 BeforeArrays = First->MarshalledCalls;
        const auto Arrays = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_arrays.das")), 0, false, First.Get());
        Expect(Arrays.bSucceeded && Arrays.Value == 1 && First->MarshalledCalls == BeforeArrays + 8, TEXT("generated_arrays_roundtrip_inout_empty_and_reuse"), Arrays.Error);
        const auto BadArray = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_arrays_bad_type.das")), 0, false, First.Get());
        Expect(!BadArray.bSucceeded && BadArray.Error.Contains(TEXT("no matching functions or generics: fixture_TransformInts")), TEXT("generated_arrays_element_type_rejected"), BadArray.Error);
    }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 2); ue_arg_int(c, \"Values.0\", 1); ue_invoke_call(c); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("missing array element")) && First->MarshalledCalls == BeforeArrayFailure, TEXT("array_missing_before_invocation"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.1\", 1); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("out of bounds")) && First->MarshalledCalls == BeforeArrayFailure, TEXT("array_bounds_before_invocation"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.-1\", 1); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("invalid array index")) && First->MarshalledCalls == BeforeArrayFailure, TEXT("array_negative_index_before_invocation"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 65537); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeArrayFailure, TEXT("array_limit_before_invocation"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", -1); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeArrayFailure, TEXT("array_negative_size_before_invocation"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_int(c, \"Values.0\", 1); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("initialize array")) && First->MarshalledCalls == BeforeArrayFailure, TEXT("array_uninitialized_before_invocation"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 1); ue_arg_string(c, \"Values.0\", \"bad\"); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == BeforeArrayFailure, TEXT("array_type_before_invocation"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.0\", 1); ue_arg_array(c, \"Values\", 1); ue_invoke_call(c); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("missing array element")) && First->MarshalledCalls == BeforeArrayFailure, TEXT("array_reset_before_invocation"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"ReturnValue\", 0); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("direction")) && First->MarshalledCalls == BeforeArrayFailure, TEXT("array_out_direction_before_invocation"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.4294967296\", 1); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("invalid array index")) && First->MarshalledCalls == BeforeArrayFailure + 0, TEXT("array_index_overflow"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 0); ue_invoke_call(c); ue_result_int(c, \"ReturnValue.0\"); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("out of bounds")) && First->MarshalledCalls == BeforeArrayFailure + 1, TEXT("array_result_bounds"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"OversizedArray\"); ue_invoke_call(c); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeArrayFailure + 1, TEXT("array_result_limit"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_result_array_size(c, \"ReturnValue\"); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("results require an executed call")) && First->MarshalledCalls == BeforeArrayFailure + 0, TEXT("array_result_before_call"), A.Error); }
    { const int32 BeforeArrayFailure = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 0); ue_invoke_call(c); ue_release_call(c); ue_result_array_size(c, \"ReturnValue\"); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("released")) && First->MarshalledCalls == BeforeArrayFailure + 1, TEXT("array_after_release"), A.Error); }
    { const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 65536); ue_arg_array(c, \"Values\", 0); ue_invoke_call(c); verify(ue_result_array_size(c, \"ReturnValue\") == 0); return 1"));
      Expect(A.bSucceeded && A.Value == 1, TEXT("array_exact_limit_then_empty_reset"), A.Error); }
    { const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_complex_arrays.das")), 0, false, First.Get());
      Expect(A.bSucceeded && A.Value == 1, TEXT("generated_complex_arrays"), A.Error); }
    { const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_complex_arrays_bad_object.das")), 0, false, First.Get());
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("no matching functions or generics: fixture_EchoActors")), TEXT("generated_complex_arrays_bad_object"), A.Error); }
    { const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_complex_arrays_bad_struct.das")), 0, false, First.Get());
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("no matching functions or generics: fixture_TransformPayloads")), TEXT("generated_complex_arrays_bad_struct"), A.Error); }
    { const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_complex_arrays_forged_object.das")), 0, false, First.Get());
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("requested class")), TEXT("generated_complex_arrays_forged_object"), A.Error); }
    { const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("array_gc_output_pins.das")), 0, false, First.Get());
      Expect(A.bSucceeded && A.Value == 1, TEXT("array_gc_output_pins"), A.Error); }
    { const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("array_gc_reset_releases_input.das")), 0, false, First.Get());
      Expect(A.bSucceeded && A.Value == 1, TEXT("array_gc_reset_releases_input"), A.Error); }
    { const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("array_gc_inout_replaces_pins.das")), 0, false, First.Get());
      Expect(A.bSucceeded && A.Value == 1, TEXT("array_gc_inout_replaces_pins"), A.Error); }
    { const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("array_gc_vm_boundary.das")), 0, false, First.Get());
      Expect(A.bSucceeded && A.Value == 1, TEXT("array_gc_vm_boundary"), A.Error); }
    { const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("array_gc_exception_fixture.das")), 0, false, First.Get());
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("array pin cleanup fixture")), TEXT("array_gc_exception_fixture"), A.Error); }
    { const int32 BeforeComplexArray = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformPayloads\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.0.Count\", 1); ue_invoke_call(c); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("missing array element")) && First->MarshalledCalls == BeforeComplexArray, TEXT("complex_array_struct_incomplete"), A.Error); }
    { const int32 BeforeComplexArray = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformPayloads\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.0.Metrics.Ratio\", 1); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == BeforeComplexArray, TEXT("complex_array_struct_wrong_leaf"), A.Error); }
    { const int32 BeforeComplexArray = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformPayloads\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.1.Count\", 1); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("out of bounds")) && First->MarshalledCalls == BeforeComplexArray, TEXT("complex_array_struct_bounds"), A.Error); }
    { const int32 BeforeComplexArray = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoActors\"); ue_arg_array(c, \"Values\", 1); ue_arg_object(c, \"Values.0\", ue_self_object()); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("incompatible class")) && First->MarshalledCalls == BeforeComplexArray, TEXT("complex_array_object_class"), A.Error); }
    { const int32 BeforeComplexArray = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoFixtureObjects\"); ue_arg_array(c, \"Values\", 1); ue_invoke_call(c); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("missing array element")) && First->MarshalledCalls == BeforeComplexArray, TEXT("complex_array_object_missing"), A.Error); }
    { const int32 BeforeComplexArray = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoModes\"); ue_arg_array(c, \"Values\", 1); ue_arg_enum(c, \"Values.0\", \"/Script/Engine.ETickingGroup\", \"TG_PrePhysics\"); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("type path mismatch")) && First->MarshalledCalls == BeforeComplexArray, TEXT("complex_array_enum_type"), A.Error); }
    { const int32 BeforeComplexArray = First->MarshalledCalls;
      const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoModes\"); ue_arg_array(c, \"Values\", 1); ue_arg_enum(c, \"Values.0\", \"/Script/DasRuntime.EDasCallMode\", \"Unknown\"); return 1"));
      Expect(!A.bSucceeded && A.Error.Contains(TEXT("unknown or reserved")) && First->MarshalledCalls == BeforeComplexArray, TEXT("complex_array_enum_symbol"), A.Error); }
    CollectGarbage(RF_NoFlags);
    Expect(!First->LastArrayCreated.IsValid(), TEXT("array_output_pins_released_after_VM_exception"));
    {
        const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_complex_arrays_expired_object.das")), 0, false, First.Get());
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("expired")), TEXT("complex_array_expired_object_rejected"), A.Error);
    }
    Runtime.ReleaseObject(First.Get());
    {
        const FString ArrayPath = Runtime.ScriptPath(TEXT("array_gc_persistent.das"));
        const auto CreatedArray = Runtime.RunPersistent(ArrayPath, 0, First.Get());
        Expect(CreatedArray.bSucceeded && CreatedArray.Value == 3, TEXT("object_array_persistent_global_created"), CreatedArray.Error);
        CollectGarbage(RF_NoFlags);
        const auto ExpiredArray = Runtime.RunPersistent(ArrayPath, 1, First.Get());
        Expect(ExpiredArray.bSucceeded && ExpiredArray.Value == 0, TEXT("object_array_persistent_global_is_weak"), ExpiredArray.Error);
        const auto RejectedArray = Runtime.RunPersistent(ArrayPath, 2, First.Get());
        Expect(!RejectedArray.bSucceeded && RejectedArray.Error.Contains(TEXT("expired")), TEXT("object_array_persistent_expired_input_rejected"), RejectedArray.Error);
    }
    Runtime.ReleaseObject(First.Get());
    { const auto H = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_hash_containers.das")), 0, false, First.Get());
      Expect(H.bSucceeded && H.Value == 1, TEXT("generated_hash_containers"), H.Error); }
    { const auto H = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_hash_bad_type.das")), 0, false, First.Get());
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("no matching functions or generics: fixture_TransformMap")), TEXT("generated_hash_bad_type"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"Values\", 2); ue_arg_int(c, \"Values.0.key\", 1); ue_arg_string(c, \"Values.0.value\", \"a\"); ue_arg_int(c, \"Values.1.key\", 1); ue_arg_string(c, \"Values.1.value\", \"b\"); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == BeforeHash + 0, TEXT("map_duplicate"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.0.key\", 1); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("missing map")) && First->MarshalledCalls == BeforeHash + 0, TEXT("map_missing_value"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 2); ue_arg_int(c, \"Values.0\", 1); ue_arg_int(c, \"Values.1\", 1); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == BeforeHash + 0, TEXT("set_duplicate"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 1); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("missing map")) && First->MarshalledCalls == BeforeHash + 0, TEXT("set_missing"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_int(c, \"Values.0.key\", 1); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("initialize hash")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_uninitialized"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.1\", 1); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("out of bounds")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_bounds"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.-1\", 1); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("invalid hash index")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_negative_index"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.4294967296\", 1); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("invalid hash index")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_overflow_index"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"Values\", 65537); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_limit"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", -1); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_negative_size"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"Values\", 1); ue_arg_string(c, \"Values.0.key\", \"bad\"); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_wrong_type"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.0\", 1); ue_arg_hash(c, \"Values\", 1); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("missing map")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_reset"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"ReturnValue\", 0); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("direction")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_direction"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_result_hash_size(c, \"ReturnValue\"); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("results require an executed call")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_early_result"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"MakeMap\"); ue_invoke_call(c); ue_release_call(c); ue_result_hash_size(c, \"Values\"); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("released")) && First->MarshalledCalls == BeforeHash + 1, TEXT("hash_released"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"OversizedMap\"); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeHash + 1, TEXT("hash_map_result_limit"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"OversizedSet\"); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeHash + 1, TEXT("hash_set_result_limit"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"MakeMap\"); ue_invoke_call(c); ue_result_int(c, \"Values.1.key\"); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("out of bounds")) && First->MarshalledCalls == BeforeHash + 1, TEXT("hash_result_bounds"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoNameSet\"); ue_arg_hash(c, \"Values\", 2); ue_arg_name(c, \"Values.0\", \"Mixed_17\"); ue_arg_name(c, \"Values.1\", \"mixed_17\"); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_name_duplicate"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoNameModeMap\"); ue_arg_hash(c, \"Values\", 2); ue_arg_name(c, \"Values.0.key\", \"Mixed_17\"); ue_arg_name(c, \"Values.1.key\", \"mixed_17\"); ue_arg_enum(c, \"Values.0.value\", \"/Script/DasRuntime.EDasCallMode\", \"Active\"); ue_arg_enum(c, \"Values.1.value\", \"/Script/DasRuntime.EDasCallMode\", \"Paused\"); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == BeforeHash + 0, TEXT("hash_name_map_duplicate"), H.Error); }
    { const auto H = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_hash_numbers.das")), 0, false, First.Get());
      Expect(H.bSucceeded && H.Value == 1, TEXT("generated_hash_numbers_finite_and_precision"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoFloatKeyMap\"); ue_arg_hash(c, \"Values\", 2); ue_arg_float(c, \"Values.0.key\", 0.0); ue_arg_bool(c, \"Values.0.value\", true); ue_arg_float(c, \"Values.1.key\", -0.0); ue_arg_bool(c, \"Values.1.value\", false); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == BeforeHash, TEXT("hash_signed_zero_duplicate"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoFloatSet\"); ue_arg_hash(c, \"Values\", 1); ue_arg_float(c, \"Values.0\", unsafe(reinterpret<float>(0x7FC00000u))); ue_invoke_call(c); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("must be finite")) && First->MarshalledCalls == BeforeHash, TEXT("hash_nan_key_rejected"), H.Error); }
    { const int32 BeforeHash = First->MarshalledCalls; const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.0\", 1); return 1"));
      Expect(!H.bSucceeded && H.Error.Contains(TEXT("requires key or value")) && First->MarshalledCalls == BeforeHash, TEXT("hash_map_suffix_required"), H.Error); }
    { const auto H = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_hash_failed_result.das")), 0, false, First.Get());
      Expect(H.bSucceeded && H.Value == 1, TEXT("hash_failed_results_blocked_after_recover"), H.Error); }
    TWeakObjectPtr<UObject> DeadOwner(Second.Get());
    First.Reset(); Second.Reset(); CollectGarbage(RF_NoFlags);
    Expect(!DeadOwner.IsValid() && Runtime.PersistentContextCount() == 0 && Runtime.SubscriptionCount() == 0, TEXT("persistent_GC_reclaims_contexts_without_pinning"));
    Expect(!Persistent(DeadOwner.Get()).bSucceeded, TEXT("persistent_dead_owner_rejected"));
    IFileManager::Get().Delete(*Scratch);

    const auto Recovery = Run(TEXT("smoke.das"));
    Expect(Recovery.bSucceeded && Recovery.Value == 42, TEXT("recovery_after_failures"), Recovery.Error);
    Report += FString::Printf(TEXT("DAS_FOUNDATION_RESULT=%s failures=%d\n"), Failures ? TEXT("FAIL") : TEXT("PASS"), Failures);
    return Failures == 0;
}
