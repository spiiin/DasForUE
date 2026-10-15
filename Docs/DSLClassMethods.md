# Class references in authored functions

`UEClassHandle` is supported in callable `ue_function` inputs, results and
`out`/`ref` parameters on authored Actors and ActorComponents. Blueprint sees an
`FClassProperty` with a native `MetaClass` constraint, equivalent to
`TSubclassOf<AActor>` for the example below.

```das
require ue_object_types

[ue_function(BlueprintCallable)]
def EchoClass(value:ActorClass):ActorClass {
    return value
}
```

`ActorClass` is a nominal class value for Actor/subclasses; `ActorRef` is an object
instance reference. `ObjectClass` represents any UObject class. The aliases in the
public `ue_object_types` module are defined with native type macros:

```das
typedef ActorClass = $UEClass<Actor>
typedef ObjectClass = $ue_foundation::UEClass<Object>
```

Custom aliases use generated native object wrapper types (or their UEObject aliases).
Declare them in a separate imported module so the macro resolves before class method
signatures are parsed, as with `$UEObject`. Unresolved type macros directly inside
non-static class methods remain a daScript parser limitation. No reader macro or
parser patch is used. Multiple aliases of one base in a module reuse one nominal
structure. Import shared aliases when exchanging values between script modules.

Typed signatures infer `arg_<name>_class`/`return_class`; no hints are required.
Explicit contradictory hints fail compilation. Different class bases and raw
`UEClassHandle` are not implicitly interchangeable. To create and convert values:

```das
let klass = ue_class_loaded(type<ActorClass>, "/Script/Engine.Actor")
var empty = default<ActorClass>
let raw = ue_class_handle(klass)
let typed = ue_class_cast(type<ActorClass>, raw)
```

Conversion checks the native inheritance constraint and accepts null. The checked
representation is rebuilt when crossing a VM/reflected method boundary. Even a
manually forged wrapper handle is checked before it is passed onward. Typed classes
also work with Spawn, component creation and class properties; typed properties
infer their base without `@ue_class_base`.

The low-level handle spelling with explicit constraints remains supported:

```das
[ue_function(BlueprintCallable,
    arg_value_class="/Script/Engine.Actor",
    return_class="/Script/Engine.Actor")]
def EchoRawClass(value:UEClassHandle):UEClassHandle { return value }
```

For raw `UEClassHandle` without a constraint the base is `/Script/CoreUObject.Object`, equivalent to an
unconstrained `UClass*`. `return_class` describes the result; `arg_<name>_class`
describes a parameter. A constrained base must be a loaded native, non-interface
class. Abstract class **values** are accepted; construction restrictions remain
the responsibility of Spawn/component creation.

`ue_class_null()` is accepted for input/result/output. Classes are checked against
the base before VM entry and when returned or published. Expired/context-foreign
handles, deprecated/superseded classes and incompatible bases fail. `ref/out`
outputs are staged; failed validation publishes no updates to the caller.

The Blueprint transport contains a GC-visible UClass value, never a VM token.
Receiving VMs create their own context-local weak class capabilities. Typed authored
method accessors use the existing reflected class marshaller. Blueprint result and
output pins use Unreal class casts to narrow the generic internal transport.

IR uses `kind: "class"` and `type_path` for the native base. Method kind 15 is
reserved for Class; previous numeric kinds remain unchanged. Saved graphs and
runtime owner checks compare the actual FClassProperty.MetaClass to the contract.
Changing a method's base/signature requires normal Blueprint reimport; body reload
does not silently change the contract.

Examples: `dsl_spawn_target.das`, `dsl_dynamic_lifecycle.das`, and the
`ClassMethods` method in `dsl_spawn_driver.das`. Reproduction:
`Scripts/TestActorSpawnPIE.ps1` and `Scripts/TestCookedCalls.ps1` (includes saved
Blueprint reopening in another process and Win64 Development NullRHI).

Class containers in authored signatures, class-valued delegate payloads and
authored-interface class signatures require separate adapters.

Validation 2026-10-15: SpawnPIE PASS (one success, zero failures/warnings),
commandlet Create + fresh-process Reopen PASS for Actor/ActorComponent class graphs,
editor foundation PASS failures=0, packaged foundation PASS failures=0, and
TestCookedCalls.ps1 complete pipeline PASS in Win64 Development NullRHI. The packaged
probe covers native ProcessEvent and cross-context calls including ref/out and
rejected input/result/output bases. Shipping/rendering were not tested in this slice.


Typed-class validation 2026-10-15: ActorClass/ObjectClass signatures and properties
without hints, qualified UEClass syntax, ActorRef macro input and repeated aliases
passed. Source negatives reject ObjectClass/raw-handle assignment to ActorClass,
raw class returns from typed methods, contradictory hints and invalid macro inputs.
SpawnPIE: one success, zero failures/warnings. Editor foundation: zero failures.
Fresh-process Blueprint reopen and complete Win64 Development NullRHI packaged
pipeline passed, including class property access, ref/out, cross-context calls and
forged typed-handle rejection. Packaged foundation: zero failures.
