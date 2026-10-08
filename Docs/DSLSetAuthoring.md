# Owned set fields

daScript already has key-only tables. Use that syntax inside `[ue_struct]`:

```das
[ue_struct(module_id="sample", BlueprintType)]
struct Tags {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Ids : table<int>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Names : table<string>
}
```

The DSL emits IR `kind: "set"` with an `element` type and creates native
`FSetProperty` / `TSet<int32>` or `TSet<FString>` storage. `table<T;void>` is
the equivalent daScript type. A two-argument table with a non-void value
remains a map. Only int/string elements and implicit empty defaults are supported.
The enclosing struct can be an Actor property or a typed method input/result.

```das
var data <- clone(self.Config)
insert(data.Names, "Ключ 世界")
insert(data.Names, "Ключ 世界") // exact duplicate: one element
erase(data.Ids, 42)             // missing element: no change
self.Config <- data

var local <- clone(self.Config)
let present = key_exists(local.Names, "Ключ 世界")
for (name in keys(local.Names)) { /* readonly iteration */ }
```

Use a local owned clone, then assign the whole property. Iteration over a
temporary `self.Config` snapshot and insert/erase/clear calls on it reject.
daScript locks tables during iteration: do not mutate the same table inside its
keys loop. Set iteration order is not a public ordering contract. Packing and
unpacking construct independent native/script values; native pointers do not escape.

Native string comparison follows reflected Unreal property equality. If two
distinct script spellings compare equal natively, packing returns a collision
error before committing the root snapshot; it does not silently discard a key.
Exact duplicate insertion is idempotent. Contains/removal use native equality
in the low-level APIs. Unicode and empty strings use owned string storage.

## Low-level snapshot APIs

- `ue_struct_set_size(snapshot,path)`
- `ue_struct_set_element_int(snapshot,path,ordinal)`
- `ue_struct_set_element_string(snapshot,path,ordinal)`
- `ue_struct_set_contains_int(snapshot,path,element)`
- `ue_struct_set_contains_string(snapshot,path,element)`
- `ue_struct_set_with_int(snapshot,path,element,present)`
- `ue_struct_set_with_string(snapshot,path,element,present)`

`with` returns a new snapshot: true inserts, false removes. Missing removal
is idempotent. Ordinal access sorts stored int/FString elements for reproducible
enumeration and rejects invalid indexes. Shape/type mismatch rejects. Strings
returned by the bridge belong to the daScript context.

Sets share the 4096 total container element and 16 MiB fixed element storage
bounds with arrays/maps. FString heap contents are not counted by that byte
budget; this is not a memory sandbox. Current helper packing copies snapshots
per insertion and is intended for bounded containers, not optimized bulk workloads.

See `Plugins/DasForUE/Content/Das/Scripts/dsl_set_authoring.das` (replace
`REGISTRY` with an actual registry object path). Validation command:
`Scripts/TestDSLTypeRegistry.ps1 -AuthoredSet -Cooked`.

Standalone set signatures, more element types, nonempty defaults and direct
self-container mutation remain later work. Structural migration with sets
rejects before native changes; migration development remains deferred.
