# Typed soft references

```das
require ue_dsl
require ue_soft_references

[ue_class(Blueprintable)]
class Example : Actor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Asset:SoftObject = ue_soft(type<SoftObject>,"/Engine/BasicShapes/Cube.Cube")
        var ActorType:SoftActorClass = ue_soft(type<SoftActorClass>,"/Script/Engine.StaticMeshActor")
    }
    def BeginPlay():void {
        // No disk load: reads the path or obtains an already loaded object.
        let path=ue_soft_path(self.Asset)
        let existing=ue_soft_get(self.Asset)
        // Explicit blocking load, yielding the usual checked object reference.
        let loaded=ue_soft_load(self.Asset)
        ue_soft_load_async("asset",self.Asset,"AssetReady")
    }
    [ue_function(BlueprintCallable)]
    def AssetReady(value:ObjectRef):void { /* use value */ }
}
```

`SoftObject` is `$UESoftObject<ObjectRef>`, `SoftActorClass` is
`$UESoftClass<ActorRef>`. Define shared aliases with a narrower generated native
object wrapper to constrain the asset type or class base. The base must be a
loaded native non-interface class; the referenced asset/class may be authored in
Blueprint. Export-text strings such as `StaticMesh'...'` are not required: use
the asset object path, including the object name. Subobject paths are rejected.

The reflected fields and function pins are real `FSoftObjectProperty` and
`FSoftClassProperty`, preserving Unreal's asset picker, serialization and cook
dependency tracking. VM transport uses the existing checked snapshot ABI with a
canonical private carrier. Its internal `struct` IR kind is not a Blueprint
struct field. The carrier owns the path and never roots the referenced asset.

`ue_soft_path`, `ue_soft_is_null`, `ue_soft_is_loaded`, `ue_soft_get` and copying
or default construction do not load assets. `default<SoftObject>` represents an
empty path. Both get and synchronous load return a nullable typed reference;
missing assets produce null. An incompatible resolved type is rejected. An
unloaded path's type cannot be known until resolution/loading.

`ue_soft_load_async(name,value,"Method")` requires an annotated void instance
method taking the matching typed object/class reference. It shares managed asset
subscriptions with `ue_asset_cancel`, `ue_asset_cancel_all` and
`ue_asset_is_pending`. Empty paths complete with null. Callbacks are deferred
through the existing owner queue, including already loaded and empty paths.
Deferred means after the VM returns; an empty completion may run before the outer
native caller returns. It does not promise a next-frame callback.
Cancellation, owner release/EndPlay and reload revoke pending delivery; replacing
a named request validates the new contract before removing the existing request.
Completion receives a regular typed object/class reference, not the soft wrapper.

Fields and method input/result/ref/out are covered in the first implementation.
Arrays, map values and authored struct members were added in the next block;
see [SoftCollections.md](SoftCollections.md). Soft keys/sets, native member
generation and structural migration policy remain separate work. Network support
remains deferred. Holding a resolved weak object reference also does not keep the asset
loaded after the request handle is released; use Unreal ownership when required.

## Verification

`Scripts/TestSoftReferences.ps1` runs Editor Development, Foundation and the
dedicated real PIE test. Foundation includes unloaded path/default roundtrips,
wrong class/path rejection and proof that a rooted snapshot does not keep its
asset alive across GC. The source smoke uses the public aliases and resolve API.
The PIE fixture covers source method calls, native soft pins, return/ref/out,
sync and async object/class loading, null completion and cancellation.

`Scripts/TestSoftReferencesPackaged.ps1` exports a unique map and a separate asset
reachable only through a soft reference, then builds/cooks/stages Win64
Development and runs packaged Foundation plus the map. The game probe verifies
that the separate asset starts unloaded and is available to async loading in the
package, retains native soft property bases and supports unloaded return/ref/out.
Test output is written to `logs/soft-references-*`.

Editor/Game Development, Foundation in Editor and packaged game (zero failures),
116 Python checks, real PIE (one success; zero warnings, failures and skips),
cook/stage/archive and the dedicated packaged map all passed. The packaged probe
also verifies native soft class method constraints, unloaded Blueprint class path
return and pending request revocation on EndPlay. Evidence and hashes are in
`logs/soft-references-validation.json` and `logs/soft-references-source-manifest.json`.
The cooked lazy path is saved on a map actor instance; declared CDO defaults remain
intact. No commit has been created for this step.

See [soft parent conversions](SoftReferenceUpcasts.md) for safe implicit widening and `ue_soft_upcast`.
