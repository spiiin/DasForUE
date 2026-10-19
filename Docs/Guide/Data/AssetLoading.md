# Asset Loading

Scripts load assets asynchronously through Unreal's Streamable Manager and AssetManager, with completion delivered to a script method on the game thread. There are three entry points: typed soft references (`ue_soft_load_async`), plain object paths (`ue_asset_load_async`), and Primary Asset IDs with bundles (`ue_primary_asset_load_async`).

None of these runs daScript on a worker thread or waits synchronously, and none is `await`/coroutine syntax: you get an explicit named request and a callback.

## Typed soft references (recommended)

A soft reference property keeps the asset path serialized and typed. Load it when needed:

```das
[ue_class(Blueprintable)]
class Pickup : Actor {
    settings=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var MeshAsset:SoftStaticMesh=ue_soft(type<SoftStaticMesh>,"/Engine/BasicShapes/Sphere.Sphere")
    }
    [ue_function(BlueprintCallable)]
    def LoadMesh():void {
        ue_soft_load_async("mesh",self.MeshAsset,"OnMeshLoaded")
    }
    [ue_function(BlueprintCallable)]
    def OnMeshLoaded(mesh:StaticMeshRef):void {
        if(!ue_is_valid(mesh)) { return }   // failure or empty path
        self.Visual.SetStaticMesh(mesh)
    }
}
```

- The callback is an annotated void instance method taking the matching typed object/class reference (not the soft wrapper).
- `ue_soft_path`, `ue_soft_is_null` and `ue_soft_is_loaded` never load anything. `ue_soft_load(value)` is an explicit blocking load.
- Empty paths complete with null. Callbacks are deferred through the owner's queue even when the asset is already loaded (after the VM returns; not necessarily next frame).
- Soft-load requests share the request namespace and the `ue_asset_is_pending` / `ue_asset_cancel` / `ue_asset_cancel_all` functions below.

`dsl_playable_pickup.das` keeps the pickup hidden and not collectible until its mesh arrives; a failed load leaves it unavailable, `LoadMesh` replaces an older request, and Destroy/EndPlay cancels it. Soft reference syntax and typing are described in [Soft references](../DSL/SoftReferences.md).

## Loading by object path

```das
ue_asset_load_async("image",self.AssetPath,"OnLoaded")

[ue_function(BlueprintCallable)]
def OnLoaded(value:ObjectRef):void {
    self.Loaded=value
    self.Succeeded=ue_object_valid(value.handle)
}
```

| Function | Purpose |
| --- | --- |
| `ue_asset_load_async(name, object_path, callback)` | Start or replace a named request. |
| `ue_asset_is_pending(name)` | `true` while pending, including a queued but undelivered completion. |
| `ue_asset_cancel(name)` | Cancel/remove this session's request. |
| `ue_asset_cancel_all()` | Number of this session's requests removed. |

- The handler's parameter type selects the expected class: `ObjectRef` accepts any UObject; a narrower native wrapper requires that class. Interface/authored-class views and extra arguments are not supported. Plain exported callbacks use `(UObjectHandle):void`.
- The path must be a valid Unreal object path (at most 1024 characters), not a file or URL.
- Request names are 1..128 characters, session-local, separate from timer/latent/input names; at most 64 requests per session.
- Failure or a wrong loaded class delivers null (a wrong class also logs a warning).
- A loaded object is kept alive only through delivery. Store it in a reflected `UObject` property if it must stay referenced; DAS wrappers themselves are weak.

Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_async_asset.das` (editable `AssetPath`, nullable `Loaded`).

## Request semantics (soft and path loads)

- Requires a persistent session whose owner is in a live game world. Starting during event binding or EndPlay is refused.
- Handler, path, type and count checks run before an existing request with the same name is replaced.
- Completion is consumed before user code runs, so a handler can start a new request under the same name.
- Cancel suppresses this request's callback and releases its handle; it does not abort a package load shared with other engine requests. A completion already queued for a revoked request is ignored.
- EndPlay, session release, world cleanup, shutdown, reinstancing and accepted reload cancel requests; a rejected reload keeps them. Requests are not restored automatically, and `BeginPlay` is not replayed.

**Cooking:** a string path alone does not put an asset in a packaged build. Use soft references, Primary Asset rules or other normal cook rules.

## Primary Assets

`ue_primary_assets` works with the project's existing `UAssetManager`. It does not change AssetManager settings, scan arbitrary disk paths, or import/cook assets: register Primary Asset types and directories in Project Settings (or native code), and make sure the assets and bundles you request are cooked.

```das
options gen2
require ue_primary_assets

let item=ue_primary_asset_id("Item","Potion")
let items=ue_primary_assets("Item")   // array<UEPrimaryAssetId>, nothing is loaded
let exists=ue_primary_asset_exists(item)
let path=ue_primary_asset_path(item)  // empty if not registered
```

`UEPrimaryAssetId` is a plain value with `asset_type` and `asset_name` string fields; copy it or store it in arrays freely. Both names must be non-empty, at most 256 characters, with no whitespace, path separators or colon, and not `None`. It is not a reflected property type for authored UPROPERTY fields.

`ue_primary_assets(type)` returns the registered IDs of one type sorted by ID (empty for unknown types, at most 4096). Discovery, `exists` and `path` never load objects.

### Loading and bundles

```das
var loaded_item:UObjectHandle

[export]
def item_loaded(asset:UObjectHandle):void {
    loaded_item=asset
    // Cast to the reference type of your data-asset class here.
}

// From a persistent entry:
ue_primary_asset_load_async("item",item,"item_loaded")
var bundles:array<string>
push(bundles,"UI")
ue_primary_asset_load_async("item",item,bundles,"item_loaded")
```

| Function | Purpose |
| --- | --- |
| `ue_primary_asset_load_async(request, id [, bundles], handler)` | Load the asset (and bundles). Handler is an exported `(UObjectHandle):void`. |
| `ue_primary_asset_pending(request):bool` | Is the callback still registered? |
| `ue_primary_asset_cancel(request):bool` | Revoke delivery; the load lease is kept. |
| `ue_primary_asset_object(id):UObjectHandle` | The loaded object, only if this VM owns a lease; never loads. |
| `ue_primary_asset_unload(id):bool` | Drop this VM's lease and its pending callbacks for that ID; returns whether it owned one. |

- Unknown IDs and native load failures deliver a null handle. Unknown bundle names on a registered asset are a validation error before the request starts.
- Completion arrives on the game thread through the managed callback queue, also when the asset is already loaded.
- Call from a persistent script owned by an object in a live game world (Actor, widget). Construction, event binding and EndPlay are refused.
- Primary request names are separate from `ue_asset_load_async` names; duplicate pending request names are rejected.

### Ownership

- Load state is shared between VMs. Each VM holds at most one lease per ID; repeated loads deliver another callback without stacking leases. When the **last** managed owner releases, native `UnloadPrimaryAsset` drops the manager's references. Other UE references may still keep objects alive.
- VM release, accepted reload and world cleanup release that VM's leases and revoke callbacks.
- Concurrent owners must request the same (normalized, deduplicated, sorted) bundle set; a different set is rejected. After all owners unload, another set can be loaded.
- The adapter refuses to take over a pre-existing native load state and will not unload a state that native code replaced. Use one ownership layer per ID: native code should not mutate the same ID's bundle state independently.

| Limit | Value |
| --- | --- |
| Bundles per request | 32 |
| Resources per VM | 32 |
| Pending deliveries per VM | 32 |
| Shared managed leases | 128 |

Example: `Plugins/DasForUE/Content/Das/Scripts/primary_assets_example.das` finds the project's `Item` type, loads the first item and has a `release_item` entry. It expects the project to register that type. To author Primary Data Assets in the DSL, see [Data assets](DataAssets.md).

See also: [Soft references](../DSL/SoftReferences.md), [Data assets](DataAssets.md), [Timers](../Gameplay/Timers.md), [Callback arguments](../Gameplay/CallbackArguments.md)
