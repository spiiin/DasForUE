# DasForUE2

New, independent daScript integration for Unreal Engine. The old `C:\src\DasForUE`
proof of concept is a reference only; none of its generated bindings are used here.

## Foundation scope

- UE runtime module with a pinned daScript 0.6.4 static compiler/runtime.
- Isolated editor commandlet and a separate C++ smoke game.
- Game-thread execution, independently owned program/context per call, checked
  entry-point signature, compile/runtime diagnostics, UTF-8 copies across the boundary.
- File access through Unreal's platform file system, including staged UFS/Pak content.
- Native C++ call from interpreted daScript and a generated AOT function. The AOT
  test requires `SimFunction::aot == true`; interpreter fallback does not pass.
- An experimental structure macro emits two successive type descriptions in the
  same process. Annotation spelling is disposable and is **not** the chosen DSL.

The foundation tests alone do **not** implement structural Unreal hot reload; see the separate editor probe below. The macro produces plain owned
declarations; it does not create `UClass`, modify CDOs, migrate actors or reinstance
Blueprints. The separate editor probe now tests these through a Blueprint backend. General UObject bindings, script-side object handles,
delegates, gameplay state migration, Shipping, replication and a production sandbox
are also outside this foundation.

## Reproduce on Windows

Prerequisites: source-built UE 5.8.3, VS 2022 17.14 / MSVC 14.44, Windows SDK
10.0.22621.0, Git, CMake. The currently tested host supports AVX2; upstream CMake
selects SIMD based on the build host. Broader CPU support has not been validated.
See `dependencies.json` for the exact upstream commit. Existing user daScript
checkouts are never used or modified.

```powershell
.\Scripts\BuildDependencies.ps1
.\Scripts\TestEditor.ps1
.\Scripts\TestPackaged.ps1
```

The default engine is `C:\src\unreal\UnrealEngine`; test scripts accept `-Engine`.
BuildDependencies accepts `-VisualStudio`. The isolated project is
`Tests\DasForUESmoke\DasForUESmoke.uproject`, and only this plugin is explicitly
enabled. Build and test logs live under `logs/`, packaged output under
`Build/Packaged/Windows/`.

## Important integration choices

- daScript uses `/MD`, no RTTI, and exceptions enabled in its UE module.
- `DAS_NO_GLOBAL_NEW_AND_DELETE` prevents daScript from replacing UE's global
  allocation functions. Do not suppress duplicate symbols with linker overrides.
- `DAS_FREE_LIST`, `DAS_FUSION`, assertion and exception defines match the pinned
  Release library. Debug CRT is not supported by this configuration.
- Unreal's `verify`, `ensure`, `check` macros are temporarily hidden while including
  daScript headers. Upstream headers are not patched.
- No C++ parser over Unreal headers and no generated engine-wide binding set.
- Runtime module dynamic unloading is disabled: compiled code and module lifetime
  need a dedicated reload design. Script compilation uses local owned contexts.
- Native type-description emission is allowed only in the active runtime context;
  a compiler/macro context cannot be blindly cast to the Unreal context subclass.

## Editor reload feasibility probe

See [the implementation report](Docs/ReloadProbe.md). Run:

```powershell
.\Scripts\TestReload.ps1
.\Scripts\TestReloadPIE.ps1
```

The schema macro feeds an internal Blueprint backend that creates a real
UBlueprintGeneratedClass with int/float fields and a fixed script-backed method.
It has passed additive structural reload, Blueprint descendant/instance migration,
CDO and instance overrides, references, transforms, GC and fresh-process reopening.
A real PIE automation test also passes, preserving different values in editor and
play worlds and verifying the editor instance after EndPIE. This is not a finished
DSL, arbitrary native UClass synthesis or a general migration system.

Typed IR and an editor service are now extracted; weak self/property access is implemented.
See [the interop guide](Docs/Interop.md) for examples, API boundaries and validation.
Next: general UObject/function bindings, managed event callbacks,
subscriptions, metadata changes and cook parity.
## Third-party notices

daScript is BSD-3-Clause, copyright Gaijin Games Kft.; its license and dependency
notices remain in `ThirdParty/daScript`. Distribution must preserve applicable notices.

## Roadmap

See [the audited feature plan](Docs/Plan.md) for the NimForUE comparison, priorities and acceptance gates.

## Validation — 2026-10-05

UE 5.8.3 / Win64 Development: all 12 foundation checks passed in both the
DasSmoke editor commandlet and the packaged game; both processes exited with 0.
The game log identifies the archived RemappedPlugins content root and mounted Pak.
The staging manifest contains the macro library, invalid-schema and UTF-8 probes.

Checks cover native roundtrip, independent contexts, syntax/signature/runtime/missing
file errors, successive macro schemas, native AOT execution without fallback,
unsupported schema rejection, UTF-8 ownership after context destruction, and recovery.
Logs: logs/smoke-editor.log, logs/smoke-packaged.log, logs/package-final.log.
This validates a headless Development foundation, not Shipping, renderer output,
UObject interoperability, actual structural reload or other platforms.

## Typed schema and self access

See [Docs/Interop.md](Docs/Interop.md). The latest suite passes 21 foundation checks
in editor and packaged game, structural reload/reopen, and a real PIE automation test.
Examples live in Plugins/DasForUE/Content/Das/Scripts; start with self_properties.das.

## Persistent contexts and logic reload

Actor calls now reuse one context per owner/script. Source and imported-file changes
prepare a candidate between calls; invalid code keeps the previous version active.
Explicit named int32 state survives logic reload. Ordinary VM globals survive calls
but are reinitialized on reload. Actor destruction, world cleanup and GC release sessions.
See [Docs/PersistentContexts.md](Docs/PersistentContexts.md) and `persistent_counter.das`.

Validation: 44 foundation checks passed in editor and packaged Win64 Development;
real PIE logic reload, independent counters, failure fallback and EndPIE cleanup passed.
Structural reload and fresh-process asset reopening also passed. Compilation is currently
synchronous and per owner; shared compiled modules and structural script-state migration
remain future work.

## Actor lifecycle callbacks

Set `LifecycleScript` on an ADasProbeActor before play to receive optional exported
`begin_play()`, `tick(delta_time: float)` and `end_play(reason: int)` handlers.
Logic reload preserves explicit state and does not replay BeginPlay. EndPlay runs
before release, including Destroy; invalid candidates retain the last working code.
See [Docs/Lifecycle.md](Docs/Lifecycle.md) and `lifecycle_actor.das` (requires the
`health:int` and `speed:float` fields from the schema_v2 example).

Validation: 66 foundation checks passed in editor and packaged Win64 Development.
`Scripts/TestLifecyclePIE.ps1` tests actual engine-delivered events, two Actors,
hot reload, failed-candidate fallback, Destroy and EndPIE. Structural regressions pass.
Lifecycle-session migration during structural Blueprint reinstancing is still pending.

## Managed multicast subscriptions

`bind_events()` declares `ue_subscribe_int("peer", "OnDasSignal", "on_signal")`;
the exported handler receives one int32. Bindings are validated before commit,
replaced on reload, and revoked on teardown without removing unrelated UE listeners.
The bridge uses weak sources/owners and opaque IDs, and queues nested events until
the active VM call finishes. See [Docs/Delegates.md](Docs/Delegates.md) and
`delegate_actor.das` for setup, behavior and current limitations.

85 foundation checks pass in editor and packaged Win64 Development.
`Scripts/TestDelegatesPIE.ps1` covers peer delivery, reload/fallback, independent
listeners, nested events and destruction during VM execution; existing regressions pass.
This slice is game-thread dynamic multicast int32 only, with self/peer sources.

## UObject handles and reflected function calls

`UObjectHandle` is an opaque, copyable daScript type backed by context-local weak
references. Handles support validity/class checks and navigation through object
properties; arbitrary handle sources can now feed managed event subscriptions.
Five validated ProcessEvent adapters cover int32, float32, void and object references.
Both native and Blueprint functions are supported; unsupported signatures fail before
dispatch. See [Docs/Objects.md](Docs/Objects.md) for the API and its limits.

Validation: 103 foundation checks pass in editor and packaged Win64 Development;
`Scripts/TestObjectsPIE.ps1` covers real Blueprint calls, engine SetOwner/GetOwner,
delegate delivery through ProcessEvent, and Destroy/GC. All existing regressions pass.
Class-specific generated types and AOT bindings remain future work.
General multi-argument marshalling is described below.

## General UFunction marshalling

Opaque call frames support multiple inputs, int32/float/double/bool/FString,
const-reference inputs, and nested reflected structs. Signature IR validates the
supported types; all inputs must be assigned before ProcessEvent. Frames initialize
and destroy UE values and are reclaimed at the end of each script invocation.
See [Docs/Marshalling.md](Docs/Marshalling.md), `marshalling.das` (test fixture), and
`actor_location.das` (ordinary Actor). This API does not choose the future DSL syntax.

Validation: 120 checks pass in editor and packaged Win64 Development; Objects PIE
also exercises the generic frame with Blueprint Echo and the engine FVector return.
Lifecycle, delegates and structural reload/reopen regressions pass.

## Object arguments and out/inout

Generic call frames now accept object inputs/outputs with class checks and temporary
strong references. Mutable scalar, string and struct references use the same named
argument/result API. Input, pure output and inout directions are checked from UE
reflection. See [Docs/InOut.md](Docs/InOut.md) and `inout_values.das` (test fixture).
Object fields inside structs and containers remain unsupported.

Validation for this slice: 134 foundation checks pass in editor and packaged Win64.
Objects PIE covers engine object arguments/results and Blueprint output pins;
the delegates PIE regression passes.

## Enums and names

Generic frames support reflected enums (including TEnumAsByte) and FName in inputs,
returns, out/inout and scalar struct fields. Enum setters require the declared type
path and a registered symbol; FName uses Unreal name semantics. See
[Docs/EnumsNames.md](Docs/EnumsNames.md) and `enum_names.das` (test fixture).
Generated daScript enum declarations and typed wrappers remain future work.

Validation for enum/FName: 152 checks pass in editor and packaged Win64; Objects PIE
passes engine enum/name calls and a Blueprint name roundtrip without test warnings.

## Generated bindings (explicit API selection)

`Scripts/GenerateBindings.ps1` exports reflection for `Bindings/Selection.json`
and produces `Bindings/Api.json` plus `ue_generated.das`. Use `-Check` to detect
stale IR/code without updating them. The current selection covers 18 Actor/fixture functions,
typed scalar arguments, generated enums, UEName and records for multiple outputs.
Wrappers check receiver class and signature before calling ProcessEvent.
See [Docs/GeneratedBindings.md](Docs/GeneratedBindings.md), `generated_calls.das`
and `generated_actor.das`. This is separate from the future class-authoring DSL.

Validation for generated bindings: 158 checks pass in editor and packaged Win64;
Objects PIE executes all five generated Actor wrappers. Eight generator tests,
freshness checking and the failed-export preservation regression pass.

## Named struct values (IR v2)

Generated wrappers now accept/return named daScript structs, including nested
values, strings, enums and UEName. FVector returns directly as a vector value;
inout returns an updated value without mutating the script input. Regenerate with
GenerateBindings.ps1; old flattened struct call sites need migration.
See [Docs/StructBindings.md](Docs/StructBindings.md). The selection now has 14 functions.

Named-struct validation: 161 checks pass in editor and packaged Win64, 15 generator
tests pass, and PIE verifies FVector input/output through real Actor scale functions.


## Typed UCLASS references (IR v3)

Generated receivers and object input/return/out/inout use distinct class structs.
Explicit upcasts/downcasts and nullable conversion preserve runtime class checks
and weak context-local ownership. Raw handles require an explicit conversion.
The selection contains 18 functions, three class identities and four value structs.
See [Docs/ClassBindings.md](Docs/ClassBindings.md) for migration and examples.

UCLASS validation: 172 foundation checks pass in editor and packaged Win64;
20 generator tests, freshness/export checks and Objects PIE pass.


## API selection and metadata policy

Selection.json now supports class_rules with include/exclude patterns and explicit
inherited-function selection. Required functions fail the export; optional and
discovered unsupported candidates produce diagnostics. Deprecated/internal APIs
require explicit opt-in. Editor-only export records selected metadata and textual
defaults; these do not alter generated calls. See [Docs/BindingPolicy.md](Docs/BindingPolicy.md).
Run Scripts/TestBindingsPolicy.py against a built editor for integration coverage.


## Typed default arguments

The generator now translates supported CPP_Default_* values into typed daScript
defaults. Calls may omit trailing inputs or explicitly override them. Unsupported
representations fail generation with function/parameter context, preserving the
previous module. Defaults follow reflected UHT values (including UHT rounding),
not reconstructed C++ expressions. See [Docs/Defaults.md](Docs/Defaults.md)
and generated_defaults.das. The current selection contains 21 functions.


## TArray value bindings

Top-level arrays of int32, float, double, bool, FString and FName are supported
for input, return and out/inout. Generated array<T> results use daScript move
semantics; inputs remain unchanged. Frames own UE storage, validate every index
and required input element, and cap each array at 65,536 elements.
See [Docs/Arrays.md](Docs/Arrays.md) and generated_arrays.das. The current selection
has 27 functions. Object/struct/enum arrays and nested containers remain future work.


## TArray structs, enums and objects (IR v4)

Generated arrays now support nested reflected value structs, enums and typed
UObject references. Frames validate every input leaf and hold object elements
through ProcessEvent, output reads and inout replacement. Returned arrays retain
weak handles only, including in persistent globals. GetAttachedActors is covered
with real Actors in PIE. Regenerate old IR with GenerateBindings.ps1.
See [Docs/ComplexArrays.md](Docs/ComplexArrays.md) and generated_complex_arrays.das.
The selection contains 36 functions. UObject fields within structs and nested
containers remain separate work.


## Scalar TMap/TSet bindings (IR v5)

Nominal wrappers carry typed Map entries and Set values, including FName and enum.
Staging values are validated for completeness and UE hash/equality uniqueness
before creating native containers. Input, return and out/inout are supported;
outputs account for sparse holes and use move semantics. Float keys must be finite;
signed zero is normalized. See [Docs/HashContainers.md](Docs/HashContainers.md),
generated_hash_containers.das and generated_hash_numbers.das. The selection contains
51 functions. Map/Set structs/objects and nested containers remain future work.


## Syntax-neutral authoring model

DasAuthoring defines modules/classes/properties/methods with stable IDs, typed
signatures/defaults, inheritance and metadata. Authoring JSON v1 is separate from
binding IR v5. Validation and change classification run in editor and packaged
runtime; the existing Actor Blueprint backend emits its supported scalar subset
after method compile/signature preflight. TestAuthoring.ps1 covers live instance
update, stable identity, refusal without mutation, save and reopen in a fresh process.
See [Docs/AuthoringModel.md](Docs/AuthoringModel.md) and Authoring/Counter.v1.json.
The chosen native annotation syntax is implemented in its first bounded slice; see Docs/DSLFrontend.md.


## Authoring methods and metadata

The Actor adapter emits up to 64 int-to-int methods with separate exported script
entry points. Methods in one owner/file share a persistent context and reload
fallback. Category/DisplayName/ToolTip are stored in Blueprint sources and reflection;
new scalar fields may be BlueprintReadOnly. See Docs/AuthoringMethods.md,
Authoring/Counter.v2.json and authoring_methods.das.


## Native annotation DSL

`require ue_dsl` enables `[ue_class]`, field `@ue_property` metadata and
`[ue_function]`. Native AST macros collect validated declarations and generate
UObject property bridges for method bodies. TestDSL.ps1 creates, updates, saves
and reopens an Actor Blueprint directly from dsl_counter_v1/v2.das. The current
slice supports int/float/bool fields, scalar same-type int/float/bool methods and void actions; manual editor Apply/Reimport and PIE reload are now available;
general signatures and constructor/component hooks remain further work.
See Docs/DSLFrontend.md for the supported syntax and limitations.

See Docs/DSLReimport.md for `Das.DSL.Apply` / `Das.DSL.Reimport` commands.

Typed signatures: Docs/DSLScalarSignatures.md and dsl_scalar_methods.das.

Mixed scalar methods (0–16 inputs): Docs/DSLMixedMethods.md, dsl_mixed_methods.das.

String/FName/double authoring: Docs/DSLTextTypes.md and dsl_text_methods.das.

Object input/result authoring: Docs/DSLObjectMethods.md and dsl_object_methods.das.

Native math method values: Docs/DSLMathValues.md and dsl_math_methods.das.

Generic reflected value structs: Docs/DSLReflectedStructs.md and dsl_reflected_methods.das.

Reflected struct properties and defaults: Docs/DSLStructFields.md. Example: Plugins/DasForUE/Content/Das/Scripts/dsl_struct_fields.das. Validation: Docs/DSLStructFieldsValidation.md.

Explicit default-only migration: Docs/DSLDefaultMigration.md (`defaults=preserve_instances`).

Stable ID field rename migration: Docs/DSLFieldRename.md (`fields=rename_by_id`).

Dependent Blueprint/map rename: Docs/DSLRenameDependencies.md (`fields=rename_with_dependencies`).

Exact numeric field widening: Docs/DSLNumericWiden.md (`fields=widen_numeric_with_dependencies`).

Numeric graph conversion and rollback: Docs/DSLNumericConversions.md (`fields=convert_numeric_with_dependencies`).


Field retirement/restoration: Docs/DSLFieldArchive.md
(`fields=archive_remove_with_dependencies` / `fields=archive_restore_with_dependencies`).
Tests: Scripts/TestDSLArchiveFieldsAsset.ps1 and Scripts/TestDSLArchiveFieldsPIE.ps1.


Lifecycle/delegate preservation during DSL reimport and field migration:
Docs/DSLLifecycleMigration.md. PIE test: Scripts/TestDSLLifecycleMigrationPIE.ps1.


Bounded structural reimport/rename rollback: Docs/DSLStructuralRollback.md.
Save/reopen test: Scripts/TestDSLStructuralRollbackAsset.ps1.


Typed UObject DSL fields: Docs/DSLObjectFields.md.
Example: Plugins/DasForUE/Content/Das/Scripts/dsl_object_fields.das.
Tests: Scripts/TestDSLObjectFieldsPIE.ps1 and Scripts/TestDSLObjectFieldsAsset.ps1.


Authored reflected type declarations: Docs/DSLTypeDeclarations.md.
Example: Plugins/DasForUE/Content/Das/Scripts/dsl_type_declarations.das.
Editor command: Das.DSL.Types.Apply Script.das /Game/Folder.


Authored enum/struct fields and method frames: Docs/DSLAuthoredValues.md.
Example: Plugins/DasForUE/Content/Das/Scripts/dsl_authored_values.das.
Tests: Scripts/TestDSLAuthoredValuesAsset.ps1 and Scripts/TestDSLAuthoredValuesPIE.ps1.


Direct plain daScript enum/struct DSL values: Docs/DSLDirectTypes.md.
Example: Plugins/DasForUE/Content/Das/Scripts/dsl_direct_types.das.
Tests: Scripts/TestDSLDirectTypesAsset.ps1 and Scripts/TestDSLDirectTypesPIE.ps1.


Stable-ID type catalogs and cooked direct values: Docs/DSLTypeRegistry.md.
Example: Plugins/DasForUE/Content/Das/Scripts/dsl_registered_types.das.
Test: Scripts/TestDSLTypeRegistry.ps1 -Cooked.


Append-only enum definition migration: Docs/DSLEnumAppend.md.
Command: Das.DSL.Types.ExtendEnums Script.das /Game/Registry.Registry.
Test: Scripts/TestDSLTypeRegistry.ps1 -EnumAppend -Cooked.


Stable enum member IDs and Switch repair: Docs/DSLEnumMembersAndSwitch.md.
Example IDs: Plugins/DasForUE/Content/Das/Scripts/dsl_enum_member_ids.das.
Test: Scripts/TestDSLTypeRegistry.ps1 -EnumAppend -EnumGraphs -Cooked.


Standard enum-node reconstruction: Docs/DSLEnumNodeRepair.md.
Test: Scripts/TestDSLTypeRegistry.ps1 -EnumAppend -EnumGraphs -EnumNodes -Cooked.


Enum member rename by stable ID: Docs/DSLEnumRename.md.
Command: Das.DSL.Types.RenameEnumMembers Script.das /Game/Registry.Registry.
Test: Scripts/TestDSLTypeRegistry.ps1 -EnumAppend -EnumGraphs -EnumNodes -EnumRename -Cooked.


Explicit enum removal/value policies: Docs/DSLEnumValuePolicies.md.
Command: Das.DSL.Types.MigrateEnumValues Script.das /Game/Registry.Registry Rules.json.


Authored struct add/rename: Docs/DSLStructFieldMigration.md.
Command: Das.DSL.Types.MigrateStructFields Script.das /Game/Registry.Registry.
Test: Scripts/TestDSLTypeRegistry.ps1 -StructFields -Cooked.


## Authored struct containers

Owned ue_struct fields now support primitive arrays, nested authored structs,
arrays of authored structs, maps with int/string keys and scalar/name values,
and int/string sets. A daScript
key-only table `table<T>` maps to native TSet, while `table<K;V>` maps to TMap.
See [Docs/DSLSetAuthoring.md](Docs/DSLSetAuthoring.md),
[Docs/DSLMapAuthoring.md](Docs/DSLMapAuthoring.md) and
[Docs/DSLPrimitiveArrays.md](Docs/DSLPrimitiveArrays.md).
Set example: `Plugins/DasForUE/Content/Das/Scripts/dsl_set_authoring.das`.
Container schema migration remains deferred; supported values travel inside
owned enclosing structs through typed frames and whole-property assignment.


## Standalone authored container methods

BlueprintCallable DSL methods can take/return primitive arrays, int/string sets
and scalar/name maps directly, with native Blueprint container pins. Twenty
shapes use private compiled owned carriers inside the existing frame ABI; no
authored wrapper struct or type registry is required. See
[Docs/DSLContainerCalls.md](Docs/DSLContainerCalls.md) and
Plugins/DasForUE/Content/Das/Scripts/dsl_container_calls.das.
Mixed scalar/container calls and packaged execution are verified. Explicit
DSL out/ref semantics remain the next step; migration work stays deferred.


## Explicit authored out/ref parameters

ue_function(ref="counter", out="total") supports primitive and current standalone
container parameters with mutable daScript references. Native Blueprint output/
reference pins use owned locals and deferred copy-back of all outputs after a
successful call. Failed execution or packing preserves ref inputs and returns
out defaults. See [Docs/DSLOutRef.md](Docs/DSLOutRef.md) and
Plugins/DasForUE/Content/Das/Scripts/dsl_out_ref.das. The next milestone is
interfaces; further out/ref types and migration policies remain separate work.


## Existing native interface implementations

ue_class(implements="/Script/Module.Interface") validates exact native method
contracts and emits real Blueprint interface function graphs. C++ Execute_* /
ProcessEvent dispatch to DSL methods supports current array and out/ref adapters.
Checked daScript membership and initial int/string dispatch to ordinary Blueprint
receivers are verified through cook. See [Docs/DSLInterfaces.md](Docs/DSLInterfaces.md)
and Plugins/DasForUE/Content/Das/Scripts/dsl_interfaces.das. Broader typed dispatch,
Authored interface definitions are supported; see Docs/DSLAuthoredInterfaces.md.
Remaining interface capabilities are later work.


## Typed interface call frames

ue_begin_interface_call(object,path,method) returns an interface-constrained
UCallHandle. Existing ue_arg_* / ue_result_* APIs cover supported scalar, struct,
array, map/set and out/inout values. Interface membership and reflected signature
are checked again at invocation. Scalar, array, hash and out/ref dispatch to
ordinary Blueprint receivers pass packaged checks. See
[Docs/DSLInterfaces.md](Docs/DSLInterfaces.md) and dsl_interfaces.das.
DSL-authored interface definitions are supported. VM reentry and native Execute fallback
remain separate capabilities.


## DSL-authored Blueprint Interfaces

ue_interface classes with annotated empty method stubs create real Blueprint Interface
assets through Das.DSL.Interfaces.Apply. Primitive/current container contracts,
inout references and named out slots use the existing Actor bridge. Runtime
generated classes preserve stable contract identity through cook and independent
recompilation. See [Docs/DSLAuthoredInterfaces.md](Docs/DSLAuthoredInterfaces.md),
dsl_interface_definition.das and dsl_interface_implementation.das. Structural
contract changes are explicitly deferred.


Repeatable `ue_implements(interface=CounterContract)` annotations resolve short
names within module_id; complete stable IDs and full Unreal paths also work.
Existing ue_class implements path lists remain compatible. Create/save interface
assets first. Unknown, ambiguous, duplicate and overlapping-method interface
contracts reject before mutation. See Docs/DSLAuthoredInterfaces.md.


DSL syntax audit and recommended follow-up work: [Docs/DSLSyntaxAudit.md](Docs/DSLSyntaxAudit.md).


## Typed UObject authoring

Import ue_object_types and use ActorRef/ObjectRef directly in reflected fields,
parameters and results. The aliases instantiate `$UEObject<Actor>`/Object in a
separate module and reuse generated nominal wrappers; native constraints follow
the declared type, without arg_name_class/return_class. This module boundary is
required by daScript's generic class-method parser restriction. Existing raw
handle syntax remains supported. See [Docs/DSLTypedObjects.md](Docs/DSLTypedObjects.md)
and dsl_typed_objects.das.


Interface methods now use `[ue_function(BlueprintCallable, ...)] def Name(...):Type {}`.
Per-method out/ref and metadata are supported; nonempty bodies reject and direct
script calls trap. Legacy abstract signatures remain readable. Metadata-only
interface reapplication preserves the generated class. See DSLAuthoredInterfaces.md.


## Shared type and annotation rules

Use parent=Actor, native/source type names or qualified stable IDs with the
existing explicit type_registry/type_folder scope. Class/interface out/ref lists
accept native tuples, e.g. ref=(counter,weights), out=(total,message), with legacy
CSV compatibility. Repeated singleton options and conflicting type constraints
reject before application. See [Docs/DSLTypeResolution.md](Docs/DSLTypeResolution.md).


Void/no-output interface methods now use real Actor Event Graph overrides, with
native Execute_* / ProcessEvent dispatch, owned input frames and body reload.
Return/out/inout methods retain function graph adapters. See
[Docs/DSLInterfaceEvents.md](Docs/DSLInterfaceEvents.md).


Preferred out/ref declarations now mark parameters directly: `var counter:int&`
means inout, `@ue_out var total:int&` means output-only, and `@ue_ref` is optional.
Legacy function lists remain compatible. See [Docs/DSLOutRef.md](Docs/DSLOutRef.md).


DSL method bodies can use source type identities instead of embedding Unreal paths:
`ue_begin_interface_call(target, ue_type(CounterContract), "Query")`. See
[DSLTypeCalls](Docs/DSLTypeCalls.md) for supported calls and scoped resolution.


Native interfaces selected for export now have typed references and generated calls:
`let api = DasProbeInterface_from(target); let result = ue_probe_Query(api, 5)`.
See [DSLGeneratedInterfaces](Docs/DSLGeneratedInterfaces.md) for outputs, ownership
and current capability limits.


Authored interface stubs now support native objects, math values and constrained
native enum/struct snapshots. See [DSLAuthoredInterfaceNativeTypes](Docs/DSLAuthoredInterfaceNativeTypes.md).


Authored interfaces can use concrete enum/struct signatures through a registry and
export isolated typed binding modules. See [DSLAuthoredInterfaceRegistryBindings](Docs/DSLAuthoredInterfaceRegistryBindings.md).


Interface bindings now support `api.Query(5)` and `api.Stats(counter, total)` with
automatic caller ref/out copyback. See [DSLInterfaceMethodCalls](Docs/DSLInterfaceMethodCalls.md).


Typed factory syntax is `ue_interface(type<CounterContract>, target)`. Explicit
source-type mappings allow `api.EchoState(InterfaceState.Active)` and natural struct
fields. See [DSLInterfaceSourceValues](Docs/DSLInterfaceSourceValues.md).
