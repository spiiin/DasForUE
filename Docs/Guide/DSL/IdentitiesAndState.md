# Typed identities, localized text and persistent state

## Classes and object references

```das
require ue_object_types
let defaults = ue_class_default(type<ActorRef>)
let actor = ue_ref(type<ActorRef>, ue_self_object())
let object = ue_ref(type<ObjectRef>, actor.handle)
```

In ordinary code, `ue_class_default(type<T>)` returns a checked weak reference to T's class default object (CDO). T is a native or authored **object reference type**, not an interface or a class-handle value. The native class must be loaded. An authored reference is a contract that can have several Blueprint implementations, so pass its concrete class handle: `ue_class_default(type<MyActorRef>, loadedClass)`. Missing/null/incompatible handles are rejected; the bridge never substitutes the native host CDO. Access requires an initialized script owner or library call; a persistent VM's global initializer cannot acquire a CDO and modify shared defaults before its candidate is accepted.

`ue_ref(type<T>, handle)` uses the same checked conversion for native objects, interface views and authored references. Null stays null, class/interface membership is checked, and the result keeps the usual weak lifetime. No class path or per-type `_from` helper is needed, even for a newly declared native wrapper with just its annotated handle field. Handles remain local to their VM; preserve gameplay data, not handles, across reload.

In a declaration default, the same spelling selects the UClass assigned to a class property:

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
SpawnType:ActorClass = ue_class_default(type<ActorRef>)
```

Here the annotation importer consumes the expression as data and applies the class reference to the generated CDO. This works in inline initializers and `ue_defaults` blocks; an ordinary method call returns the CDO object instead. See [Defaults](Defaults.md).

## Localized text

```das
module gameplay public
require ue_text
let caption = ue_loctext(PickupReady, "Ready to collect")
```

The key is an identifier; the source is a string literal. The namespace is the defining daScript module (`gameplay` above), falling back to the source file's base name when there is no named module. Translation identity is namespace + key, so choose a named module when filenames may change. The source text is not part of the identity.

The macro is available in normal expressions, reflected text-property defaults and text-parameter defaults. It expands to the existing FText transport and retains localization history. A numeric/dynamic key or source expression is rejected at compile time. Culture-invariant text still uses `ue_text_invariant`.

## Persistent scalar globals

```das
module gameplay public
require ue_foundation
var @ue_state Counter:int = 0
var @ue_state Message:string = "Initial"

[export]
def main(amount:int):int {
    Counter += amount
    return Counter
}
```

The supported daScript grammar is `var @ue_state Name:Type`, with the annotation after `var`. The planned `[ue_state] var` is not a global-variable form in this grammar.

Supported values are `int`, `bool`, `float`, `double` and `string`. The initializer must be a compile-time scalar constant, or may be omitted for the zero/empty value. Shared/constant/local declarations, containers, objects, delegates and VM references are rejected. Use ordinary UPROPERTY data when state belongs in Unreal's object model.

Each owner and script session has separate state. On accepted reload, the bridge copies the previous VM's marked globals into owned native scalar values and UTF-8 text, then uses them while initializing the candidate VM. Dependent ordinary global initializers therefore see the restored value. No old VM address or string-heap pointer is retained.

Changing an initializer keeps the existing value. Changing a marked variable's type rejects the candidate and keeps the working VM. Rename the variable to start a new state identity, or release the script session to reset it. Keys are module + variable name; named modules avoid filename-based identity. Invalid source, failed initialization and failed binding preserve the existing state. Destroy, EndPlay, world cleanup and explicit release keep the existing session cleanup rules.

Ordinary globals still reset on reload. This mechanism does not serialize a save game, replicate state or persist across an editor/game restart.

## Path-literal lint

A user source literal such as `"/Script/Engine.Actor"` produces `DAS_DSL_PATH_LITERAL` when the compile's loaded modules already provide a typed wrapper. This warning suggests the wrapper and typed identity helpers; compilation continues. Macro-generated transport paths are not reported as user literals.

Binding modules (`ue_*`), fixture/probe scripts and sources under Tests are excluded. Asset paths without a corresponding generated type remain ordinary asset identifiers.
