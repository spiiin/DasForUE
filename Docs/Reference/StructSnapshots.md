# Struct Snapshots (UEStructValue)

`UEStructValue` is a generic, immutable handle to a copy of any supported reflected struct, native (`/Script/...`) or DSL-authored. It lets scripts read and build struct values by field path without a generated daScript type for each struct. Most code should prefer typed structs ([Structs](../Guide/DSL/Structs.md), generated `S_*` types); use snapshots for generic code, `UEStructValue` properties and method signatures.

## Methods with struct parameters

```das
options gen2
options gc
require ue_dsl

[ue_class(module_id="reflected",Blueprintable)]
class StructBridge {
    [ue_function(BlueprintCallable,
                 arg_value_struct="/Script/DasRuntime.DasCallPayload",
                 return_struct="/Script/DasRuntime.DasCallPayload")]
    def Edit(value:UEStructValue; amount:int):UEStructValue {
        var result = ue_struct_with_int(value,"Count",ue_struct_get_int(value,"Count")+amount)
        result = ue_struct_with_string(result,"Label",ue_struct_get_string(value,"Label")+" !")
        result = ue_struct_with_float(result,"Metrics.Ratio",ue_struct_get_float(value,"Metrics.Ratio")+0.5)
        return result
    }
}
```

```text
Das.DSL.Apply dsl_reflected_methods.das /Game/DasExamples/BP_Struct reflected.StructBridge
```

Every `UEStructValue` input needs `arg_<name>_struct="<type path>"` and a result needs `return_struct`. The Blueprint gets a real typed struct pin (not a wildcard). The signature checks the exact type path and reflected layout at call time; a result of another struct type is rejected. Renaming, reordering or retyping an existing parameter requires a [migration](../Guide/Workflow/Migrations.md).

Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_reflected_methods.das`; enum fields: `dsl_struct_enum_methods.das`.

## Struct properties

```das
@ue_property=(EditAnywhere,BlueprintReadWrite)
Payload : UEStructValue = ue_struct_default(
    "/Script/DasRuntime.DasCallPayload", "(Count=10,Label=\"Initial\")")
```

`ue_struct_value(path)` creates a value with the native constructor default; `ue_struct_default(path, text)` applies a strict subset of Unreal export text (named fields, nested structs, quoted strings, scalars, names, registered enum symbols). Unknown or duplicate fields, malformed text and trailing data are rejected before the Blueprint changes; omitted fields keep the constructor default. Initializer arguments must be string literals. See [Defaults](../Guide/DSL/Defaults.md).

`self.Payload` reads a snapshot; assign a changed snapshot back to publish it:

```das
self.Payload = ue_struct_with_int(self.Payload,"Count",42)
```

Writes check the exact struct identity and `BlueprintReadOnly`. The equivalent explicit calls are `ue_get_self_struct("Payload")` and `ue_set_self_struct("Payload", value)`.

## Helper functions

| Group | Functions |
| --- | --- |
| Create | `ue_struct_value(path)`, `ue_struct_default(path, text)` |
| Read leaf | `ue_struct_get_int/float/double/bool/string/name/byte/text/object(value, path)` |
| Write leaf (returns a new snapshot) | `ue_struct_with_int/float/double/bool/string/name/byte/text/object(value, path, new_value)` |
| Enum | `ue_struct_get_enum(value, path, enum_path):string`, `ue_struct_with_enum(value, path, enum_path, symbol)` |
| Nested / array | `ue_struct_nested_get(value, path, index)`, `ue_struct_nested_with(value, path, index, child)`, `ue_struct_array_size(value, path)`, `ue_struct_array_resize(value, path, count)` |
| Map / set | `ue_struct_map_size/key/get_*/with_*`, `ue_struct_set_size/element_*/contains_*/with_*` — see [Containers](../Guide/DSL/Containers.md) |
| Batch edit | `ue_struct_edit(value) $(var draft:UEStructValue) { ... }` |
| Lifetime | `ue_struct_collect()`, `ue_struct_value_count()` |

Paths use dots for nested structs and indices for arrays: `Metrics.Ratio`, `Rows.1.Priority`. For `nested_get`/`nested_with`, index `-1` selects a plain nested struct member and a non-negative index selects an array element. Names are returned as `UENameValue` (`ue_name_value(text)` to construct). Object leaves accept and return `UObjectHandle`; the snapshot keeps referenced objects visible to Unreal GC.

```das
var child = ue_struct_value("/Script/DasRuntime.DasObjectSnapshot")
child = ue_struct_with_object(child, "Object", ue_get_self_object("Target"))
var saved = ue_struct_value("/Script/DasRuntime.DasNestedObjectSnapshot")
saved = ue_struct_nested_with(saved, "Details", -1, child)
saved = ue_struct_array_resize(saved, "Items", 1)
saved = ue_struct_nested_with(saved, "Items", 0, child)
```

(`struct_object_snapshot.das`; requires the test fixture struct types.)

### Enum fields

- The enum type path must match the field's enum exactly.
- Short (`Active`) and qualified (`EDasCallMode::Active`) registered symbols are accepted case-insensitively; the canonical short symbol is returned.
- Numbers, display labels, redirects, unknown symbols and `_MAX` are rejected. Hidden metadata is not used, so the rule is identical in Editor and cooked builds.
- The whole input snapshot is validated for invalid enum values on receipt, so a corrupted value never reaches the method body.
- Enum-backed `FByteProperty` is supported; plain `uint8` without a `UEnum` is a byte leaf, and bitmask combinations are not.

## Value semantics

Snapshots are immutable. Each `with` helper makes a native deep copy and updates the copy; the source and its aliases never change, so assigning a token never creates a mutable alias. Identical snapshots are deduplicated. Inputs and results are copied with `UScriptStruct` operations; host results are independent of the VM after return. A token is private to its context and cannot be used from another one. No pointer into actor memory is exposed.

## Scoped batch edits

Several writes on one snapshot copy the root each time. `ue_struct_edit` makes one private copy, runs a block on it and returns a validated immutable snapshot:

```das
let original=ue_get_self_struct("Snapshot")
let changed=ue_struct_edit(original) $(var draft:UEStructValue) {
    draft=ue_struct_map_with_int(draft,"IntMap",0,10)
    draft=ue_struct_map_with_int(draft,"IntMap",1,20)
    draft=ue_struct_set_with_int(draft,"IntSet",7,true)
    draft=ue_struct_array_resize(draft,"Rows",2)
    draft=ue_struct_with_byte(draft,"Rows.1.Priority",uint8(42))
}
ue_set_self_struct("Snapshot",changed)   // publication is a separate step
```

- Writes to the draft modify one buffer; reads inside the block see the current draft. The source stays unchanged.
- The draft expires when the block ends, on success or error; storing it in a global does not keep it valid.
- An unhandled error discards the draft. A failed write invalidates the draft immediately, so catching the error inside the block cannot publish a partial write. Full validation runs when the block completes.
- Independent nested edit blocks are allowed up to depth 8; editing an active draft again is rejected. Each block is separate, not a transaction across snapshots. Side effects inside the block (Unreal calls, property writes, globals) are not rolled back.
- A nested snapshot obtained inside the block is still an independent value: write it back with `ue_struct_nested_with` or the map adapter.

## Lifetime and garbage collection

A context retains at most 4096 live snapshots at once. Unreachable snapshots are reclaimed by mark/sweep over VM roots (globals, arrays/tables, active arguments and locals):

- Automatically after each persistent call, even without `options gc`.
- Inside a long-running call under pressure (around 256 retained snapshots), which requires `options gc` so the runtime can see live locals.
- On demand with `ue_struct_collect()`; `ue_struct_value_count()` reports the retained count.

If the runtime cannot attribute an active frame (missing `options gc`, foreign or position-less frames, AOT/JIT code), it does not free anything: explicit collection reports an error and pressure collection is skipped. Keep snapshots in typed VM roots. Releasing the context frees all snapshots.

## Field path cache

Repeated access to the same path string is parsed and resolved once per context (up to 128 paths; longer than 1024 UTF-16 units use temporary storage). Array indices are still checked against the current length on every access, and stale properties after an editor struct recompile are re-resolved. Empty paths and overlong segments are rejected with a missing-field error.

## Limitations

- Supported leaves: int32, float, double, bool, FString, FName, byte, FText, enums, object references, nested supported structs and supported arrays/maps/sets. Fixed C++ arrays, structs with native `AddStructReferencedObjects` hooks, recursive types and unsupported leaf kinds are rejected; reflection does not make arbitrary C++ state safe to copy.
- `UUserDefinedStruct` assets, including DSL-authored struct types, are accepted after the same reflection validation; field paths may use authored names or the internal GUID-suffixed names.
- Snapshots inside one value share the container limits: 4096 elements and 16 MiB.
- Existing math values (`UEVector` …) and generated `S_*` types are separate types; `UEStructValue` does not replace them.

See also: [Structs](../Guide/DSL/Structs.md) · [Containers](../Guide/DSL/Containers.md) · [Self access](../Guide/DSL/SelfAccess.md) · [Type support](TypeSupport.md) · [Limitations](Limitations.md)
