# UObject sets and map keys

Direct ue_class properties, native self fields and ue_function inputs/results/
out/inout support UObject sets and UObject-key maps. Unreal stores real
FSetProperty/FMapProperty containers with class-constrained FObjectProperty keys.

## Source representation

daScript table keys cannot be reference-wrapper structs such as ActorRef. The
binding supplies a nominal by-value key type through `$UEObjectKey<Reference>`.
The public `ue_object_keys` module predefines ActorKey and ObjectKey. A key is an
opaque, context-owned object identity; it is not a UObject pointer or a persistent
integer ID. Ordinary object values still use ActorRef/ObjectRef and other wrappers.

```das
require ue_dsl
require ue_object_keys

[ue_class(Blueprintable)]
class ActorRegistry {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Actors:table<ActorKey>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Scores:table<ActorKey;int>

    [ue_function(BlueprintCallable)]
    def Remember(actor:ActorRef):void {
        let key=ue_key(actor)
        insert(self.Actors,key)
        self.Scores[key]=10
    }
    [ue_function(BlueprintCallable)]
    def CopyActors(values:table<ActorKey>):table<ActorKey> {
        return <- clone(values)
    }
    [ue_function(BlueprintCallable)]
    def ChangeScores(var values:table<ActorKey;int>&;
                     @ue_out var previous:table<ActorKey;int>&):void {
        previous := values
        values[default<ActorKey>]=99
    }
}
```

Define custom key aliases in an imported module, so type macros resolve before
the importing ue_class annotation processes fields and signatures:

```das
module game_keys public
require ue_object_keys public
require game_references public
typedef EnemyKey = $UEObjectKey<EnemyRef>
```

Native class wrappers and authored ue_object_ref wrappers are accepted. Authored
keys preserve the stable authored class ID as well as the native host class.
The method-value fixture uses a separate generated keys module to avoid source
dependency cycles. Interface views, class handles, weak/soft references and raw
UObjectHandle as the generic key type are not accepted.

`ue_key(reference)` infers the nominal key type. `ue_key(type<ActorKey>, value)`
performs an explicit checked conversion from a reference or UObjectHandle;
`ue_key_ref(key)` restores the key's typed reference and `ue_key_valid(key)` tests
whether its non-null object is live. `default<ActorKey>` is the null key. Null is
supported and has ordinary Unreal set/map equality. Keys of different nominal
types require explicit checked conversion.

## Identity and lifetime

Identity tokens are interned by FObjectKey (object index and serial), independently
of the ExpectedClass on an ordinary object handle. Two references to the same
object therefore produce one key after conversion to the same nominal key type.
Native equality/hashing is provided by FObjectProperty, not by token integers.
Tokens from another VM context, forged tokens, expired objects and destroyed
Actors are rejected before native publication. An expired nonzero VM key does not
become null. `ue_key_valid` returns false; `ue_key_ref` and publication throw.

Native UPROPERTY containers own strong object references. Owned FInstancedStruct
snapshots participate in reference collection. Method frame traversal already
pins map keys and set elements, including nested values, for the call lifetime.
The VM token tables hold weak references and use monotonically assigned tokens;
they do not keep objects alive after native ownership/snapshot/frame roots end.

Reading native collections containing destroyed keys fails instead of merging
keys or silently dropping entries. Recovery is whole-property assignment of an
empty local table; it does not require reading the previous invalid collection.
Unreal itself can alter native references during GC; the bridge validates the
current native storage, including duplicate identities, rather than reconstructing
identities already removed by the engine.

## Operations and scope

insert, erase, clear, key_exists, indexed map access/compound writes, local clone
and whole-property clone assignment (`:=`) reuse the existing container lowering.
Iterate local copies using keys/values. Readonly and exact key/value type checks
apply to all mutations. Writes publish only after complete snapshot validation.
Native self inference requires one visible canonical native reference wrapper;
import its key alias module as well to keep key types shared across modules.

Map values use the existing scalar/name/math/authored-struct/enum/object/interface
families. Native enum-class map values retain strong enum storage; authored and
regular byte enums retain FByteProperty. Unsupported shapes are rejected.

Existing limits remain: 4096 total snapshot elements, 16 MiB, 1024 object hash
carrier combinations and 65536 object identities per VM context. Defaults are
implicit empty; direct nested containers, arbitrary struct keys, authored
ue_struct member extensions and structural container declaration migrations are
separate work. No daScript SDK or persisted IR/JSON format changes are needed.

Examples: `ue_object_keys.das`, `ue_method_object_keys.das`,
`NativeArraySelf.ObjectHashes/ExpiredObjectHashes` and ValueDriver object-key
properties/methods in `dsl_method_value_driver.das`.
