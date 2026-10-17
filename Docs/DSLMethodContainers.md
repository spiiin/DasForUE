# Containers in typed authored method calls

Container signatures now use ordinary DAS values through authored references:

```das
var values:array<int>
push(values,1)
let copy = api.CopyInts(values)

var previous:array<int>
api.ChangeInts(values, previous)

var weights:table<string; double>
weights["a"] = 2.5lf
var oldWeights:table<string; double>
api.ChangeMap(weights, oldWeights)

var tags:table<string>
insert(tags,"red")
let tagCopy = api.CopySet(tags)
```

Target methods use the same annotations and parameter directions as scalar methods:

```das
[ue_function(BlueprintCallable)]
def CopyInts(values:array<int>):array<int> {
    return <- clone(values)
}

[ue_function(BlueprintCallable)]
def ChangeInts(var values:array<int>&;
               @ue_out var previous:array<int>&):void {
    previous <- clone(values)
    push(values,9)
}
```

Supported standalone types follow the existing carrier policy:

- array<int/float/double/bool/string/UENameValue> and arrays of authored structs.
- table<int> and table<string> as sets.
- table<int|string; int/float/double/bool/string/UENameValue> as maps.

Authored struct arguments/results may contain supported arrays, maps, sets and
nested structs. Value-module imports and registry/folder resolution remain explicit.
Wrong container element/key/value types produce compilation errors. Direct nested
containers, enum/object standalone arrays and richer map/set type combinations
remain outside the existing carrier policy.

The compiler rebuilds container source types from the declaration and compile-chain
source descriptors. It uses existing pack/unpack helpers and owned carrier snapshots;
the public API does not expose UEStructValue or require per-method hand-written glue.
ref/out publication and results move the newly unpacked owned DAS values. Inputs,
returned values and previous values remain independent; mutable containers in target
methods should use clone when a separate copy is required.

The native adapter copies complete snapshots into the existing UFunction frame.
Whole initialized roots are tracked separately from scalar-by-scalar initialization.
Snapshot validation, bounded traversal and recursive object pins run before invocation.
Return/out snapshots pass the same validation and source unpacking. Limits include
the existing aggregate snapshot limit of 4096 container elements and 16 MiB, plus
bounded native traversal. Call frames are finalized on success and recovery.

Unreal exposes Blueprint container inputs by reference in reflected flags. For a
declared DAS in parameter the authored adapter accepts that physical representation
while sending an owned copy and publishing no argument update. Explicit ref/out
directions retain their copy-in/copy-out semantics. All outputs are unpacked before
caller variables change; failures preserve caller ref values. Unreal field writes
performed by the callee are not rolled back.

Examples: ue_method_values.das, dsl_method_value_target.das and
dsl_method_value_driver.das. TestMethodValues.ps1 -Cooked exercises the definitions
with a GUID registry in Create, Reopen, actual PIE and a packaged game.
Native object/interface and math signatures are supported by DSLMethodObjectMath.md.
Authored views in reflected signatures, automatic reference/import discovery and
general migrations remain separate work.

Validation: [DSLMethodContainersValidation](DSLMethodContainersValidation.md).

Native object/interface and math method signatures:
[DSLMethodObjectMath](DSLMethodObjectMath.md).

Update: FName map keys and sets in direct properties and method signatures are
supported; see [DSLNameContainers.md](DSLNameContainers.md).

Update 2026-10-17: native and authored enum sets are supported in native self
fields, direct class properties and method inputs/results/out/inout; see
[DSLEnumSets.md](DSLEnumSets.md).

Update 2026-10-17: UObject sets and object-key maps are supported through native
self fields, direct properties and method inputs/results/out/inout using typed
UEObjectKey values; see [DSLObjectHashContainers.md](DSLObjectHashContainers.md).
