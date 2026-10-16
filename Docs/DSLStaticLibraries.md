# Static Blueprint function libraries

Libraries provide worldless functions and functions with an explicit WorldContext. Declare one public library
per source file. Import it with the existing `Das.DSL.Apply` / `Das.DSL.Reimport`
workflow; the resulting asset is a real Blueprint Function Library.

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

Use native daScript static-method syntax from another DAS module:

```das
require gameplay_math
let sum = GameplayMath`Add(7,5)
var counter = 10
var doubled = 0
GameplayMath`Stats(counter,doubled)
```

Blueprint calls have ordinary typed pins and no target. Unreal retains its
hidden internal __WorldContext parameter for Blueprint library graphs;
CallableWithoutWorldContext makes it optional. The bridge does not consume it.
`BlueprintPure` produces a data node; callable functions have execution pins.
Supported signatures use the existing method ABI: primitive values, UENameValue,
UEVector/UEVector2D/UERotator/UETransform, typed object/interface references,
typed classes, authored enum/struct and supported array/map/set shapes. Structs
and containers cross the native boundary as validated snapshots. Objects remain
checked weak references. The same container element/key/value restrictions apply
as for ordinary authored methods. Delegate values use the existing delegate ABI;
delegate out/ref remains unsupported. Maximum: 16 parameters. Mutable references
and @ue_out use the same copyback rules as ordinary methods.

Class/function metadata and comment tooltips use existing rules. IDs are derived
automatically; explicit id/module_id remain optional. Authored enum/struct types
use the existing explicit `type_registry` or `type_folder` on ue_library. The
generated library CDO retains the registry and referenced value assets as cook
dependencies. Reimport refuses a missing registry before altering the library.
Untyped UEStructValue/UEEnumValue signatures are intentionally rejected: use the
declared source type instead.

```das
require ue_object_types
require ue_library_generated

[ue_library(Category="Gameplay")]
class GameQueries {
    // In Blueprint, context is hidden and supplied from the calling object.
    [ue_function(BlueprintPure, WorldContext="context")]
    def static Time(context:ObjectRef):double {
        return ue_lib_Time()
    }

    [ue_function(BlueprintCallable)]
    def static Append(var values:array<int>&; @ue_out var position:UEVector&):int {
        push(values,7)
        position=ue_vector(1.0lf,2.0lf,3.0lf)
        return length(values)
    }
}
```

Direct DAS calls supply context explicitly:

```das
let time = GameQueries`Time(context)
```
WorldContext must name an input UObject reference, including a compatible typed
object wrapper. Missing, null, destroyed, template and non-game-world contexts
are rejected. The scope uses that object even when the caller belongs to another
world; nested scopes restore the caller's context on success and failure. Native
library calls with automatic WorldContext, object queries and explicit actor/
component operations resolve the selected world. No implicit self is introduced.
Timers, subscriptions, input and lifecycle operations that require a script
instance remain instance APIs.

Worldless functions keep Unreal's hidden optional __WorldContext implementation
parameter and CallableWithoutWorldContext policy. Context-bound functions use
the named context parameter for the WorldContext metadata and require it.

Libraries reject properties, components, static fields, inheritance, instance
methods, lifecycle, events and const methods. Do not use a
library as a gameplay-state singleton. Direct DAS calls execute in the caller's
VM; Blueprint calls without WorldContext use a persistent context per source
file on the native bridge CDO. Blueprint calls with WorldContext use a persistent
context per supplied object and source file, revoked with that object/world.
Keep library functions stateless.

## Parameter defaults

Static input parameters support ordinary daScript literal defaults:

```das
[ue_library]
class Math {
    [ue_function(BlueprintPure)]
    def static Add(left:int=7; right:int=-2):int {
        return left+right
    }
}

// All arguments, the last argument or neither argument may be supplied.
let full = Math`Add(10,3)  // 13
let tail = Math`Add(10)    // 8
let both = Math`Add()      // 5
```

Supported default types: int, float, double, bool and string (including Unicode
and an empty string). Numeric defaults preserve the target type's precision;
negative literals are supported. Defaults must be trailing scalar input values.
WorldContext and out/ref parameters cannot have defaults. Function calls,
computed expressions, objects/classes, enum/struct, containers, delegates and
UENameValue defaults remain unsupported and fail during declaration collection.
Ordinary Actor and ActorComponent ue_function defaults use the same literal rules;
see [Method parameter defaults](DSLParameterDefaults.md).

The optional parameter `default` field in authoring IR stores a primitive JSON
number/bool/string. Absence means required; an empty string is an explicit default.
The native ABI still receives a complete frame. daScript fills omitted source
arguments; Unreal fills disconnected Blueprint input pins from function metadata.
Direct C++ ProcessEvent callers must still construct all parameters themselves.

Default changes are metadata updates, not structural signature changes. Reimport
updates function metadata and new call nodes. Explicit pin values already entered
by the user are preserved, including after recompilation. Removing a default
clears the old metadata, so new Blueprint nodes use Unreal's usual type default.
No automatic migration of unloaded dependent Blueprint/maps is performed. Source
default changes require reimport, like other saved declaration changes; body-only
reload remains supported independently.

Blueprint graphs store the canonical declaration contract. Body-only changes use
the persistent runtime reload mechanism; an invalid candidate keeps the last
working revision. Source signature changes are rejected against the saved
contract. Updating an existing library asset structurally is deferred; create a
new asset until a library migration policy is implemented. Metadata reimport is
supported. Runtime errors do not publish partially updated out/ref values.

Examples: `Plugins/DasForUE/Content/Das/Scripts/dsl_math_library.das`,
`dsl_library_values.das`, `dsl_library_values_calls.das` and
`ue_library_values_types.das`. Tests: `Scripts/TestStaticLibrary.ps1` (Create/Reopen,
flags, signatures, refusal cases, reload and failure outputs),
`Scripts/TestGameplayRuntimePIE.ps1` and `Scripts/TestCookedGameplay.ps1`
(separate Blueprint caller and actual game runtime). These are NullRHI tests;
Shipping and rendered gameplay are outside this validation.

Validation: Create/Reopen + independent Blueprint recompile passed for scalar
and complex libraries. Covered enum/struct, typed objects/classes, math,
array/map/set, mixed complex out/ref + result, null/destroyed WorldContext,
invalid context declarations, explicit calls across two worlds, recovered errors
and a nested authored Actor ConstructionScript using its own runtime contract.
Existing body reload and last-working rejection tests passed.

Strict Unreal RuntimePIE passed with a separate Blueprint caller whose named
WorldContext is supplied automatically. Editor and packaged foundation reported
failures=0. Final Win64 Development cook/package/gameplay passed for the saved
registry, enum/struct assets, complex libraries and automatic-context Blueprint
caller. Shipping and rendered gameplay were not tested.

Logs: library-values-editor-final.log, library-values-pie-final.log,
library-values-foundation-final.log, library-values-cooked-final.log,
cooked-gameplay-runtime.log and smoke-packaged.log in logs/.

Parameter-default validation: Create/Reopen and independent Blueprint recompile
passed. Covered omitted/partial/explicit DAS arguments, negative int/float,
precise double, bool, Unicode and empty strings; six invalid default declarations;
default-only reimport retaining an explicit caller pin and its result; new nodes
receiving changed defaults; removed defaults clearing old metadata. Existing
WorldContext/complex/reload tests passed. Strict RuntimePIE and Win64 Development
cook/package/gameplay passed with an omitted-argument Blueprint caller. Editor
and packaged foundation reported failures=0. Logs: library-defaults-editor.log,
library-defaults-pie.log, library-defaults-foundation.log,
library-defaults-cooked.log, cooked-gameplay-runtime.log and smoke-packaged.log.
Shipping and rendered gameplay were not tested.
