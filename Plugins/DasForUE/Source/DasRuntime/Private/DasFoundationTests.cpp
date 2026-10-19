#include "DasCallbackFrames.h"
#include "DasCallbackTypes.h"
#include "DasRuntime.h"
#include "DasPropertyTypes.h"
#include "DasSoftReferences.h"
#include "DasDefaultValueProbe.h"
#include "DasSnapshotFields.h"
#include "DasReflectedTypes.h"
#include "DasNumericLibrary.h"
#include "UObject/StructOnScope.h"
#include <limits>
#include "DasSchema.h"
#include "DasAuthoring.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "DasObjectFixture.h"
#include "DasIntDelegateProxy.h"
#include "UObject/StrongObjectPtr.h"
#include "Modules/ModuleManager.h"
#include "Engine/World.h"

FString DasFoundationExternManifest();

bool RunDasFoundationTests(FString& Report)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Failures = 0;
    auto Expect = [&](bool Condition, const TCHAR* Name, const FString& Detail = FString())
    {
        Report += FString::Printf(TEXT("%s %s %s\n"), Condition ? TEXT("PASS") : TEXT("FAIL"), Name, *Detail);
        if (!Condition)
            ++Failures;
    };
    FString PropertyTypeError;
    const bool PropertyTypesOK = CheckDasPropertyTypes(PropertyTypeError);
    Expect(PropertyTypesOK, TEXT("shared_type_descriptor_and_property_factory"), PropertyTypeError);
    FString SoftError;
    const bool SoftOK = CheckDasSoftReferences(SoftError);
    Expect(SoftOK, TEXT("soft_reference_no_load_roundtrip_defaults_and_GC"), SoftError);
    FString CallbackError;
    const bool CallbacksOK = CheckDasCallbackTypes(CallbackError) && CheckDasCallbackFrames(CallbackError);
    Expect(CallbacksOK, TEXT("shared_callback_capabilities_and_payload_validation"), CallbackError);
#if WITH_EDITOR
    {
        // Script-visible extern surface; the baseline is updated deliberately when the API changes.
        const FString Manifest = DasFoundationExternManifest();
        const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
        const FString Baseline = Repo / TEXT("Bindings/FoundationExterns.txt");
        if (IFileManager::Get().DirectoryExists(*(Repo / TEXT("logs"))))
            FFileHelper::SaveStringToFile(Manifest, *(Repo / TEXT("logs/foundation-externs.txt")),
                                                      FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        FString Expected;
        if (FFileHelper::LoadFileToString(Expected, *Baseline))
        {
            Expected.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
            Expect(Expected == Manifest, TEXT("foundation_extern_manifest_matches_baseline"),
                                              TEXT("diff logs/foundation-externs.txt Bindings/FoundationExterns.txt"));
        }
    }
#endif
#if WITH_EDITOR
    {
        // Compile-time DSL checks: method references and native ue_event delegates.
        auto Authoring = [&](const TCHAR* Name, FString& Error)
        {
            FDasAuthoringModule Model;
            return Runtime.ReadAuthoringScript(Runtime.ScriptPath(Name), Model, Error);
        };
        FString RefError, BadRefError, UnknownError, ArityError;
        Expect(Authoring(TEXT("dsl_method_refs.das"), RefError), TEXT("dsl_method_references_compile"), RefError);
        Expect(!Authoring(TEXT("dsl_method_refs_bad.das"), BadRefError) && !BadRefError.IsEmpty(),
                          TEXT("dsl_method_reference_to_field_rejected"));
        Expect(!Authoring(TEXT("dsl_event_unknown.das"), UnknownError) &&
                          UnknownError.Contains(TEXT("multicast delegate OnTakeAnyDamagee")),
                                                TEXT("dsl_unknown_native_event_rejected_at_compile_time"),
                                                     UnknownError);
        Expect(!Authoring(TEXT("dsl_event_arity.das"), ArityError) &&
                          ArityError.Contains(TEXT("with 1 parameter")),
                                              TEXT("dsl_native_event_arity_rejected_at_compile_time"), ArityError);
    }
#endif
    auto Run = [&](const TCHAR* Name)
    {
        return Runtime.RunFile(Runtime.ScriptPath(Name), 40);
    };
    const auto Good = Run(TEXT("smoke.das"));
    Expect(Good.bSucceeded && Good.Value == 42, TEXT("native_roundtrip"), Good.Error);
    const auto SoftSmoke = Run(TEXT("soft_references_smoke.das"));
    Expect(SoftSmoke.bSucceeded && SoftSmoke.Value == 42,
           TEXT("soft_source_aliases_default_path_and_resolve_without_load"), SoftSmoke.Error);
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
    Expect(Schema1.bSucceeded && Schema1.Types.Num() == 1 &&
           Schema1.Types[0].Name == TEXT("ProbeActor") && Schema1.Types[0].Fields == TEXT("speed:float;"),
                                                                                          TEXT("macro_schema_v1"),
                                                                                               Schema1.Error);
    const auto Schema2 = Run(TEXT("schema_v2.das"));
    Expect(Schema2.bSucceeded && Schema2.Types.Num() == 1 &&
           Schema2.Types[0].Fields == TEXT("speed:float;health:int;"), TEXT("macro_schema_v2_same_process"),
                                                                            Schema2.Error);
    const auto Aot = Runtime.RunFile(Runtime.ScriptPath(TEXT("aot_smoke.das")), 40, true);
    Expect(Aot.bSucceeded && Aot.Value == 42 && Aot.bUsedAot, TEXT("aot_native_execution"), Aot.Error);
    const auto InvalidSchema = Run(TEXT("schema_invalid.das"));
    Expect(!InvalidSchema.bSucceeded &&
           InvalidSchema.Error.Contains(TEXT("ue_probe")), TEXT("unsupported_schema_rejected"), InvalidSchema.Error);
    const auto Unicode = Run(TEXT("unicode.das"));
    Expect(Unicode.bSucceeded && Unicode.Types.Num() == 1 &&
           Unicode.Types[0].Name == TEXT("Тип") && Unicode.Types[0].Fields ==
                                                       TEXT("здоровье:int;"),
                                                            TEXT("utf8_owned_after_context_destroyed"), Unicode.Error);
    const auto NoSelf = Run(TEXT("self_properties.das"));
    Expect(!NoSelf.bSucceeded && NoSelf.Error.Contains(TEXT("self is missing or expired")),
                                                       TEXT("missing_self_rejected"));
    TStrongObjectPtr<UDasObjectFixture> Fixture(NewObject<UDasObjectFixture>());
    TWeakObjectPtr<UObject> WeakSelf(Fixture.Get());
    auto RunSelf = [&](const TCHAR* Name)
    {
        return Runtime.RunFile(Runtime.ScriptPath(Name), 5, false, WeakSelf);
    };
    const auto Access = RunSelf(TEXT("self_properties.das"));
    Expect(Access.bSucceeded && Access.Value == 105 && Fixture->health == 105 && Fixture->speed == 38.f,
           TEXT("self_scalar_read_write"), Access.Error);
    {
        // Informational: cost of one self int read plus write, compile time excluded by differencing.
        Fixture->health = 0;
        const int32 Iterations = 20000;
        const double Start = FPlatformTime::Seconds();
        const auto Short = Runtime.RunFile(Runtime.ScriptPath(TEXT("self_access_benchmark.das")), 1, false, WeakSelf);
        const double Middle = FPlatformTime::Seconds();
        const auto Long =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("self_access_benchmark.das")), Iterations + 1, false, WeakSelf);
        const double End = FPlatformTime::Seconds();
        const double NsPerAccess = FMath::Max(0.0, (End - Middle) - (Middle - Start)) * 1e9 / Iterations;
        Expect(Short.bSucceeded && Long.bSucceeded && Long.Value == Iterations + 2,
               TEXT("self_int_read_write_benchmark"),
                    FString::Printf(TEXT("ns_per_read_write=%.0f %s"), NsPerAccess, *Long.Error));
        Fixture->health = 105;
    }
    const auto WrongType = RunSelf(TEXT("self_wrong_type.das"));
    Expect(!WrongType.bSucceeded && WrongType.Error.Contains(TEXT("different scalar type")),
                                                             TEXT("self_wrong_type_rejected"));
    const auto MissingField = RunSelf(TEXT("self_missing_field.das"));
    Expect(!MissingField.bSucceeded && MissingField.Error.Contains(TEXT("does not exist")),
                                                                   TEXT("self_missing_property_rejected"));
    const auto ReadOnly = RunSelf(TEXT("self_readonly.das"));
    Expect(!ReadOnly.bSucceeded && ReadOnly.Error.Contains(TEXT("read-only")) && Fixture->readOnly == 7,
                                                           TEXT("self_readonly_write_rejected"));
    Fixture.Reset();
    CollectGarbage(RF_NoFlags);
    Expect(!WeakSelf.IsValid(), TEXT("self_weak_reference_does_not_pin_object"));
    const auto Expired = RunSelf(TEXT("self_properties.das"));
    Expect(!Expired.bSucceeded && Expired.Error.Contains(TEXT("self is missing or expired")),
                                                         TEXT("expired_self_rejected"));
    FDasClassSchema Typed;
    FString SchemaError;
    Expect(FDasClassSchema::FromDeclaration({TEXT("ActorType"), TEXT("Actor"), TEXT("speed:float;health:int;")},
                                                  TEXT("smoke.das"), Typed, SchemaError) && Typed.Fields.Num() == 2 &&
                                                  Typed.Fields[0].Kind == EDasScalarKind::Float32 &&
                                                  Typed.Fields[1].Kind == EDasScalarKind::Int32,
                                             TEXT("typed_schema_from_macro_wire"), SchemaError);
    Expect(!FDasClassSchema::FromDeclaration({TEXT("ActorType"), TEXT("Actor"), TEXT("speed:int;SPEED:float;")},
                                                   TEXT("smoke.das"), Typed, SchemaError) && Typed.Fields.Num() == 2 &&
                                                   Typed.Fields[0].Kind == EDasScalarKind::Float32,
                                              TEXT("invalid_schema_preserves_previous_IR"));

    FString MethodPreflightError;
    Expect(Runtime.ValidateIntMethod(Runtime.ScriptPath(TEXT("authoring_eval_v1.das")), MethodPreflightError),
                                     TEXT("authoring_method_compile_preflight_without_self"), MethodPreflightError);
    Expect(Runtime.ValidateIntMethod(Runtime.ScriptPath(TEXT("panic.das")), MethodPreflightError),
                                     TEXT("authoring_method_preflight_does_not_execute_body"), MethodPreflightError);
    Expect(!Runtime.ValidateIntMethod(Runtime.ScriptPath(TEXT("wrong_signature.das")), MethodPreflightError),
                                      TEXT("authoring_method_signature_preflight"), MethodPreflightError);
    FDasAuthoringModule Model;
    Model.Id = TEXT("authoring-tests");
    FDasDeclaredClass Decl;
    Decl.Id = TEXT("authoring-tests.actor");
    Decl.Name = TEXT("AuthoredCounter");
    Decl.NativeParent = TEXT("/Script/Engine.Actor");
    FDasDeclaredProperty Score;
    Score.Id = TEXT("authoring-tests.score");
    Score.Name = TEXT("Score");
    Score.Default.Kind = TEXT("number");
    Score.Default.Number = 10;
    Decl.Properties.Add(Score);
    FDasDeclaredMethod Evaluate;
    Evaluate.Id = TEXT("authoring-tests.evaluate");
    Evaluate.Name = TEXT("Evaluate");
    Evaluate.ScriptFile = TEXT("authoring_eval_v1.das");
    FDasDeclaredParameter Input;
    Input.Id = TEXT("authoring-tests.input");
    Input.Name = TEXT("Input");
    Evaluate.Parameters.Add(Input);
    Decl.Methods.Add(Evaluate);
    Model.Classes.Add(Decl);
    FString ModelJson, ModelError;
    Expect(Model.Validate(ModelError) && Model.ToJson(ModelJson, ModelError),
           TEXT("authoring_valid_module_and_normalization"), ModelError);
    FDasAuthoringModule ParsedModel;
    FString Reencoded;
    Expect(FDasAuthoringModule::FromJson(ModelJson, ParsedModel, ModelError) &&
               ParsedModel.ToJson(Reencoded, ModelError) && Reencoded == ModelJson,
           TEXT("authoring_json_roundtrip"), ModelError);
    FDasClassSchema ActorSchema;
    Expect(Model.ToActorSchema(Decl.Id, ActorSchema, ModelError) && ActorSchema.StableId == Decl.Id &&
               ActorSchema.Fields[0].StableId == Score.Id && ActorSchema.Method.StableId == Evaluate.Id &&
               ActorSchema.Method.InputStableId == Input.Id,
           TEXT("authoring_actor_adapter_and_stable_ids"), ModelError);
    {
        auto Bad = Model;
        Bad.Classes[0].Properties[0].Id = Evaluate.Id;
        Expect(!Bad.Validate(ModelError) && ModelError.Contains(TEXT("duplicate")), TEXT("authoring_duplicate_ids"),
                                                                                         ModelError);
        Bad = Model;
        auto Duplicate = Score;
        Duplicate.Id = TEXT("authoring-tests.other");
        Duplicate.Name = TEXT("SCORE");
        Bad.Classes[0].Properties.Add(Duplicate);
        Expect(!Bad.Validate(ModelError), TEXT("authoring_case_insensitive_member_collision"), ModelError);
        Bad = Model;
        Bad.Classes[0].Properties[0].Default.Number = 2147483648.0;
        Expect(!Bad.Validate(ModelError), TEXT("authoring_int_default_range"), ModelError);
        Bad = Model;
        Bad.Classes[0].Properties[0].Default.Kind = TEXT("bool");
        Expect(!Bad.Validate(ModelError), TEXT("authoring_default_type"), ModelError);
        Bad = Model;
        Bad.Classes[0].Methods[0].ScriptFile = TEXT("../outside.das");
        Expect(!Bad.Validate(ModelError), TEXT("authoring_script_path_boundary"), ModelError);
        Bad = Model;
        Bad.Classes[0].Properties[0].Type.Kind = TEXT("void");
        Expect(!Bad.Validate(ModelError), TEXT("authoring_void_property"), ModelError);
        Bad = Model;
        Bad.Classes[0].Methods[0].Parameters[0].Direction = TEXT("wrong");
        Expect(!Bad.Validate(ModelError), TEXT("authoring_parameter_direction"), ModelError);
        Bad = Model;
        Bad.Classes[0].NativeParent.Reset();
        Bad.Classes[0].ParentId = TEXT("missing");
        Expect(!Bad.Validate(ModelError), TEXT("authoring_missing_parent"), ModelError);
        Bad = Model;
        Bad.Classes[0].Metadata.Add(TEXT("Unexpected"), TEXT("x"));
        Expect(!Bad.Validate(ModelError), TEXT("authoring_metadata_allowlist"), ModelError);
    }
    {
        auto Inherited = Model;
        FDasDeclaredClass Child;
        Child.Id = TEXT("authoring-tests.child");
        Child.Name = TEXT("ChildCounter");
        Child.ParentId = Decl.Id;
        Inherited.Classes.Add(Child);
        Expect(Inherited.Validate(ModelError), TEXT("authoring_class_inheritance_graph"), ModelError);
        Inherited.Classes[0].NativeParent.Reset();
        Inherited.Classes[0].ParentId = Child.Id;
        Expect(!Inherited.Validate(ModelError) && ModelError.Contains(TEXT("cycle")),
                                                                      TEXT("authoring_inheritance_cycle"), ModelError);
        Inherited = Model;
        auto Conflict = Score;
        Conflict.Id = TEXT("authoring-tests.child.score");
        Child.Properties.Add(Conflict);
        Inherited.Classes.Add(Child);
        Expect(!Inherited.Validate(ModelError) &&
               ModelError.Contains(TEXT("Inherited")), TEXT("authoring_inherited_member_collision"), ModelError);
    }
    {
        auto Next = Decl;
        Next.Properties[0].Metadata.Add(TEXT("Category"), TEXT("Gameplay"));
        auto Delta = FDasAuthoringModule::Compare(Decl, Next);
        Expect(Delta.bMetadata && !Delta.bStructural && !Delta.bDefaults && !Delta.bImplementation,
               TEXT("authoring_metadata_only_diff"));
        Next = Decl;
        Next.Methods[0].ScriptFile = TEXT("authoring_eval_v2.das");
        Delta = FDasAuthoringModule::Compare(Decl, Next);
        Expect(Delta.bImplementation && !Delta.bStructural, TEXT("authoring_implementation_diff"));
        Next = Decl;
        Next.Properties[0].Default.Number = 11;
        Delta = FDasAuthoringModule::Compare(Decl, Next);
        Expect(Delta.bDefaults && !Delta.bStructural, TEXT("authoring_default_diff"));
        Next = Decl;
        Next.Properties[0].Name = TEXT("RenamedScore");
        Delta = FDasAuthoringModule::Compare(Decl, Next);
        Expect(Delta.bStructural && Delta.Added.IsEmpty() && Delta.Removed.IsEmpty(),
               TEXT("authoring_rename_preserves_member_identity"));
        Next = Decl;
        Next.Properties[0].Id = TEXT("replacement-score");
        Delta = FDasAuthoringModule::Compare(Decl, Next);
        Expect(Delta.bStructural && Delta.Added.Num() == 1 && Delta.Removed.Num() == 1,
               TEXT("authoring_identity_replacement_diff"));
    }
    {
        auto Rich = Model;
        Rich.Classes[0].Methods[0].bStatic = true;
        Expect(Rich.Validate(ModelError) && !Rich.ToActorSchema(Decl.Id, ActorSchema, ModelError) &&
               ActorSchema.Fields.Num() == 1 &&
               ActorSchema.Method.ScriptFile == TEXT("authoring_eval_v1.das"),
                                                     TEXT("authoring_backend_capabilities_preserve_candidate"),
                                                          ModelError);
        Rich = Model;
        Rich.Classes[0].Properties[0].Type.Kind = TEXT("string");
        Rich.Classes[0].Properties[0].Default.Kind = TEXT("string");
        Rich.Classes[0].Properties[0].Default.String = TEXT("text");
        Expect(Rich.Validate(ModelError) && Rich.ToActorSchema(Decl.Id, ActorSchema, ModelError) &&
                   ActorSchema.Fields[0].Kind == EDasScalarKind::String,
               TEXT("authoring_string_backend_supported"), ModelError);
        Rich = Model;
        auto& P = Rich.Classes[0].Properties[0];
        P.Type.Kind = TEXT("object");
        P.Type.TypePath = Decl.Id;
        P.Default.Kind = TEXT("null");
        Expect(Rich.Validate(ModelError), TEXT("authoring_object_self_reference"), ModelError);
    }
    {
        auto Previous = ParsedModel;
        const FString Bad = ModelJson.Replace(TEXT("\"schema_version\": 1"), TEXT("\"schema_version\": 99"));
        Expect(!FDasAuthoringModule::FromJson(Bad, ParsedModel, ModelError) && ParsedModel.Id == Previous.Id &&
                   ParsedModel.Classes[0].Id == Previous.Classes[0].Id,
               TEXT("authoring_invalid_json_preserves_previous_model"), ModelError);
        Expect(!FDasAuthoringModule::FromJson(
            TEXT("{\"schema_version\":1,\"module_id\":\"x\",\"classes\":[],\"typo\":true}"), ParsedModel, ModelError),
            TEXT("authoring_unknown_json_key"), ModelError);
        const FString BadBool = ModelJson.Replace(TEXT("\"editable\": true"), TEXT("\"editable\": \"yes\""));
        Expect(!FDasAuthoringModule::FromJson(BadBool, ParsedModel, ModelError) &&
               ModelError.Contains(TEXT("boolean")), TEXT("authoring_strict_json_bool"), ModelError);
    }

    // Each run owns a unique scratch script; source plugin examples are never modified.
    const FString Scratch = FPaths::ProjectSavedDir() / TEXT("DasPersistent") /
                                                             FGuid::NewGuid().ToString(EGuidFormats::Digits) /
                                                             TEXT("counter.das");
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Scratch), true);
    const FString Counter = TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int {\n let n = ue_state_get_int(\"count\", 0) + input\n ue_state_set_int(\"count\", n)\n return n + OFFSET\n}\n");
    auto Write = [&](const FString& Text)
    {
        return FFileHelper::SaveStringToFile(Text, *Scratch, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    TStrongObjectPtr<UDasObjectFixture> First(NewObject<UDasObjectFixture>());
    TStrongObjectPtr<UDasObjectFixture> Second(NewObject<UDasObjectFixture>());
    auto Persistent = [&](UObject* Object, int32 Input = 1)
    {
        return Runtime.RunPersistent(Scratch, Input, Object);
    };
    Expect(Write(Counter.Replace(TEXT("OFFSET"), TEXT("0"))), TEXT("persistent_write_fixture"));
    auto P1 = Persistent(First.Get());
    auto P2 = Persistent(First.Get());
    auto POther = Persistent(Second.Get());
    Expect(P1.bSucceeded && P1.Value == 1 && P2.Value == 2 && P2.Revision == 1 && POther.Value == 1,
           TEXT("persistent_independent_state_and_cached_revision"), P1.Error);
    Expect(Write(Counter.Replace(TEXT("OFFSET"), TEXT("100"))), TEXT("persistent_write_revision"));
    P1 = Persistent(First.Get());
    POther = Persistent(Second.Get());
    Expect(P1.bSucceeded && P1.Value == 103 && POther.Value == 102 && P1.Revision == 2,
           TEXT("persistent_reload_preserves_each_counter"), P1.Error);
    Expect(Write(TEXT("this is invalid daScript")), TEXT("persistent_write_invalid"));
    P1 = Persistent(First.Get());
    P2 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 104 && !P1.ReloadError.IsEmpty() && P2.Value == 105 && P2.Revision == 2,
           TEXT("persistent_bad_source_keeps_previous_code_and_state"), P1.Error);
    Expect(Write(TEXT("options gen2\n[export]\ndef main():int { return 7 }\n")),
                 TEXT("persistent_write_bad_signature"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 106 &&
           P1.ReloadError.Contains(TEXT("main")), TEXT("persistent_bad_signature_fallback"));
    Expect(Write(Counter.Replace(TEXT("OFFSET"), TEXT("200"))), TEXT("persistent_repair_source"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 207 && P1.Revision == 3 && P1.ReloadError.IsEmpty(),
           TEXT("persistent_recovery"), P1.Error);
    const FString InitWrites = TEXT("options gen2\nrequire ue_foundation\ndef initialize():int { ue_set_self_int(\"health\", 999); return 1 }\nvar initialized = initialize()\n[export]\ndef main(input:int):int { return initialized + input }\n");
    Expect(Write(InitWrites), TEXT("persistent_write_unsafe_initializer"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 208 && !P1.ReloadError.IsEmpty() && First->health == 100 && P1.Revision == 3,
           TEXT("persistent_rejected_initializer_cannot_mutate_actor"), P1.ReloadError);
    const FString Panicking = TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int { if (input < 0) { panic(\"negative\") }; return ue_state_get_int(\"count\", 0) }\n");
    Expect(Write(Panicking), TEXT("persistent_write_runtime_failure_fixture"));
    P1 = Persistent(First.Get(), -1);
    P2 = Persistent(First.Get());
    Expect(!P1.bSucceeded &&
           P1.Error.Contains(TEXT("negative")) && P2.bSucceeded && P2.Value == 8 && P1.Revision == P2.Revision,
                             TEXT("persistent_context_recovers_after_runtime_exception"), P1.Error + P1.ReloadError);
    Expect(Write(Counter.Replace(TEXT("OFFSET"), TEXT("200"))), TEXT("persistent_restore_counter_fixture"));
    Runtime.ReleaseObject(First.Get());
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 201 && P1.Revision == 1, TEXT("persistent_explicit_release_resets_session"));
    const FString HelperPath = FPaths::GetPath(Scratch) / TEXT("counter_values.das");
    auto WriteHelper = [&](int32 Value)
    {
        return FFileHelper::SaveStringToFile(FString::Printf(
            TEXT("options gen2\nmodule counter_values public\ndef counter_offset():int { return %d }\n"), Value),
            *HelperPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    const FString WithDependency = Counter.Replace(
        TEXT("require ue_foundation"),
             TEXT("require ue_foundation\nrequire counter_values")).Replace(TEXT("OFFSET"), TEXT("counter_offset()"));
    Expect(WriteHelper(10) && Write(WithDependency), TEXT("persistent_write_dependency"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 12, TEXT("persistent_dependency_initial"), P1.Error + P1.ReloadError);
    const uint32 DependencyRevision = P1.Revision;
    Expect(WriteHelper(20), TEXT("persistent_change_dependency_only"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.Value == 23 && P1.Revision == DependencyRevision + 1,
           TEXT("persistent_dependency_change_triggers_reload"), P1.Error + P1.ReloadError);
    const FString Globals =
        TEXT("options gen2\nvar calls = 0\n[export]\ndef main(input:int):int { calls += 1; return calls + input }\n");
    Expect(Write(Globals), TEXT("persistent_write_globals_fixture"));
    P1 = Persistent(First.Get(), 0);
    P2 = Persistent(First.Get(), 0);
    Expect(P1.bSucceeded && P1.Value == 1 && P2.bSucceeded && P2.Value == 2 && P1.Revision == P2.Revision,
           TEXT("persistent_vm_globals_reused_between_calls"), P1.Error + P1.ReloadError);
    IFileManager::Get().Delete(*HelperPath);
    Runtime.ReleaseObject(First.Get());
    FString LifecycleSource;
    Expect(FFileHelper::LoadFileToString(LifecycleSource, *Runtime.ScriptPath(TEXT("lifecycle_actor.das"))) &&
                                         Write(LifecycleSource),
                                         TEXT("lifecycle_fixture"));
    auto Event = [&](EDasCallback Kind, float Delta = 0.f, int32 Reason = 0)
    {
        return Runtime.RunLifecycle(Scratch, Kind, First.Get(), Delta, Reason);
    };
    auto Begin = Event(EDasCallback::BeginPlay);
    auto BeginAgain = Event(EDasCallback::BeginPlay);
    Expect(Begin.bSucceeded && BeginAgain.bSucceeded && Persistent(First.Get(), 0).Value == 1 && First->health == 101,
           TEXT("lifecycle_begin_once"), Begin.Error);
    auto Tick = Event(EDasCallback::Tick, 0.25f);
    Expect(Tick.bSucceeded && First->speed == 37.25f && Persistent(First.Get()).Value == 1,
           TEXT("lifecycle_float_delta"), Tick.Error);
    Expect(Write(LifecycleSource.Replace(TEXT("delta_time * 1.0"), TEXT("delta_time * 10.0"))),
                 TEXT("lifecycle_write_reload"));
    Tick = Event(EDasCallback::Tick, 0.5f);
    Expect(Tick.bSucceeded && Tick.Revision == 2 && First->speed == 42.25f && Persistent(First.Get(), 0).Value == 1 &&
               Persistent(First.Get()).Value == 2 && First->health == 101,
           TEXT("lifecycle_reload_does_not_repeat_begin"), Tick.Error);
    Expect(Write(TEXT("options gen2\n[export]\ndef tick(delta_time:int) {}\n")), TEXT("lifecycle_write_bad_signature"));
    Tick = Event(EDasCallback::Tick, 0.5f);
    Expect(Tick.bSucceeded && Tick.ReloadError.Contains(TEXT("tick")) && Tick.Revision == 2 && First->speed == 47.25f,
                                                        TEXT("lifecycle_rejects_hook_signature_keeps_old_version"),
                                                             Tick.ReloadError);
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
    Expect(End.bSucceeded && EndAgain.bSucceeded && First->health == 203, TEXT("lifecycle_end_once_uses_live_code"),
                                                                               End.Error);
    Expect(!Event(EDasCallback::Tick, 1.f).bSucceeded, TEXT("lifecycle_tick_after_end_rejected"));
    Runtime.ReleaseObject(First.Get());
    Expect(Write(TEXT("options gen2\n[export]\ndef tick(delta_time:float) {}\n")),
                 TEXT("lifecycle_only_script_fixture"));
    Expect(Event(EDasCallback::BeginPlay).bSucceeded && Event(EDasCallback::Tick, 0.1f).bSucceeded &&
               Event(EDasCallback::EndPlay).bSucceeded,
           TEXT("lifecycle_optional_hooks_without_main"));
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
    Expect(Write(
        TEXT("options gen2\nrequire ue_foundation\n[export]\ndef begin_play() { ue_set_self_int(\"health\", ue_get_self_int(\"health\") + 1); panic(\"begin failed\") }\n[export]\ndef tick(dt:float) {}\n")),
        TEXT("lifecycle_begin_panic_fixture"));
    First->health = 100;
    Begin = Event(EDasCallback::BeginPlay);
    Tick = Event(EDasCallback::Tick, 0.1f);
    Expect(!Begin.bSucceeded && Begin.Error.Contains(TEXT("begin failed")) && Tick.bSucceeded && First->health == 101,
                                                     TEXT("lifecycle_begin_panic_not_replayed"), Begin.Error);
    Runtime.ReleaseObject(First.Get());

    FString DelegateSource;
    Expect(FFileHelper::LoadFileToString(DelegateSource, *Runtime.ScriptPath(TEXT("delegate_actor.das"))),
                                         TEXT("delegate_fixture_loaded"));
    DelegateSource = DelegateSource.Replace(TEXT("\"peer\""), TEXT("\"self\""));
    Expect(Write(DelegateSource), TEXT("delegate_fixture_written"));
    FScriptDelegate Foreign;
    Foreign.BindUFunction(First.Get(), GET_FUNCTION_NAME_CHECKED(UDasObjectFixture, RecordForeignSignal));
    First->OnDasSignal.AddUnique(Foreign);
    P1 = Persistent(First.Get());
    First->OnDasSignal.Broadcast(3);
    Expect(P1.bSucceeded && Persistent(First.Get()).Value == 3 && First->ForeignReceived == 3 &&
               Runtime.SubscriptionCount(First.Get()) == 1,
           TEXT("delegate_native_broadcast_to_script"), P1.Error);
    uint64 OldSubscriptionId = 0;
    for (UObject* Bound : First->OnDasSignal.GetAllObjects())
        if (auto* Proxy = Cast<UDasIntDelegateProxy>(Bound))
            OldSubscriptionId = Proxy->SubscriptionId;
    Expect(OldSubscriptionId != 0, TEXT("delegate_has_opaque_token"));
    const FString DelegateV2 = DelegateSource.Replace(TEXT("+ value)"), TEXT("+ value * 10)"));
    Expect(Write(DelegateV2), TEXT("delegate_write_new_handler"));
    P1 = Persistent(First.Get());
    First->OnDasSignal.Broadcast(2);
    Runtime.DispatchIntEvent(OldSubscriptionId, 1000);
    Expect(P1.bSucceeded && Persistent(First.Get()).Value == 23 && Runtime.SubscriptionCount(First.Get()) == 1 &&
               First->OnDasSignal.GetAllObjects().Num() == 2 && First->ForeignReceived == 5,
           TEXT("delegate_reload_replaces_once_and_revokes_old_token"), P1.ReloadError);
    const FString BadHandler = DelegateSource.Replace(
        TEXT("on_signal(value:int)"), TEXT("on_signal(value:float)")).Replace(TEXT("+ value)"), TEXT("+ int(value))"));
    Expect(Write(BadHandler), TEXT("delegate_bad_handler_fixture"));
    P1 = Persistent(First.Get());
    First->OnDasSignal.Broadcast(1);
    Expect(P1.bSucceeded && P1.ReloadError.Contains(TEXT("event handler")) && Persistent(First.Get()).Value == 33 &&
                                                    Runtime.SubscriptionCount(First.Get()) == 1,
                                                    TEXT("delegate_bad_handler_keeps_old_subscription"),
                                                         P1.ReloadError);
    Expect(Write(DelegateSource.Replace(TEXT("OnDasSignal"), TEXT("OnFloatSignal"))),
                 TEXT("delegate_bad_event_fixture"));
    P1 = Persistent(First.Get());
    First->OnDasSignal.Broadcast(1);
    Expect(P1.bSucceeded && !P1.ReloadError.IsEmpty() && Persistent(First.Get()).Value == 43,
           TEXT("delegate_rejects_wrong_UE_signature"));
    const FString SubscribeLine = TEXT("ue_subscribe_int(\"self\", \"OnDasSignal\", \"on_signal\")");
    Expect(Write(DelegateSource.Replace(*SubscribeLine, *(SubscribeLine + TEXT("\n    ") + SubscribeLine))),
                 TEXT("delegate_duplicate_fixture"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && P1.ReloadError.Contains(TEXT("duplicate")) && Runtime.SubscriptionCount(First.Get()) == 1,
                                                    TEXT("delegate_duplicate_candidate_is_atomic"), P1.ReloadError);
    const int32 HealthBeforeBinding = First->health;
    Expect(Write(DelegateSource.Replace(*SubscribeLine, TEXT("ue_set_self_int(\"health\", 999)"))),
                 TEXT("delegate_unsafe_binding_fixture"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && !P1.ReloadError.IsEmpty() && First->health == HealthBeforeBinding &&
               Runtime.SubscriptionCount(First.Get()) == 1,
           TEXT("delegate_rejected_binding_has_no_actor_side_effects"));
    Expect(
        Write(DelegateV2.Replace(TEXT("def on_signal(value:int) {"),
                                      TEXT("def on_signal(value:int) {\n if (value < 0) { panic(\"event failed\") }"))),
              TEXT("delegate_panic_fixture"));
    P1 = Persistent(First.Get());
    First->OnDasSignal.Broadcast(-1);
    First->OnDasSignal.Broadcast(1);
    Expect(P1.bSucceeded && Persistent(First.Get()).Value == 53 && Runtime.SubscriptionCount(First.Get()) == 1,
           TEXT("delegate_handler_recovers_after_panic"), P1.ReloadError);
    Expect(Event(EDasCallback::BeginPlay).bSucceeded && Event(EDasCallback::EndPlay).bSucceeded &&
               Runtime.SubscriptionCount(First.Get()) == 0 && First->OnDasSignal.GetAllObjects().Num() == 1,
           TEXT("delegate_EndPlay_removes_only_owned_bindings"));
    Runtime.ReleaseObject(First.Get());
    Expect(Write(DelegateSource), TEXT("delegate_GC_fixture"));
    P1 = Persistent(First.Get());
    Expect(P1.bSucceeded && Runtime.SubscriptionCount(First.Get()) == 1, TEXT("delegate_before_owner_GC"));
    Runtime.ReleaseObject(First.Get());
    TStrongObjectPtr<UDasObjectFixture> ObjectTarget(NewObject<UDasObjectFixture>());
    First->Target = ObjectTarget.Get();
    auto ObjectRun = [&](const FString& Body)
    {
        Write(TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int {\n") + Body +
                   TEXT("\n}\n"));
        return Runtime.RunFile(Scratch, 5, false, First.Get());
    };
    const auto ObjectExample = Runtime.RunFile(Runtime.ScriptPath(TEXT("object_calls.das")), 5, false, First.Get());
    Expect(ObjectExample.bSucceeded && ObjectExample.Value == 105 && ObjectTarget->health == 105,
           TEXT("object_native_UFunction_example"), ObjectExample.Error);
    auto Obj = ObjectRun(TEXT("let obj = ue_get_object(ue_self_object(), \"Target\")\nverify(ue_object_is_a(obj, \"/Script/CoreUObject.Object\"))\nlet scaled = ue_call_float(obj, \"ScaleSpeed\", 2.0)\nverify(scaled == 74.0)\nue_object_set_int(obj, \"health\", 17)\nreturn ue_object_get_int(obj, \"health\")"));
    Expect(Obj.bSucceeded && Obj.Value == 17, TEXT("object_scalar_properties_and_float_call"), Obj.Error);
    Obj = ObjectRun(TEXT("let obj = ue_call_object(ue_self_object(), \"GetTargetObject\")\nue_call_void_object(ue_self_object(), \"SetFixtureTarget\", obj)\nreturn ue_object_get_int(obj, \"health\")"));
    Expect(Obj.bSucceeded && Obj.Value == 17 && First->Target == ObjectTarget.Get(),
           TEXT("object_return_and_typed_argument"), Obj.Error);
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
    Obj = Persistent(First.Get());
    ObjectTarget->OnDasSignal.Broadcast(7);
    Expect(Obj.bSucceeded && Persistent(First.Get()).Value == 7 && Runtime.SubscriptionCount(First.Get()) == 1,
           TEXT("object_arbitrary_source_subscription"), Obj.Error);
    const int32 BeforeCandidate = ObjectTarget->health;
    Expect(Write(ObjectDelegate.Replace(
        TEXT("ue_subscribe_object_int(ue_get_object(ue_self_object(), \"Target\"), \"OnDasSignal\", \"on_value\")"),
             TEXT("ue_call_void_int(ue_get_object(ue_self_object(), \"Target\"), \"SetHealthValue\", 999)"))),
                 TEXT("object_call_in_candidate_fixture"));
    Obj = Persistent(First.Get());
    Expect(Obj.bSucceeded && !Obj.ReloadError.IsEmpty() && ObjectTarget->health == BeforeCandidate &&
               Runtime.SubscriptionCount(First.Get()) == 1,
           TEXT("object_candidate_cannot_call_UFunction"));
    Runtime.ReleaseObject(First.Get());
    const FString CachedHandle = TEXT("options gen2\nrequire ue_foundation\nvar cached = ue_null_object()\n[export]\ndef main(input:int):int { if (input == 0) { cached = ue_get_object(ue_self_object(), \"Target\"); return 1 }; if (input == 1) { if (ue_object_valid(cached)) { return 1 }; return 0 }; return ue_object_get_int(cached, \"health\") }\n");
    Expect(Write(CachedHandle), TEXT("object_cached_handle_fixture"));
    Obj = Persistent(First.Get(), 0);
    Expect(Obj.bSucceeded && Persistent(First.Get(), 1).Value == 1, TEXT("object_handle_survives_repeated_calls"),
                                                                         Obj.Error);
    TWeakObjectPtr<UObject> WeakTarget(ObjectTarget.Get());
    First->Target = nullptr;
    ObjectTarget.Reset();
    CollectGarbage(RF_NoFlags);
    Obj = Persistent(First.Get(), 1);
    const auto ExpiredObject = Persistent(First.Get(), 2);
    Expect(!WeakTarget.IsValid() && Obj.bSucceeded && Obj.Value == 0 && !ExpiredObject.bSucceeded &&
           ExpiredObject.Error.Contains(TEXT("expired")), TEXT("object_cached_handle_is_weak_and_expires_after_GC"),
                                                               ExpiredObject.Error);
    Runtime.ReleaseObject(First.Get());
    const auto Marshalled = Runtime.RunFile(Runtime.ScriptPath(TEXT("marshalling.das")), 5, false, First.Get());
    Expect(Marshalled.bSucceeded && Marshalled.Value == 12 && First->MarshalledCalls == 1,
           TEXT("marshal_multiarg_constref_struct_unicode"), Marshalled.Error);
#include "DasReflectedValueTests.inl"
#include "DasSelfValueTests.inl"
#include "DasSnapshotValueTests.inl"
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_string(c, \"Label\", \"ok\")\nue_arg_bool(c, \"bAccept\", true)\nue_invoke_call(c)\nverify(ue_result_bool(c, \"ReturnValue\"))\nue_release_call(c)\nreturn 1"));
        (void)Before;
        Expect(M.bSucceeded && M.Value == 1, TEXT("marshal_bool_return"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_string(c, \"Label\", \"ok\")\nue_invoke_call(c)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("missing input")) && First->MarshalledCalls == Before,
                                                 TEXT("marshal_missing_input"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(
            TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_int(c, \"Label\", 7)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before,
                                                 TEXT("marshal_wrong_type"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_bool(c, \"Missing\", true)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("unknown parameter")) && First->MarshalledCalls == Before,
                                                 TEXT("marshal_extra_input"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nverify(ue_result_bool(c, \"ReturnValue\"))\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("executed call")) && First->MarshalledCalls == Before,
                                                 TEXT("marshal_early_result"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"OutValue\")\nue_invoke_call(c)\nverify(ue_result_int(c, \"Value\") == ue_get_self_int(\"health\"))\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before, TEXT("marshal_out_ref"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"GetTargetObject\")\nue_invoke_call(c)\nverify(!ue_object_valid(ue_result_object(c, \"ReturnValue\")))\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before, TEXT("marshal_null_object_result"),
                                                                                      M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_release_call(c)\nue_invoke_call(c)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("released")) && First->MarshalledCalls == Before,
                                                 TEXT("marshal_released"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformPayload\")\nue_arg_int(c, \"Payload.Count\", 7)\nue_invoke_call(c)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("missing input")) && First->MarshalledCalls == Before,
                                                 TEXT("marshal_partial_struct"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("for (i in range(600)) {\n let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\n ue_arg_string(c, \"Label\", \"строка\")\n ue_arg_bool(c, \"bAccept\", true)\n ue_invoke_call(c)\n ue_release_call(c)\n}\nreturn 1"));
        (void)Before;
        Expect(M.bSucceeded && M.Value == 1, TEXT("marshal_repeat_release"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(
            TEXT("for (i in range(257)) { let c = ue_begin_call(ue_self_object(), \"AcceptLabel\") }\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("frame limit")) && First->MarshalledCalls == Before,
                                                 TEXT("marshal_budget"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_string(c, \"Label\", \"ok\")\nue_arg_bool(c, \"bAccept\", true)\nue_invoke_call(c)\nue_invoke_call(c)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("execute once")) && First->MarshalledCalls == Before + 1,
                                                 TEXT("marshal_double_invoke"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nue_arg_string(c, \"Label\", \"\")\nue_arg_bool(c, \"bAccept\", false)\nue_invoke_call(c)\nreturn ue_result_int(c, \"ReturnValue\")"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before + 1,
                                                 TEXT("marshal_wrong_result"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")\nlet copy = c\nue_release_call(c)\nue_invoke_call(copy)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("released")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("marshal_copied_handle_revoked"), M.Error);
    }
    Expect(Write(ObjectDelegate.Replace(
        TEXT("ue_subscribe_object_int(ue_get_object(ue_self_object(), \"Target\"), \"OnDasSignal\", \"on_value\")"),
             TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\")"))),
                 TEXT("marshal_candidate_fixture"));
    const auto CandidateCall = Persistent(First.Get());
    Expect(!CandidateCall.bSucceeded && CandidateCall.Error.Contains(TEXT("forbidden in bind_events")),
                                                                     TEXT("marshal_candidate_cannot_prepare_calls"),
                                                                          CandidateCall.Error);
    Runtime.ReleaseObject(First.Get());
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"MutateValues\")\nue_arg_int(c, \"Count\", 7)\nue_arg_string(c, \"Text\", \"Привет\")\nue_invoke_call(c)\nverify(ue_result_int(c, \"Count\") == 10)\nverify(ue_result_int(c, \"ReturnValue\") == 20)\nverify(ue_result_bool(c, \"Accepted\"))\nlet description = ue_result_string(c, \"Description\")\nverify(ue_result_string(c, \"Text\") == description)\nue_release_call(c)\nverify(description == \"Привет 世界\")\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("inout_scalar_inout"),
                                                                                          M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeObject\")\nue_arg_object(c, \"Input\", ue_self_object())\nue_arg_object(c, \"InOut\", ue_null_object())\nue_invoke_call(c)\nverify(ue_object_valid(ue_result_object(c, \"InOut\")))\nverify(ue_object_valid(ue_result_object(c, \"ReturnValue\")))\nverify(!ue_object_valid(ue_result_object(c, \"Output\")))\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("inout_object_inout"),
                                                                                          M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"MutateValues\"); ue_arg_int(c, \"Count\", 1); ue_invoke_call(c); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("missing input")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("inout_inout_missing"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(
            TEXT("let c = ue_begin_call(ue_self_object(), \"OutValue\"); ue_arg_int(c, \"Value\", 1); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("direction")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("inout_out_cannot_set"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\"); ue_arg_string(c, \"Label\", \"ok\"); ue_arg_bool(c, \"bAccept\", true); ue_invoke_call(c); let text = ue_result_string(c, \"Label\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("direction")) && First->MarshalledCalls == Before + 1,
                                                 TEXT("inout_constref_not_output"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"SetFixtureTarget\"); ue_arg_int(c, \"Value\", 1); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("inout_object_wrong_scalar"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let made = ue_begin_call(ue_self_object(), \"NewFixture\")\nue_invoke_call(made)\nlet obj = ue_result_object(made, \"ReturnValue\")\nlet gc1 = ue_begin_call(ue_self_object(), \"ForceTestGC\"); ue_invoke_call(gc1); ue_release_call(gc1)\nverify(ue_object_valid(obj))\nlet next = ue_begin_call(ue_self_object(), \"ExchangeObject\")\nue_arg_object(next, \"Input\", obj)\nue_arg_object(next, \"InOut\", ue_null_object())\nue_release_call(made)\n{ let gc2 = ue_begin_call(ue_self_object(), \"ForceTestGC\"); ue_invoke_call(gc2); ue_release_call(gc2) }\nverify(ue_object_valid(obj))\nue_invoke_call(next)\nverify(ue_object_valid(ue_result_object(next, \"InOut\")))\nue_release_call(next)\n{ let gc3 = ue_begin_call(ue_self_object(), \"ForceTestGC\"); ue_invoke_call(gc3); ue_release_call(gc3) }\nverify(!ue_object_valid(obj))\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 2, TEXT("inout_pins_during_gc"),
                                                                                          M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let made = ue_begin_call(ue_self_object(), \"NewFixture\")\nue_invoke_call(made)\nlet obj = ue_result_object(made, \"ReturnValue\")\nue_release_call(made)\nlet gc4 = ue_begin_call(ue_self_object(), \"ForceTestGC\"); ue_invoke_call(gc4); ue_release_call(gc4)\nlet next = ue_begin_call(ue_self_object(), \"SetFixtureTarget\")\nue_arg_object(next, \"Value\", obj)\nreturn 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("expired")) && First->MarshalledCalls == Before + 1,
                                                 TEXT("inout_expired_argument"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"SetFixtureTarget\"); ue_arg_object(c, \"Value\", ue_null_object()); ue_invoke_call(c); return 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 0, TEXT("inout_null_argument"),
                                                                                          M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"MutatePayload\")\nue_arg_int(c, \"Payload.Count\", 2)\nue_arg_string(c, \"Payload.Label\", \"ok\")\nue_arg_bool(c, \"Payload.bEnabled\", true)\nue_arg_double(c, \"Payload.Weight\", double(2))\nue_arg_float(c, \"Payload.Metrics.Ratio\", 3.0)\nue_invoke_call(c)\nverify(ue_result_int(c, \"Payload.Count\") == 6)\nverify(ue_result_int(c, \"Copy.Count\") == 6)\nverify(ue_result_string(c, \"Copy.Label\") == \"ok!\")\nverify(ue_result_float(c, \"Copy.Metrics.Ratio\") == 3.0)\nreturn 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("inout_struct_inout"),
                                                                                          M.Error);
    }
    First->Target = WrongClass.Get();
    const auto WrongObjectInput = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"SetFixtureTarget\"); ue_arg_object(c, \"Value\", ue_get_object(ue_self_object(), \"Target\")); return 1"));
    Expect(!WrongObjectInput.bSucceeded &&
           WrongObjectInput.Error.Contains(TEXT("incompatible class")) && First->Target == WrongClass.Get(),
                                           TEXT("inout_object_class_checked_before_call"), WrongObjectInput.Error);
    First->Target = nullptr;
    const auto InOutExample = Runtime.RunFile(Runtime.ScriptPath(TEXT("inout_values.das")), 0, false, First.Get());
    Expect(InOutExample.bSucceeded && InOutExample.Value == 1, TEXT("inout_shipped_example"), InOutExample.Error);
    Expect(Write(
        TEXT("options gen2\nrequire ue_foundation\nvar cached = ue_null_object()\n[export]\ndef main(input:int):int {\n    if (input == 0) {\n        let c = ue_begin_call(ue_self_object(), \"NewFixture\")\n        ue_invoke_call(c)\n        cached = ue_result_object(c, \"ReturnValue\")\n        return 1\n    }\n    if (ue_object_valid(cached)) { return 1 }\n    return 0\n}\n")),
        TEXT("inout_automatic_cleanup_fixture"));
    const auto BeforeAutomaticCleanup = Persistent(First.Get(), 0);
    CollectGarbage(RF_NoFlags);
    const auto AfterAutomaticCleanup = Persistent(First.Get(), 1);
    Expect(BeforeAutomaticCleanup.bSucceeded && AfterAutomaticCleanup.bSucceeded && AfterAutomaticCleanup.Value == 0,
           TEXT("inout_frame_pins_released_at_callback_exit"), AfterAutomaticCleanup.Error);
    Runtime.ReleaseObject(First.Get());
    const auto EnumNameExample = Runtime.RunFile(Runtime.ScriptPath(TEXT("enum_names.das")), 0, false, First.Get());
    Expect(EnumNameExample.bSucceeded && EnumNameExample.Value == 1, TEXT("enum_name_example_inputs_returns_out_inout"),
                                                                          EnumNameExample.Error);
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
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("enum_name_name_case"),
                                                                                          M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EqualNames\"); ue_arg_name(c, \"A\", \"\"); ue_arg_name(c, \"B\", \"None\"); ue_invoke_call(c); verify(ue_result_bool(c, \"ReturnValue\")); return 1"));
        Expect(M.bSucceeded && M.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("enum_name_name_none"),
                                                                                          M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasLegacyMode\", \"DasLegacyIdle\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type path mismatch")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("enum_name_wrong_enum_type"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasCallMode\", \"Unknown\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("unknown or reserved")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("enum_name_missing_enum_member"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasCallMode\", \"7\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("unknown or reserved")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("enum_name_numeric_enum_text"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasCallMode\", \"EDasCallMode_MAX\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("unknown or reserved")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("enum_name_enum_max"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(
            TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_int(c, \"Input\", 7); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("enum_name_enum_as_int"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeName\"); ue_arg_string(c, \"Input\", \"Wrong\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("enum_name_string_as_name"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AcceptLabel\"); ue_arg_name(c, \"Label\", \"Wrong\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("enum_name_name_as_string"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EqualNames\"); ue_arg_name(c, \"A\", \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("too long")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("enum_name_long_name"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"InvalidMode\"); ue_invoke_call(c); let value = ue_result_enum(c, \"ReturnValue\", \"/Script/DasRuntime.EDasCallMode\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("unknown or reserved enum value")) &&
                                                 First->MarshalledCalls == Before + 1,
                                                 TEXT("enum_name_invalid_enum_result"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"InvalidMode\"); ue_invoke_call(c); let value = ue_result_enum(c, \"ReturnValue\", \"/Script/DasRuntime.EDasLegacyMode\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("type path mismatch")) && First->MarshalledCalls == Before + 1,
                                                 TEXT("enum_name_wrong_enum_result_type"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasCallMode\", \"Idle\"); ue_invoke_call(c); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("missing input")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("enum_name_enum_inout_required"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeName\"); ue_arg_name(c, \"Previous\", \"Wrong\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("direction")) && First->MarshalledCalls == Before + 0,
                                                 TEXT("enum_name_name_out_direction"), M.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto
            M = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"ExchangeMode\"); ue_arg_enum(c, \"Input\", \"/Script/DasRuntime.EDasCallMode\", \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"); return 1"));
        Expect(!M.bSucceeded && M.Error.Contains(TEXT("enum member is too long")) && First->MarshalledCalls == Before,
                                                 TEXT("enum_name_long_enum_member"), M.Error);
    }
    const auto Generated = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_calls.das")), 0, false, First.Get());
    Expect(Generated.bSucceeded && Generated.Value == 10,
           TEXT("generated_wrappers_execute_typed_inputs_results_structs"), Generated.Error);
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_bad_name.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: actor_ActorHasTag")),
                                                 TEXT("generated_bad_name"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_bad_enum.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: fixture_ExchangeMode")),
                                                 TEXT("generated_bad_enum"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_wrong_enum.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: fixture_ExchangeMode")),
                                                 TEXT("generated_wrong_enum"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_bad_receiver.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("requested class")), TEXT("generated_bad_receiver"), G.Error);
    }
    const int32 GuardHealth = First->health;
    const auto StaleBinding = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"AddHealth\"); ue_expect_signature(c, \"old signature\"); ue_arg_int(c, \"Value\", 999); ue_invoke_call(c); return 1"));
    Expect(!StaleBinding.bSucceeded &&
           StaleBinding.Error.Contains(TEXT("regenerate bindings")) && First->health == GuardHealth,
                                       TEXT("generated_signature_guard_prevents_side_effects"), StaleBinding.Error);
    {
        const int32 Before = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoNestedHashes\"); ue_arg_array(c, \"Data.Items\", 1); ue_arg_hash(c,\"Data.Items.0.Tags\",0); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("missing")) && First->MarshalledCalls == Before,
                                                 TEXT("nested_hash_missing_header"), H.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoNestedHashes\"); ue_arg_array(c, \"Data.Items\", 1); ue_arg_hash(c,\"Data.Items.0.Values\",1); ue_arg_int(c,\"Data.Items.0.Values.0.key\",5); ue_arg_hash(c,\"Data.Items.0.Tags\",0); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("missing")) && First->MarshalledCalls == Before,
                                                 TEXT("nested_hash_missing_value"), H.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoNestedHashes\"); ue_arg_array(c, \"Data.Items\", 1); ue_arg_hash(c,\"Data.Items.0.Values\",2); ue_arg_int(c,\"Data.Items.0.Values.0.key\",5); ue_arg_string(c,\"Data.Items.0.Values.0.value\",\"a\"); ue_arg_int(c,\"Data.Items.0.Values.1.key\",5); ue_arg_string(c,\"Data.Items.0.Values.1.value\",\"b\"); ue_arg_hash(c,\"Data.Items.0.Tags\",0); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == Before,
                                                 TEXT("nested_hash_duplicate_key"), H.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoNestedHashes\"); ue_arg_array(c, \"Data.Items\", 1); ue_arg_hash(c,\"Data.Items.0.Values\",0); ue_arg_hash(c,\"Data.Items.0.Tags\",2); ue_arg_string(c,\"Data.Items.0.Tags.0\",\"same\"); ue_arg_string(c,\"Data.Items.0.Tags.1\",\"same\"); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == Before,
                                                 TEXT("nested_hash_duplicate_set_element"), H.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoNestedHashes\"); ue_arg_array(c, \"Data.Items\", 1); ue_arg_hash(c,\"Data.Items.0.Values\",1); ue_arg_hash(c,\"Data.Items.0.Tags\",1); ue_arg_array(c,\"Data.Items\",0); ue_invoke_call(c); verify(ue_result_array_size(c,\"ReturnValue.Items\")==0); return 1"));
        Expect(H.bSucceeded && H.Value == 1 && First->MarshalledCalls == Before + 1,
               TEXT("nested_hash_array_resize_discards_pending"), H.Error);
    }
    {
        const int32 Before = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoNestedHashes\"); ue_arg_array(c, \"Data.Items\", 1); ue_arg_hash(c,\"Data.Items.0.Values\",2); ue_arg_int(c,\"Data.Items.0.Values.0.key\",-100); ue_arg_string(c,\"Data.Items.0.Values.0.value\",\"drop\"); ue_arg_int(c,\"Data.Items.0.Values.1.key\",5); ue_arg_string(c,\"Data.Items.0.Values.1.value\",\"Value 世界\"); ue_arg_hash(c,\"Data.Items.0.Tags\",2); ue_arg_string(c,\"Data.Items.0.Tags.0\",\"drop\"); ue_arg_string(c,\"Data.Items.0.Tags.1\",\"kept\"); ue_invoke_call(c); verify(ue_result_hash_size(c,\"ReturnValue.Items.0.Values\")==1); verify(ue_result_int(c,\"ReturnValue.Items.0.Values.0.key\")==5); verify(ue_result_string(c,\"ReturnValue.Items.0.Values.0.value\")==\"Value 世界\"); verify(ue_result_hash_size(c,\"ReturnValue.Items.0.Tags\")==1); verify(ue_result_string(c,\"ReturnValue.Items.0.Tags.0\")==\"kept\"); return 1"));
        Expect(H.bSucceeded && H.Value == 1 && First->MarshalledCalls == Before + 1, TEXT("nested_hash_sparse_output"),
                                                                                          H.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_bad_struct.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: fixture_TransformPayload")),
                                                 TEXT("generated_bad_struct"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_fake_vector.das")), 0, false, First.Get());
        Expect(!G.bSucceeded &&
               G.Error.Contains(TEXT("no matching functions or generics: actor_SetActorRelativeScale3D")),
                                TEXT("generated_fake_vector"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_flat_struct.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: fixture_TransformPayload")),
                                                 TEXT("generated_flat_struct"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_roundtrip.das")), 0, false, First.Get());
        Expect(G.bSucceeded && G.Value == 1, TEXT("class_roundtrip"), G.Error);
    }
    {
        const auto G =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("class_bad_static_receiver.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: actor_GetOwner")),
                                                 TEXT("class_bad_static_receiver"), G.Error);
    }
    {
        const auto G =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("class_bad_static_argument.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: fixture_SetFixtureTarget")),
                                                 TEXT("class_bad_static_argument"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_raw_receiver.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("no matching functions or generics: actor_GetOwner")),
                                                 TEXT("class_raw_receiver"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_forged_receiver.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("requested class")), TEXT("class_forged_receiver"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_null_receiver.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("null")), TEXT("class_null_receiver"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_weak_gc.das")), 0, false, First.Get());
        Expect(G.bSucceeded && G.Value == 1, TEXT("class_weak_gc"), G.Error);
    }
    {
        const auto G = Runtime.RunFile(Runtime.ScriptPath(TEXT("class_expired_cast.das")), 0, false, First.Get());
        Expect(!G.bSucceeded && G.Error.Contains(TEXT("expired")), TEXT("class_expired_cast"), G.Error);
    }
    Runtime.ReleaseObject(First.Get());
    {
        const FString Path = Runtime.ScriptPath(TEXT("class_persistent.das"));
        const auto Created = Runtime.RunPersistent(Path, 0, First.Get());
        Expect(Created.bSucceeded && Created.Value == 1, TEXT("class_persistent_nullable_global_and_factory"),
                                                              Created.Error);
        CollectGarbage(RF_NoFlags);
        const auto TypedExpired = Runtime.RunPersistent(Path, 1, First.Get());
        Expect(TypedExpired.bSucceeded && TypedExpired.Value == 0, TEXT("class_persistent_weak_after_callback"),
                                                                        TypedExpired.Error);
        const auto Rejected = Runtime.RunPersistent(Path, 2, First.Get());
        Expect(!Rejected.bSucceeded && Rejected.Error.Contains(TEXT("expired")),
                                                               TEXT("class_persistent_expired_call_rejected"),
                                                                    Rejected.Error);
    }
    Runtime.ReleaseObject(First.Get());
    {
        const int32 BeforeDefaults = First->MarshalledCalls;
        const auto Defaults =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_defaults.das")), 0, false, First.Get());
        Expect(Defaults.bSucceeded && Defaults.Value == 1 && First->MarshalledCalls == BeforeDefaults + 4,
               TEXT("generated_defaults_omitted_overridden_and_repeated"), Defaults.Error);
        const auto BadDefault =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_defaults_bad_type.das")), 0, false, First.Get());
        Expect(!BadDefault.bSucceeded &&
               BadDefault.Error.Contains(TEXT("no matching functions or generics: fixture_PolicyDefault")),
                                         TEXT("generated_defaults_explicit_type_rejected"), BadDefault.Error);
    }
    {
        const int32 BeforeArrays = First->MarshalledCalls;
        const auto Arrays = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_arrays.das")), 0, false, First.Get());
        Expect(Arrays.bSucceeded && Arrays.Value == 1 && First->MarshalledCalls == BeforeArrays + 8,
               TEXT("generated_arrays_roundtrip_inout_empty_and_reuse"), Arrays.Error);
        const auto BadArray =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_arrays_bad_type.das")), 0, false, First.Get());
        Expect(!BadArray.bSucceeded &&
               BadArray.Error.Contains(TEXT("no matching functions or generics: fixture_TransformInts")),
                                       TEXT("generated_arrays_element_type_rejected"), BadArray.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 2); ue_arg_int(c, \"Values.0\", 1); ue_invoke_call(c); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("missing array element")) &&
                                                 First->MarshalledCalls == BeforeArrayFailure,
                                                 TEXT("array_missing_before_invocation"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.1\", 1); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("out of bounds")) && First->MarshalledCalls == BeforeArrayFailure,
                                                 TEXT("array_bounds_before_invocation"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.-1\", 1); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("invalid array index")) &&
                                                 First->MarshalledCalls == BeforeArrayFailure,
                                                 TEXT("array_negative_index_before_invocation"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 65537); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeArrayFailure,
                                                 TEXT("array_limit_before_invocation"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", -1); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeArrayFailure,
                                                 TEXT("array_negative_size_before_invocation"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_int(c, \"Values.0\", 1); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("initialize array")) &&
                                                 First->MarshalledCalls == BeforeArrayFailure,
                                                 TEXT("array_uninitialized_before_invocation"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 1); ue_arg_string(c, \"Values.0\", \"bad\"); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == BeforeArrayFailure,
                                                 TEXT("array_type_before_invocation"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.0\", 1); ue_arg_array(c, \"Values\", 1); ue_invoke_call(c); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("missing array element")) &&
                                                 First->MarshalledCalls == BeforeArrayFailure,
                                                 TEXT("array_reset_before_invocation"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"ReturnValue\", 0); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("direction")) && First->MarshalledCalls == BeforeArrayFailure,
                                                 TEXT("array_out_direction_before_invocation"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.4294967296\", 1); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("invalid array index")) &&
                                                 First->MarshalledCalls == BeforeArrayFailure + 0,
                                                 TEXT("array_index_overflow"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 0); ue_invoke_call(c); ue_result_int(c, \"ReturnValue.0\"); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("out of bounds")) &&
                                                 First->MarshalledCalls == BeforeArrayFailure + 1,
                                                 TEXT("array_result_bounds"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A =
            ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"OversizedArray\"); ue_invoke_call(c); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("size limit")) &&
                                                 First->MarshalledCalls == BeforeArrayFailure + 1,
                                                 TEXT("array_result_limit"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_result_array_size(c, \"ReturnValue\"); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("results require an executed call")) &&
                                                 First->MarshalledCalls == BeforeArrayFailure + 0,
                                                 TEXT("array_result_before_call"), A.Error);
    }
    {
        const int32 BeforeArrayFailure = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 0); ue_invoke_call(c); ue_release_call(c); ue_result_array_size(c, \"ReturnValue\"); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("released")) && First->MarshalledCalls == BeforeArrayFailure + 1,
                                                 TEXT("array_after_release"), A.Error);
    }
    {
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformInts\"); ue_arg_array(c, \"Values\", 65536); ue_arg_array(c, \"Values\", 0); ue_invoke_call(c); verify(ue_result_array_size(c, \"ReturnValue\") == 0); return 1"));
        Expect(A.bSucceeded && A.Value == 1, TEXT("array_exact_limit_then_empty_reset"), A.Error);
    }
    {
        const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_complex_arrays.das")), 0, false, First.Get());
        Expect(A.bSucceeded && A.Value == 1, TEXT("generated_complex_arrays"), A.Error);
    }
    {
        const auto A =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_complex_arrays_bad_object.das")), 0, false, First.Get());
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("no matching functions or generics: fixture_EchoActors")),
                                                 TEXT("generated_complex_arrays_bad_object"), A.Error);
    }
    {
        const auto A =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_complex_arrays_bad_struct.das")), 0, false, First.Get());
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("no matching functions or generics: fixture_TransformPayloads")),
                                                 TEXT("generated_complex_arrays_bad_struct"), A.Error);
    }
    {
        const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_complex_arrays_forged_object.das")), 0, false,
                                                          First.Get());
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("requested class")),
                                                 TEXT("generated_complex_arrays_forged_object"), A.Error);
    }
    {
        const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("array_gc_output_pins.das")), 0, false, First.Get());
        Expect(A.bSucceeded && A.Value == 1, TEXT("array_gc_output_pins"), A.Error);
    }
    {
        const auto A =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("array_gc_reset_releases_input.das")), 0, false, First.Get());
        Expect(A.bSucceeded && A.Value == 1, TEXT("array_gc_reset_releases_input"), A.Error);
    }
    {
        const auto A =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("array_gc_inout_replaces_pins.das")), 0, false, First.Get());
        Expect(A.bSucceeded && A.Value == 1, TEXT("array_gc_inout_replaces_pins"), A.Error);
    }
    {
        const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("array_gc_vm_boundary.das")), 0, false, First.Get());
        Expect(A.bSucceeded && A.Value == 1, TEXT("array_gc_vm_boundary"), A.Error);
    }
    {
        const auto A =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("array_gc_exception_fixture.das")), 0, false, First.Get());
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("array pin cleanup fixture")), TEXT("array_gc_exception_fixture"),
                                                                                          A.Error);
    }
    {
        const int32 BeforeComplexArray = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformPayloads\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.0.Count\", 1); ue_invoke_call(c); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("missing array element")) &&
                                                 First->MarshalledCalls == BeforeComplexArray,
                                                 TEXT("complex_array_struct_incomplete"), A.Error);
    }
    {
        const int32 BeforeComplexArray = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformPayloads\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.0.Metrics.Ratio\", 1); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == BeforeComplexArray,
                                                 TEXT("complex_array_struct_wrong_leaf"), A.Error);
    }
    {
        const int32 BeforeComplexArray = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformPayloads\"); ue_arg_array(c, \"Values\", 1); ue_arg_int(c, \"Values.1.Count\", 1); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("out of bounds")) && First->MarshalledCalls == BeforeComplexArray,
                                                 TEXT("complex_array_struct_bounds"), A.Error);
    }
    {
        const int32 BeforeComplexArray = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoActors\"); ue_arg_array(c, \"Values\", 1); ue_arg_object(c, \"Values.0\", ue_self_object()); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("incompatible class")) &&
                                                 First->MarshalledCalls == BeforeComplexArray,
                                                 TEXT("complex_array_object_class"), A.Error);
    }
    {
        const int32 BeforeComplexArray = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoFixtureObjects\"); ue_arg_array(c, \"Values\", 1); ue_invoke_call(c); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("missing array element")) &&
                                                 First->MarshalledCalls == BeforeComplexArray,
                                                 TEXT("complex_array_object_missing"), A.Error);
    }
    {
        const int32 BeforeComplexArray = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoModes\"); ue_arg_array(c, \"Values\", 1); ue_arg_enum(c, \"Values.0\", \"/Script/Engine.ETickingGroup\", \"TG_PrePhysics\"); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("type path mismatch")) &&
                                                 First->MarshalledCalls == BeforeComplexArray,
                                                 TEXT("complex_array_enum_type"), A.Error);
    }
    {
        const int32 BeforeComplexArray = First->MarshalledCalls;
        const auto A = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoModes\"); ue_arg_array(c, \"Values\", 1); ue_arg_enum(c, \"Values.0\", \"/Script/DasRuntime.EDasCallMode\", \"Unknown\"); return 1"));
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("unknown or reserved")) &&
                                                 First->MarshalledCalls == BeforeComplexArray,
                                                 TEXT("complex_array_enum_symbol"), A.Error);
    }
    CollectGarbage(RF_NoFlags);
    Expect(!First->LastArrayCreated.IsValid(), TEXT("array_output_pins_released_after_VM_exception"));
    {
        const auto A = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_complex_arrays_expired_object.das")), 0,
                                                          false, First.Get());
        Expect(!A.bSucceeded && A.Error.Contains(TEXT("expired")), TEXT("complex_array_expired_object_rejected"),
                                                                        A.Error);
    }
    Runtime.ReleaseObject(First.Get());
    {
        const FString ArrayPath = Runtime.ScriptPath(TEXT("array_gc_persistent.das"));
        const auto CreatedArray = Runtime.RunPersistent(ArrayPath, 0, First.Get());
        Expect(CreatedArray.bSucceeded && CreatedArray.Value == 3, TEXT("object_array_persistent_global_created"),
                                                                        CreatedArray.Error);
        CollectGarbage(RF_NoFlags);
        const auto ExpiredArray = Runtime.RunPersistent(ArrayPath, 1, First.Get());
        Expect(ExpiredArray.bSucceeded && ExpiredArray.Value == 0, TEXT("object_array_persistent_global_is_weak"),
                                                                        ExpiredArray.Error);
        const auto RejectedArray = Runtime.RunPersistent(ArrayPath, 2, First.Get());
        Expect(!RejectedArray.bSucceeded &&
               RejectedArray.Error.Contains(TEXT("expired")), TEXT("object_array_persistent_expired_input_rejected"),
                                                                   RejectedArray.Error);
    }
    Runtime.ReleaseObject(First.Get());
    {
        const auto H =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_hash_containers.das")), 0, false, First.Get());
        Expect(H.bSucceeded && H.Value == 1, TEXT("generated_hash_containers"), H.Error);
    }
    {
        const auto H = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_hash_bad_type.das")), 0, false, First.Get());
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("no matching functions or generics: fixture_TransformMap")),
                                                 TEXT("generated_hash_bad_type"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"Values\", 2); ue_arg_int(c, \"Values.0.key\", 1); ue_arg_string(c, \"Values.0.value\", \"a\"); ue_arg_int(c, \"Values.1.key\", 1); ue_arg_string(c, \"Values.1.value\", \"b\"); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("map_duplicate"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.0.key\", 1); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("missing map")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("map_missing_value"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 2); ue_arg_int(c, \"Values.0\", 1); ue_arg_int(c, \"Values.1\", 1); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("set_duplicate"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 1); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("missing map")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("set_missing"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_int(c, \"Values.0.key\", 1); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("initialize hash")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_uninitialized"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.1\", 1); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("out of bounds")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_bounds"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.-1\", 1); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("invalid hash index")) &&
                                                 First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_negative_index"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.4294967296\", 1); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("invalid hash index")) &&
                                                 First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_overflow_index"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"Values\", 65537); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_limit"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", -1); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_negative_size"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"Values\", 1); ue_arg_string(c, \"Values.0.key\", \"bad\"); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("type mismatch")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_wrong_type"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformSet\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.0\", 1); ue_arg_hash(c, \"Values\", 1); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("missing map")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_reset"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"ReturnValue\", 0); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("direction")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_direction"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_result_hash_size(c, \"ReturnValue\"); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("results require an executed call")) &&
                                                 First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_early_result"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"MakeMap\"); ue_invoke_call(c); ue_release_call(c); ue_result_hash_size(c, \"Values\"); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("released")) && First->MarshalledCalls == BeforeHash + 1,
                                                 TEXT("hash_released"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H =
            ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"OversizedMap\"); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeHash + 1,
                                                 TEXT("hash_map_result_limit"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H =
            ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"OversizedSet\"); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("size limit")) && First->MarshalledCalls == BeforeHash + 1,
                                                 TEXT("hash_set_result_limit"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"MakeMap\"); ue_invoke_call(c); ue_result_int(c, \"Values.1.key\"); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("out of bounds")) && First->MarshalledCalls == BeforeHash + 1,
                                                 TEXT("hash_result_bounds"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoNameSet\"); ue_arg_hash(c, \"Values\", 2); ue_arg_name(c, \"Values.0\", \"Mixed_17\"); ue_arg_name(c, \"Values.1\", \"mixed_17\"); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_name_duplicate"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoNameModeMap\"); ue_arg_hash(c, \"Values\", 2); ue_arg_name(c, \"Values.0.key\", \"Mixed_17\"); ue_arg_name(c, \"Values.1.key\", \"mixed_17\"); ue_arg_enum(c, \"Values.0.value\", \"/Script/DasRuntime.EDasCallMode\", \"Active\"); ue_arg_enum(c, \"Values.1.value\", \"/Script/DasRuntime.EDasCallMode\", \"Paused\"); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == BeforeHash + 0,
                                                 TEXT("hash_name_map_duplicate"), H.Error);
    }
    {
        const auto H = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_hash_numbers.das")), 0, false, First.Get());
        Expect(H.bSucceeded && H.Value == 1, TEXT("generated_hash_numbers_finite_and_precision"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoFloatKeyMap\"); ue_arg_hash(c, \"Values\", 2); ue_arg_float(c, \"Values.0.key\", 0.0); ue_arg_bool(c, \"Values.0.value\", true); ue_arg_float(c, \"Values.1.key\", -0.0); ue_arg_bool(c, \"Values.1.value\", false); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("duplicate")) && First->MarshalledCalls == BeforeHash,
                                                 TEXT("hash_signed_zero_duplicate"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"EchoFloatSet\"); ue_arg_hash(c, \"Values\", 1); ue_arg_float(c, \"Values.0\", unsafe(reinterpret<float>(0x7FC00000u))); ue_invoke_call(c); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("must be finite")) && First->MarshalledCalls == BeforeHash,
                                                 TEXT("hash_nan_key_rejected"), H.Error);
    }
    {
        const int32 BeforeHash = First->MarshalledCalls;
        const auto H = ObjectRun(TEXT("let c = ue_begin_call(ue_self_object(), \"TransformMap\"); ue_arg_hash(c, \"Values\", 1); ue_arg_int(c, \"Values.0\", 1); return 1"));
        Expect(!H.bSucceeded && H.Error.Contains(TEXT("requires key or value")) && First->MarshalledCalls == BeforeHash,
                                                 TEXT("hash_map_suffix_required"), H.Error);
    }
    {
        const auto H =
            Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_hash_failed_result.das")), 0, false, First.Get());
        Expect(H.bSucceeded && H.Value == 1, TEXT("hash_failed_results_blocked_after_recover"), H.Error);
    }
    {
        const FString NamedPath = Runtime.ScriptPath(TEXT("named_entries.das"));
        FString EntryError;
        Expect(Runtime.ValidateIntMethod(NamedPath, EntryError, TEXT("alpha")) &&
                                         !Runtime.ValidateIntMethod(NamedPath, EntryError, TEXT("wrong")),
                                                                    TEXT("named_entry_compile_signature_preflight"),
                                                                         EntryError);
        Runtime.ReleaseObject(First.Get());
        const auto Alpha = Runtime.RunNamedPersistent(NamedPath, TEXT("alpha"), 0, First.Get());
        const auto Beta = Runtime.RunNamedPersistent(NamedPath, TEXT("beta"), 0, First.Get());
        const auto AlphaAgain = Runtime.RunNamedPersistent(NamedPath, TEXT("alpha"), 0, First.Get());
        Expect(Alpha.bSucceeded && Alpha.Value == 1 && Beta.bSucceeded && Beta.Value == 12 && AlphaAgain.Value == 3,
               TEXT("named_entries_share_globals_without_main"));
        Expect(!Runtime.RunNamedPersistent(NamedPath, TEXT("wrong"), 0, First.Get()).bSucceeded,
                                           TEXT("named_entry_wrong_signature_rejected"));
        Expect(!Runtime.RunNamedPersistent(NamedPath, TEXT("missing"), 0, First.Get()).bSucceeded,
                                           TEXT("named_entry_missing_rejected"));
        Expect(Runtime.RunNamedPersistent(NamedPath, TEXT("alpha"), 0, First.Get()).Value == 4,
                                          TEXT("named_entry_error_keeps_working_context"));
        Runtime.ReleaseObject(First.Get());
    }
    {
        Runtime.ReleaseObject(First.Get());
        const FString NamedSource = TEXT("options gen2\n[export]\ndef alpha(input:int):int { return input+7 }\n[export]\ndef beta(input:int):int { return input+9 }\n");
        Write(NamedSource);
        const auto NamedFirst = Runtime.RunNamedPersistent(Scratch, TEXT("alpha"), 0, First.Get());
        Write(TEXT("invalid replacement"));
        const auto NamedFallback = Runtime.RunNamedPersistent(Scratch, TEXT("beta"), 0, First.Get());
        Expect(NamedFirst.bSucceeded && NamedFirst.Value == 7 && NamedFallback.bSucceeded && NamedFallback.Value == 9 &&
                   !NamedFallback.ReloadError.IsEmpty(),
               TEXT("named_entry_reload_keeps_previous_version"), NamedFallback.Error);
        Write(NamedSource.Replace(TEXT("+7"), TEXT("+17")).Replace(TEXT("+9"), TEXT("+19")));
        const auto NamedRepaired = Runtime.RunNamedPersistent(Scratch, TEXT("alpha"), 0, First.Get());
        Expect(NamedRepaired.bSucceeded && NamedRepaired.Value == 17 && NamedRepaired.Revision == 2 &&
                   NamedRepaired.ReloadError.IsEmpty(),
               TEXT("named_entry_reload_recovers"), NamedRepaired.Error);
        Write(TEXT("options gen2\n[export]\ndef alpha(input:int):int { return input+100 }\n[export]\ndef beta(input:float):float { return input }\n"));
        const auto CrossEntryFallback = Runtime.RunNamedPersistent(Scratch, TEXT("alpha"), 0, First.Get());
        Expect(CrossEntryFallback.bSucceeded && CrossEntryFallback.Value == 17 &&
                   !CrossEntryFallback.ReloadError.IsEmpty(),
               TEXT("named_reload_checks_previously_used_other_entry"), CrossEntryFallback.Error);
        const auto SignatureFallback = Runtime.RunNamedPersistent(Scratch, TEXT("beta"), 0, First.Get());
        Expect(SignatureFallback.bSucceeded && SignatureFallback.Value == 19 &&
                   !SignatureFallback.ReloadError.IsEmpty(),
               TEXT("named_entry_wrong_signature_reload_fallback"), SignatureFallback.Error);
        Runtime.ReleaseObject(First.Get());
    }
    {
        FDasAuthoringModule DSL;
        FString DSLError;
        const bool Read = Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_counter_v1.das")), DSL, DSLError);
        Expect(Read && DSL.Id == TEXT("example") && DSL.Classes.Num() == 1 && DSL.Classes[0].Properties.Num() == 2 &&
                                      DSL.Classes[0].Methods.Num() == 1,
                                      TEXT("dsl_annotations_collect_model"), DSLError);
        const auto* DSLScore = Read ? DSL.Classes[0].Properties.FindByPredicate(
                                          [](const auto& P) { return P.Id == TEXT("example.counter.score"); })
                                    : nullptr;
        Expect(Read && DSL.Classes[0].Methods[0].EntryPoint ==
                           TEXT("ue_Counter_Evaluate") && DSLScore &&
                                DSLScore->Metadata.FindRef(TEXT("ToolTip")) ==
                                                           TEXT("Score tooltip"),
                                                                TEXT("dsl_ids_metadata_and_generated_entry"));
        TStrongObjectPtr<UDasObjectFixture> DSLObject(NewObject<UDasObjectFixture>());
        const auto Updated = Runtime.RunNamedPersistent(
            Runtime.ScriptPath(TEXT("dsl_fixture.das")), TEXT("ue_ScalarFixture_Update"), 5, DSLObject.Get());
        Expect(Updated.bSucceeded && Updated.Value == 10 && DSLObject->health == 10 && DSLObject->speed == 3.75f,
               TEXT("dsl_self_assignment_compound_and_float"), Updated.Error);
        FString DSLSource;
        FFileHelper::LoadFileToString(DSLSource, *Runtime.ScriptPath(TEXT("dsl_fixture.das")));
        DSLSource = DSLSource.Replace(TEXT("require ue_dsl"), TEXT("require ue_foundation"));
        Write(DSLSource);
        const auto DSLInitial =
            Runtime.RunNamedPersistent(Scratch, TEXT("ue_ScalarFixture_Update"), 5, DSLObject.Get());
        const FString DSLBody = DSLSource.Replace(TEXT("return self.health"), TEXT("return self.health + 100"));
        Write(DSLBody);
        const auto DSLReload = Runtime.RunNamedPersistent(Scratch, TEXT("ue_ScalarFixture_Update"), 5, DSLObject.Get());
        Expect(DSLInitial.bSucceeded && DSLInitial.Value == 10 && DSLReload.bSucceeded && DSLReload.Value == 110 &&
                   DSLReload.Revision == 2,
               TEXT("dsl_runtime_body_reload"), DSLReload.Error);
        Write(TEXT("broken DSL"));
        const auto DSLBroken = Runtime.RunNamedPersistent(Scratch, TEXT("ue_ScalarFixture_Update"), 5, DSLObject.Get());
        Expect(DSLBroken.bSucceeded && DSLBroken.Value == 110 && !DSLBroken.ReloadError.IsEmpty(),
               TEXT("dsl_runtime_syntax_fallback"));
        Write(DSLBody.Replace(TEXT("health : int = 10"), TEXT("health : int = 11")));
        const auto DSLDeclaration =
            Runtime.RunNamedPersistent(Scratch, TEXT("ue_ScalarFixture_Update"), 5, DSLObject.Get());
        Expect(DSLDeclaration.bSucceeded && DSLDeclaration.Value == 110 &&
               DSLDeclaration.ReloadError.Contains(TEXT("declaration changed")),
                                                   TEXT("dsl_runtime_declaration_gate_fallback"));
        Write(DSLBody.Replace(
            TEXT("health : int = 10"),
                 TEXT("missingHealth : int = 10")).Replace(TEXT("self.health"), TEXT("self.missingHealth")));
        const auto DSLOwnerMismatch =
            Runtime.RunNamedPersistent(Scratch, TEXT("ue_ScalarFixture_Update"), 5, DSLObject.Get());
        Expect(DSLOwnerMismatch.bSucceeded && DSLOwnerMismatch.Value == 110 &&
               DSLOwnerMismatch.ReloadError.Contains(TEXT("property differs")),
                                                     TEXT("dsl_runtime_owner_mismatch_fallback"));
        Write(DSLBody);
        const auto DSLRecovery =
            Runtime.RunNamedPersistent(Scratch, TEXT("ue_ScalarFixture_Update"), 5, DSLObject.Get());
        Expect(DSLRecovery.bSucceeded && DSLRecovery.Value == 110 && DSLRecovery.ReloadError.IsEmpty(),
               TEXT("dsl_runtime_reload_recovery"));
        Runtime.ReleaseObject(DSLObject.Get());

        Expect(!Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_bad_unknown_flag.das")), DSL, DSLError) &&
            DSLError.Contains(TEXT("Unknown ue_class")) &&
                              DSL.Id == TEXT("example") && DSL.Classes.Num() == 1,
                                             TEXT("dsl_unknown_flag_reject_preserve_model"), DSLError);
        Expect(!Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_bad_conflicting_flags.das")), DSL, DSLError) &&
            DSLError.Contains(TEXT("Conflicting")) &&
                              DSL.Id == TEXT("example") && DSL.Classes.Num() == 1,
                                             TEXT("dsl_conflicting_flags_reject_preserve_model"), DSLError);
        Expect(!Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_bad_unsupported_type.das")), DSL, DSLError) &&
            DSLError.Contains(TEXT("int/float")) && DSL.Id == TEXT("example") && DSL.Classes.Num() == 1,
                                                                   TEXT("dsl_unsupported_type_reject_preserve_model"),
                                                                        DSLError);
        Expect(!Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_bad_unsupported_signature.das")), DSL, DSLError) &&
            DSLError.Contains(TEXT("Unsupported parameter")) &&
                              DSL.Id == TEXT("example") && DSL.Classes.Num() == 1,
                                             TEXT("dsl_unsupported_signature_reject_preserve_model"), DSLError);
        Expect(!Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_bad_readonly_write.das")), DSL, DSLError) &&
            DSLError.Contains(TEXT("Cannot write")) && DSL.Id == TEXT("example") && DSL.Classes.Num() == 1,
                                                                      TEXT("dsl_readonly_write_reject_preserve_model"),
                                                                           DSLError);
        Expect(!Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_bad_reserved_argument.das")), DSL, DSLError) &&
            DSLError.Contains(TEXT("Reserved internal")) &&
                              DSL.Id == TEXT("example") && DSL.Classes.Num() == 1,
                                             TEXT("dsl_reserved_argument_reject_preserve_model"), DSLError);
        Expect(!Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_bad_private_field.das")), DSL, DSLError) &&
            DSLError.Contains(TEXT("Script-private")) && DSL.Id == TEXT("example") && DSL.Classes.Num() == 1,
                                                                        TEXT("dsl_private_field_reject_preserve_model"),
                                                                             DSLError);
    }
    {
        const FString Path = Runtime.ScriptPath(TEXT("dsl_scalar_methods.das"));
        FString Error;
        FDasAuthoringModule ScalarModel;
        Expect(Runtime.ReadAuthoringScript(Path, ScalarModel, Error) && ScalarModel.Classes.Num() == 1 &&
                   ScalarModel.Classes[0].Methods.Num() == 4 && ScalarModel.Classes[0].Properties.Num() == 3,
               TEXT("dsl_typed_model_collect"), Error);
        const auto* ScalarBool = ScalarModel.Classes.Num() == 1
                                     ? ScalarModel.Classes[0].Properties.FindByPredicate(
                                           [](const auto& P) { return P.Name == TEXT("bScalar"); })
                                     : nullptr;
        Expect(ScalarBool && ScalarBool->Type.Kind == TEXT("bool") && ScalarBool->Default.Kind ==
                                                                          TEXT("bool") && ScalarBool->Default.Boolean,
                                                                               TEXT("dsl_bool_literal_true_model"));
        Expect(Runtime.ValidateScalarMethod(
            Path, Error, TEXT("ue_TypedFixture_Scale"), EDasMethodSignature::Float) &&
            Runtime.ValidateScalarMethod(Path, Error, TEXT("ue_TypedFixture_Toggle"), EDasMethodSignature::Bool) &&
                                         Runtime.ValidateScalarMethod(
                                             Path, Error, TEXT("ue_TypedFixture_Reset"), EDasMethodSignature::Void),
                                             TEXT("dsl_typed_compile_preflight"), Error);
        TStrongObjectPtr<UDasObjectFixture> Object(NewObject<UDasObjectFixture>());
        Object->speed = 1.5f;
        Object->health = 10;
        const auto Float = Runtime.RunNamedFloatPersistent(Path, TEXT("ue_TypedFixture_Scale"), 2.25f, Object.Get());
        const auto Bool = Runtime.RunNamedBoolPersistent(Path, TEXT("ue_TypedFixture_Toggle"), true, Object.Get());
        const auto Int = Runtime.RunNamedPersistent(Path, TEXT("ue_TypedFixture_Check"), 5, Object.Get());
        Expect(Float.bSucceeded && Float.FloatValue == 3.75f && Bool.bSucceeded && Bool.BoolValue && Object->bScalar &&
                   Int.bSucceeded && Int.Value == 15,
               TEXT("dsl_typed_runtime_shared_owner"), Float.Error + Bool.Error + Int.Error);
        const auto Wrong = Runtime.RunNamedPersistent(Path, TEXT("ue_TypedFixture_Scale"), 0, Object.Get());
        Expect(!Wrong.bSucceeded &&
               Runtime.RunNamedBoolPersistent(Path, TEXT("ue_TypedFixture_Toggle"), false, Object.Get()).bSucceeded,
                                              TEXT("dsl_typed_wrong_signature_keeps_context"));
        const auto Void = Runtime.RunNamedVoidPersistent(Path, TEXT("ue_TypedFixture_Reset"), Object.Get());
        Expect(Void.bSucceeded && Object->health == 0 && !Object->bScalar && Object->speed == 3.75f,
               TEXT("dsl_void_action_and_bool_writes"), Void.Error);
        Runtime.ReleaseObject(Object.Get());
        FString Source;
        FFileHelper::LoadFileToString(Source, *Path);
        Source = Source.Replace(TEXT("require ue_dsl"), TEXT("require ue_foundation"));
        Write(Source);
        Runtime.RunNamedFloatPersistent(Scratch, TEXT("ue_TypedFixture_Scale"), 1.f, Object.Get());
        Runtime.RunNamedBoolPersistent(Scratch, TEXT("ue_TypedFixture_Toggle"), true, Object.Get());
        Write(Source.Replace(TEXT("def Toggle(input:bool):bool { self.bScalar = input; return self.bScalar }"),
                                  TEXT("def Toggle(input:int):int { return input }")));
        const auto Kept = Runtime.RunNamedFloatPersistent(Scratch, TEXT("ue_TypedFixture_Scale"), 1.f, Object.Get());
        Expect(Kept.bSucceeded && !Kept.ReloadError.IsEmpty() &&
               Runtime.RunNamedBoolPersistent(Scratch, TEXT("ue_TypedFixture_Toggle"), false, Object.Get()).bSucceeded,
                                              TEXT("dsl_reload_checks_other_scalar_signatures"));
        Runtime.ReleaseObject(Object.Get());
        const FString FreeTyped = TEXT("options gen2\nvar calls=0\n[export]\ndef alpha(input:float):float { calls+=1; return input+float(calls) }\n[export]\ndef beta(input:bool):bool { calls+=1; return input && calls==2 }\n");
        Write(FreeTyped);
        const auto FreeFloat = Runtime.RunNamedFloatPersistent(Scratch, TEXT("alpha"), 2.5f, Object.Get());
        const auto FreeBool = Runtime.RunNamedBoolPersistent(Scratch, TEXT("beta"), true, Object.Get());
        Expect(FreeFloat.bSucceeded && FreeFloat.FloatValue == 3.5f && FreeBool.bSucceeded && FreeBool.BoolValue,
               TEXT("typed_entries_share_VM_globals"));
        Write(FreeTyped.Replace(TEXT("def beta(input:bool):bool { calls+=1; return input && calls==2 }"),
                                     TEXT("def beta(input:int):int { return input }")));
        const auto TypedFallback = Runtime.RunNamedFloatPersistent(Scratch, TEXT("alpha"), 2.5f, Object.Get());
        Expect(TypedFallback.bSucceeded && TypedFallback.FloatValue == 5.5f &&
               TypedFallback.ReloadError.Contains(TEXT("Previously used entry point signature changed: beta")),
                                                  TEXT("typed_reload_preserves_other_entry_contract"));
        Runtime.ReleaseObject(Object.Get());
    }
    {
        const FString MixedPath = Runtime.ScriptPath(TEXT("dsl_mixed_methods.das"));
        FString MixedError;
        FDasAuthoringModule MixedModel;
        const bool ReadMixed = Runtime.ReadAuthoringScript(MixedPath, MixedModel, MixedError);
        FDasClassSchema MixedSchema;
        Expect(ReadMixed && MixedModel.ToActorSchema(TEXT("mixed.MixedCounter"), MixedSchema, MixedError) &&
                                                     MixedSchema.Methods.Num() == 5,
                                                     TEXT("mixed_DSL_auto_ids_model"), MixedError);
        const auto* Compute =
            ReadMixed ? MixedSchema.Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Compute"); })
                      : nullptr;
        Expect(Compute && Compute->Parameters.Num() == 3 &&
               Compute->Parameters[0].Name ==
                   TEXT("count") && Compute->Parameters[0].StableId ==
                                        TEXT("mixed.MixedCounter.Compute.count") &&
                                             Runtime.ValidateFrameMethod(MixedPath, MixedError, Compute->EntryPoint,
                                                                         Compute->Contract()),
                                             TEXT("mixed_frame_preflight_and_parameter_identity"), MixedError);
        TStrongObjectPtr<UDasObjectFixture> MixedObject(NewObject<UDasObjectFixture>());
        MixedObject->health = 10;
        MixedObject->speed = 1.5f;
        FDasScalarValue A;
        A.IntValue = 2;
        FDasScalarValue B;
        B.Kind = 1;
        B.FloatValue = 0.5f;
        FDasScalarValue C;
        C.Kind = 2;
        C.BoolValue = true;
        const auto MixedResult = Runtime.RunNamedFramePersistent(
            MixedPath, TEXT("ue_MixedCounter_Compute"), {A, B, C}, EDasMethodSignature::Float, MixedObject.Get());
        Expect(MixedResult.bSucceeded && MixedResult.FloatValue == 24.f && MixedObject->health == 12 &&
                   MixedObject->speed == 2.f,
               TEXT("mixed_frame_runtime_order_and_types"), MixedResult.Error);
        const auto Empty = Runtime.RunNamedFramePersistent(MixedPath, TEXT("ue_MixedCounter_ReadScore"), {},
                                                                           EDasMethodSignature::Int, MixedObject.Get());
        const auto Changed = Runtime.RunNamedFramePersistent(
            MixedPath, TEXT("ue_MixedCounter_BoolAsInt"), {C}, EDasMethodSignature::Int, MixedObject.Get());
        Expect(Empty.bSucceeded && Empty.Value == 12 && Changed.bSucceeded && Changed.Value == 12,
               TEXT("frame_zero_args_and_mixed_return"));
        const auto WrongOrder = Runtime.RunNamedFramePersistent(
            MixedPath, TEXT("ue_MixedCounter_Compute"), {B, A, C}, EDasMethodSignature::Float, MixedObject.Get());
        const auto MissingArgs = Runtime.RunNamedFramePersistent(
            MixedPath, TEXT("ue_MixedCounter_Compute"), {A, B}, EDasMethodSignature::Float, MixedObject.Get());
        Expect(!WrongOrder.bSucceeded && !MissingArgs.bSucceeded && MixedObject->health == 12,
               TEXT("frame_bad_arity_and_order_no_execution"));
        A.IntValue = 7;
        const auto Action = Runtime.RunNamedFramePersistent(
            MixedPath, TEXT("ue_MixedCounter_ResetWith"), {A, C}, EDasMethodSignature::Void, MixedObject.Get());
        Expect(Action.bSucceeded && MixedObject->health == 7 && MixedObject->speed == 1.5f,
               TEXT("frame_void_with_inputs"), Action.Error);
        Runtime.ReleaseObject(MixedObject.Get());
        const auto BadCold = Runtime.RunNamedFramePersistent(
            MixedPath, TEXT("ue_MixedCounter_Compute"), {A}, EDasMethodSignature::Float, MixedObject.Get());
        A.IntValue = 2;
        const auto RecoveredCold = Runtime.RunNamedFramePersistent(
            MixedPath, TEXT("ue_MixedCounter_Compute"), {A, B, C}, EDasMethodSignature::Float, MixedObject.Get());
        Expect(!BadCold.bSucceeded && RecoveredCold.bSucceeded,
               TEXT("frame_wrong_first_contract_recovers_without_source_edit"), RecoveredCold.Error);
        FDasScalarValue Invalid;
        Invalid.Kind = 99;
        Expect(!Runtime.RunNamedFramePersistent(
            MixedPath,
            TEXT("ue_MixedCounter_Compute"), {Invalid, B, C}, EDasMethodSignature::Float, MixedObject.Get()).bSucceeded,
            TEXT("frame_invalid_tag_rejected"));
        TArray<FDasScalarValue> TooMany;
        TooMany.SetNum(17);
        Expect(!Runtime.RunNamedFramePersistent(
            MixedPath,
            TEXT("ue_MixedCounter_Compute"), TooMany, EDasMethodSignature::Float, MixedObject.Get()).bSucceeded,
            TEXT("frame_arity_limit_rejected"));
        Runtime.ReleaseObject(MixedObject.Get());
        const FString FrameSource = TEXT("options gen2\nvar calls=0\n[export]\ndef calc(a:int;b:float;c:bool):float { calls+=1; if(c){return float(a)+b+float(calls)};return 0.0 }\n[export]\ndef gate(a:bool;b:int):bool { return a && b>0 }\n");
        Write(FrameSource);
        Runtime.RunNamedFramePersistent(Scratch, TEXT("calc"), {A, B, C}, EDasMethodSignature::Float,
                                                      MixedObject.Get());
        Runtime.RunNamedFramePersistent(Scratch, TEXT("gate"), {C, A}, EDasMethodSignature::Bool, MixedObject.Get());
        Write(FrameSource.Replace(TEXT("gate(a:bool;b:int):bool { return a && b>0 }"),
                                       TEXT("gate(a:int;b:bool):bool { return b && a>0 }")));
        const auto FrameFallback = Runtime.RunNamedFramePersistent(
            Scratch, TEXT("calc"), {A, B, C}, EDasMethodSignature::Float, MixedObject.Get());
        Expect(FrameFallback.bSucceeded && FrameFallback.FloatValue == 4.5f &&
               FrameFallback.ReloadError.Contains(
                   TEXT("Previously used entry point signature changed: gate")),
                   TEXT("frame_reload_validates_other_mixed_contract_and_preserves_globals"), FrameFallback.Error);
        Runtime.ReleaseObject(MixedObject.Get());
    }
    {
        const FString TextPath = Runtime.ScriptPath(TEXT("dsl_text_methods.das"));
        FString TextError;
        FDasAuthoringModule TextModel;
        Expect(Runtime.ReadAuthoringScript(TextPath, TextModel, TextError) && TextModel.Classes.Num() == 1 &&
                   TextModel.Classes[0].Properties.Num() == 3,
               TEXT("text_double_nominal_name_DSL_model"), TextError);
        TStrongObjectPtr<UDasObjectFixture> TextObject(NewObject<UDasObjectFixture>());
        FDasScalarValue S;
        S.Kind = 5;
        S.StringValue = TEXT("Привет 世界");
        FDasScalarValue T;
        T.Kind = 5;
        T.StringValue = TEXT(" !");
        const auto Echo = Runtime.RunNamedFramePersistent(TextPath, TEXT("ue_TextCounter_Echo"), {S, T},
                                                                         EDasMethodSignature::String, TextObject.Get());
        Expect(Echo.bSucceeded && Echo.StringValue == TEXT("Привет 世界 !") && TextObject->DSLText == Echo.StringValue,
                                                           TEXT("frame_unicode_string_owned_result"), Echo.Error);
        S.StringValue = TEXT("changed host buffer");
        const auto Recall = Runtime.RunNamedFramePersistent(
            TextPath, TEXT("ue_TextCounter_Recall"), {}, EDasMethodSignature::String, TextObject.Get());
        Expect(Recall.bSucceeded &&
               Recall.StringValue == TEXT("Привет 世界") &&
                                          Echo.StringValue ==
                                              TEXT("Привет 世界 !"),
                                                   TEXT("frame_string_input_survives_host_change_and_return"),
                                                        Recall.Error);
        FDasScalarValue D;
        D.Kind = 6;
        D.DoubleValue = 9007199254740990.0;
        FDasScalarValue Delta;
        Delta.Kind = 6;
        Delta.DoubleValue = 1.0;
        const auto Precise = Runtime.RunNamedFramePersistent(
            TextPath, TEXT("ue_TextCounter_Precise"), {D, Delta}, EDasMethodSignature::Double, TextObject.Get());
        Expect(Precise.bSucceeded && Precise.DoubleValue == 9007199254740991.0 &&
                   TextObject->DSLPrecise == Precise.DoubleValue,
               TEXT("frame_double_64bit_precision"), Precise.Error);
        FDasScalarValue N;
        N.Kind = 7;
        N.NameValue = FName(TEXT("Item_42"));
        FDasScalarValue Lower;
        Lower.Kind = 7;
        Lower.NameValue = FName(TEXT("item_42"));
        const auto Rename = Runtime.RunNamedFramePersistent(TextPath, TEXT("ue_TextCounter_Rename"), {N},
                                                                           EDasMethodSignature::Name, TextObject.Get());
        const auto Equal = Runtime.RunNamedFramePersistent(TextPath, TEXT("ue_TextCounter_NamesEqual"), {N, Lower},
                                                                          EDasMethodSignature::Bool, TextObject.Get());
        Expect(Rename.bSucceeded && Rename.NameValue == N.NameValue && TextObject->DSLKey == N.NameValue &&
                   Equal.bSucceeded && Equal.BoolValue,
               TEXT("frame_name_case_number_semantics"), Rename.Error + Equal.Error);
        N.NameValue = NAME_None;
        const auto None = Runtime.RunNamedFramePersistent(TextPath, TEXT("ue_TextCounter_Rename"), {N},
                                                                         EDasMethodSignature::Name, TextObject.Get());
        Expect(None.bSucceeded && None.NameValue.IsNone(), TEXT("frame_name_none_roundtrip"), None.Error);
        Expect(!Runtime.RunNamedFramePersistent(
            TextPath, TEXT("ue_TextCounter_Rename"), {T}, EDasMethodSignature::Name, TextObject.Get()).bSucceeded,
            TEXT("name_string_nominal_types_distinct"));
        Runtime.ReleaseObject(TextObject.Get());
        FString TextSource;
        FFileHelper::LoadFileToString(TextSource, *TextPath);
        TextSource = TextSource.Replace(TEXT("require ue_dsl"), TEXT("require ue_foundation"));
        Write(TextSource);
        S.StringValue = TEXT("Unicode 世界");
        T.StringValue = TEXT(" !");
        const auto OldText = Runtime.RunNamedFramePersistent(
            Scratch, TEXT("ue_TextCounter_Echo"), {S, T}, EDasMethodSignature::String, TextObject.Get());
        Write(TextSource.Replace(TEXT("return self.DSLText"), TEXT("return self.DSLText + \" v2\"")));
        const auto NewText = Runtime.RunNamedFramePersistent(
            Scratch, TEXT("ue_TextCounter_Echo"), {S, T}, EDasMethodSignature::String, TextObject.Get());
        Expect(OldText.bSucceeded && NewText.bSucceeded &&
               OldText.StringValue == TEXT("Unicode 世界 !") &&
                                           NewText.StringValue == TEXT("Unicode 世界 ! v2"),
                                                                       TEXT("string_results_owned_across_valid_reload"),
                                                                            NewText.Error);
        Write(TEXT("broken replacement"));
        const auto LastText = Runtime.RunNamedFramePersistent(
            Scratch, TEXT("ue_TextCounter_Echo"), {S, T}, EDasMethodSignature::String, TextObject.Get());
        Expect(LastText.bSucceeded && LastText.StringValue == NewText.StringValue && !LastText.ReloadError.IsEmpty(),
               TEXT("string_last_good_reload_fallback"));
        Runtime.ReleaseObject(TextObject.Get());
        FDasScalarValue Embedded;
        Embedded.Kind = 5;
        Embedded.StringValue = TEXT("abc");
        Embedded.StringValue[1] = 0;
        const auto EmbeddedResult = Runtime.RunNamedFramePersistent(
            TextPath, TEXT("ue_TextCounter_Echo"), {Embedded, T}, EDasMethodSignature::String, TextObject.Get());
        Expect(!EmbeddedResult.bSucceeded &&
               EmbeddedResult.Error.Contains(TEXT("embedded NUL")),
                                             TEXT("string_embedded_NUL_rejected_without_truncation"),
                                                  EmbeddedResult.Error);
        Runtime.ReleaseObject(TextObject.Get());
        Expect(OldText.StringValue == TEXT("Unicode 世界 !") &&
                                           Rename.NameValue ==
                                               FName(TEXT("Item_42")),
                                                     TEXT("string_name_results_owned_after_context_release"));
    }
    {
        const FString ObjectPath = Runtime.ScriptPath(TEXT("dsl_object_methods.das"));
        FString ObjectError;
        FDasAuthoringModule ObjectModel;
        Expect(Runtime.ReadAuthoringScript(ObjectPath, ObjectModel, ObjectError) && ObjectModel.Classes.Num() == 1,
               TEXT("object_DSL_model_contracts"), ObjectError);
        TStrongObjectPtr<UDasObjectFixture> ObjectOwner(NewObject<UDasObjectFixture>());
        TStrongObjectPtr<UDasObjectFixture> TargetObject(NewObject<UDasObjectFixture>());
        FDasScalarValue O;
        O.Kind = 8;
        O.ObjectValue = TargetObject.Get();
        const auto ObjectEcho = Runtime.RunNamedFramePersistent(
            ObjectPath, TEXT("ue_ObjectBridge_Echo"), {O}, EDasMethodSignature::Object, ObjectOwner.Get());
        Expect(ObjectEcho.bSucceeded && ObjectEcho.ObjectValue.Get() == TargetObject.Get(),
               TEXT("object_frame_CMRES_roundtrip"), ObjectEcho.Error);
        Expect(Runtime.RunNamedFramePersistent(
            ObjectPath, TEXT("ue_ObjectBridge_Remember"), {O}, EDasMethodSignature::Void, ObjectOwner.Get()).bSucceeded,
            TEXT("object_handle_stored_in_VM"));
        O.ObjectValue = nullptr;
        const auto NullObjectResult = Runtime.RunNamedFramePersistent(
            ObjectPath, TEXT("ue_ObjectBridge_Echo"), {O}, EDasMethodSignature::Object, ObjectOwner.Get());
        Expect(NullObjectResult.bSucceeded && !NullObjectResult.ObjectValue.IsValid(), TEXT("object_null_roundtrip"),
                                                                                            NullObjectResult.Error);
        TWeakObjectPtr<UObject> ObjectWeakTarget(TargetObject.Get());
        TargetObject.Reset();
        CollectGarbage(RF_NoFlags);
        const auto WeakStateFrame = Runtime.RunNamedFramePersistent(
            ObjectPath, TEXT("ue_ObjectBridge_IsRememberedValid"), {}, EDasMethodSignature::Bool, ObjectOwner.Get());
        Expect(!ObjectWeakTarget.IsValid() && !ObjectEcho.ObjectValue.IsValid() && WeakStateFrame.bSucceeded &&
                   !WeakStateFrame.BoolValue,
               TEXT("object_results_and_VM_globals_do_not_pin_GC"), WeakStateFrame.Error);
        const auto ObjectExpired = Runtime.RunNamedFramePersistent(
            ObjectPath, TEXT("ue_ObjectBridge_Recall"), {}, EDasMethodSignature::Object, ObjectOwner.Get());
        Expect(!ObjectExpired.bSucceeded && ObjectExpired.Error.Contains(TEXT("expired")),
                                                                         TEXT("expired_object_return_rejected"),
                                                                              ObjectExpired.Error);
        UObject* Temporary = NewObject<UDasObjectFixture>();
        CastChecked<UDasObjectFixture>(Temporary)->health = 73;
        TWeakObjectPtr<UObject> WeakTemporary(Temporary);
        O.ObjectValue = Temporary;
        const auto AcrossGC = Runtime.RunNamedFramePersistent(
            ObjectPath, TEXT("ue_ObjectBridge_ReadAcrossGC"), {O}, EDasMethodSignature::Int, ObjectOwner.Get());
        Expect(AcrossGC.bSucceeded && AcrossGC.Value == 73 && WeakTemporary.IsValid(),
               TEXT("object_inputs_pinned_only_during_eval"), AcrossGC.Error);
        O.ObjectValue = nullptr;
        CollectGarbage(RF_NoFlags);
        Expect(!WeakTemporary.IsValid(), TEXT("object_input_pin_released_after_call"));
        O.ObjectValue = ObjectOwner.Get();
        O.ObjectClass = TEXT("/Script/Engine.Actor");
        Expect(!Runtime.RunNamedFramePersistent(ObjectPath, TEXT("ue_ObjectBridge_ActorIdentity"), {O},
                                                                 EDasMethodSignature::Object, ObjectOwner.Get(),
                                                                 TEXT("/Script/Engine.Actor")).bSucceeded,
                                                TEXT("object_argument_class_checked_before_eval"));
        const auto WrongResult = Runtime.RunNamedFramePersistent(
            ObjectPath, TEXT("ue_ObjectBridge_WrongActorResult"), {}, EDasMethodSignature::Object, ObjectOwner.Get(),
                             TEXT("/Script/Engine.Actor"));
        Expect(!WrongResult.bSucceeded &&
               WrongResult.Error.Contains(TEXT("wrong class")), TEXT("object_result_class_checked"), WrongResult.Error);
        Runtime.ReleaseObject(ObjectOwner.Get());
    }
    {
        const FString MathPath = Runtime.ScriptPath(TEXT("dsl_math_methods.das"));
        FString MathError;
        FDasAuthoringModule MathModel;
        Expect(Runtime.ReadAuthoringScript(MathPath, MathModel, MathError) && MathModel.Classes.Num() == 1,
               TEXT("math_struct_DSL_model"), MathError);
        TStrongObjectPtr<UDasObjectFixture> MathOwner(NewObject<UDasObjectFixture>());
        FDasScalarValue VectorArg;
        VectorArg.Kind = 9;
        VectorArg.VectorValue = FVector(9007199254740988.0, 2.25, 3.5);
        FDasScalarValue DeltaArg;
        DeltaArg.Kind = 6;
        DeltaArg.DoubleValue = 1.0;
        const auto Shifted = Runtime.RunNamedFramePersistent(
            MathPath, TEXT("ue_MathBridge_Shift"), {VectorArg, DeltaArg}, EDasMethodSignature::Vector, MathOwner.Get());
        Expect(Shifted.bSucceeded && Shifted.VectorValue.X == 9007199254740989.0 && Shifted.VectorValue.Y == 2.25 &&
                   VectorArg.VectorValue.X == 9007199254740988.0,
               TEXT("vector_double_precision_and_input_copy"), Shifted.Error);
        FDasScalarValue RotationArg;
        RotationArg.Kind = 10;
        RotationArg.RotatorValue = FRotator(12.5, 20.25, 3.75);
        DeltaArg.DoubleValue = 5.0;
        const auto Turned =
            Runtime.RunNamedFramePersistent(MathPath, TEXT("ue_MathBridge_Turn"), {RotationArg, DeltaArg},
                                                           EDasMethodSignature::Rotator, MathOwner.Get());
        Expect(Turned.bSucceeded && Turned.RotatorValue.Equals(FRotator(12.5, 25.25, 3.75), 1e-12),
               TEXT("rotator_named_components_CMRES"), Turned.Error);
        FDasScalarValue TransformArg;
        TransformArg.Kind = 11;
        TransformArg.TransformValue = FTransform(FRotator(0, 90, 0), FVector(10, 20, 30), FVector(2, 3, 4));
        const auto PassedTransform =
            Runtime.RunNamedFramePersistent(MathPath, TEXT("ue_MathBridge_PassTransform"), {TransformArg},
                                                           EDasMethodSignature::Transform, MathOwner.Get());
        Expect(PassedTransform.bSucceeded && PassedTransform.TransformValue.Equals(TransformArg.TransformValue, 1e-12),
               TEXT("transform_full_value_CMRES"), PassedTransform.Error);
        VectorArg.VectorValue = FVector(1, 2, 3);
        const auto Applied =
            Runtime.RunNamedFramePersistent(MathPath, TEXT("ue_MathBridge_Apply"), {TransformArg, VectorArg},
                                                           EDasMethodSignature::Vector, MathOwner.Get());
        Expect(Applied.bSucceeded && Applied.VectorValue.Equals(
                                         TransformArg.TransformValue.TransformPosition(VectorArg.VectorValue), 1e-12),
               TEXT("transform_position_native_semantics"), Applied.Error);
        Expect(Runtime.RunNamedFramePersistent(
            MathPath,
            TEXT("ue_MathBridge_Remember"), {TransformArg}, EDasMethodSignature::Void, MathOwner.Get()).bSucceeded,
            TEXT("transform_global_value_copy"));
        TransformArg.TransformValue = FTransform::Identity;
        const auto Recalled = Runtime.RunNamedFramePersistent(
            MathPath, TEXT("ue_MathBridge_Recall"), {}, EDasMethodSignature::Transform, MathOwner.Get());
        Expect(Recalled.bSucceeded && Recalled.TransformValue.Equals(PassedTransform.TransformValue, 1e-12),
               TEXT("transform_stored_value_independent_of_host"), Recalled.Error);
        Expect(!Runtime.RunNamedFramePersistent(MathPath,
                                                TEXT("ue_MathBridge_PassTransform"), {VectorArg},
                                                     EDasMethodSignature::Transform, MathOwner.Get())
                                                    .bSucceeded,
                                                TEXT("math_nominal_struct_types_distinct"));
        VectorArg.VectorValue.X = std::numeric_limits<double>::infinity();
        Expect(!Runtime.RunNamedFramePersistent(MathPath,
                                                TEXT("ue_MathBridge_Shift"), {VectorArg, DeltaArg},
                                                     EDasMethodSignature::Vector, MathOwner.Get())
                                                    .bSucceeded,
                                                TEXT("nonfinite_vector_input_rejected"));
        TransformArg.TransformValue.SetRotation(FQuat(0, 0, 0, 2));
        Expect(!Runtime.RunNamedFramePersistent(MathPath,
                                                TEXT("ue_MathBridge_PassTransform"), {TransformArg},
                                                     EDasMethodSignature::Transform, MathOwner.Get())
                                                    .bSucceeded,
                                                TEXT("unnormalized_transform_input_rejected"));
        Runtime.ReleaseObject(MathOwner.Get());
        Expect(PassedTransform.TransformValue.Equals(Recalled.TransformValue, 1e-12),
               TEXT("math_results_owned_after_context_release"));
    }
    {
        FString StructError;
        Expect(ValidateDasValueStruct(TEXT("/Script/DasRuntime.DasCallPayload"), StructError),
                                      TEXT("reflected_nested_scalar_struct_capability"), StructError);
        Expect(!ValidateDasValueStruct(TEXT("/Script/Engine.HitResult"), StructError),
                                       TEXT("reflected_object_opaque_struct_rejected"), StructError);
        const FString StructPath = Runtime.ScriptPath(TEXT("dsl_reflected_methods.das"));
        FDasAuthoringModule StructModel;
        Expect(Runtime.ReadAuthoringScript(StructPath, StructModel, StructError), TEXT("reflected_struct_DSL_model"),
                                                                                       StructError);
        TStrongObjectPtr<UDasObjectFixture> StructOwner(NewObject<UDasObjectFixture>());
        FDasCallPayload Payload;
        Payload.Count = 7;
        Payload.Label = TEXT("Привет 世界");
        Payload.Metrics.Ratio = 1.25f;
        Payload.bEnabled = true;
        Payload.Weight = 1.23456789012345;
        FDasScalarValue ValueArg;
        ValueArg.Kind = 12;
        ValueArg.ObjectClass = TEXT("/Script/DasRuntime.DasCallPayload");
        ValueArg.StructValue.InitializeAs(FDasCallPayload::StaticStruct(), reinterpret_cast<const uint8*>(&Payload));
        FDasScalarValue AmountArg;
        AmountArg.IntValue = 3;
        const auto Edited = Runtime.RunNamedFramePersistent(
            StructPath, TEXT("ue_StructBridge_Edit"), {ValueArg, AmountArg}, EDasMethodSignature::Struct,
                             StructOwner.Get(), ValueArg.ObjectClass);
        const auto* Result = Edited.StructValue.GetPtr<FDasCallPayload>();
        Expect(Edited.bSucceeded && Result && Result->Count == 10 &&
               Result->Label == TEXT("Привет 世界 !") && Result->Metrics.Ratio == 1.75f && Result->bEnabled &&
                                     Result->Weight == Payload.Weight,
                                     TEXT("reflected_struct_recursive_copy_update"), Edited.Error);
        Expect(ValueArg.StructValue.Get<FDasCallPayload>().Count == 7 &&
                   ValueArg.StructValue.Get<FDasCallPayload>().Label == Payload.Label,
               TEXT("reflected_struct_input_immutable"));
        Expect(Runtime.RunNamedFramePersistent(
            StructPath,
            TEXT("ue_StructBridge_Remember"), {ValueArg}, EDasMethodSignature::Void, StructOwner.Get()).bSucceeded,
            TEXT("reflected_struct_global_value"));
        ValueArg.StructValue.GetMutable<FDasCallPayload>().Count = 99;
        const auto Recall = Runtime.RunNamedFramePersistent(
            StructPath, TEXT("ue_StructBridge_Recall"), {}, EDasMethodSignature::Struct, StructOwner.Get(),
                             TEXT("/Script/DasRuntime.DasCallPayload"));
        Expect(Recall.bSucceeded && Recall.StructValue.Get<FDasCallPayload>().Count == 7,
               TEXT("reflected_struct_global_independent_of_host"), Recall.Error);
        const auto Wrong = Runtime.RunNamedFramePersistent(
            StructPath, TEXT("ue_StructBridge_Wrong"), {ValueArg}, EDasMethodSignature::Struct, StructOwner.Get(),
                             TEXT("/Script/DasRuntime.DasCallMetrics"));
        Expect(!Wrong.bSucceeded && Wrong.Error.Contains(TEXT("type mismatch")),
                                                         TEXT("reflected_struct_wrong_result_identity"), Wrong.Error);
        Runtime.ReleaseObject(StructOwner.Get());
        Expect(Edited.StructValue.GetPtr<FDasCallPayload>() &&
                   Edited.StructValue.GetPtr<FDasCallPayload>()->Count == 10,
               TEXT("reflected_struct_owned_after_context_release"));
    }
    {
        TStrongObjectPtr<UDasObjectFixture> SnapshotOwner(NewObject<UDasObjectFixture>());
        const auto LongLoop =
            Runtime.RunPersistent(Runtime.ScriptPath(TEXT("struct_snapshot_gc.das")), 20000, SnapshotOwner.Get());
        Expect(LongLoop.bSucceeded && LongLoop.Value == 19999, TEXT("struct_snapshot_gc_long_loop_active_locals"),
                                                                    LongLoop.Error);
        Expect(Runtime.StructSnapshotCount(SnapshotOwner.Get()) <= 4 &&
                   Runtime.StructSnapshotsReclaimed(SnapshotOwner.Get()) > 10000,
               TEXT("struct_snapshot_gc_reclaims_churn_preserves_global_array"));
        const auto AgainLoop =
            Runtime.RunPersistent(Runtime.ScriptPath(TEXT("struct_snapshot_gc.das")), 10000, SnapshotOwner.Get());
        Expect(AgainLoop.bSucceeded && AgainLoop.Value == 9999 && Runtime.StructSnapshotCount(SnapshotOwner.Get()) <= 4,
               TEXT("struct_snapshot_gc_globals_survive_second_call"), AgainLoop.Error);
        Runtime.ReleaseObject(SnapshotOwner.Get());
        bool Boundaries = true;
        for (int32 I = 0; I < 5000; ++I)
        {
            const auto Value =
                Runtime.RunPersistent(Runtime.ScriptPath(TEXT("struct_snapshot_boundary.das")), I, SnapshotOwner.Get());
            if (!Value.bSucceeded || Value.Value != I)
            {
                Boundaries = false;
                break;
            }
        }
        Expect(Boundaries && Runtime.StructSnapshotCount(SnapshotOwner.Get()) == 1,
               TEXT("struct_snapshot_call_boundary_without_gc_option"));
        Runtime.ReleaseObject(SnapshotOwner.Get());
        Expect(Runtime.StructSnapshotCount(SnapshotOwner.Get()) == 0, TEXT("struct_snapshot_release_reclaims_all"));
    }
    {
        TStrongObjectPtr<UDasObjectFixture> SnapshotOwner(NewObject<UDasObjectFixture>());
        UObject* Owned = NewObject<UDasObjectFixture>();
        TWeakObjectPtr<UObject> Weak(Owned);
        SnapshotOwner->Target = Owned;
        const FString Script = Runtime.ScriptPath(TEXT("struct_object_snapshot.das"));
        auto Store = Runtime.RunPersistent(Script, 0, SnapshotOwner.Get());
        Expect(Store.bSucceeded, TEXT("object_snapshot_nested_array_pack"), Store.Error);
        SnapshotOwner->Target = nullptr;
        Owned = nullptr;
        CollectGarbage(RF_NoFlags);
        const auto Read = Runtime.RunPersistent(Script, 1, SnapshotOwner.Get());
        Expect(Weak.IsValid() && Read.bSucceeded && Read.Value == 1,
               TEXT("object_snapshot_nested_array_pins_survive_GC_and_copy"), Read.Error);
        SnapshotOwner->Target = NewObject<UDasObjectFixture>();
        const auto Wrong = Runtime.RunPersistent(Script, 3, SnapshotOwner.Get());
        Expect(!Wrong.bSucceeded && Wrong.Error.Contains(TEXT("incompatible class")),
                                                         TEXT("object_snapshot_wrong_class_rejected"), Wrong.Error);
        SnapshotOwner->Target = nullptr;
        const auto Clear = Runtime.RunPersistent(Script, 2, SnapshotOwner.Get());
        CollectGarbage(RF_NoFlags);
        Expect(Clear.bSucceeded && !Weak.IsValid(), TEXT("object_snapshot_collection_releases_pins"), Clear.Error);
        TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
        AActor* Target = World->SpawnActor<AActor>();
        SnapshotOwner->Target = Target;
        const auto ActorStore = Runtime.RunPersistent(Script, 4, SnapshotOwner.Get());
        Target->Destroy();
        SnapshotOwner->Target = nullptr;
        CollectGarbage(RF_NoFlags);
        const auto Destroyed = Runtime.RunPersistent(Script, 5, SnapshotOwner.Get());
        Expect(ActorStore.bSucceeded && Destroyed.bSucceeded && Destroyed.Value == 0,
               TEXT("object_snapshot_destroyed_actor_reads_null"), Destroyed.Error);
        Runtime.ReleaseObject(SnapshotOwner.Get());
        World->DestroyWorld(false);
    }
    {
        TStrongObjectPtr<UDasObjectFixture> InterfaceOwner(NewObject<UDasObjectFixture>());
        auto* NativeTarget = NewObject<UDasReferenceFixture>();
        TWeakObjectPtr<UObject> WeakNative(NativeTarget);
        InterfaceOwner->Target = NativeTarget;
        const FString InterfaceScript = Runtime.ScriptPath(TEXT("interface_reference_lifetime.das"));
        const auto StoredInterface = Runtime.RunPersistent(InterfaceScript, 0, InterfaceOwner.Get());
        Expect(StoredInterface.bSucceeded && InterfaceOwner->Events.GetObject() == NativeTarget &&
                   InterfaceOwner->Events.GetInterface() != nullptr,
               TEXT("interface_reference_native_pointer_set"), StoredInterface.Error);
        InterfaceOwner->Target = nullptr;
        CollectGarbage(RF_NoFlags);
        const auto ReadInterface = Runtime.RunPersistent(InterfaceScript, 1, InterfaceOwner.Get());
        Expect(WeakNative.IsValid() && ReadInterface.bSucceeded && ReadInterface.Value == 1,
               TEXT("interface_UPROPERTY_keeps_object_alive"), ReadInterface.Error);
        const auto ClearedInterface = Runtime.RunPersistent(InterfaceScript, 2, InterfaceOwner.Get());
        CollectGarbage(RF_NoFlags);
        const auto RememberedInterface = Runtime.RunPersistent(InterfaceScript, 3, InterfaceOwner.Get());
        Expect(ClearedInterface.bSucceeded && !WeakNative.IsValid() && RememberedInterface.bSucceeded &&
                   RememberedInterface.Value == 0,
               TEXT("interface_VM_wrapper_is_weak"), RememberedInterface.Error);
        InterfaceOwner->Target = NewObject<UDasObjectFixture>();
        const auto BadInterface = Runtime.RunPersistent(InterfaceScript, 4, InterfaceOwner.Get());
        Expect(!BadInterface.bSucceeded && !InterfaceOwner->Events.GetObject(),
               TEXT("interface_self_nonimplementer_rejected_without_write"), BadInterface.Error);
        Runtime.ReleaseObject(InterfaceOwner.Get());
    }
    {
        FString StructDefaultError;
        FInstancedStruct ParsedDefault;
        Expect(ParseDasStructDefault(
            TEXT("/Script/DasRuntime.DasCallPayload"),
                 TEXT("(Metrics=(Ratio=1.5),Count=10,Label=\"Initial\",bEnabled=True,Weight=2.25)"), ParsedDefault,
                      StructDefaultError) &&
                 ParsedDefault.GetPtr<FDasCallPayload>() && ParsedDefault.Get<FDasCallPayload>().Count == 10 &&
                 ParsedDefault.Get<FDasCallPayload>().Metrics.Ratio == 1.5f,
            TEXT("struct_default_recursive_import"), StructDefaultError);
        Expect(!ParseDasStructDefault(TEXT("/Script/DasRuntime.DasCallPayload"),
                                           TEXT("(Unknown=10)"), ParsedDefault, StructDefaultError),
                                      TEXT("struct_default_unknown_field_rejected"));
        Expect(!ParseDasStructDefault(TEXT("/Script/DasRuntime.DasCallPayload"),
                                           TEXT("(Count=10) trailing"), ParsedDefault, StructDefaultError),
                                      TEXT("struct_default_trailing_text_rejected"));
        Expect(!ParseDasStructDefault(TEXT("/Script/DasRuntime.DasNamedMode"),
                                           TEXT("(Mode=Invalid)"), ParsedDefault, StructDefaultError),
                                      TEXT("struct_default_unknown_enum_rejected"));
        FDasAuthoringModule StructFieldsModel;
        Expect(Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_struct_fields.das")), StructFieldsModel,
                                                              StructDefaultError),
                                           TEXT("struct_fields_native_DSL_model"), StructDefaultError);
        {
            FString E;
            const FString EnumPath = TEXT("/Script/DasRuntime.EDasCallMode");
            Expect(ValidateDasEnumNumber(EnumPath, 42, E), TEXT("enum_value_valid_sparse_native_member"), E);
            Expect(!ValidateDasEnumNumber(EnumPath, 255, E), TEXT("enum_reserved_unknown_value_refused"), E);
            Expect(!ValidateDasEnumNumber(TEXT("/Script/DasRuntime.NotAnEnum"), 42, E),
                                          TEXT("enum_unknown_type_refused"), E);
            const FString Temp =
                FPaths::ProjectSavedDir() /
                (TEXT("EnumValueToken_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das"));
            const FString Text = TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int { return ue_enum_number(ue_enum_value(\"/Script/DasRuntime.EDasCallMode\",\"Paused\")) }\n");
            FFileHelper::SaveStringToFile(Text, *Temp, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
            const auto Value = Runtime.RunFile(Temp, 0);
            Expect(Value.bSucceeded && Value.Value == 42, TEXT("enum_value_POD_native_ABI"), Value.Error);
            FFileHelper::SaveStringToFile(Text.Replace(TEXT("Paused"), TEXT("not_a_member")), *Temp,
                                                       FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
            const auto Bad = Runtime.RunFile(Temp, 0);
            Expect(!Bad.bSucceeded, TEXT("enum_unknown_symbol_runtime_refused"), Bad.Error);
            IFileManager::Get().Delete(*Temp);
        }
        {
            FDasReflectedTypes Types;
            FString TypeError;
            const bool Read =
                Runtime.ReadReflectedTypes(Runtime.ScriptPath(TEXT("dsl_type_declarations.das")), Types, TypeError);
            Expect(Read && Types.Enums.Num() == 1 && Types.Structs.Num() == 1,
                   TEXT("reflected_type_annotations_native_AST"), TypeError);
            Expect(Read && Types.Enums[0].Entries[0].Value == 7 && Types.Enums[0].Entries[1].Value == 42 &&
                       Types.Structs[0].Properties.Num() == 4,
                   TEXT("reflected_types_explicit_values_and_scalar_fields"), TypeError);
            FString Wire;
            FDasReflectedTypes Roundtrip;
            Expect(Read && Types.ToJson(Wire, TypeError) && FDasReflectedTypes::FromJson(Wire, Roundtrip, TypeError) &&
                       Roundtrip.Enums[0].Entries[1].Value == 42,
                   TEXT("reflected_type_model_roundtrip"), TypeError);
            if (Read)
            {
                auto Bad = Types;
                Bad.Enums[0].Entries[1].Value = 7;
                Expect(!Bad.Validate(TypeError), TEXT("reflected_enum_duplicate_values_refused"));
                Bad = Types;
                Bad.Enums[0].Entries[1].Value = 255;
                Expect(!Bad.Validate(TypeError), TEXT("reflected_enum_reserved_byte_value_refused"));
                Bad = Types;
                Bad.Structs[0].Properties[0].Type.Kind = TEXT("object");
                Bad.Structs[0].Properties[0].Type.TypePath = TEXT("/Script/CoreUObject.Object");
                Expect(!Bad.Validate(TypeError), TEXT("initial_authored_struct_object_field_refused"));
            }
            FString Source;
            FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_type_declarations.das")));
            const FString Temp =
                FPaths::ProjectSavedDir() /
                (TEXT("TypeAnnotationNegative_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das"));
            FFileHelper::SaveStringToFile(Source.Replace(TEXT("Active = 42"), TEXT("Active")), *Temp,
                                                         FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
            auto Unchanged = Types;
            Expect(!Runtime.ReadReflectedTypes(Temp, Unchanged, TypeError) && Unchanged.Id == Types.Id,
                   TEXT("implicit_enum_value_refused_without_partial_model"), TypeError);
            IFileManager::Get().Delete(*Temp);
        }
        {
            FDasAuthoringModule Objects;
            FString ObjectError;
            const bool ReadObjects =
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_object_fields.das")), Objects, ObjectError);
            Expect(ReadObjects && Objects.Classes.Num() == 1 && Objects.Classes[0].Properties.Num() == 4,
                   TEXT("object_fields_frontend_typed_nulls"), ObjectError);
            FDasClassSchema ObjectSchema;
            Expect(ReadObjects &&
                   Objects.ToActorSchema(TEXT("objectfields.ObjectFields"), ObjectSchema, ObjectError) &&
                                         ObjectSchema.Fields.Num() == 4 &&
                                         ObjectSchema.Fields.ContainsByPredicate(
                                             [](const auto& F)
                                             {
                                                 return F.Name == TEXT("Target") && F.Kind == EDasScalarKind::Object &&
                                                                       F.ObjectClass == TEXT("/Script/Engine.Actor");
                                             }),
                                         TEXT("object_fields_schema_native_class"), ObjectError);
            FString Wire;
            FDasAuthoringModule Roundtrip;
            Expect(ReadObjects && Objects.ToJson(Wire, ObjectError) &&
                   FDasAuthoringModule::FromJson(Wire, Roundtrip, ObjectError) &&
                   Roundtrip.Classes[0].Properties[0].Default.Kind ==
                       TEXT("null"), TEXT("object_fields_authoring_null_roundtrip"), ObjectError);
            if (ReadObjects)
            {
                auto Bad = Objects;
                Bad.Classes[0].Properties[0].Type.TypePath = TEXT("/Game/NotLoaded.BP_C");
                Expect(!Bad.ToActorSchema(TEXT("objectfields.ObjectFields"), ObjectSchema, ObjectError),
                                          TEXT("object_fields_unloaded_blueprint_type_refused"), ObjectError);
            }
            FString Source;
            FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_object_fields.das")));
            const FString Temporary =
                FPaths::ProjectSavedDir() /
                (TEXT("ObjectFieldNegative_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das"));
            auto Reject = [&](const FString& Text, const TCHAR* Label)
            {
                const bool Written =
                    FFileHelper::SaveStringToFile(Text, *Temporary, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
                auto Candidate = Objects;
                Expect(Written && !Runtime.ReadAuthoringScript(Temporary, Candidate, ObjectError) &&
                           Candidate.Id == Objects.Id,
                       Label, ObjectError);
            };
            Reject(Source.Replace(TEXT("/Script/Engine.ActorComponent"), TEXT("/Script/Engine.MissingComponent")),
                                  TEXT("object_field_unknown_class_refused"));
            Reject(Source.Replace(TEXT("ue_object_value(\"/Script/Engine.Actor\")"), TEXT("ue_null_object()")),
                                  TEXT("object_field_untyped_initializer_refused"));
            Reject(Source.Replace(TEXT("def SetTarget(value:UObjectHandle):void { self.Target = value }"),
                                       TEXT("def SetTarget(value:UObjectHandle):void { self.Observed = value }")),
                                  TEXT("object_field_readonly_write_refused_by_macro"));
            IFileManager::Get().Delete(*Temporary);
        }
        FDasClassSchema StructFieldsSchema;
        Expect(StructFieldsModel.ToActorSchema(
            TEXT("structfields.StructFields"), StructFieldsSchema, StructDefaultError) &&
            StructFieldsSchema.Fields.Num() == 2 && StructFieldsSchema.Fields[0].Kind == EDasScalarKind::Struct,
            TEXT("struct_fields_schema_adapter"), StructDefaultError);
        FString EnumStructError;
        Expect(ValidateDasValueStruct(TEXT("/Script/DasRuntime.DasNamedMode"), EnumStructError) &&
                                      ValidateDasValueStruct(TEXT("/Script/DasRuntime.DasEnumBundle"), EnumStructError),
                                                             TEXT("reflected_struct_enum_leaf_capabilities"),
                                                                  EnumStructError);
        const FString EnumStructPath = Runtime.ScriptPath(TEXT("dsl_struct_enum_methods.das"));
        FDasAuthoringModule EnumStructModel;
        Expect(Runtime.ReadAuthoringScript(EnumStructPath, EnumStructModel, EnumStructError),
               TEXT("enum_struct_DSL_model"), EnumStructError);
        TStrongObjectPtr<UDasObjectFixture> EnumStructOwner(NewObject<UDasObjectFixture>());
        FDasNamedMode Named;
        Named.Mode = EDasCallMode::Idle;
        Named.Key = FName(TEXT("Item_42"));
        FDasScalarValue NamedArg;
        NamedArg.Kind = 12;
        NamedArg.ObjectClass = TEXT("/Script/DasRuntime.DasNamedMode");
        NamedArg.StructValue.InitializeAs(FDasNamedMode::StaticStruct(), reinterpret_cast<const uint8*>(&Named));
        FDasScalarValue SymbolArg;
        SymbolArg.Kind = 5;
        SymbolArg.StringValue = TEXT("active");
        const auto UpdatedEnum = Runtime.RunNamedFramePersistent(
            EnumStructPath, TEXT("ue_EnumStructBridge_SetMode"), {NamedArg, SymbolArg}, EDasMethodSignature::Struct,
                                 EnumStructOwner.Get(), NamedArg.ObjectClass);
        const auto* UpdatedNamed = UpdatedEnum.StructValue.GetPtr<FDasNamedMode>();
        Expect(UpdatedEnum.bSucceeded && UpdatedNamed && UpdatedNamed->Mode == EDasCallMode::Active &&
                   UpdatedNamed->Key == Named.Key &&
                   NamedArg.StructValue.Get<FDasNamedMode>().Mode == EDasCallMode::Idle,
               TEXT("enum_leaf_symbol_update_preserves_input_and_name"), UpdatedEnum.Error);
        NamedArg.StructValue = UpdatedEnum.StructValue;
        const auto ReadEnum =
            Runtime.RunNamedFramePersistent(EnumStructPath, TEXT("ue_EnumStructBridge_ReadMode"), {NamedArg},
                                                                 EDasMethodSignature::String, EnumStructOwner.Get());
        Expect(ReadEnum.bSucceeded && ReadEnum.StringValue == TEXT("Active"), TEXT("enum_leaf_canonical_symbol_read"),
                                                                                   ReadEnum.Error);
        SymbolArg.StringValue = TEXT("EDasCallMode::Paused");
        const auto QualifiedEnum = Runtime.RunNamedFramePersistent(
            EnumStructPath, TEXT("ue_EnumStructBridge_SetMode"), {NamedArg, SymbolArg}, EDasMethodSignature::Struct,
                                 EnumStructOwner.Get(), NamedArg.ObjectClass);
        Expect(QualifiedEnum.bSucceeded && QualifiedEnum.StructValue.GetPtr<FDasNamedMode>() &&
                   QualifiedEnum.StructValue.Get<FDasNamedMode>().Mode == EDasCallMode::Paused,
               TEXT("enum_leaf_qualified_symbol"), QualifiedEnum.Error);
        SymbolArg.StringValue = TEXT("not-a-member");
        const auto BadSymbol = Runtime.RunNamedFramePersistent(
            EnumStructPath, TEXT("ue_EnumStructBridge_SetMode"), {NamedArg, SymbolArg}, EDasMethodSignature::Struct,
                                 EnumStructOwner.Get(), NamedArg.ObjectClass);
        Expect(!BadSymbol.bSucceeded && BadSymbol.Error.Contains(TEXT("unknown or reserved")),
                                                                 TEXT("enum_leaf_unknown_symbol_rejected"),
                                                                      BadSymbol.Error);
        SymbolArg.StringValue =
            StaticEnum<EDasCallMode>()->GetNameStringByIndex(StaticEnum<EDasCallMode>()->NumEnums() - 1);
        const auto MaxSymbol = Runtime.RunNamedFramePersistent(
            EnumStructPath, TEXT("ue_EnumStructBridge_SetMode"), {NamedArg, SymbolArg}, EDasMethodSignature::Struct,
                                 EnumStructOwner.Get(), NamedArg.ObjectClass);
        Expect(!MaxSymbol.bSucceeded && MaxSymbol.Error.Contains(TEXT("unknown or reserved")),
                                                                 TEXT("enum_leaf_MAX_symbol_rejected"),
                                                                      MaxSymbol.Error);
        const auto WrongEnum =
            Runtime.RunNamedFramePersistent(EnumStructPath, TEXT("ue_EnumStructBridge_WrongEnumType"), {NamedArg},
                                                                 EDasMethodSignature::String, EnumStructOwner.Get());
        Expect(!WrongEnum.bSucceeded && WrongEnum.Error.Contains(TEXT("type path mismatch")),
                                                                 TEXT("enum_leaf_identity_mismatch_rejected"),
                                                                      WrongEnum.Error);
        NamedArg.StructValue.GetMutable<FDasNamedMode>().Mode = static_cast<EDasCallMode>(255);
        const auto InvalidStored =
            Runtime.RunNamedFramePersistent(EnumStructPath, TEXT("ue_EnumStructBridge_ReadMode"), {NamedArg},
                                                                 EDasMethodSignature::String, EnumStructOwner.Get());
        Expect(!InvalidStored.bSucceeded &&
               InvalidStored.Error.Contains(TEXT("Unknown or reserved enum value")),
                                            TEXT("enum_leaf_invalid_input_snapshot_rejected"), InvalidStored.Error);
        FDasEnumBundle Bundle;
        Bundle.Nested.Key = Named.Key;
        FDasScalarValue BundleArg;
        BundleArg.Kind = 12;
        BundleArg.ObjectClass = TEXT("/Script/DasRuntime.DasEnumBundle");
        BundleArg.StructValue.InitializeAs(FDasEnumBundle::StaticStruct(), reinterpret_cast<const uint8*>(&Bundle));
        const auto BundleResult = Runtime.RunNamedFramePersistent(
            EnumStructPath, TEXT("ue_EnumStructBridge_UpdateBundle"), {BundleArg}, EDasMethodSignature::Struct,
                                 EnumStructOwner.Get(), BundleArg.ObjectClass);
        const auto* UpdatedBundle = BundleResult.StructValue.GetPtr<FDasEnumBundle>();
        Expect(BundleResult.bSucceeded && UpdatedBundle && UpdatedBundle->Legacy == DasLegacyActive &&
                   UpdatedBundle->Nested.Mode == EDasCallMode::Paused && UpdatedBundle->Nested.Key == Named.Key,
               TEXT("enum_byte_leaf_and_nested_enum_update"), BundleResult.Error);
        Runtime.ReleaseObject(EnumStructOwner.Get());
    }
    {
        FDasScalarValue NumericInput, NumericOutput;
        FString NumericError;
        NumericInput.Kind = 6;
        auto IntRule = [&](double N, EDasIntegerRule Rule, int32 Expected, const TCHAR* Label)
        {
            NumericInput.DoubleValue = N;
            Expect(ConvertDasNumber(NumericInput, 0, Rule, EDasFloatRule::Exact, NumericOutput, NumericError) &&
                       NumericOutput.IntValue == Expected,
                   Label, NumericError);
        };
        IntRule(-1.75, EDasIntegerRule::Truncate, -1, TEXT("numeric_truncate_toward_zero"));
        IntRule(-1.25, EDasIntegerRule::Floor, -2, TEXT("numeric_floor_negative"));
        IntRule(-1.75, EDasIntegerRule::Ceil, -1, TEXT("numeric_ceil_negative"));
        IntRule(-2.5, EDasIntegerRule::NearestAway, -3, TEXT("numeric_nearest_half_away_negative"));
        IntRule(2.5, EDasIntegerRule::NearestAway, 3, TEXT("numeric_nearest_half_away_positive"));
        IntRule(double(MIN_int32), EDasIntegerRule::Exact, MIN_int32, TEXT("numeric_exact_int32_min"));
        IntRule(double(MAX_int32), EDasIntegerRule::Exact, MAX_int32, TEXT("numeric_exact_int32_max"));
        NumericInput.DoubleValue = 1.5;
        Expect(!ConvertDasNumber(NumericInput, 0, EDasIntegerRule::Exact, EDasFloatRule::Exact, NumericOutput,
                                 NumericError),
               TEXT("numeric_fraction_requires_rounding"));
        NumericInput.DoubleValue = double(MAX_int32) + 1;
        Expect(!ConvertDasNumber(NumericInput, 0, EDasIntegerRule::Truncate, EDasFloatRule::Exact, NumericOutput,
                                 NumericError),
               TEXT("numeric_integer_overflow_refused"));
        NumericInput.DoubleValue = 0.1;
        Expect(!ConvertDasNumber(NumericInput, 1, EDasIntegerRule::Exact, EDasFloatRule::Exact, NumericOutput,
                                 NumericError),
               TEXT("numeric_float_precision_exact_refuses_loss"));
        Expect(ConvertDasNumber(NumericInput, 1, EDasIntegerRule::Exact, EDasFloatRule::RoundNearest, NumericOutput,
                                NumericError) &&
                   NumericOutput.FloatValue == 0.1f,
               TEXT("numeric_float_round_is_explicit"));
        NumericInput.DoubleValue = std::numeric_limits<double>::max();
        Expect(!ConvertDasNumber(NumericInput, 1, EDasIntegerRule::Exact, EDasFloatRule::RoundNearest, NumericOutput,
                                 NumericError),
               TEXT("numeric_float_overflow_refused"));
        NumericInput.DoubleValue = std::numeric_limits<double>::infinity();
        Expect(!ConvertDasNumber(NumericInput, 6, EDasIntegerRule::Exact, EDasFloatRule::Exact, NumericOutput,
                                 NumericError),
               TEXT("numeric_infinity_refused"));
        NumericInput.DoubleValue = std::numeric_limits<double>::quiet_NaN();
        Expect(!ConvertDasNumber(NumericInput, 0, EDasIntegerRule::NearestAway, EDasFloatRule::RoundNearest,
                                 NumericOutput, NumericError),
               TEXT("numeric_NaN_refused"));
        NumericInput.Kind = 0;
        NumericInput.IntValue = 16777217;
        Expect(!ConvertDasNumber(NumericInput, 1, EDasIntegerRule::Exact, EDasFloatRule::Exact, NumericOutput,
                                 NumericError),
               TEXT("numeric_large_integer_precision_refused"));
        Expect(ConvertDasNumber(NumericInput, 1, EDasIntegerRule::Exact, EDasFloatRule::RoundNearest, NumericOutput,
                                NumericError) &&
                   NumericOutput.FloatValue == 16777216.f,
               TEXT("numeric_large_integer_explicit_round"));
        NumericInput.Kind = 1;
        NumericInput.FloatValue = 0.1f;
        Expect(ConvertDasNumber(NumericInput, 6, EDasIntegerRule::Exact, EDasFloatRule::Exact, NumericOutput,
                                NumericError) &&
                   NumericOutput.DoubleValue == double(0.1f),
               TEXT("numeric_float_to_double_exact"));
        NumericInput.Kind = 6;
        NumericInput.DoubleValue = 1;
        Expect(!ConvertDasNumber(NumericInput, 0, static_cast<EDasIntegerRule>(99), EDasFloatRule::Exact, NumericOutput,
                                 NumericError),
               TEXT("numeric_unknown_policy_refused"));
    }
    {
        auto* Library = UDasNumericLibrary::StaticClass()->GetDefaultObject();
        for (const FName Name :
             {FName(TEXT("IntToDouble")), FName(TEXT("FloatToDouble")), FName(TEXT("IntToFloat")),
                                                                              FName(TEXT("DoubleToFloat")),
                                                                                    FName(TEXT("FloatToInt")),
                                                                                          FName(TEXT("DoubleToInt"))})
        {
            auto* Function = Library->FindFunction(Name);
            FStructOnScope Memory(Function);
            auto* Value = FindFProperty<FNumericProperty>(Function, TEXT("Value"));
            void* Address = Value->ContainerPtrToValuePtr<void>(Memory.GetStructMemory());
            if (Value->IsFloatingPoint())
                Value->SetFloatingPointPropertyValue(Address, 2.0);
            else
                Value->SetIntPropertyValue(Address, int64(2));
            Library->ProcessEvent(Function, Memory.GetStructMemory());
            auto* Result = CastField<FNumericProperty>(Function->GetReturnProperty());
            const void* Returned = Result->ContainerPtrToValuePtr<void>(Memory.GetStructMemory());
            const double Number = Result->IsFloatingPoint() ? Result->GetFloatingPointPropertyValue(Returned)
                                                            : double(Result->GetSignedIntPropertyValue(Returned));
            Expect(Number == 2.0, *(FString(TEXT("numeric_native_thunk_")) + Name.ToString()));
        }
    }
    TWeakObjectPtr<UObject> DeadOwner(Second.Get());
    First.Reset();
    Second.Reset();
    CollectGarbage(RF_NoFlags);
    Expect(!DeadOwner.IsValid() && Runtime.PersistentContextCount() == 0 && Runtime.SubscriptionCount() == 0,
           TEXT("persistent_GC_reclaims_contexts_without_pinning"));
    Expect(!Persistent(DeadOwner.Get()).bSucceeded, TEXT("persistent_dead_owner_rejected"));
    IFileManager::Get().Delete(*Scratch);

    {
        FDasReflectedTypes MemberTypes;
        FString Issue, Wire;
        const bool Compiled =
            Runtime.ReadReflectedTypes(Runtime.ScriptPath(TEXT("dsl_enum_member_ids.das")), MemberTypes, Issue);
        Expect(Compiled && MemberTypes.Enums.Num() == 1 &&
               MemberTypes.Enums[0].Entries[1].Id == TEXT("memberids.State.running"), TEXT("enum_member_ID_annotation"),
                                                                                           Issue);
        Expect(Compiled && MemberTypes.Enums[0].Entries[0].Id == TEXT("memberids.State.Idle"),
                                                                      TEXT("enum_member_ID_automatic"));
        if (Compiled)
        {
            MemberTypes.Enums[0].Entries[1].Id = MemberTypes.Enums[0].Entries[0].Id;
            Expect(!MemberTypes.Validate(Issue), TEXT("enum_member_ID_duplicate_refused"));
        }
        FDasReflectedTypes Legacy;
        Expect(FDasReflectedTypes::FromJson(
            TEXT("{\"schema_version\":1,\"module_id\":\"legacy\",\"enums\":[{\"id\":\"legacy.Mode\",\"name\":\"Mode\",\"metadata\":{},\"entries\":[{\"name\":\"Idle\",\"value\":7}]}],\"structs\":[]}"),
                Legacy, Issue) &&
            Legacy.Enums[0].Entries[0].Id == TEXT("legacy.Mode.Idle") && Legacy.ToJson(Wire, Issue) &&
                                                  Wire.Contains(TEXT("\"schema_version\": 2")),
                                                                TEXT("enum_member_ID_legacy_wire_upgrade"), Issue);
    }
    {
        FDasReflectedTypes Sets, Roundtrip;
        FString Issue, Wire;
        const bool Compiled =
            Runtime.ReadReflectedTypes(Runtime.ScriptPath(TEXT("dsl_set_authoring.das")), Sets, Issue);
        Expect(Compiled && Sets.Structs.Num() == 1 && Sets.Structs[0].Properties.Num() == 2,
               TEXT("set_example_declarations"), Issue);
        if (Compiled && Sets.Structs.Num() == 1)
        {
            Expect(Sets.ToJson(Wire, Issue) && FDasReflectedTypes::FromJson(Wire, Roundtrip, Issue) &&
                   Roundtrip.Structs[0].Properties[0].Type.Kind ==
                       TEXT("set") && Roundtrip.Structs[0].Properties[0].Type.Element->Kind ==
                                          TEXT("int") && Roundtrip.Structs[0].Properties[1].Type.Element->Kind ==
                                                             TEXT("string"), TEXT("set_IR_roundtrip"), Issue);
            auto Invalid = Sets;
            auto Element = MakeShared<FDasDeclaredType>();
            Element->Kind = TEXT("float");
            Invalid.Structs[0].Properties[0].Type.Element = Element;
            Expect(!Invalid.Validate(Issue), TEXT("set_IR_unsupported_element_refused"));
            Invalid = Sets;
            auto MapKey = MakeShared<FDasDeclaredType>();
            MapKey->Kind = TEXT("int");
            Invalid.Structs[0].Properties[0].Type.Key = MapKey;
            Expect(!Invalid.Validate(Issue), TEXT("set_IR_map_shape_refused"));
        }
    }
    {
        FDasReflectedTypes Maps, Roundtrip;
        FString Issue, Wire;
        const bool Compiled = Runtime.ReadReflectedTypes(Runtime.ScriptPath(TEXT("dsl_scalar_maps.das")), Maps, Issue);
        Expect(Compiled && Maps.Structs.Num() == 1 && Maps.Structs[0].Properties.Num() == 3,
               TEXT("scalar_map_example_declarations"), Issue);
        if (Compiled && Maps.Structs.Num() == 1)
        {
            const bool Parsed = Maps.ToJson(Wire, Issue) && FDasReflectedTypes::FromJson(Wire, Roundtrip, Issue);
            const auto* Weights = Parsed ? Roundtrip.Structs[0].Properties.FindByPredicate(
                                               [](const auto& P) { return P.Name == TEXT("Weights"); })
                                         : nullptr;
            const auto* Enabled = Parsed ? Roundtrip.Structs[0].Properties.FindByPredicate(
                                               [](const auto& P) { return P.Name == TEXT("Enabled"); })
                                         : nullptr;
            const auto* Labels = Parsed ? Roundtrip.Structs[0].Properties.FindByPredicate(
                                              [](const auto& P) { return P.Name == TEXT("Labels"); })
                                        : nullptr;
            Expect(Weights && Enabled && Labels &&
                   Weights->Type.Value->Kind ==
                       TEXT("double") && Enabled->Type.Value->Kind ==
                                             TEXT("bool") &&
                                                  Labels->Type.Value->Kind ==
                                                      TEXT("name"), TEXT("scalar_map_IR_preserves_value_types"), Issue);
            auto Invalid = Maps;
            auto Element = MakeShared<FDasDeclaredType>();
            Element->Kind = TEXT("array");
            auto Inner = MakeShared<FDasDeclaredType>();
            Inner->Kind = TEXT("float");
            Element->Element = Inner;
            Invalid.Structs[0].Properties[0].Type.Value = Element;
            Expect(!Invalid.Validate(Issue), TEXT("scalar_map_IR_nested_container_refused"));
            Invalid = Maps;
            auto Key = MakeShared<FDasDeclaredType>();
            Key->Kind = TEXT("float");
            Invalid.Structs[0].Properties[0].Type.Key = Key;
            Expect(!Invalid.Validate(Issue), TEXT("scalar_map_IR_unsupported_key_refused"));
        }
    }
    {
        FDasAuthoringModule Containers, Roundtrip;
        FString Issue, Wire;
        const bool Compiled =
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_container_calls.das")), Containers, Issue);
        Expect(Compiled && Containers.Classes.Num() == 1 && Containers.Classes[0].Methods.Num() == 21,
               TEXT("container_calls_example_declarations"), Issue);
        if (Compiled)
        {
            FDasClassSchema Adapter;
            Expect(Containers.ToJson(Wire, Issue) && FDasAuthoringModule::FromJson(Wire, Roundtrip, Issue) &&
                   Roundtrip.ToActorSchema(TEXT("containers.ContainerActor"), Adapter, Issue) &&
                                           Adapter.Methods.Num() == 21,
                                           TEXT("container_calls_IR_native_adapter_roundtrip"), Issue);
        }
    }
    {
        FDasAuthoringModule OutRef, Roundtrip;
        FString Issue, Wire;
        const bool Compiled = Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_out_ref.das")), OutRef, Issue);
        Expect(Compiled && OutRef.Classes.Num() == 1 && OutRef.Classes[0].Methods.Num() == 30,
               TEXT("out_ref_example_declarations"), Issue);
        if (Compiled)
        {
            const auto* Adjust =
                OutRef.Classes[0].Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Adjust"); });
            Expect(Adjust && Adjust->Parameters[0].Direction ==
                                 TEXT("inout") && Adjust->Parameters[1].Direction ==
                                                      TEXT("out") && Adjust->Parameters[2].Direction ==
                                                                         TEXT("in"),
                                                                              TEXT("out_ref_annotation_directions"));
            FDasClassSchema Adapter;
            const bool Parsed = OutRef.ToJson(Wire, Issue) && FDasAuthoringModule::FromJson(Wire, Roundtrip, Issue) &&
                                Roundtrip.ToActorSchema(TEXT("outref.OutActor"), Adapter, Issue);
            const auto* Projected =
                Parsed ? Adapter.Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Adjust"); })
                       : nullptr;
            Expect(Projected && Projected->Contract().InputDirections == TArray<int32>{2, 1, 0},
                   TEXT("out_ref_IR_adapter_contract_roundtrip"), Issue);
        }
    }
    {
        FDasAuthoringModule Interfaces, Roundtrip;
        FString Issue, Wire;
        const bool Compiled =
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_interfaces.das")), Interfaces, Issue);
        Expect(Compiled && Interfaces.Classes.Num() == 1 &&
               Interfaces.Classes[0].Interfaces == TArray<FString>{TEXT("/Script/DasRuntime.DasProbeInterface")},
                                                                   TEXT("interface_example_contract_compiles"), Issue);
        if (Compiled)
        {
            FDasClassSchema Adapter;
            Expect(Interfaces.ToJson(Wire, Issue) && FDasAuthoringModule::FromJson(Wire, Roundtrip, Issue) &&
                   Roundtrip.ToActorSchema(TEXT("interfaces.InterfaceActor"), Adapter, Issue) &&
                                           Adapter.Interfaces == Interfaces.Classes[0].Interfaces,
                                           TEXT("interface_IR_identity_roundtrip"), Issue);
            auto Changed = Interfaces.Classes[0];
            Changed.Interfaces.Reset();
            Expect(FDasAuthoringModule::Compare(Interfaces.Classes[0], Changed).bStructural,
                   TEXT("interface_membership_change_is_structural"));
        }
    }
    {
        FDasAuthoringModule Interfaces, Roundtrip;
        FString Issue, Wire;
        const bool Compiled =
            Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_definition.das")), Interfaces, Issue);
        Expect(Compiled && Interfaces.Classes.Num() == 1 && Interfaces.Classes[0].Methods.Num() == 14,
               TEXT("authored_interface_stub_definition_compiles"), Issue);
        if (Compiled)
        {
            Expect(Interfaces.ToJson(Wire, Issue) && FDasAuthoringModule::FromJson(Wire, Roundtrip, Issue) &&
                   Roundtrip.Classes[0].Id == TEXT("authoredinterface.CounterContract"),
                                                   TEXT("authored_interface_IR_stable_identity"), Issue);
            const auto* Stats =
                Interfaces.Classes[0].Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Stats"); });
            Expect(Stats && Stats->Parameters[0].Direction ==
                                TEXT("inout") && Stats->Parameters[1].Direction ==
                                                     TEXT("out"), TEXT("authored_interface_method_ref_out_directions"));
        }
    }
    {
        TStrongObjectPtr<UDasObjectFixture> NonInterface(NewObject<UDasObjectFixture>());
        const FString InterfacePath = Runtime.ScriptPath(TEXT("generated_interface_calls.das"));
        auto Null = Runtime.RunFile(InterfacePath, 9, false, NonInterface.Get());
        Expect(Null.bSucceeded && Null.Value == 1, TEXT("generated_interface_null_reference_membership"), Null.Error);
        auto NullCall = Runtime.RunFile(InterfacePath, 10, false, NonInterface.Get());
        Expect(!NullCall.bSucceeded && NullCall.Error.Contains(TEXT("null")),
                                                               TEXT("generated_interface_null_dispatch_refused"),
                                                                    NullCall.Error);
        auto Wrong = Runtime.RunFile(InterfacePath, 0, false, NonInterface.Get());
        Expect(!Wrong.bSucceeded && Wrong.Error.Contains(TEXT("does not implement")),
                                                         TEXT("generated_interface_nonimplementer_conversion_refused"),
                                                              Wrong.Error);
        auto Forged = Runtime.RunFile(InterfacePath, 11, false, NonInterface.Get());
        Expect(!Forged.bSucceeded && Forged.Error.Contains(TEXT("does not implement")),
                                                           TEXT("generated_interface_forged_reference_refused"),
                                                                Forged.Error);
        auto Nominal = Runtime.RunFile(Runtime.ScriptPath(TEXT("generated_interface_wrong_type.das")), 0, false,
                                                          NonInterface.Get());
        Expect(!Nominal.bSucceeded && Nominal.Error.Contains(TEXT("ue_probe_Query")),
                                                             TEXT("generated_interface_wrong_nominal_type_refused"),
                                                                  Nominal.Error);
    }
    const auto EnumCopy = Run(TEXT("enum_local_copy.das"));
    Expect(EnumCopy.bSucceeded && EnumCopy.Value == 7, TEXT("enum_token_local_copy_and_reassignment"), EnumCopy.Error);
    {
        FDasAuthoringModule TypeCalls;
        FString TypeCallIssue;
        Expect(Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_type_calls.das")), TypeCalls, TypeCallIssue),
                                           TEXT("body_type_tokens_and_short_strings_compile"), TypeCallIssue);
        for (const auto* InvalidName : {TEXT("wrong_kind"), TEXT("unknown"), TEXT("expression"), TEXT("arity")})
        {
            const FString InvalidFile = TEXT("dsl_type_calls_bad_")+ FString(InvalidName) + TEXT(".das");
            const bool Refused =
                !Runtime.ReadAuthoringScript(Runtime.ScriptPath(InvalidFile), TypeCalls, TypeCallIssue);
            const FString Required = FString(InvalidName) == TEXT("wrong_kind")
                                     ? TEXT("Unknown authored interface")
                                     : FString(InvalidName) == TEXT("unknown")
                                           ? TEXT("Unknown class type identity")
                                           : FString(InvalidName) == TEXT("expression")
                                                 ? TEXT("requires a type name or literal identity")
                                                 : TEXT("requires exactly one type identity");
            Expect(Refused && TypeCallIssue.Contains(Required),
                   *(FString(TEXT("body_type_token_refuses_")) + FString(InvalidName)), TypeCallIssue);
        }
    }
    {
        FDasAuthoringModule TypedModel;
        FString Issue;
        const bool Compiled =
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_typed_objects.das")), TypedModel, Issue);
        Expect(Compiled && TypedModel.Classes.Num() == 1, TEXT("typed_object_macro_example_compiles"), Issue);
        if (Compiled)
        {
            const auto& C = TypedModel.Classes[0];
            const auto* Get = C.Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("GetTarget"); });
            const auto* Echo = C.Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("EchoActor"); });
            const auto* Target = C.Properties.FindByPredicate([](const auto& P) { return P.Name == TEXT("Target"); });
            Expect(C.Properties.Num() == 2 && Target &&
                   Target->Type.Kind == TEXT("object") &&
                                             Target->Type.TypePath ==
                                                 TEXT("/Script/Engine.Actor") &&
                                                      Target->Default.Kind ==
                                                          TEXT("null"),
                                                               TEXT("typed_object_field_infers_native_class_and_null"));
            Expect(Get && Echo &&
                   Get->ReturnType.TypePath == TEXT("/Script/Engine.Actor") &&
                                                    Echo->Parameters[0].Type.TypePath == Get->ReturnType.TypePath,
                                                    TEXT("typed_object_parameters_results_infer_native_class"));
        }
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_typed_objects_bad_macro.das")), TypedModel,
                                                               Issue),
                                            TEXT("typed_object_nonobject_macro_argument_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_typed_objects_bad_assignment.das")),
                                                               TypedModel, Issue),
                                            TEXT("typed_object_incompatible_source_assignment_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_typed_objects_bad_constraint.das")),
                                                               TypedModel, Issue),
                                            TEXT("typed_object_conflicting_constraint_refused"));
        Expect(!Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_typed_objects_bad_dimension.das")), TypedModel, Issue) &&
            Issue.Contains(TEXT("fixed-array")), TEXT("typed_object_fixed_array_not_lowered_to_scalar"));
    }
    {
        FDasAuthoringModule NativeInterface;
        FString NativeIssue;
        const bool NativeOK = Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_definition.das")),
                                                                                NativeInterface, NativeIssue);
        auto Method = [&](const TCHAR* Name) -> const FDasDeclaredMethod*
        {
            return NativeOK ? NativeInterface.Classes[0].Methods.FindByPredicate([&](const auto& M)
                                                                                 { return M.Name == Name; })
                            : nullptr;
        };
        const auto* ActorMethod = Method(TEXT("EchoActor"));
        const auto* VectorMethod = Method(TEXT("EchoVector"));
        const auto* EnumMethod = Method(TEXT("EchoMode"));
        const auto* StructMethod = Method(TEXT("EchoPayload"));
        Expect(ActorMethod && ActorMethod->ReturnType.TypePath ==
                                  TEXT("/Script/Engine.Actor") &&
                                       ActorMethod->Parameters[0].Type.TypePath == ActorMethod->ReturnType.TypePath,
                                       TEXT("authored_interface_typed_object_identity_inferred"), NativeIssue);
        Expect(VectorMethod && VectorMethod->ReturnType.TypePath ==
                                   TEXT("/Script/CoreUObject.Vector"),
                                        TEXT("authored_interface_math_identity_inferred"), NativeIssue);
        Expect(EnumMethod && StructMethod &&
               EnumMethod->ReturnType.TypePath ==
                   TEXT("/Script/DasRuntime.EDasCallMode") &&
                        StructMethod->ReturnType.TypePath ==
                            TEXT("/Script/DasRuntime.DasCallPayload"),
                                 TEXT("authored_interface_native_enum_struct_constraints"), NativeIssue);
        for (const auto* RefType : {TEXT("object"), TEXT("math")})
        {
            const FString File = TEXT("dsl_interface_native_")+ FString(RefType) + TEXT("_ref.das");
            Expect(Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(File), NativeInterface, NativeIssue) &&
                   NativeInterface.Classes[0].Methods[0].Parameters[0].Direction ==
                       (FString(RefType) == TEXT("math")? TEXT("out"): TEXT("inout")),
                        *(TEXT("authored_interface_native_accepts_")+ FString(RefType) + TEXT("_ref")), NativeIssue);
        }
        for (const auto* Name :
             {TEXT("constraint"), TEXT("wrong_kind"), TEXT("missing_enum"), TEXT("missing_struct"), TEXT("unused")})
        {
            const FString File = TEXT("dsl_interface_native_bad_")+ FString(Name) + TEXT(".das");
            Expect(!Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(File), NativeInterface, NativeIssue),
                   *(TEXT("authored_interface_native_refuses_")+ FString(Name)), NativeIssue);
        }
    }
    {
        FDasAuthoringModule StubInterfaces;
        FString StubIssue;
        Expect(Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_abstract_legacy.das")),
                                                                 StubInterfaces, StubIssue) &&
                                              StubInterfaces.Classes[0].Methods.Num() == 4,
                                              TEXT("interface_legacy_abstract_syntax_remains_readable"), StubIssue);
        Expect(Runtime.ReadAuthoredInterfaces(
            Runtime.ScriptPath(TEXT("dsl_interface_definition.das")), StubInterfaces, StubIssue) &&
            StubInterfaces.Classes[0]
                .Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Query"); })
                ->Metadata.FindRef(TEXT("Category")) ==
                                   TEXT("Counter"), TEXT("interface_method_annotation_metadata_collected"), StubIssue);
        FDasAuthoringModule PureInterfaces;
        FString PureIssue;
        const bool PureRead = Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_pure.das")),
                                                                                PureInterfaces, PureIssue);
        Expect(PureRead && PureInterfaces.Classes.Num() == 1 && PureInterfaces.Classes[0].Methods.Num() == 3 &&
                   PureInterfaces.Classes[0].Methods.ContainsByPredicate(
                       [](const auto& M) { return M.Name == TEXT("Query") && M.bPure && M.bCallable; }) &&
                   PureInterfaces.Classes[0].Methods.ContainsByPredicate(
                       [](const auto& M) { return M.Name == TEXT("Snapshot") && M.bPure; }) &&
                   PureInterfaces.Classes[0].Methods.ContainsByPredicate(
                       [](const auto& M) { return M.Name == TEXT("Impure") && !M.bPure; }),
               TEXT("interface_pure_collector_flags_out_and_impure_optout"), PureIssue);
        FString PureJson;
        FDasAuthoringModule PureRoundTrip;
        Expect(PureRead && PureInterfaces.ToJson(PureJson, PureIssue) &&
                   FDasAuthoringModule::FromJson(PureJson, PureRoundTrip, PureIssue) &&
                   PureRoundTrip.Classes[0].Methods.ContainsByPredicate([](const auto& M)
                                                                        { return M.Name == TEXT("Query") && M.bPure; }),
               TEXT("interface_pure_IR_roundtrip"), PureIssue);
        Expect(!Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_pure_bad_void.das")),
                                                                  PureInterfaces, PureIssue),
                                               TEXT("interface_pure_void_without_output_refused"));
        Expect(!Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_pure_bad_callable.das")),
                                                                  PureInterfaces, PureIssue),
                                               TEXT("interface_pure_noncallable_refused"));
        Expect(!Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_stub_bad_body.das")),
                                                                  StubInterfaces, StubIssue),
                                               TEXT("interface_stub_body_refused"));
        Expect(!Runtime.ReadAuthoredInterfaces(
            Runtime.ScriptPath(TEXT("dsl_interface_stub_bad_missing_annotation.das")), StubInterfaces, StubIssue),
            TEXT("interface_stub_missing_annotation_refused"));
        Expect(!Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_stub_bad_unknown_out.das")),
                                                                  StubInterfaces, StubIssue),
                                               TEXT("interface_stub_unknown_out_refused"));
        Expect(!Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_stub_bad_overlap.das")),
                                                                  StubInterfaces, StubIssue),
                                               TEXT("interface_stub_overlap_refused"));
        Expect(Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_stub_bad_missing_ref.das")),
                                                                 StubInterfaces, StubIssue),
                                              TEXT("interface_stub_mutable_ref_infers_inout"), StubIssue);
        Expect(!Runtime.ReadAuthoredInterfaces(
            Runtime.ScriptPath(TEXT("dsl_interface_stub_bad_duplicate_annotation.das")), StubInterfaces, StubIssue),
            TEXT("interface_stub_duplicate_annotation_refused"));
        Expect(!Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_stub_bad_unknown_flag.das")),
                                                                  StubInterfaces, StubIssue),
                                               TEXT("interface_stub_unknown_flag_refused"));
        Expect(!Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_stub_bad_default.das")),
                                                                  StubInterfaces, StubIssue),
                                               TEXT("interface_stub_default_refused"));
        Expect(!Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_stub_bad_field.das")),
                                                                  StubInterfaces, StubIssue),
                                               TEXT("interface_stub_field_refused"));
        Expect(!Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_interface_stub_bad_static.das")),
                                                                  StubInterfaces, StubIssue),
                                               TEXT("interface_stub_static_refused"));
        const auto StubCall = Run(TEXT("dsl_interface_stub_call.das"));
        Expect(!StubCall.bSucceeded && StubCall.Error.Contains(TEXT("declaration stub cannot be called directly")),
                                                               TEXT("interface_stub_direct_call_traps"),
                                                                    StubCall.Error);
    }
    {
        FDasAuthoringModule RulesModel, LegacyModel;
        FString RulesIssue, RulesWire, LegacyWire;
        const bool RulesOK =
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_type_syntax.das")), RulesModel, RulesIssue);
        Expect(RulesOK && RulesModel.Classes[0].NativeParent == TEXT("/Script/Engine.Actor"),
                                                                     TEXT("type_syntax_parent_native_name_resolves"),
                                                                          RulesIssue);
        if (RulesOK)
        {
            const auto& C = RulesModel.Classes[0];
            const auto* Echo = C.Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Echo"); });
            const auto* Payload = C.Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Payload"); });
            const auto* Mode = C.Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Mode"); });
            Expect(Echo &&
                   Echo->ReturnType.TypePath == TEXT("/Script/Engine.Actor") &&
                                                     Echo->Parameters[0].Type.TypePath == Echo->ReturnType.TypePath,
                                                     TEXT("native_class_names_canonicalize"));
            Expect(Payload && Payload->ReturnType.TypePath ==
                                  TEXT("/Script/DasRuntime.DasCallPayload") && Mode &&
                                       Mode->ReturnType.TypePath == TEXT("/Script/DasRuntime.EDasCallMode"),
                                                                         TEXT("native_enum_struct_names_canonicalize"));
        }
        Expect(Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_interface_native_names.das")), RulesModel, RulesIssue) &&
            RulesModel.Classes[0].Interfaces[0] == TEXT("/Script/DasRuntime.DasProbeInterface"),
                                                        TEXT("native_interface_name_uses_shared_resolver"), RulesIssue);
        Expect(Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_out_ref.das")), RulesModel, RulesIssue) &&
            Runtime.ReadAuthoringScript(
                Runtime.ScriptPath(TEXT("dsl_out_ref_csv_legacy.das")), LegacyModel, RulesIssue) &&
                RulesModel.ToJson(RulesWire, RulesIssue) && LegacyModel.ToJson(LegacyWire, RulesIssue) &&
                RulesWire.Replace(TEXT("dsl_out_ref.das"), TEXT("dsl_out_ref_csv_legacy.das")) == LegacyWire,
                                  TEXT("tuple_and_CSV_directions_produce_identical_contracts"), RulesIssue);
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_type_syntax_bad_parent_missing.das")),
                                                               RulesModel, RulesIssue),
                                            TEXT("type_syntax_parent_missing_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_type_syntax_bad_wrong_kind.das")), RulesModel,
                                                               RulesIssue),
                                            TEXT("type_syntax_wrong_kind_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_type_syntax_bad_duplicate_function.das")),
                                                               RulesModel, RulesIssue),
                                            TEXT("type_syntax_duplicate_function_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_type_syntax_bad_duplicate_constraint.das")),
                                                               RulesModel, RulesIssue),
                                            TEXT("type_syntax_duplicate_constraint_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_type_syntax_bad_duplicate_return.das")),
                                                               RulesModel, RulesIssue),
                                            TEXT("type_syntax_duplicate_return_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_type_syntax_bad_duplicate_callable.das")),
                                                               RulesModel, RulesIssue),
                                            TEXT("type_syntax_duplicate_callable_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_tuple_bad_duplicate.das")), RulesModel,
                                                               RulesIssue),
                                            TEXT("tuple_duplicate_refused"));
        Expect(
            !Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_tuple_bad_overlap.das")), RulesModel, RulesIssue),
                                         TEXT("tuple_overlap_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_tuple_bad_wrong_value.das")), RulesModel,
                                                               RulesIssue),
                                            TEXT("tuple_wrong_value_refused"));
        Expect(
            !Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_tuple_bad_unknown.das")), RulesModel, RulesIssue),
                                         TEXT("tuple_unknown_refused"));
        Expect(!Runtime.ReadAuthoringScript(
            Runtime.ScriptPath(TEXT("dsl_tuple_bad_empty.das")), RulesModel, RulesIssue), TEXT("tuple_empty_refused"));
        Expect(Runtime.ReadAuthoredInterfaces(
            Runtime.ScriptPath(TEXT("dsl_interface_method_id.das")), RulesModel, RulesIssue) &&
            RulesModel.Classes[0].Methods.ContainsByPredicate(
                [](const auto& M) { return M.Name == TEXT("Query") && M.Id == TEXT("contract.score"); }),
            TEXT("interface_explicit_method_ID_roundtrips"), RulesIssue);
    }
    {
        FDasAuthoringModule ParameterModel, ExplicitParameterModel;
        FString ParameterIssue, ImplicitWire, ExplicitWire;
        const bool ParameterOK = Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_parameter_directions.das")),
                                                                                ParameterModel, ParameterIssue);
        Expect(ParameterOK, TEXT("parameter_metadata_directions_compile"), ParameterIssue);
        if (ParameterOK)
        {
            const auto& C = ParameterModel.Classes[0];
            const auto* Adjust = C.Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Adjust"); });
            const auto* Bump = C.Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Bump"); });
            Expect(Adjust && Adjust->Parameters[0].Direction ==
                                 TEXT("inout") &&
                                      Adjust->Parameters[1].Direction ==
                                          TEXT("out") &&
                                               Adjust->Parameters[2].Direction ==
                                                   TEXT("in"), TEXT("parameter_inferred_ref_out_and_input_contract"));
            Expect(Bump && Bump->Parameters[0].Name ==
                               TEXT("value") &&
                                    Bump->Parameters[0].Id ==
                                        TEXT("paramdirections.Counter.Bump.value") &&
                                             Bump->Parameters[0].Direction ==
                                                 TEXT("inout"),
                                                      TEXT("inferred_single_ref_preserves_source_name_and_ID"));
        }
        Expect(ParameterOK &&
               Runtime.ReadAuthoringScript(
                   Runtime.ScriptPath(TEXT("dsl_parameter_directions_explicit.das")), ExplicitParameterModel,
                                      ParameterIssue) &&
                   ParameterModel.ToJson(ImplicitWire, ParameterIssue) &&
                   ExplicitParameterModel.ToJson(ExplicitWire, ParameterIssue) &&
                   ImplicitWire.Replace(TEXT("dsl_parameter_directions.das"),
                                             TEXT("dsl_parameter_directions_explicit.das")) == ExplicitWire,
                                        TEXT("optional_ue_ref_marker_has_identical_contract"), ParameterIssue);
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_parameter_direction_bad_by_value.das")),
                                                               ParameterModel, ParameterIssue),
                                            TEXT("parameter_direction_by_value_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_parameter_direction_bad_constant.das")),
                                                               ParameterModel, ParameterIssue),
                                            TEXT("parameter_direction_constant_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_parameter_direction_bad_overlap.das")),
                                                               ParameterModel, ParameterIssue),
                                            TEXT("parameter_direction_overlap_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_parameter_direction_bad_duplicate.das")),
                                                               ParameterModel, ParameterIssue),
                                            TEXT("parameter_direction_duplicate_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_parameter_direction_bad_false.das")),
                                                               ParameterModel, ParameterIssue),
                                            TEXT("parameter_direction_false_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_parameter_direction_bad_wrong_value.das")),
                                                               ParameterModel, ParameterIssue),
                                            TEXT("parameter_direction_wrong_value_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_parameter_direction_bad_unknown.das")),
                                                               ParameterModel, ParameterIssue),
                                            TEXT("parameter_direction_unknown_refused"));
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_parameter_direction_bad_legacy_conflict.das")),
                                                               ParameterModel, ParameterIssue),
                                            TEXT("parameter_direction_legacy_conflict_refused"));
    }
#include "DasDeclarationGroupTests.inl"
#include "DasCommentTooltipTests.inl"
#include "DasDelegateValueTests.inl"
#include "DasInheritanceTests.inl"
    const auto Recovery = Run(TEXT("smoke.das"));
    Expect(Recovery.bSucceeded && Recovery.Value == 42, TEXT("recovery_after_failures"), Recovery.Error);
    Report += FString::Printf(TEXT("DAS_FOUNDATION_RESULT=%s failures=%d\n"),
                                   Failures ? TEXT("FAIL") : TEXT("PASS"), Failures);
    return Failures == 0;
}
