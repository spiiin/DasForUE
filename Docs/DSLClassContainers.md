# Direct ue_class container properties

Arrays, maps and sets can be declared directly on authored Actor/ActorComponent
classes. They become real Unreal FArrayProperty/FMapProperty/FSetProperty fields,
with native Blueprint container pins; carrier structs are only an internal
snapshot transport representation.

```das
require ue_foundation
require ue_dsl

[ue_class(Blueprintable)]
class Counter {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Scores : array<int>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Weights : table<string; double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Tags : table<string>

    [ue_function(BlueprintCallable)]
    def AddScore(value : int) {
        push(self.Scores, value)
        self.Weights["last"] = double(value)
        insert(self.Tags, "changed")
    }
}

// A typed authored reference exposes the same fields in ordinary functions.
def edit(api : CounterRef) {
    api.Scores[0] += 1
    push(api.Scores, 10)
    var copy = api.Scores
    copy[0] = 99
    api.Scores := copy
    clear(api.Tags)
}
```

The reference in the last example must be declared/imported through the existing
ue_object_ref mechanism. Indexed access assumes an existing element. Getters
return owned copies; mutable function arguments and iterators still require a
local copy followed by explicit whole-property assignment.

## Supported types and defaults

The same carrier-backed shapes as standalone method values are supported:
primitive/Name arrays and arrays of authored ue_struct; int/string sets;
int/string-key maps with supported primitive/Name values. No new element/value
families are introduced by this stage. Shapes without an existing carrier reject.

Defaults must be omitted and are empty. Explicit initializers, including explicit
empty constructors, are rejected; nonempty declaration/CDO defaults need a
separate default/migration contract. BlueprintReadOnly applies to whole-field
replacement, indexed writes and container operations.

Reads and writes use the common native property snapshot adapters. The native
property must exactly match the carrier Values property, and every candidate is
validated before publication. Caller copies and failed index/packing operations
do not modify native storage. Existing snapshot size and lifetime limits remain.

## Reload scope

Body-only reimport is supported without replacing the generated class. Changes to
the declaration of an existing class containing direct containers are refused.
The structural rollback preflight also refuses direct-container classes before
using its earlier struct-only capture path. General container class migrations
remain deferred, as previously agreed.

Examples: ValueTarget.DirectContainers and exercise_values in the method-value
fixtures. See DSLClassContainersValidation.md and DSLIndexedContainers.md.

Update: multi-level self writes and operations are covered in DSLMultiIndexSelf.md.

Update: arrays of UEVector/UERotator/UETransform are supported in DSLMathArrays.md.

Update: authored enum arrays are supported in DSLEnumArrays.md.

Update: native object/interface arrays are supported in DSLReferenceArrays.md.

Update: FName map keys and sets in direct properties and method signatures are
supported; see [DSLNameContainers.md](DSLNameContainers.md).

Update 2026-10-17: native and authored enum sets are supported in native self
fields, direct class properties and method inputs/results/out/inout; see
[DSLEnumSets.md](DSLEnumSets.md).
