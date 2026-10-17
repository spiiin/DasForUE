# Soft reference parent conversions

Soft references widen along the native Unreal class hierarchy. The conversion preserves the asset path and does not resolve or load the asset. Null values obey the same type rules as non-null values.

```das
require ue_soft_upcast_types

let curve=ue_soft(type<SoftCurve>,"/Game/Curves/Speed.Speed")
let asset:SoftObject=curve

def use_asset(value:SoftObject):string { return ue_soft_path(value) }
let path=use_asset(curve)

def as_asset(value:SoftCurve):SoftObject { return value }

// Explicit widening can select the desired overload.
let sameAsset=ue_soft_upcast(type<SoftObject>,curve)

let child=ue_soft(type<SoftMeshActorClass>,"/Game/Actors/Enemy.Enemy_C")
let actor:SoftActorClass=child
let root:SoftObjectClass=actor
```

The aliases in `ue_soft_upcast_types.das` demonstrate `$UESoftObject<CurveAssetRef>` and `$UESoftClass<SoftMeshActorRef>`. User aliases for other native types use the same mechanism; no per-class conversion implementation is needed.

Supported implicit contexts: typed variable initialization, ordinary assignment, concrete function inputs and returns, generated native property setters and native method inputs. Existing exact overloads take priority; otherwise the nearest unique parent overload is selected. The source expression is evaluated once.

Only object-to-object and class-to-class widening is allowed. Downcasts, unrelated constraints and object/class mixing are rejected, including for null values. Runtime validation repeats the same constraint check to protect the low-level bridge. Source snapshots remain unchanged; failed native conversion leaves its output unchanged.

Mutable `ref/out` arguments remain invariant: create a parent-typed variable first, then pass that variable. Arrays and maps also remain invariant; convert their elements explicitly when building another container. No automatic downcast based on the currently resolved object is provided.

Focused validation: `Scripts/TestSoftUpcasts.ps1` builds the Editor unless `-SkipBuild` is specified, then runs only the new native/runtime/compiler probes. It checks loaded, unloaded and null references, class hierarchy chains, overload selection, exactly-once evaluation, generated native fields/methods, and eleven invalid source cases. It creates no saved map or Blueprint assets. This iteration intentionally does not run Foundation, PIE or packaged Development.

Validation (2026-10-17): Editor Development build and focused native/compiler/runtime commandlet passed, including eleven rejected source cases. No full Foundation, PIE or packaged run was requested for this iteration. Evidence: `logs/soft-upcasts-validation.json`.
