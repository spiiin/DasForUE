# Data Assets

You can declare `DataAsset` and `PrimaryDataAsset` classes in the DSL, create asset instances from them in the editor, register Primary Assets with bundles for the AssetManager, and check an asset with a preflight before cooking.

## Declaring a data asset class

`DataAsset` and `PrimaryDataAsset` are native DSL roots, like `SaveGame`. Fields and defaults become reflected Blueprint properties; a data-only class needs no methods.

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
        // Loaded with the UI bundle; an empty reference adds no dependency.
        var @AssetBundles="UI" Icon:SoftObject
    }
    [ue_function(BlueprintPure)]
    def ComputeValue(amount:int):int {return self.Score+amount}
}

// Ordinary data assets use the same field DSL; methods are optional.
[ue_class(Blueprintable)]
class TuningDefinition : DataAsset {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Strength:double=1.5lf
    }
}
```

Example: `Plugins/DasForUE/Content/Das/Scripts/dataasset_items.das`.

- Methods can read/write fields and call supported methods. A data asset has no game world, so world-bound services (traces, spawning, timers) must be called from an Actor or widget.
- Actor lifecycle hooks, components and `ue_async_physics` are rejected on these roots.
- Normal authored inheritance and reference rules apply. Native hosts are `UDasDataAsset` and `UDasPrimaryDataAsset`.

## Primary Asset ID

Ordinary `DataAsset` instances have no Primary Asset ID. `PrimaryDataAsset` instances report `<type>:<instance name>`, for example `Item:Potion`.

- `primary_asset_type="..."` sets the type; without it, the authored class name is the type. It is a valid DSL identifier of at most 128 characters, and only allowed on `PrimaryDataAsset` classes.
- The type is stored in a serialized native property, so runtime ID lookup does not depend on editor metadata. Authored subclasses inherit an explicit parent type unless they override it.
- The instance name is the UE asset name; there is no separate string ID field. Templates/CDOs do not count as Primary Assets.

## Asset bundles

`AssetBundles` is field metadata listing one or more comma-separated bundle names:

```das
var @ue_property="EditAnywhere" @AssetBundles="UI,Gameplay" Icon:SoftObject
```

or shared by a property group:

```das
presentation=ue_properties(EditAnywhere,BlueprintReadOnly,AssetBundles="UI") {
    var Icon:SoftObject
    var Preview:SoftObject
}
```

- Allowed on `PrimaryDataAsset` properties. Names are non-empty, at most 128 characters, letters/digits/underscore/hyphen; up to 32 names, no duplicates.
- UE's `UpdateAssetBundleData`/PreSave collects the soft references and the saved package carries bundle registry data. Empty soft references add nothing; hard object references are not treated as soft dependencies.
- After editing a soft field, save the asset again to refresh its bundle tags.

## Creating instances in the editor

Import the class first, then create an instance (editor console, outside PIE):

```text
Das.DSL.Apply dataasset_items.das /Game/Definitions/BP_ItemDefinition dataasset_items.ItemDefinition
Das.DSL.Asset.Create /Game/Definitions/BP_ItemDefinition /Game/Items/Potion
```

`Das.DSL.Asset.Create` needs a compiled, non-abstract authored asset Blueprint and a **new** `/Game` package. It copies class defaults, registers the object with the AssetRegistry and marks it dirty. An existing package (on disk or in memory) is refused, never overwritten. The command does not save anything or change project settings: save **both** the class Blueprint and the new asset yourself. Edit per-instance values in the normal Details panel (for the sample, set `Icon`).

## AssetManager setup

Configure the project's AssetManager scan settings for the type, for example:

| Setting | Value |
| --- | --- |
| Primary Asset Type | `Item` |
| Asset Base Class | `DasPrimaryDataAsset` (or the imported concrete class) |
| Directories | `/Game/Items` |
| Has Blueprint Classes | `false` (these are data instances, not Blueprint class assets) |
| Is Editor Only | `false` for resources the game needs |

Creating an asset never adds scan directories or forces AlwaysCook; set cook rules that include the assets, their bundles, scripts and classes.

## Loading and using an instance

Load through the Primary Asset API (see [Asset loading](AssetLoading.md)), then view the result through an authored reference type:

```das
[ue_object_ref(script="dataasset_items.das",type="ItemDefinition")]
struct ItemRef {handle:UObjectHandle}

// From a persistent Actor/widget entry:
// ue_primary_asset_load_async("potion",ue_primary_asset_id("Item","Potion"),"on_loaded")

[export]
def on_loaded(asset:UObjectHandle):void {
    if(!ue_object_valid(asset)) {return}
    let item=ue_ref(type<ItemRef>,asset)
    let total=item.ComputeValue(5)
    let icon_path=ue_soft_path(item.Icon)
}
```

## Changing the class

Body-only reimport (method implementations) works as usual. Changes to declarations, defaults or the Primary Asset type of these roots are refused before any defaults migration: create a new class, or keep the declaration unchanged while editing method bodies. Existing instances are not migrated and no ID redirectors are generated.

## Preflight before cooking

`Das.DSL.Asset.Check` inspects an authored asset and reports problems that would break it in a cooked build. It never saves assets, rescans, changes cook rules or loads through the AssetManager (ordinary object/Blueprint/source loading can happen).

```text
Das.DSL.Asset.Check /Game/Items/Potion
```

For CI, run the commandlet in a fresh editor process:

```text
UnrealEditor-Cmd.exe Project.uproject -run=DasAssetCheck -Asset=/Game/Items/Potion -Report=C:/BuildReports/Potion.json -unattended -NullRHI
```

Exit codes: `0` no errors (warnings allowed), `1` preflight failed, `2` missing arguments or report could not be written. The report's parent directory must exist; no report is written unless requested. Native API: `FDasAssetPreflight::Inspect` (results expose `HasErrors`, `HasIssue`, `ToJson`).

What it checks:

- The instance and every authored Blueprint in its class hierarchy are saved with no unsaved changes; generated classes are current with saved authored identity/model.
- The source compiles and its declaration matches the saved Blueprint model, reflected schema and AssetBundles metadata.
- Class sources and method scripts exist under the project or plugin `Content/Das/Scripts` directories staged by the standard build rules (respecting the `Runtime.ScriptPath` override).
- The Primary ID and serialized class/instance types agree, and the AssetManager registers this exact instance path under its ID with runtime data-instance scan settings.
- Bundle data matches the AssetManager entries, and every referenced bundle package exists.
- `NeverCook` is an error. Rules without an AlwaysCook guarantee produce a warning (a map, label or target cook may still include it legitimately). Plain DataAssets always get an inclusion warning because their cook roots come from the project.

Diagnostic codes: `asset_not_saved`, `asset_unsaved_changes`, `class_not_saved`, `class_unsaved_changes`, `class_stale`, `authored_identity_mismatch`, `script_missing`, `script_outside_staging_roots`, `script_compile_failed`, `class_declaration_stale`, `class_schema_mismatch`, `class_bundle_metadata_mismatch`, `primary_type_mismatch`, `primary_instance_type_mismatch`, `primary_not_registered`, `primary_type_wrong_scan_mode`, `bundle_package_missing`, `bundle_registry_stale`, `asset_never_cook`, `cook_inclusion_unproven`.

```json
{
  "passed_preflight": false,
  "packaged_verified": false,
  "issues": [
    {"severity":"error", "code":"script_missing", "resource":".../item.das", "message":"Restore the DAS file used by the saved class"}
  ]
}
```

Each issue has severity, code, resource and a suggested action. A passing preflight is **not** a cook certificate: `packaged_verified` is always `false`. It does not inspect build receipts/Paks, every transitive DAS import, custom build rules, chunking or all dependency cook policies, and it repairs nothing. For an authoritative check, cook, stage and run the packaged game.

See also: [Asset loading](AssetLoading.md), [Soft references](../DSL/SoftReferences.md), [Classes](../DSL/Classes.md), [Apply and hot reload](../Workflow/ApplyAndHotReload.md), [Migrations](../Workflow/Migrations.md)
