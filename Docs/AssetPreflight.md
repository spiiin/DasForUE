# Authored asset cook/staging preflight

The Editor-only preflight inspects an authored DataAsset / PrimaryDataAsset.
It diagnoses the saved class/source/registry prerequisites before an expensive
cook. It does not save assets, rescan directories, change cook rules or start
asset loads through AssetManager. Normal UE object/Blueprint/source loading and
transient compiler allocations may occur during inspection.

## Run

In the editor console:

```text
Das.DSL.Asset.Check /Game/Items/Potion
```

For CI, run against the project configuration in a fresh Editor commandlet:

```text
UnrealEditor-Cmd.exe Project.uproject -run=DasAssetCheck -Asset=/Game/Items/Potion -Report=C:/BuildReports/Potion.json -unattended -NullRHI
```

The report's parent directory must exist. Exit codes: 0 if no error diagnostics
(warnings are permitted), 1 for failed preflight, 2 for missing arguments or
failure to write the requested report. No report file is written unless requested.
The native service is `FDasAssetPreflight::Inspect`; reports expose HasErrors,
HasIssue and ToJson.

## Scope

- The instance and each authored Blueprint in its class hierarchy must have a
  saved package without unsaved changes.
- Generated classes must be current, with saved authored identity/model metadata.
- The source must compile; its declaration must match the saved Blueprint model
  and reflected schema. AssetBundles metadata must match the source declaration.
- Saved class sources and method-entry scripts must exist under the project or
  plugin Content/Das/Scripts directories used by the standard DasRuntime.Build.cs
  UFS staging rules. The project override selected by Runtime.ScriptPath is used.
- Primary IDs and serialized class/instance primary types must agree. The current
  AssetManager must register the exact instance path under its ID, using runtime
  data-instance scan settings, not editor-only or Blueprint-class settings.
- Native bundle collection is compared with the current AssetManager bundle
  entries. Every referenced bundle package must exist on disk.
- NeverCook is an error. Rules without an AlwaysCook guarantee produce a warning:
  a map, label, manager or target cook can still legitimately include the resource.
  Ordinary DataAssets also receive an inclusion warning because their cook roots
  must come from the project.

Important codes include asset_not_saved, asset_unsaved_changes, class_not_saved,
class_unsaved_changes, class_stale, authored_identity_mismatch, script_missing,
script_outside_staging_roots, script_compile_failed, class_declaration_stale,
class_schema_mismatch, class_bundle_metadata_mismatch, primary_type_mismatch,
primary_instance_type_mismatch, primary_not_registered,
primary_type_wrong_scan_mode, bundle_package_missing, bundle_registry_stale,
asset_never_cook and cook_inclusion_unproven.

Each diagnostic has severity, code, resource and a suggested action. For example:

```json
{
  "passed_preflight": false,
  "packaged_verified": false,
  "issues": [
    {"severity":"error", "code":"script_missing", "resource":".../item.das", "message":"Restore the DAS file used by the saved class"}
  ]
}
```

## Boundaries

A successful preflight is **not** a cook or packaged-game certificate.
`packaged_verified` is always false. The JSON staging_scope describes only saved
class sources and method entries in standard UFS roots. It does not inspect an
actual build receipt/Pak, every arbitrary transitive DAS import, custom build
rules, platform chunk installation or all dependency cook policies.

Bundle comparison uses the live object and current AssetManager registry state.
UE PostLoad can refresh in-memory bundle data while loading the requested object;
this command does not prove that an already-produced cooked registry has those
tags. For an authoritative delivery check, use a fresh cook/stage and packaged
runtime against the intended project/platform/target configuration.

The preflight does not repair anything automatically. Save assets/classes,
resolve source declaration differences, configure AssetManager scan settings or
fix cook roots explicitly. No user assets or project settings are rewritten.

## Focused validation

The existing `Scripts/TestAuthoredAssets.ps1` runs one combined commandlet
scenario. It now checks a saved/discovered authored asset's preflight plus
controlled dirty-instance, missing-source metadata, stale-bundle and NeverCook
cases on its own temporary fixtures, then continues the real async load/method/
GC/unload scenario. No extra PIE/packaged or full Foundation run is added.
The final source/DLL hashes and engine BuildId are recorded in
`logs/authored-assets-validation.json`.

Accepted validation on 2026-10-18: Editor Development build PASS and one
combined runtime scenario PASS (21 checks), with stable source/DLL hashes and
engine BuildId. PIE and packaged runs were not performed.
