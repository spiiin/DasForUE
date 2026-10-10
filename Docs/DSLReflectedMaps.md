# Enum and reference map values

Implementation added 2026-10-10; **not built or tested**. No test, commandlet,
PIE, cook or runtime import was run for this batch at the user's request.

`table<int; T>` and `table<string; T>` now have source/native transport for:
- Authored ue_enum values, retaining the exact Unreal enum identity.
- Typed native object and native interface references.
- Authored class reference views in class properties and function signatures.

Class fields and reflected input/result/ref/out use the same converters.
ue_struct members accept authored enums and native object/interface wrappers;
authored class views inside ue_struct remain deferred along with their bootstrap
and identity contract. Authored interface view maps and native enum maps are
also separate work. Defaults remain implicit empty maps.

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
States:table<string;MapStatus>
@ue_property=(EditAnywhere, BlueprintReadWrite)
Actors:table<string;ActorRef>

[ue_function(BlueprintCallable)]
def Activate(name:string) {
    insert(self.States,name,MapStatus.Active)
}
```

Examples are `ue_struct_map_types.das`, `ue_struct_map_actor_refs.das` and
`dsl_reflected_maps.das` in the plugin's Content/Das/Scripts folder. Import the
types, then dsl_struct_maps.das, then dsl_reflected_maps.das into
`/Game/DasStructMaps` using the existing workflows. This has not been exercised.

Call signature validation now accepts reflected struct and object/interface map
values and recursively checks them; key hashing rules remain unchanged. This also
closes the call-frame restriction left by the preceding struct-map batch.

Native Blueprint storage is a TMap with byte enum, object or interface value
pins. Rooted carriers encode key kind, value category and full reflected type
path; their bounded cache checks enum layout/member IDs and object/interface
category. Dynamic carrier restoration loads only authored enums under /Game and
native reference classes under /Script.

Enum insertion checks identity and usable values before storing the snapshot.
The temporary default byte of a new entry is replaced before publication, so
sparse enums without zero do not require an artificial zero member. Snapshot
validation also rejects unknown/reserved map enum values originating natively.

Reference insertion checks class/interface; typed authored converters additionally
check each non-null value's logical stable ID. Physical host classes remain shared,
so raw Blueprint/C++ storage alone does not enforce logical authored identity.
Reads return owned maps with checked weak handles; native properties and temporary
GC-referenced snapshots retain their normal Unreal ownership semantics.

Source helper keys now include the nominal reference wrapper for both arrays and
map values, preventing converter collisions between two wrappers of one physical
class. Wrapper types remain distinct. Null/expired read semantics use existing
object adapters; invalid non-null writes fail before property publication.

Readonly, clone/replacement, exact key types, string equality collision checks,
aggregate 4096-element/16 MiB bounds, and per-publication failure rules apply.
Multiple statements are not a whole-function property transaction.

Pending validation: build and type/class import/reopen; both key families; sparse
enum and wrong/reserved enum values; object/interface incompatibility; null and
destroyed Actors; authored stable-ID mismatch and nominal wrapper isolation;
readonly diagnostics; self/foreign mutations; input/result/ref/out and failed
copyback; forced GC; Blueprint pins; PIE and packaged Development.
