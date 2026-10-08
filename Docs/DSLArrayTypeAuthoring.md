# Authored struct array fields

```das
[ue_struct(id="sample.Group", module_id="sample", BlueprintType)]
struct Group {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Items : array<Leaf>
}
```

Leaf must be a declared authored ue_struct in the same module. Arrays initially
have an implicit empty default, with no field initializer. The child-first native
factory creates an FArrayProperty whose inner FStructProperty has the child's
actual asset identity. Private IR uses kind=array with element.kind=struct and
the earlier stable-ID reference. Missing/recursive references and unsupported
element/container shapes reject; a fixed array is not a dynamic array.

The enclosing authored struct can be a ue_class property, typed method argument
or result. Recursive generated helpers resize a new native snapshot and pack each
element, then unpack snapshots into dynamic daScript arrays with owned elements.
Nested authored structs may contain further supported struct arrays, under the
existing depth/element/byte limits. Stable-ID source aliases remain supported.

daScript structs containing arrays require its clone/move semantics:

```das
var data <- clone(self.Config)
var item = Leaf()
item.Count = 42
push_clone(data.Items, item)
self.Config <- data
```

Use `return <- result` for a moved local result and clone const inputs when editing
them. Gen2 iteration uses `for (item in data.Items) { ... }`. Empty child struct
constructors and scalar literal defaults retain the previous semantics.

Whole self-property move assignment commits the packed value to the native Actor.
Reads of self.Config create owned unpacked snapshots. Direct indexed assignment
through self, such as self.Config.Items[0].Count += 1, explicitly rejects; clone,
edit locally and assign the whole property instead. Mutating a temporary obtained
from self (including passing self.Config.Items to a container mutator) does not
automatically commit it. This is the same ownership boundary, not a native alias.

Snapshot element and byte bounds remain 4096 total elements and 16 MiB of dynamic
array element storage. Helpers use checked native element identities, indexes and
sizes. Current helpers copy owned snapshots while composing values; very large
arrays are not optimized for bulk packing. No native element reference escapes.

Low-level owned APIs: ue_struct_array_size(value,path),
ue_struct_array_resize(value,path,count), ue_struct_nested_get(value,path,index),
ue_struct_nested_with(value,path,index,child). Index -1 selects an ordinary nested
struct member; nonnegative indexes select struct array elements. with/resize return
new owned snapshots, and size/index/type violations reject. These APIs do not
change the native Actor until the resulting root snapshot is assigned to self.

Example: Content/Das/Scripts/dsl_array_authoring.das in the plugin. Replace REGISTRY
with the registry object path for your project. Test:
Scripts/TestDSLTypeRegistry.ps1 -AuthoredArray -Cooked.

The first scope is struct-array fields inside authored struct definitions. Primitive
arrays, standalone ue_class array fields or array method signatures, set/map,
nonempty declared defaults and direct indexed self setters remain future work.
The existing scalar-leaf migration policy supports the stored array elements;
changing the outer container/member definition still needs its own policy.

Next: primitive/standalone array coverage and set/map declarations with key/identity
rules, then broader mutation and container definition/graph migration policies.
