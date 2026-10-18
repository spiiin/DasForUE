# AssetManager / Primary Assets

The managed adapter uses the project's existing `UAssetManager`. It does not
replace AssetManager settings, scan arbitrary disk paths, or import/cook assets.
Register Primary Asset types/directories in the project settings (or through
native project code). The assets and requested bundles must be present in the
cook for a packaged game.

## IDs, discovery and loading

```das
options gen2
require ue_primary_assets

let item=ue_primary_asset_id("Item","Potion")
let items=ue_primary_assets("Item")   // array<UEPrimaryAssetId>, no object load
let exists=ue_primary_asset_exists(item)
let path=ue_primary_asset_path(item)  // empty if not registered
```

`UEPrimaryAssetId` is a DAS value with `asset_type` and `asset_name` string fields,
converted to native `FPrimaryAssetId` at each call. It is not an object pointer or
VM token and can be copied or stored in ordinary DAS arrays. Native entry points
validate the two names (nonempty, at most 256 characters, no whitespace, path
separators or colon, and not None). This helper value is not a new reflected
storage type for authored UPROPERTY fields.

Discovery snapshots registered IDs for one type and sorts them by ID. Unknown
types return an empty array. The result limit is 4096 IDs; the private query table
allows eight snapshots per VM and the public helper releases each after copying.
No synchronous asset loading occurs in find/exists/path.

Call the APIs from a persistent script owned by an object in a live game world,
for example an Actor or widget. Construction, bind_events and EndPlay are rejected.

```das
var loaded_item:UObjectHandle

[export]
def item_loaded(asset:UObjectHandle):void {
    loaded_item=asset
    if (ue_object_valid(asset)) {
        // Cast to the generated reference view for your data-asset class.
    }
}

// Inside an exported persistent entry:
// ue_primary_asset_load_async("item",item,"item_loaded")
// Or load an explicit bundle set:
// var bundles:array<string>
// push(bundles,"UI")
// ue_primary_asset_load_async("item",item,bundles,"item_loaded")
```

The callback must be exported `(UObjectHandle):void`. Unknown IDs and native load
failure deliver a null handle. Unknown bundle names on a registered asset are a
validation error before starting a request. Completion is delivered on the game
thread through the existing managed callback queue. It also works when the asset
is already loaded. The registration is consumed before the handler executes.

The sample `Plugins/DasForUE/Content/Das/Scripts/primary_assets_example.das` searches
for the project's Item type, loads its first item and exposes a release entry.
It expects the project to register that type; it intentionally has no hard-coded
engine asset or project-specific class definition.

## Ownership, cancellation and unload

- `ue_primary_asset_pending(request)` checks callback registration.
- `ue_primary_asset_cancel(request)` revokes delivery, keeping the VM's load lease.
- `ue_primary_asset_object(id)` returns the already loaded object only if this VM
  owns a lease; otherwise null. It never loads synchronously.
- `ue_primary_asset_unload(id)` removes this VM's lease and its outstanding
  callbacks for that ID. It returns whether this VM owned a lease.

The AssetManager load state is shared across DAS VMs. One VM owns at most one
lease per ID; repeated loads deliver another callback without accumulating
ownership counts. Releasing one VM's lease does not unload another VM's resource.
The last managed owner calls native UnloadPrimaryAsset, dropping the manager's
root and bundle references. Other UE object references can still keep objects
alive; unload does not forcibly destroy objects. Object handles themselves retain
the existing weak checked semantics. VM release, accepted reload and world
cleanup release that VM's leases and revoke callbacks.

Concurrent managed owners must use the same normalized bundle set (deduplicated
and sorted). Different sets are rejected rather than silently replacing shared
state. Explicit unload by all owners permits a subsequent load with another set.
Limits: 32 bundles/request, 32 resources and 32 pending deliveries per VM, 128
shared managed resource leases. Duplicate pending request names are rejected;
Primary Asset names are separate from ordinary `ue_asset_load_async` requests.

The adapter refuses to take ownership of a pre-existing native AssetManager load
state. On release it checks the state handle and does not unload a replacement
state installed by native code. Native code must not independently mutate the
same managed ID's bundle state or use its load state as an implicit lease: UE's
global AssetManager API has no independent external ownership counters. Use one
ownership layer for a given ID. Ordinary UObject references are unaffected.
Cancellation does not promise to abort shared package I/O.

## Focused check

`Scripts/TestPrimaryAssets.ps1` builds the smoke Editor and runs one
`DasPrimaryAssets` commandlet scenario, with no full Foundation/PIE/packaged run.
It creates two real temporary .uasset packages under a unique Saved directory,
registers a unique Primary Asset type/ID with a UI bundle, then verifies unloaded
initial state, discovery, async callback/data, bundle loading, post-callback GC,
two owners, last-owner unload, null failure, cancellation and session release.
Only that unique type, mount, probe script and those exact temporary files are
removed afterwards. Source/DLL hashes and the engine BuildId are recorded in
`logs/primary-assets-validation.json`; moving sources/DLLs reject validation.
This is an Editor commandlet disk-load check, not certification of project scan
settings, cooked asset inclusion or platform-specific streaming behavior.

Latest focused validation (2026-10-18): Editor Development build PASS and
15 checks in one commandlet scenario PASS, with stable source/DLL hashes and
engine BuildId. The public example compiles. No full Foundation/PIE/packaged run.
