# Authored References

An authored reference (a *view*) is a typed handle to another instance of a
DSL-authored Actor or ActorComponent class. Through it you read and write that
object's fields and call its `BlueprintCallable` methods with ordinary member
syntax — `actor.Score += 1`, `actor.Behavior.Evaluate(5)` — without property-name
strings, entry point names or marshalling code.

References to native classes (`ActorRef`, `ActorClass`) are described in
[ObjectAndClassReferences.md](ObjectAndClassReferences.md).

## Declaring a view

Declare each view once, in a reusable public module:

```das
options gen2
module ue_actor_refs public
require ue_dsl public

[ue_object_ref(script="dsl_actor_component_owner.das", type="ComponentOwner")]
struct ComponentOwnerRef { handle : UObjectHandle }
```

- `script` is the local plugin script file and `type` the short class name; no
  Unreal paths or stable IDs are needed. With a source catalog you can write
  `[ue_object_ref(type="CounterActor")]` or pick a catalog with
  `catalog="my_catalog.json"` (see [ModulesAndTypeIdentity.md](ModulesAndTypeIdentity.md)).
  `script` and `catalog` together are rejected.
- Fields, methods and read-only rules come from the named source class. Adding a
  new authored class needs only a new view declaration.
- Views are nominal: two view structs for the same class are distinct types. A
  `typedef` can alias an existing view.
- Imports are explicit: `require` the view module wherever you use it. Views of the
  current class and cyclic Actor/Component references are supported.

## Obtaining a view

```das
require ue_actor_refs

// From any UObjectHandle (checked: liveness and authored class ID):
let actor = ue_ref(type<ComponentOwnerRef>, ue_component_owner(ue_self_object()))

// Inside ActorComponent methods and callbacks, the owning Actor:
let owner = ue_owner(type<ComponentOwnerRef>)

// Null:
var none = default<ComponentOwnerRef>
```

`ue_ref` and `ue_owner` are compile-time call macros usable in DSL methods,
callbacks and ordinary functions. `ue_owner` requires an Actor-derived view and
fails at runtime when executed on an Actor rather than a Component or when the
owner has a different authored identity. `ue_ref` also accepts native wrapper types
such as `ActorRef` or interface wrappers. Views are also obtained from fields,
parameters, results and component members (`actor.Behavior`).

A view contains only a weak, context-local handle. It does not copy fields and does
not keep its target alive. A `let` binding fixes which object the view refers to
but still allows writing the object's writable fields.

## Field access

```das
require ue_scalar_refs

// Ordinary function: no ue_class or annotation required.
def add_score(actor:ScalarTargetRef; amount:int):int {
    actor.Number += amount
    return actor.Number
}
```

| Field kind | Read | Write |
| --- | --- | --- |
| `int`, `float`, `double`, `bool`, `string`, `UENameValue` | yes | `=`, `+=`, `-=`, `*=`, `/=` |
| Authored `ue_enum` / `ue_struct` (natural source types) | owned copy | whole-value assignment |
| `UEEnumValue` / `UEStructValue` (explicit low-level fields) | owned copy | whole-value assignment |
| Native object / interface wrappers, `UObjectHandle` | checked weak handle | assignment (incl. null) |
| Authored view fields (e.g. `Partner:ValueTargetRef`) | view | assignment (incl. null) |
| Component members | typed component view | no replacement |
| Arrays / maps / sets | owned copy | `:=`, indexed writes, `push`, `clear`, ... (see [Containers.md](Containers.md)) |
| Delegate fields / Event Dispatchers | see [DelegatesAndEvents.md](DelegatesAndEvents.md) | `actor.Callback = ...`, `actor.OnEvent.Broadcast(5)` |

Struct values read through a view are independent copies. Nested in-place writes
such as `api.Settings.Point.Count = 1` are not supported through another object's
view; modify a local copy and assign it back:

```das
require ue_method_values   // declares MethodSettings / MethodState
require my_target_refs     // declares ValueTargetRef

def edit_settings(api:ValueTargetRef):void {
    var settings = api.Settings         // MethodSettings
    settings.Point.Position.x += 1.0lf
    settings.Weights["bonus"] = 2.5lf
    api.Settings = settings
    api.State = MethodState.Active
}
```

Natural enum/struct fields require importing their declaration module, and the
target class must name its `type_registry`/`type_folder` (see
[Structs.md](Structs.md), [Enums.md](Enums.md)). Wrong struct types and wrong
enums — even with identical numeric values — are compile errors.

**Checks.** Unknown fields and assignments to `BlueprintReadOnly` fields fail at
compile time. Every access checks the handle's liveness and owner context and the
target's authored class ID, then resolves the reflected field again (no property
pointers are cached across reinstancing). Field-kind mismatches, forged handles and
null or destroyed targets fail at runtime. Enum writes require the same UEnum and a
valid member; struct writes validate exact struct identity and layout; object writes
check the declared class/interface. Failed writes leave the field unchanged.

## Calling authored methods

```das
require ue_actor_refs
require ue_component_types

let actor = ue_owner(type<ComponentOwnerRef>)
let result = actor.Behavior.Evaluate(5)

var value = 10
let changed = actor.Behavior.Accumulate(value)   // var int& = ref (inout)

var total = -1
actor.Behavior.MakeScore(5, total)               // @ue_out = output only
```

The target methods are ordinary `[ue_function(BlueprintCallable)]` methods:

```das
[ue_function(BlueprintCallable)]
def Accumulate(var value:int&):int {
    value += self.Bonus
    return value
}

[ue_function(BlueprintCallable)]
def MakeScore(input:int; @ue_out var total:int&):void {
    total = self.Bonus + input
}
```

Ordinary functions use the same syntax:
`def evaluate_component(c:ScoreComponent; input:int):int { return c.Evaluate(input) }`.

- Unknown methods, wrong argument count/types and immutable arguments for ref/out
  slots are compile errors. The receiver is evaluated once.
- Supported signature types: primitives, `UENameValue`, natural
  authored enums/structs, `UEEnumValue`/`UEStructValue`, native object/interface
  wrappers, authored views, `UEVector`/`UERotator`/`UETransform`, and the supported
  arrays/maps/sets (see [Containers.md](Containers.md)). Unsupported container shapes
  are rejected.
- Dispatch goes through the reflected UFunction and `ProcessEvent`, so Blueprint
  overrides are honored. Each call re-checks liveness, authored class identity and
  the current reflected signature (names, directions, types); an incompatible
  signature is rejected before invocation. Parameters are matched by name, so a
  Blueprint may reorder pins.
- `ref`/`out` use copy-in/copy-out. `out` starts with the type's default; the old
  caller value is not sent. The return value and all outputs are converted before
  any caller variable changes; a failed call leaves caller variables untouched.
  Field writes already made by the callee are *not* rolled back. The same variable
  in several slots is copied back in declaration order.
- Math values are checked for finite components and normalized rotations.
- Calls may nest, including calls back into a running Actor/Component or the same
  context. Nesting is limited to 32 levels (shared with interface and delegate
  calls). A nested error propagates to the calling script and can be caught with
  `try ... recover`. A context is never reloaded while it is executing; source
  changes are rejected until the outer execution ends. Events stay queued until
  the outer execution finishes.

## Views in fields and signatures

Views can be stored in `ue_class` properties and used as method parameters,
results and ref/out arguments:

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Partner : ValueTargetRef
@ue_property=(EditAnywhere, BlueprintReadWrite)
Peers : array<CounterRef>

[ue_function(BlueprintCallable)]
def EchoAuthored(value:ValueTargetRef):ValueTargetRef { return value }

[ue_function(BlueprintCallable)]
def RefAuthored(var value:ValueTargetRef&; @ue_out var previous:ValueTargetRef&):void {
    previous = value
    value = default<ValueTargetRef>
}

[ue_function(BlueprintCallable)]
def CopyPeers(values:array<CounterRef>):array<CounterRef> { return <- clone(values) }
```

```das
api.Partner = target
let returned = api.EchoAuthored(api)
var current = returned
var previous : ValueTargetRef
api.RefAuthored(current, previous)
push(self.Peers, peer)
api.Peers := api.CopyPeers(api.Peers)
```

Self-referencing fields and signatures work before the Blueprint exists. Omit
initializers: fields default to null, arrays to empty.

Physically, Unreal stores these values as objects of the common host class
(`DasProbeActor` or `DasScriptComponent`); the logical authored class is recorded as
an `authored_id` in the type description. Every non-null value is checked against
that ID when packed or unpacked, including Blueprint-to-script entry, returned and
out values and each array element. Blueprint or C++ code can assign any instance of
the host class; the next typed script read rejects a wrong one. To clear malformed
array data, assign a whole empty array. Wrong view types fail at compile time.

Fields are ordinary Unreal object properties with normal ownership and GC; they do
not create objects or components. Changing an existing field's `authored_id` on
Apply is rejected.

## Identity on imported Blueprints

The hidden authored identity is stored on the generated CDO and survives save and
cook. Blueprints imported by older plugin versions receive it on a validated
body-only reimport; reimport and save them before packaging. A conflicting existing
identity is refused.

## Limits

- Authored views as `ue_struct` members (including arrays) are not supported.
- Blueprint pins and pickers use the generic host class, not the concrete
  generated Blueprint class.
- No automatic import or reference discovery: `require` view and value modules
  explicitly.
- No in-place nested struct writes through another object's view.
- Component member replacement through a view is not supported.
- Non-empty defaults and structural migrations of view fields are not supported.

## Examples

`Plugins/DasForUE/Content/Das/Scripts/`: `dsl_actor_component_owner.das`,
`dsl_component_behavior.das`, `ue_actor_refs.das`, `ue_component_types.das`,
`ue_scalar_refs.das`, `dsl_ref_scalar_target.das`, `dsl_ref_scalar_driver.das`,
`dsl_ref_same_class.das`, `ue_reference_functions.das` (ordinary functions),
`ue_method_values.das`, and the templates `dsl_method_value_target.das`,
`dsl_method_value_driver.das`, `ue_method_target_refs.das` (their placeholders such
as `METHOD_REFS_MODULE` are replaced by the test runner).

Try it: `Scripts/TestMethodValues.ps1 -Cooked`.

See also: [ObjectAndClassReferences.md](ObjectAndClassReferences.md) ·
[SelfAccess.md](SelfAccess.md) · [Functions.md](Functions.md) ·
[Components.md](Components.md) · [Containers.md](Containers.md) ·
[Interfaces.md](Interfaces.md) · [ModulesAndTypeIdentity.md](ModulesAndTypeIdentity.md)
