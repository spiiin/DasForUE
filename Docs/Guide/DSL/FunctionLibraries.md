# Function libraries

A `[ue_library]` class turns static daScript functions into a real Blueprint
Function Library: Blueprint nodes without a target, also callable from other
daScript modules. Separately, native C++ and engine Blueprint Function Libraries
can be called from daScript through generated bindings.

## Authoring a library

Declare one public library per source file and import it with the usual
`Das.DSL.Apply` / `Das.DSL.Reimport` commands.

```das
options gen2
module gameplay_math public
require ue_dsl

[ue_library(Category="Gameplay")]
class GameplayMath {
    // Adds two scores. This comment becomes the Blueprint tooltip.
    [ue_function(BlueprintPure)]
    def static Add(left:int; right:int):int {
        return left+right
    }

    [ue_function(BlueprintCallable)]
    def static Stats(var counter:int&; @ue_out var doubled:int&):void {
        counter+=3
        doubled=counter*2
    }
}
```

Call it from another module with daScript's static-method syntax:

```das
require gameplay_math
let sum = GameplayMath`Add(7,5)
var counter = 10
var doubled = 0
GameplayMath`Stats(counter,doubled)
```

- `BlueprintPure` gives a data node; `BlueprintCallable` gives execution pins.
- Class/function metadata and comment tooltips follow the usual rules. IDs are
  automatic; `id`/`module_id` remain optional.
- Libraries cannot have properties, components, static fields, inheritance,
  instance methods, lifecycle hooks, events or const methods.

Examples: `Plugins/DasForUE/Content/Das/Scripts/dsl_math_library.das`,
`dsl_library_values.das`, `dsl_library_values_calls.das`,
`ue_library_values_types.das`, `dsl_function_libraries.das`.

## Signatures

Library functions use the same frame ABI as authored methods:

- primitives, `UENameValue`, `UEVector`/`UEVector2D`/`UERotator`/`UETransform`;
- typed object and interface references, typed classes;
- authored enums and structs (declared source types; untyped
  `UEStructValue`/`UEEnumValue` are rejected);
- supported array/map/set shapes, with the usual element/key/value restrictions;
- delegate values (delegate out/ref is not supported);
- mutable references and `@ue_out`, with the same copyback rules as methods
  ([Functions](Functions.md#out-and-ref-parameters));
- at most 16 parameters.

Structs and containers cross the native boundary as validated snapshots; objects
remain checked weak references. Runtime errors never publish partial out/ref values.

Authored enum/struct types need an explicit `type_registry` or `type_folder` on
`ue_library`. The library CDO keeps the registry and referenced type assets as cook
dependencies, and reimport refuses a missing registry before changing the library.

```das
require ue_object_types
require ue_library_generated

[ue_library(Category="Gameplay")]
class GameQueries {
    // In Blueprint, context is hidden and supplied from the calling object.
    [ue_function(BlueprintPure, WorldContext="context")]
    def static Time(context:ObjectRef):double {
        return ue_static(GameplayStatics.GetTimeSeconds)
    }

    [ue_function(BlueprintCallable)]
    def static Append(var values:array<int>&; @ue_out var position:UEVector&):int {
        push(values,7)
        position=ue_vector(1.0lf,2.0lf,3.0lf)
        return length(values)
    }
}
```

## World context

- **Worldless functions** (no `WorldContext`) keep Unreal's hidden optional
  `__WorldContext` parameter with `CallableWithoutWorldContext`; the bridge ignores it.
- **Context-bound functions** name an input UObject reference with
  `WorldContext="context"`. Blueprint hides the pin and supplies the calling
  object; direct daScript calls pass it explicitly:

```das
let time = GameQueries`Time(context)
```

- Missing, null, destroyed, template and non-game-world contexts are rejected.
  The supplied object's world is used even when the caller lives in another world;
  nested scopes restore the caller's context on success and failure.
- Native library calls with automatic WorldContext, object queries and explicit
  Actor/component operations inside the function use the selected world. There is
  no implicit `self`: timers, subscriptions, input and lifecycle operations that
  need a script instance are not available.

## State and execution

Keep library functions stateless; do not use a library as a gameplay singleton.

| Caller | Executes in |
| --- | --- |
| daScript (``Lib`Fn(...)``) | the caller's VM |
| Blueprint, no WorldContext | a persistent context per source file on the bridge CDO |
| Blueprint, with WorldContext | a persistent context per context object and source file, revoked with that object/world |

## Parameter defaults

```das
[ue_library]
class Math {
    [ue_function(BlueprintPure)]
    def static Add(left:int=7; right:int=-2):int {
        return left+right
    }
}

let full = Math`Add(10,3)  // 13
let tail = Math`Add(10)    // 8
let both = Math`Add()      // 5
```

Trailing scalar input literals only (`int`, `float`, `double`, `bool`, `string`).
WorldContext and out/ref parameters, function calls, computed expressions,
objects/classes, enums/structs, containers, delegates and `UENameValue` defaults
are rejected. daScript fills omitted arguments; Unreal fills disconnected pins
from metadata; direct C++ `ProcessEvent` callers must supply everything.

Changing a default is a metadata update: reimport refreshes function metadata and
new nodes, and keeps pin values users entered. Removing a default clears it.
See [Defaults](Defaults.md#parameter-defaults).

## Updating a library

- Body-only changes use the normal runtime reload; an invalid candidate keeps the
  last working revision.
- Metadata and default changes are applied by reimport.
- Signature changes are rejected against the saved contract. Structural updates
  of an existing library asset are not supported yet: create a new asset.

## Calling native function libraries

Static `BlueprintCallable` functions of loaded `UBlueprintFunctionLibrary`
subclasses (engine libraries and your own C++ libraries) can be exported into a
generated module and called through compile-time macros:

```das
require ue_library_generated
let magnitude = ue_static(KismetMathLibrary.Abs, -3.0lf)
let seconds = ue_static(GameplayStatics.GetTimeSeconds)
let total = ue_static(DasFunctionLibraryFixture.Add, 20,22)
```

- Aliases come from `Bindings/FunctionLibraries.json`; use `class_rules` or explicit
  aliases to export other supported functions. Unsupported parameter types get
  explicit export diagnostics; latent and CustomThunk functions are rejected.
- A `WorldContext` parameter is hidden and filled with the current script owner,
  which must have a compatible type and a game world. Scripts cannot override it.
  The owner/world is checked again right before the call. There is no fallback to
  a global editor/PIE world, the first player or the library CDO. Worldless pure
  calls need no world.
- Class parameters use `UEClassHandle`.

Generation is described in [Generated bindings](../../Reference/GeneratedBindings.md).
Try it: `Scripts/TestFunctionLibraries.ps1`, `Scripts/TestStaticLibrary.ps1`.

See also: [Functions](Functions.md) · [Defaults](Defaults.md) ·
[Classes](Classes.md) · [Generated bindings](../../Reference/GeneratedBindings.md) ·
[Gameplay services](../Gameplay/GameplayServices.md)
