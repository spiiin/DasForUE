# Interfaces

DSL classes can implement existing native `UInterface`s, define new Blueprint
Interfaces in daScript, and call interface methods on any object — native, Blueprint
or DSL-authored — through typed, checked interface references.

## Implementing native interfaces

```das
[ue_class(module_id="sample", Blueprintable),
 ue_implements(interface="/Script/DasRuntime.DasProbeInterface")]
class InterfaceActor {
    [ue_function(BlueprintCallable)]
    def Query(Input:int):int { return Input + 7 }
    [ue_function(BlueprintCallable)]
    def Label():string { return "Interface 世界" }
    [ue_function(BlueprintCallable)]
    def Stats(var Counter:int&; @ue_out var Total:int&):void {
        Counter += 3
        Total = Counter * 2
    }
    // ...implement every remaining interface method.
}
```

- `ue_implements(interface="...")` is repeatable; annotation order does not matter.
  The older list form `ue_class(implements="/Script/A,/Script/B")` (up to 16 paths)
  is still accepted.
- Every reflected interface method must be implemented with exactly matching
  parameter names, types, directions, return type, purity (`BlueprintPure`) and
  constness (`def const`). Ordinary `UObject` methods inherited by the interface are
  not part of the contract. Missing or incompatible methods are rejected **before**
  the Blueprint is modified.
- Methods with a return value or out/ref parameters become real interface function
  graphs. Methods with a `void` result and no outputs become overriding Event Graph
  nodes (see *Interface events* below).
- The generated class passes `ImplementsInterface`; `ProcessEvent`, Blueprint
  interface messages and C++ `Execute_*` calls all reach the DSL body.
- Membership survives save, reopen and cook. Body-only reimport keeps the contract
  and hot-reloads the logic without changing the Blueprint class.

## Defining interfaces

Declare a Blueprint Interface with ordinary annotated methods and **empty bodies**:

```das
options gen2
require ue_dsl

[ue_interface(module_id="sample", BlueprintType)]
class CounterContract {
    [ue_function(BlueprintCallable, Category="Counter", DisplayName="Get Score")]
    def Query(Input:int):int {}

    [ue_function(BlueprintCallable)]
    def Label():string {}

    [ue_function(BlueprintCallable)]
    def Weights(Values:array<double>):array<double> {}

    [ue_function(BlueprintCallable)]
    def Stats(var Counter:int&; @ue_out var Total:int&):void {}

    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void {}

    [ue_function(BlueprintPure)]
    def const Peek():int {}
}
```

Rules:

- Bodies must be empty: statements, fields, static methods, constructors and
  finalizers are rejected. The empty body is a signature, not an implementation;
  calling it directly from daScript fails with
  `ue_interface declaration stub cannot be called directly`.
- Each method has exactly one `ue_function` annotation with `BlueprintCallable` or
  `BlueprintPure`. Supported options: `BlueprintPure`, `id`, `Category`,
  `DisplayName`, `ToolTip`. Unknown options and parameter defaults are rejected.
- Directions: `var T&` is inout (ref), `@ue_out var T&` is output only, `@ue_ref` is
  an optional explicit inout marker. Const refs and conflicting markers are
  rejected.
- 1–64 methods per interface, at most 16 parameters per method.
- Stable IDs default to `module_id.ClassName` and member names; explicit `id`
  options are available. Authored enum/struct parameters need
  `type_registry="..."` (or `type_folder`) on `ue_interface`, as for classes.
- The older `def abstract` form with interface-level `out="Method.Parameter"` lists
  is still readable and derives the same native signatures.

### Creating the interface asset

```text
Das.DSL.Interfaces.Apply dsl_interface_definition.das /Game/DasInterfaces
```

This creates real Blueprint Interface assets and marks them dirty — **save them**
in the editor. Reapplying an identical definition reuses the same objects;
metadata-only changes (Category, DisplayName, ToolTip) update in place. Conflicting
packages and structural contract changes are rejected before modification. The
stable ID and contract survive cooking.

### Implementing an authored interface

```das
[ue_class(module_id="sample", Blueprintable),
 ue_implements(interface=CounterContract)]
class CounterActor {
    [ue_function(BlueprintCallable)]
    def Query(Input:int):int { return Input + 7 }
    // ...every remaining method with its exact signature.
}
```

Create and save the interface assets **before** applying the implementing class.
The `interface` value can be:

| Form | Example | Resolves to |
| --- | --- | --- |
| Short name | `interface=CounterContract` | `<implementing module_id>.CounterContract` |
| Stable ID | `interface="contracts.CounterContract"` | Cross-module or explicit IDs |
| Generated class path | `interface="/Game/DasInterfaces/CounterContract.CounterContract_C"` | That exact asset |
| Native path | `interface="/Script/DasRuntime.DasProbeInterface"` | Native interface |

The unquoted identifier is a name string, not a type expression. In the editor,
saved interfaces are found by a searchable StableId tag, so they resolve without
being loaded; nothing is created implicitly. Multiple assets with one stable ID
are ambiguous — use the explicit path. For assets created by an older plugin
version, reapply the identical definition and save to add the tag. In a cooked
game, IDs resolve from loaded interface classes; implementing Blueprints hard-reference
them, so they are cooked as dependencies.

Method names must be distinct across all interfaces a class implements, even if
the signatures match.

### Interface events

Interface methods with a `void` result and no out/inout parameters use real
Event Graph nodes in the implementing Actor Blueprint, both for authored interfaces
and for native `BlueprintNativeEvent` declarations:

```das
[ue_class(module_id="game", Blueprintable), ue_implements(interface=ScoreEvents)]
class ScoreActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 0
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void { self.Score += amount }
    [ue_function(BlueprintCallable)]
    def ClearScore():void { self.Score = 0 }
}
```

Inputs (including arrays and strings) are copied into owned values. Event nodes keep
stable member IDs across reimport. Removing or renaming an event is a structural
change and is rejected.

## Calling interfaces

### Interface references

An interface reference is a nominal wrapper around a checked weak object handle.
Native interfaces get wrappers from the regular binding generator
(`ue_generated`, selected in `Bindings/Selection.json`). Authored interfaces get
them from a separate generated module:

```text
python Scripts/GenerateBindings.py <exported IR>.json <out>.das --module ue_contract_bindings
```

A minimal selection entry is
`{ "class": "/Game/.../ValueContract.ValueContract_C", "function": "EchoState", "alias": "ue_contract_EchoState" }`.
Regenerate after moving or changing a contract; an ID mismatch is rejected at
compile time.

```das
require ue_foundation
require ue_contract_bindings

let api = ue_ref(type<CounterContract>, target)
if (CounterContract_valid(api)) {
    let answer = api.Query(5)

    var counter = 10
    var total = -1
    api.Stats(counter, total)      // counter and total updated after the call
}
```

| Helper | Behavior |
| --- | --- |
| `ue_ref(type<T>, obj)` | Checked construction; null allowed; a non-null expired or non-implementing object is rejected |
| `T_null()` | Empty reference |
| `T_valid(ref)` | False for null, expired or non-implementing objects |
| `T_raw(ref)` | Underlying handle |

Friendly names are generated only when unambiguous; otherwise the canonical
generated names remain. Authored names omit the `_C` suffix. Calling through a null
reference is an error. A wrong interface type fails type checking, and a manually
constructed wrapper is still validated at dispatch.

For a quick membership test without wrappers:
`ue_implements_interface(target, ue_type(CounterContract))` (or a path string).

### Method calls

`api.Method(args)` is ordinary daScript dot-call syntax over generated functions:

- The return value becomes the result; every out and inout parameter is a `var`
  argument updated after the call. Output-only values are not sent to the callee.
- All results are extracted and converted before any caller variable changes; a
  failed call copies nothing back. Earlier side effects of the callee are not
  rolled back. One variable passed to several slots is copied back in declaration
  order.
- Container results are owned script values.
- Each call checks context ownership, membership, and that the declared and actual
  method signatures still match (guards against reload/signature changes).

The lower-level generated aliases remain available, e.g. `ue_probe_Query(api, 5)`;
with several outputs they return an `<alias>_Result` struct with `out_<parameter>`
fields instead of mutating arguments.

### Calls into DSL implementations

Interface calls can enter authored DSL Actors and ActorComponents, including the
current object and chains between objects. Finite recursion is allowed; the shared
authored/interface/delegate call depth is 32. A callee error propagates to the
caller and can be caught with `try ... recover`; no partial outputs are published,
and later calls keep working. A context is never restarted by hot reload while
executing.

```das
require ue_text
require ue_text_byte_interface_bindings

def UpdateExample(target:UObjectHandle):void {
    let api = ue_ref(type<TextByteContract>, target)
    let title = api.EchoText(ue_loctext(Title, "Example"))
    var bytes:array<uint8> = [uint8(0)]
    var messages:array<FText>
    api.ChangeArrays(bytes, messages)
}
```

Calls run on the game thread and require a reflected implementation on the
receiver. Native C++ `Execute_*` implementations without a reflected function are
not reachable through reflection.

### Explicit typed calls

Use a typed interface reference with `ue_call(type<DasProbeInterface>, Query, target, 5)`. Names and argument types are checked during compilation; the normal reflected membership and signature checks still run at invocation. See [reflected call macros](../../Reference/LowLevelCalls.md).

### Natural source values in generated calls

By default, generated wrappers expose authored enums/structs as generated value
types (`InterfaceState_from_symbol("Active")`, structs with `f_<field>` members).
To use your natural `ue_enum`/`ue_struct` declarations instead, pass a mapping
manifest:

```text
python Scripts/GenerateBindings.py ... --module ue_contract_bindings --source-types manifest.json
```

```json
{
  "module": "ue_interface_values",
  "registry": "/Game/Types/TypesRegistry.TypesRegistry",
  "types": [
    { "path": "/Game/Types/InterfaceState.InterfaceState", "kind": "enum",
      "name": "InterfaceState", "id": "interfacevalues.State",
      "members": { "Idle": 7, "Active": 42 } },
    { "path": "/Game/Types/InterfaceSettings.InterfaceSettings", "kind": "struct",
      "name": "InterfaceSettings", "id": "interfacevalues.Settings",
      "members": { "count_2_<native-guid>": "Count", "title_4_<native-guid>": "Title" } }
  ]
}
```

Use the exact native field paths from the exported IR. Every nested enum/struct
needs its own entry. Then:

```das
let state = api.EchoState(InterfaceState.Active)
var settings = InterfaceSettings()
settings.Details.Amount = 21
push_clone(settings.Items, InterfaceDetails())
var copied <- api.EchoSettings(settings)   // independent copy
```

Enums map by member identity (not ordinal); structs map field by field, including
nested structs, arrays, names, object references and maps/sets. Declaration IDs,
registry binding, GUIDs and layouts are checked at compile time. Arrays of arrays
and hash containers containing other containers are rejected. Generic call frames
are bounded (16 levels, 65,536 elements per array, 1,048,576 traversal nodes).

## Values in interface signatures

Authored interface methods accept the same value types as `ue_class` methods, as
inputs, results, `ref` and `out`:

| Type | Notes |
| --- | --- |
| `int`, `float`, `double`, `bool`, `string`, `uint8`, `FText` | `uint8` is a raw byte, not an enum |
| Supported arrays, maps, sets | See [Containers.md](Containers.md); includes arrays of authored structs |
| `ActorRef`, `ObjectRef`, other native wrappers | Class inferred from the wrapper |
| `UObjectHandle` | Defaults to `UObject`; constrain with `arg_<name>_class`/`return_class` |
| `ActorClass` and other `UEClass<T>` | See [ObjectAndClassReferences.md](ObjectAndClassReferences.md) |
| `UEVector`, `UERotator`, `UETransform` | Exact core struct identities |
| Authored `ue_enum`/`ue_struct` | Via `type_registry`/`type_folder`; no annotations |
| `UEEnumValue`, `UEStructValue` | Need `arg_<name>_enum`/`_struct` and `return_enum`/`_struct` |
| Interface wrappers, e.g. `DasEventInterface` | `FInterfaceProperty` pins |

```das
[ue_interface(module_id="contracts", BlueprintType, type_registry="/Game/Types/Registry.Registry")]
class ValueContract {
    [ue_function(BlueprintCallable)]
    def EchoActor(value:ActorRef):ActorRef {}
    [ue_function(BlueprintCallable)]
    def EchoState(value:InterfaceState):InterfaceState {}
    [ue_function(BlueprintCallable, arg_value_enum=EDasCallMode, return_enum=EDasCallMode)]
    def EchoMode(value:UEEnumValue):UEEnumValue {}
    [ue_function(BlueprintCallable)]
    def RefEvents(var value:DasEventInterface&; @ue_out var previous:DasEventInterface&):void {}
    [ue_function(BlueprintCallable)]
    def RefTransform(var value:UETransform&; @ue_out var previous:UETransform&):void {}
}
```

Output-only parameters start at the type's default: null reference, first valid enum
member, the struct's default, zero vector/rotator, identity transform. Outputs are
validated (membership, enum identity, struct layout, finite math, normalized
transforms) and published only if the implementation returns normally. Matching
redundant constraints are accepted; conflicting or unused ones are rejected. Create
the value assets and registry before the interface (the registry must be hard
referenced, e.g. by the implementing Actor, to be cooked).

### Interface references in fields

Interface wrappers can be `ue_class` and `ue_struct` fields and method parameters:

```das
[ue_class(module_id="example", Blueprintable)]
class EventHolder {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : DasEventInterface
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Targets : array<DasEventInterface>
    [ue_function(BlueprintCallable)]
    def Store(value:DasEventInterface):void { self.Target = value }
}
```

Fields default to null and are real `FInterfaceProperty`/`TScriptInterface`
values holding a strong reference under UE GC; the script wrapper itself is weak.
Destroyed actors read as null. Membership is checked with `ImplementsInterface`, so
Blueprint-only implementations (whose native interface pointer is null) work. Live
non-implementers are rejected before a field is replaced or a method is invoked.
`ue_event` handlers can also receive a `UObject*` payload as an interface view
(see [DelegatesAndEvents.md](DelegatesAndEvents.md)).

## Limits

- Interface membership and structural contract changes on existing assets
  (adding, removing or retyping methods, switching purity) are rejected.
- Interface inheritance is not supported.
- Method names must be unique across all interfaces of one class.
- Static, latent, RPC/network methods and delegate-valued parameters are not
  supported in interface methods.
- Arrays and hash containers of authored interface wrappers, and interface hash
  keys/sets, are not supported (arrays of native interface wrappers are).
- No automatic asset discovery: generated wrappers come from an explicit selection,
  and interface assets must exist before implementations are applied.
- Native `Execute_*` fallback for receivers without a reflected implementation is
  not available.

## Examples

`Plugins/DasForUE/Content/Das/Scripts/`: `dsl_interfaces.das` (native interface and
low-level calls), `dsl_interface_definition.das`, `dsl_interface_implementation.das`,
`dsl_interface_registry_definition.das`, `dsl_interface_registry_implementation.das`,
`ue_interface_values.das`, `generated_interface_calls.das`,
`ue_interface_reentry_contract.das`, `ue_interface_reentry_calls.das`,
`dsl_interface_reentry_actor.das`, `dsl_interface_reentry_component.das`,
`dsl_text_byte_interface.das`, `ue_text_byte_interface_bindings.das` (a generated
authored-interface binding module).

Try it: `Scripts/TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked`.

See also: [Classes.md](Classes.md) · [Functions.md](Functions.md) ·
[ObjectAndClassReferences.md](ObjectAndClassReferences.md) ·
[DelegatesAndEvents.md](DelegatesAndEvents.md) ·
[GeneratedBindings.md](../../Reference/GeneratedBindings.md) ·
[LowLevelCalls.md](../../Reference/LowLevelCalls.md) ·
[ModulesAndTypeIdentity.md](ModulesAndTypeIdentity.md)
