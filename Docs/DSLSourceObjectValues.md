# Source struct Name and UObject fields

Natural `ue_struct` declarations can contain context-owned `UENameValue` and
typed native UObject wrappers. Unreal weak/class/interface fields, object
arrays inside authored structs and non-null authored object defaults remain
separate adapters.

```das
require ue_dsl
require ue_object_types

[ue_struct(module_id="example", BlueprintType)]
struct TargetInfo {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Key : UENameValue
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : ActorRef
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Object : ObjectRef
}
```

These fields have implicit `None`/null defaults. Assign a Name with
`ue_name_value("TargetKey_7")` and inspect it with `ue_name_text` or
`ue_names_equal`. Source binding converters bridge names through the generated
UEName string representation. Object conversion copies a checked context handle
into the appropriate nominal wrapper; it never copies UObject memory. The source
compiler validates the exact native object class against the reflected property.

Reflected object leaves are supported in nested call-frame structs, including
struct elements in arrays. Inputs and outputs retain the existing call-frame
pins, class checks, required assignment checks and traversal limits. Invalid
input handles reject before invocation. Destroyed output objects become null.

The interpreter context is an Unreal `FGCObject` and reports each stored
FInstancedStruct through `AddStructReferencedObjects`. This retains reflected
object references (and the struct type) until the owning snapshot is reclaimed
or the context is released. Unreal GC can also clear destroyed references in the
snapshot memory itself. Incoming native struct frames are also copied into
a GC-reported frame for compilation/reload and invocation. Native structs with opaque reference callbacks, weak/
soft/lazy/class/interface fields and unimplemented containers stay rejected.

This ownership differs from an ordinary ActorRef/ObjectRef variable: its handle
is weak. A native struct snapshot and a reflected UObject property hold strong
references. Dropping a script variable alone is not a guarantee of immediate
release; snapshot root collection must reclaim the old token. Session owners retained
through a snapshot must still release their runtime session on teardown. The existing
`options gc`/`ue_struct_collect()` and call-boundary root collection apply.

Copies own independent fields, strings and arrays, but object identity is shared:
changing a copied Target field does not change the original field; modifying the
referenced UObject affects the same object. The context roots only its stored
snapshots. Host-side FInstancedStruct copies must follow normal Unreal GC rules.

Tests cover nested fields/array elements, Name suffixes and None, null/default
objects, wrong classes, strong retention across GC, snapshot reclamation and
destroyed Actor reads after GC. The generated authored-interface probe exercises
natural values through an independent Blueprint receiver in editor reopen and
packaged game. General migration logic remains deferred.

See DSLSourceObjectValuesValidation.md for verified commands and results.
