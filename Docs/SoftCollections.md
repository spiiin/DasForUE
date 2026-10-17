# Soft references in collections and authored records

```das
require ue_dsl
require ue_soft_record

[ue_class(Blueprintable,type_folder="/Game/DasSoftReferences/Types")]
class Example : Actor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Assets:array<SoftObject>
        var Classes:table<string;SoftActorClass>
        var Record:SoftRecord
    }
    [ue_function(BlueprintCallable)]
    def SetAsset(value:SoftObject):void {
        push(self.Assets,value)
        self.Assets[0]=value
        self.Record.Asset=value
    }
}
```

The imported `ue_soft_record.das` declares an ordinary authored reflected struct:

```das
[ue_struct]
struct SoftRecord {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Asset:SoftObject
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    ActorType:SoftActorClass
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Items:array<SoftObject>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Named:table<string;SoftActorClass>
}
```

Apply authored types through the existing type service/registry before applying
the actor, as with other authored structs. Soft fields use implicit empty paths;
nonempty struct member declaration defaults are deliberately rejected. Set their
values on instances or in ordinary DAS code. Arrays and maps in authored structs
likewise retain the existing empty declaration default rule. Standalone array/map
properties can use literal defaults with `ue_soft(type<SoftAlias>,path)` values.

Both `UESoftObject<T>` and `UESoftClass<T>` are supported as array elements and map
values. Existing supported map key kinds retain their policies. Soft references
as keys or set elements remain rejected; no token-based hashing is exposed.
Arrays/maps stay native containers of `FSoftObjectProperty`/`FSoftClassProperty`,
and authored records use the same native soft leaves, not wrapper structs in UE.

Reading/copying/serializing a collection, including native method return/ref/out,
does not load its paths or root its assets. VM/native conversion uses the common
property snapshot codec and checked typed wrappers. A detached collection or
record remains independent when the original owner property changes. Invalid
paths and resolved wrong-base values are validated before publishing a snapshot.

The existing soft references test runners exercise this block too. The fixture
checks default paths, indexed assignment, push, map insertion/lookup, authored
record members and nested containers, snapshot isolation, source method calls and
array return/ref/out. The native probe checks real soft property storage for
object/class arrays and maps with int/string/name/byte/object keys, descriptor
roundtrips, unloaded defaults and rejection of soft sets. Cooked validation also
checks behavior without editor metadata.

Editor/Game Development, both Foundation runs (zero failures), 116 Python
checks, real PIE (zero warnings/failures/skips), cook/stage/archive and packaged
Development passed. Packaged C++ `ProcessEvent` calls additionally validate array
return/ref/out, soft class map return and authored soft record return with unloaded
paths. See `logs/soft-collections-validation.json` and the source manifest.
Also fixed rejection of overlong field names before `FName` construction and
restored the empty-path diagnostic in the shared snapshot field resolver. No commit.
