# Maps with math and authored struct values

Implementation added 2026-10-10. This batch has **not been built or tested**;
no commandlet, PIE, cook or regression runs were performed at the user's request.

Supported source shapes are `table<int; T>` and `table<string; T>`, where `T`
is UEVector, UERotator, UETransform, or an authored ue_struct. They apply to
direct ue_class properties, ue_struct members, and reflected input/result/ref/out
signatures. Existing typed-reference accessors reuse the same converters.

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Entries:table<string;MapEntry>

[ue_function(BlueprintCallable)]
def Increase(name:string; amount:int) {
    self.Entries[name].Score += amount
    push(self.Entries[name].Samples,double(amount))
}
```

Examples:
- `Plugins/DasForUE/Content/Das/Scripts/ue_struct_map_types.das`
- `Plugins/DasForUE/Content/Das/Scripts/dsl_struct_maps.das`

Import the type module using the existing reflected-type workflow into
`/Game/DasStructMaps` before importing the class. Explicit registries remain
supported. Type dependency ordering now includes map struct values; recursive
definitions and depth above 16 remain rejected.

Blueprint storage is an actual TMap with the exact struct value pin, not an
opaque JSON payload. Internal rooted carriers encode the key kind and struct
path and retain a layout fingerprint. The cache is bounded to 1024 map shapes;
changed layouts or path collisions are refused rather than silently reused.

Packing/unpacking copies each value through the existing typed struct/math
adapters. Reads return owned snapshots; clone/replace, field/index mutations and
insert_clone use existing publication rules. Nested data counts toward the
aggregate 4096-element/16 MiB snapshot limits. Math validity is checked
recursively, including normalized Transform rotation. Exact native struct
identity is checked before inserting a transported value. String key collisions
under Unreal equality retain the existing refusal policy.

Empty implicit property defaults only. Enum/reference map values are now implemented separately (validation pending;
see DSLReflectedMaps.md). Other key types,
live mutable projections and structural migrations remain separate work. A
failure inside a multi-statement function does not roll back earlier successful
statements; each property publication is validated independently.

Pending validation: build; reflected model/dependency import and reopen;
Blueprint value pins; both key families with all math types and container-bearing
authored structs; self/foreign mutations; result/ref/out and failed copyback;
wrong struct identity; readonly errors; invalid math, limits and string collisions;
PIE and packaged Development roundtrips.
