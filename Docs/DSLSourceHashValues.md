# Source struct map/set fields

Authored source bindings now convert daScript tables to owned native transport
containers inside ue_struct values, including nested structs and struct-array
elements. The existing DSL grammar is unchanged:

```das
[ue_struct(module_id="example", BlueprintType)]
struct Stats {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Weights : table<string; double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Labels : table<int; UENameValue>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Tags : table<string>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Ids : table<int>
}
```

Maps use int/string keys and int/float/double/bool/string/Name values. Sets use
int/string elements. Fields have implicit empty defaults. Source table iteration
uses paired keys/values iterators, which support const input tables without
inserting values. Set insertion and map assignment reconstruct fresh source
tables on return. Name values use the same checked context-name conversion as
ordinary struct fields. No reader macro or function annotation lists are needed.

```das
var settings = InterfaceSettings()
settings.Details.Weights["Unicode 世界"] = 2.5lf
settings.Details.Labels[7] = ue_name_value("MapName_9")
insert(settings.Details.Tags, "Tag 世界")
var copied <- api.EchoSettings(settings)
copied.Details.Weights["Unicode 世界"] = 99.0lf
// settings.Details.Weights["Unicode 世界"] remains 2.5lf.
```

The low-level representation remains an owned array of map entries/set elements;
the facade converts to natural tables. Hash carrier declarations needed by structs
are emitted before those structs. Shape/type-marker checks and explicit source
registry/ID/GUID/layout checks are retained. Internal generated references are
qualified by module to coexist with the native bindings imported by source types.

Native call frames now allow reflected scalar map/set fields inside structs.
Concrete paths expand array and hash wildcards with a bounded traversal budget.
Every required header, key, value and element must be assigned before invocation.
UE hash/equality checks reject duplicate keys/elements. Parent array resize clears
pending hash data below that array, preventing stale or incomplete slots from being
committed. Output hash indices use valid native slots, including containers with
holes after removal. Source reconstruction also rejects duplicate keys/elements.

Container order is unspecified; callers use keys/elements rather than insertion
order. Input/output values own independent table/array storage and use move
semantics where needed. Existing GC behavior for object fields is retained.
Generic call-frame limits remain 65,536 elements per container, 16 traversal
levels and 1,048,576 expanded traversal nodes. DSL snapshots retain their existing
4,096-total-element limit.

Maps with object/struct/container values, non-int/string source keys/elements,
arrays directly containing map/set, explicit nonempty defaults and standalone
array-of-authored-struct signatures remain separate work. General migrations stay
deferred. Existing top-level native scalar hash support remains available.

Validation is recorded in DSLSourceHashValuesValidation.md. The test fixture is
Plugins/DasForUE/Content/Das/Scripts/ue_interface_values.das.
