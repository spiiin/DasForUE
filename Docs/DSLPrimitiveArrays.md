# Primitive array fields in authored structs

Supported dynamic array elements in ue_struct now include int, float, double,
bool, string and UENameValue, alongside authored struct elements:

```das
[ue_struct(id="sample.Values", module_id="sample", BlueprintType)]
struct Values {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Numbers : array<int>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Labels : array<string>
}
```

Arrays use implicit empty defaults. Populate a local owned value with ordinary
daScript operations, then commit the enclosing Actor property:

```das
var data <- clone(self.Config)
push(data.Numbers, 42)
push(data.Labels, "Hello 世界")
self.Config <- data
```

The native factory creates a correctly typed FArrayProperty. Snapshot validation
accepts the same scalar shapes with the existing depth/element/byte limits.
Generated pack/unpack loops preserve order, length, scalar values and ownership.
Typed methods still pass the enclosing struct rather than a standalone array.
Double values retain native precision; strings are owned and names use the
runtime's name-token mapping. The name setter binds by const-reference, avoiding
an unsupported managed-handle R2V path in the pinned daScript simulation.

Low-level scalar access accepts a terminal array index, e.g. Numbers.0 or Labels.1.
Existing checked ue_struct_get_int/float/double/bool/string/name and with counterparts
then enforce the actual element type. Resize remains bounded and returns a new
owned snapshot. Mutations through self require whole-root commit as before; direct
indexed self writes reject. Primitive array helpers use element iteration during
packing and scalar reads during unpacking.

Example: Content/Das/Scripts/dsl_primitive_arrays.das in the plugin. Replace REGISTRY
with your project registry object path. Test:
Scripts/TestDSLTypeRegistry.ps1 -PrimitiveArray -Cooked.

Array-of-array, enum/object elements, set/map, standalone ue_class arrays or array
function signatures, nonempty defaults and direct indexed self setters remain
outside this step. Snapshot copies currently prioritize correctness over bulk
packing performance.

## Migration work deferred

By user decision, broader migration development is paused in the implementation
roadmap. Existing supported migrations remain available. A changing type stored
inside a wrapper containing primitive arrays rejects before mutation; that storage
policy is not silently expanded by this runtime/DSL feature. Changes of the outer
container definition, set/map keys, defaults and graph repair stay in the deferred
matrix. Use new isolated assets for those schema experiments until a policy exists.

Next functionality: set/map declarations and typed signatures, then out/ref and
interfaces. These features do not require implementing universal hot reload first.
