# Nested authored source values in interface calls

Source-type manifests now support nested mapped enum/struct fields and arrays of
supported mapped values. Each nested enum/struct identity must have an explicit
matching entry in the same manifest; unmapped or wrong-kind children reject.
The existing registry binding/ID/GUID/layout/default compiler checks remain.

```das
var settings = InterfaceSettings()
settings.Details.Amount = 21
settings.Details.Text = "Nested 世界"

var item = InterfaceDetails()
item.Amount = 31
item.Text = "Item 世界"
push_clone(settings.Items, item)

var copied <- api.EchoSettings(settings)
copied.Items[0].Amount = 99
// settings.Items[0].Amount remains 31.
```

Converters recursively create owned generated/source values, resizing and filling
each array element with its required value conversion. Move semantics are used for
structures containing arrays. Generated call frames now accept reflected array
fields inside structs, including struct elements, while preserving signature checks,
required input assignment, bounds/depth limits and independent copyback slots.

Wildcard field paths expand one array level at a time. Native invocation is allowed
only after every required concrete leaf/array header has been assigned. Output array
sizes are validated before copying. Each traversal has a bounded expansion budget
of 1,048,576 nodes, maximum 16 levels, and the existing 65,536-element per-array bound.
These generic call-frame limits are separate from the existing authored snapshot
limit (4,096 total elements) used by the DSL Actor implementation.

Raw arrays-of-arrays remain rejected. Name/object fields were subsequently
added in DSLSourceObjectValues.md; map/set fields in nested source structs and
struct-array elements were subsequently added in DSLSourceHashValues.md. Hash
values containing other containers remain rejected. This stage does not add standalone array-of-authored-struct
signatures to ue_class/ue_interface: the tested arrays are contained in authored
struct values. Direct hash support and existing primitive/container calls remain.

The test fixture module ue_interface_values.das now defines InterfaceDetails and
InterfaceSettings with Details and Items. The binding test constructs two distinct
elements, preserves Unicode and values, mutates returned data, invokes again and
checks empty arrays plus nested defaults. Three value assets are explicitly held
in the Actor's registry for save/reopen/cook. General migrations remain deferred.
