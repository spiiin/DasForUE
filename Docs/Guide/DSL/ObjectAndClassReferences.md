# Object and Class References

DSL classes can hold, pass and return references to Unreal objects (`ActorRef`,
`ObjectRef`, ...) and to Unreal classes (`ActorClass`, `ObjectClass`, ...). These
are typed, checked, weak script values; on the Unreal side they become ordinary
`FObjectProperty`/`FClassProperty` fields and object/class pins.

References to *authored* DSL classes (typed views with field access and method
calls) are covered in [AuthoredReferences.md](AuthoredReferences.md); interface
references are covered in [Interfaces.md](Interfaces.md).

## Reference kinds

| Type | Meaning | Unreal side |
| --- | --- | --- |
| `ActorRef`, `ObjectRef` (any `$UEObject<T>` alias) | Instance of a native class `T` or a subclass (including Blueprint subclasses) | `FObjectProperty` / object pin with `T` as class |
| `ActorClass`, `ObjectClass` (any `$UEClass<T>` alias) | A class deriving from native `T` | `FClassProperty` with `MetaClass = T` (like `TSubclassOf<T>`) |
| Native interface wrapper, e.g. `DasEventInterface` | Object implementing a native interface | `FInterfaceProperty` — see [Interfaces.md](Interfaces.md) |
| `UObjectHandle` | Low-level untyped object handle; constraint via annotations | `FObjectProperty`, default base `UObject` |
| `UEClassHandle` | Low-level untyped class handle; constraint via annotations | `FClassProperty`, default base `UObject` |

`ActorRef` and `ObjectRef` are distinct nominal types: there is no implicit upcast
or downcast between them, even for null values. The same holds for `ActorClass`
vs `ObjectClass` and for raw handles vs typed values. Instance references and
class references are never interchangeable.

## Declaring type aliases

The bundled `ue_object_types` module already declares the common aliases:

```das
options gen2
module ue_object_types public
require ue_foundation public
require ue_generated public

typedef ActorRef = $UEObject<C_Actor_adf65d5b>
typedef ObjectRef = $ue_foundation::UEObject<Object>
typedef ActorClass = $UEClass<C_Actor_adf65d5b>
typedef ObjectClass = $ue_foundation::UEClass<Object>
```

`$UEObject<T>` / `$UEClass<T>` are type macros that resolve to an existing
generated native wrapper type; `T` is a short native name exported by the binding
generator (ambiguous names are not exported; hash-qualified `C_*` names remain
available). Names of native bind types (`Actor`, `Pawn`, `ActorComponent`,
`SceneComponent`, `World`, `GameInstance` from `ue_engine`) are not exported
either: there `T` is the full `C_*` name, as for `ActorRef` above. The native class must be part of the selected reflection IR. The
qualified forms `$ue_foundation::UEObject<...>` work too.

**Declare your own aliases in a separate module and `require` it.** The daScript
parser treats an unresolved type macro inside a class method signature as a
generic parameter and rejects it; importing a compiled alias module resolves the
macro first. Defining the alias directly above the class in the same file does not
help. Import the same alias module everywhere values are exchanged; several
aliases of one base inside a module share one nominal type.

`ue_object_types` also provides helpers: `ue_is_valid(ref)`,
`ue_same_object(a, b)` (true only for two live references to the same object),
`ue_class_valid(cls)` and `ue_class_path(cls)`.

## Object properties

```das
options gen2
options gc
require ue_dsl
require ue_object_types

[ue_class(Blueprintable)]
class TargetHolder {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : ActorRef
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    Observed : ActorRef

    [ue_function(BlueprintCallable)]
    def SetTarget(target:ActorRef):void { self.Target = target }

    [ue_function(BlueprintCallable)]
    def GetTarget():ActorRef { return self.Target }

    [ue_function(BlueprintCallable)]
    def Clear():void { self.Target = default<ActorRef> }
}
```

- The class constraint comes from the wrapper type; no annotation is needed.
- Omit initializers: the CDO default is null. Use `default<ActorRef>` for an empty
  value in code.
- The field is a **strong** Unreal reference: the object stays alive while the
  owner is reachable. The script value is only a weak, context-local handle.
- Reading the field creates a fresh typed handle. Assigning null clears it. A wrong
  class, an expired or foreign handle, or a `BlueprintReadOnly` field refuses the
  write (read-only assignments are already rejected at compile time).
- A destroyed Actor reads as null and is never resurrected. `Destroy` still
  destroys an Actor even if another object's property refers to it.

The legacy untyped form is still supported. The initializer names a loaded
`/Script/` class and produces null:

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Target : UObjectHandle = ue_object_value("/Script/Engine.Actor")
```

Object fields participate in stable-ID rename, structural rollback and
retirement/restoration (see [Migrations.md](../Workflow/Migrations.md)); referenced
objects are pinned and remapped during the replacement.

## Object parameters and results

Typed wrappers work as `ue_function` inputs, results and `ref`/`out` parameters:

```das
[ue_function(BlueprintCallable)]
def EchoActor(target:ActorRef):ActorRef { return target }

[ue_function(BlueprintCallable)]
def RefActor(var value:ActorRef&; @ue_out var previous:ActorRef&):void {
    previous = value
    value = default<ActorRef>
}
```

With raw `UObjectHandle`, constrain classes explicitly with
`arg_<name>_class` and `return_class` (a loaded native class path):

```das
[ue_function(BlueprintCallable,
             arg_actor_class="/Script/Engine.Actor",
             return_class="/Script/Engine.Actor")]
def ActorIdentity(actor:UObjectHandle):UObjectHandle { return actor }
```

A matching explicit constraint on a typed wrapper is accepted; a conflicting one,
an unused class annotation, a class annotation on a non-object parameter, or a
non-native/unloaded path is a compile error.

Runtime behavior:

- Null inputs and results are valid; an output-only object starts as null.
- Every non-null input is pinned (strong) for the duration of the call, so the body
  may trigger GC safely. After return the pin is released.
- The actual class is checked: a wrong input is rejected before the body runs, a
  wrong or expired result is not passed to the caller. Destroyed Actors count as
  invalid even before GC collects them.
- `ref`/`out` values are staged; a failed call publishes no outputs.
- A Blueprint that stores a returned object in its own property owns that
  reference under normal UE GC rules.

When a Blueprint class is reinstanced in the editor, weak handles held by other
VMs are redirected to the replacement object; an incompatible replacement
invalidates the reference.

## Object references through other objects and in arrays

Native wrapper fields keep their types when read through a typed view of another
authored object (`api` below is an authored reference, see
[AuthoredReferences.md](AuthoredReferences.md)):

```das
let actor = api.NativeActor                 // ActorRef
api.NativeActor = anotherActor
api.NativeEvents = ue_ref(type<DasEventInterface>, api.handle)
api.NativeActor = default<ActorRef>
```

Getters and setters re-check the reflected field's declared class/interface, also
for null assignments. Read-only fields expose no setter.

Arrays of native object or native interface wrappers are supported in `ue_class`
and `ue_struct` properties and in method input/result/ref/out signatures:

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Actors : array<ActorRef>
@ue_property=(EditAnywhere, BlueprintReadWrite)
Events : array<DasEventInterface>

push(self.Actors, actor)
push(self.Actors, default<ActorRef>)
self.Actors[0] = actor
var copy = api.CopyActors(api.Actors)
api.Actors := copy
clear(api.Events)
```

Element types are nominal (`array<ActorRef>` is not `array<ObjectRef>`). The UE
property holds strong references; getters return owned arrays of weak handles,
not a live view. Null elements round-trip, destroyed Actors read as null, and wrong
classes/interfaces are rejected per element. Definitions default to empty. Object
sets and object-key maps are described in [Containers.md](Containers.md).

## Class references

### Creating and converting class values

```das
require ue_object_types

let klass = ue_class_loaded(type<ActorClass>, "/Script/Engine.Actor")
var empty = default<ActorClass>
let raw = ue_class_handle(klass)                 // UEClassHandle
let typed = ue_class_cast(type<ActorClass>, raw) // checked, accepts null
```

`ue_class_loaded` resolves an **already loaded** class; it does not load assets.
The type argument constrains resolution: native types check inheritance, authored
references also check the authored type ID. Missing, deprecated and superseded
classes are rejected. Abstract classes are valid *values*; Spawn and component
creation still refuse to instantiate them. Use `ue_class_valid` and
`ue_class_path` to inspect a value.

Class handles are weak, scoped to the owner session and invalid after it is
released. They are runtime values, not serialized references: use a class
property when the choice must be saved.

```das
let klass = ue_class_loaded(type<DynamicLifeRef>, classPath)
let component = ue_component_create(type<DynamicLifeRef>, actor, "DynamicLife", klass)
```

Spawn and component creation accept class values and permit compatible Blueprint
subclasses; see [Components.md](Components.md) and
[SpawnAndQueries.md](../Gameplay/SpawnAndQueries.md).

### Class properties

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
SpawnClass : ActorClass

// Low-level form with an explicit base:
@ue_property=(EditAnywhere, BlueprintReadWrite)
@ue_class_base="/Script/DasRuntime.DasScriptComponent"
ComponentClass : UEClassHandle
```

The field is a real `FClassProperty`; the Details panel shows the standard class
picker filtered by the base. Typed properties infer the base; `@ue_class_base` is
needed only for raw handles. Initializers are omitted (null). Values selected on
the Blueprint CDO or an instance survive unchanged reimport and are a hard
dependency for save and cook. Runtime assignment accepts null and compatible
classes and obeys `BlueprintReadOnly`. The picker filters by native inheritance;
exact authored identity is checked when the class is used for creation.

### Class parameters and results

```das
[ue_function(BlueprintCallable)]
def EchoClass(value:ActorClass):ActorClass { return value }

[ue_function(BlueprintCallable)]
def ChangeClass(var value:ActorClass&; @ue_out var previous:ActorClass&):ActorClass {
    previous = value
    value = default<ActorClass>
    return previous
}

// Low-level spelling with explicit constraints:
[ue_function(BlueprintCallable,
    arg_value_class="/Script/Engine.Actor",
    return_class="/Script/Engine.Actor")]
def EchoRawClass(value:UEClassHandle):UEClassHandle { return value }
```

Blueprint sees a class pin with the native `MetaClass` (`TSubclassOf<AActor>` here).
Typed signatures infer `arg_<name>_class`/`return_class`; contradictory hints fail
compilation. A constrained base must be a loaded native, non-interface class.
`ue_class_null()` / `default<...>` is accepted everywhere. Classes are checked
against the base before entry and when returned or published; expired or foreign
handles, superseded classes and wrong bases fail, and failed `ref`/`out` validation
publishes nothing. Changing a method's class base is a signature change that
requires a normal Blueprint reimport.

Generated native calls also accept class arguments; for example
`GetActorOfClass` from GameplayStatics:

```das
require ue_library_generated
require ue_object_types
let klass = ue_class_loaded(type<ActorRef>, "/Script/Engine.Actor")
let actor = ue_static(GameplayStatics.GetActorOfClass, klass)
```

See [FunctionLibraries.md](FunctionLibraries.md) and
[reflected call macros](../../Reference/LowLevelCalls.md).

### Classes in delegates, interfaces, events and timers

The same `ActorClass` type works in `ue_delegate`/`ue_multicast` signatures,
authored interface methods (including ref/out), `ue_event` handlers and timer
captures, without per-method annotations:

```das
let kind = ue_class_cast(type<ActorClass>,
    ue_class_loaded(type<ActorRef>, "/Script/Engine.StaticMeshActor"))
ue_timer_next_tick("spawn", "SpawnLater", kind)
ue_timer_start("repeat", 0.1f, "SpawnLater", true, kind)

[ue_function(BlueprintCallable)]
def SpawnLater(kind:ActorClass):void { /* use kind */ }
```

Single delegates support class input/result/ref/out; multicast delegates follow
their usual void/ref rules; queued `ue_event` handlers receive classes by value.
A native event may widen its class to an ancestor in the script handler, but
narrowing is rejected before subscription. Timers and queued events keep captured
classes alive until delivery or cancellation; a capture of a superseded class is
not delivered. See [DelegatesAndEvents.md](DelegatesAndEvents.md) and
[Timers.md](../Gameplay/Timers.md).

## Limits

- Only loaded native `/Script` classes exported in the reflection IR can be used as
  object/class constraints. Interface types are not allowed as `$UEObject` or
  `UEClass` bases.
- Arrays, maps and sets of class references are not supported.
- Arrays of authored interface wrappers are not supported. Arrays of authored
  views are covered in [AuthoredReferences.md](AuthoredReferences.md).
- Typed wrappers add declaration/boundary typing only; calling native members still
  goes through generated bindings.
- Class default migrations are not supported.
- Soft references: see [SoftReferences.md](SoftReferences.md).

## Examples

`Plugins/DasForUE/Content/Das/Scripts/`: `dsl_typed_objects.das`,
`dsl_object_fields.das`, `dsl_object_methods.das`, `dsl_spawn_target.das`,
`dsl_dynamic_lifecycle.das`, `dsl_spawn_driver.das` (method `ClassMethods`),
`dsl_class_callbacks.das`, `dsl_class_callback_interface.das`,
`ue_class_callback_signals.das`, `ue_object_types.das`.

Apply an example class to a Blueprint:

```text
Das.DSL.Apply dsl_object_methods.das /Game/DasExamples/BP_Objects objects.ObjectBridge
```

Try it: `Scripts/TestClassCallbacks.ps1`.

See also: [AuthoredReferences.md](AuthoredReferences.md) ·
[Interfaces.md](Interfaces.md) · [DelegatesAndEvents.md](DelegatesAndEvents.md) ·
[Containers.md](Containers.md) · [SoftReferences.md](SoftReferences.md) ·
[ModulesAndTypeIdentity.md](ModulesAndTypeIdentity.md) ·
[TypeSupport.md](../../Reference/TypeSupport.md)
