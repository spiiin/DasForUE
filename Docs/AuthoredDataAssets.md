# Authored DataAsset / PrimaryDataAsset

`DataAsset` and `PrimaryDataAsset` are native DSL root markers, like SaveGame.
They use reflected Blueprint fields/defaults and the common checked frame ABI.
Data-only definitions need no dummy method. Actor lifecycle hooks, components
and async physics hooks are rejected. Asset methods can read/write values and
call supported methods; a DataAsset has no game world, so world-bound gameplay
services must be called from an Actor/widget instead.

## Definition

```das
options gen2
require ue_dsl
require ue_soft_references

[ue_class(Blueprintable,primary_asset_type="Item")]
class ItemDefinition : PrimaryDataAsset {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Item score configured in each asset instance.
        var Score:int=10
        var Label:string="Potion"
        // AssetManager loads this soft reference with the UI bundle.
        var @AssetBundles="UI" Icon:SoftObject
    }
    [ue_function(BlueprintPure)]
    def ComputeValue(amount:int):int {return self.Score+amount}
}

[ue_class(Blueprintable)]
class TuningDefinition : DataAsset {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Strength:double=1.5lf
    }
}
```

The full source is `Plugins/DasForUE/Content/Das/Scripts/dataasset_items.das`.
The ordinary authored inheritance/reference rules also apply. Both roots have
native runtime hosts (`UDasDataAsset`, `UDasPrimaryDataAsset`); no new runtime
JSON dispatcher or separate method ABI is introduced.

## Create an asset in the editor

First import the class Blueprint, then create a data-asset instance. Use the
editor console, outside PIE:

```text
Das.DSL.Apply dataasset_items.das /Game/Definitions/BP_ItemDefinition dataasset_items.ItemDefinition
Das.DSL.Asset.Create /Game/Definitions/BP_ItemDefinition /Game/Items/Potion
```

The second command is backed by `FDasDataAssetService::Create`. It requires a
compiled, nonabstract authored asset Blueprint and a new /Game package. It
copies the class defaults, registers the object with AssetRegistry and marks it
dirty. An existing disk or in-memory package is refused, never overwritten.
Save **both** the class Blueprint and the created asset through the editor before
using them across editor restarts or cooking. The creation command does not save
or change project configuration automatically. Per-instance edits are made in
the ordinary asset Details panel; for the sample, set Icon to a desired asset.

Ordinary DataAsset instances have no Primary Asset ID. PrimaryDataAsset instances
return `<type>:<instance name>`, for example `Item:Potion`. The explicit type is
validated and stored as a native serialized CDO/instance property; runtime ID
lookup does not depend on editor metadata. If omitted, the authored class name
is the type. Explicit parent type metadata is inherited by authored subclasses
unless overridden. Templates/CDOs do not identify instance Primary Assets.
`primary_asset_type` is rejected on other roots. Type names must be valid DSL
identifiers with at most 128 characters. Instance names come from UE asset names,
not another field or string ID in the DSL.

## Bundles

`AssetBundles` is field metadata, supporting several names separated by commas:

```das
var @ue_property="EditAnywhere" @AssetBundles="UI,Gameplay" Icon:SoftObject
```

It can also be shared through a property declaration group:

```das
presentation=ue_properties(EditAnywhere,BlueprintReadOnly,AssetBundles="UI") {
    var Icon:SoftObject
    var Preview:SoftObject
}
```

Metadata is emitted onto the reflected Blueprint property. Bundle names are
nonempty, at most 128 characters, composed of letters/digits/underscore/hyphen;
there may be at most 32 names, with no duplicates. Bundles are accepted on
PrimaryDataAsset properties. UE's `UpdateAssetBundleData` / PreSave collects the
soft references, and SavePackage serializes bundle registry data. Empty soft
references add no dependency. The adapter does not reinterpret hard object
references as soft dependencies. The existing property backend emits native soft properties for DSL soft references;
UE reads the metadata directly from those properties.

Editing a soft field requires saving the asset again to refresh its bundle tags.
The focused check covers a single SoftObject field, not every nested shape.

## AssetManager discovery and load

Configure the project's AssetManager scan settings for the type, for example:

- Primary Asset Type: Item
- Asset Base Class: DasPrimaryDataAsset (or the imported concrete class)
- Directories: /Game/Items
- Has Blueprint Classes: false (these are data instances, not Blueprint class assets)
- Is Editor Only: false for resources needed by the game

Use project-appropriate cook rules and include referenced bundles/scripts/classes.
Creating an asset does not silently add scan directories or force AlwaysCook.
See [Primary Assets](PrimaryAssets.md) for managed search/load/unload and ownership.

```das
let id=ue_primary_asset_id("Item","Potion")
// From a persistent Actor/widget entry:
// ue_primary_asset_load_async("potion",id,"on_loaded")
```

In the callback, cast the result to an ordinary authored reference view:

```das
[ue_object_ref(script="dataasset_items.das",type="ItemDefinition")]
struct ItemRef {handle:UObjectHandle}

[export]
def on_loaded(asset:UObjectHandle):void {
    if (!ue_object_valid(asset)) {return}
    let item=ue_ref(type<ItemRef>,asset)
    let total=item.ComputeValue(5)
    let icon_path=ue_soft_path(item.Icon)
}
```

## Reimport and focused check

Body-only reimport uses the existing route. Asset declaration/default/ID migration
is deliberately refused for these new roots; create a new class or leave the
definition unchanged when updating method bodies. No migration of existing user
asset instances or ID redirector generation is claimed.

`Scripts/TestAuthoredAssets.ps1` builds the Editor and runs one
`DasAuthoredAssets` commandlet scenario. It imports both roots, creates instances,
checks ID and duplicate protection, builds native bundle metadata, saves class
Blueprints/instances, scans the unique test directory, drops loaded instances,
then searches/async loads through DAS and calls a typed authored method after GC.
It unloads the resource and removes only its own temporary scripts and exact
unique test asset files. Source/DLL hashes and engine BuildId are recorded in
`logs/authored-assets-validation.json`. No full Foundation/PIE/packaged run or
cross-process/cooked asset-loading certification is part of this iteration.

Latest focused validation (2026-10-18): Editor Development build PASS,
16 checks in one commandlet scenario PASS, stable source/DLL hashes and engine
BuildId; no engine Error/ensure. No full Foundation/PIE/packaged run.

For read-only checks before cook/staging, see [Asset preflight](AssetPreflight.md).
The combined authored-assets scenario now passes 21 checks including preflight
controls; this does not certify a cooked or staged build.
