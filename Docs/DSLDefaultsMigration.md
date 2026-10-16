# Safe defaults reimport

`FDasDSLService::Reimport(Blueprint, Error)` uses `Automatic` policy. When the
saved declaration and the new declaration differ only in defaults, it selects
`PreserveOverrides`. An unchanged declaration with a matching CDO keeps the existing body-only path, including PIE; an edited CDO uses the staged merge. Other changes follow the existing `Refuse` import path and
its migration checks. Explicit `Refuse` and `PreserveInstances` retain their old
meaning; explicit `PreserveOverrides` refuses any layout, metadata, hierarchy,
method signature or callback contract change.

In the editor console:

```text
Das.DSL.Reimport /Game/Folder/BP_Name
Das.DSL.Reimport /Game/Folder/BP_Name defaults=preserve_overrides
```

The first command chooses the safe merge automatically for defaults. The second
requires a default-only declaration. `defaults=refuse` selects the old strict
policy; `defaults=preserve_instances` keeps every existing instance value.

## Value rules

The previous DSL model in `DasDSLModel` is the source baseline. For each owned
CDO property or declared component default:

* actual value equals previous DSL default: apply the new default;
* actual value differs: preserve it as a manual override;
* supported structs merge per reflected member. A quaternion is atomic;
* children, archetypes and editor instances compare with the previous actual
  value of their own archetype and inherit its merged value where unchanged.

For example, `Score: 10 -> 20` updates an unchanged `10`, while an edited `15`
stays `15`. This covers inline initializers and `self.Property` entries in
`ue_defaults`, ordinary Blueprint children, SCS component templates, existing
inheritable component override templates, and placed editor instances. Component
default keys can be added or removed: an absent key uses the component class CDO
as its baseline. Other component templates and their class CDO are independent.
Supported values are the same static scalars, enums, math/reflected structs and
strong asset/class references accepted by the defaults importer. New components,
new inherited component-template override entries and executable/computed defaults
are separate work. Adding/removing inherited authored property defaults is supported;
see [DSLInheritedPropertyDefaults.md](DSLInheritedPropertyDefaults.md).

This is a value comparison, not an edit-history tracker. A manually entered value
equal to the old default follows the new default. Existing nonempty containers
and delegate bindings are preserved rather than interpreted as source defaults.
This does not add container initializer syntax.

## Safety and persistence

The editor validates the unchanged reflected layout and stages old/actual/new
values before writing. It discovers ordinary `/Game` Blueprint/map referencers
using AssetRegistry and pins that scope. The scope is bounded to 256 packages and
8192 staged objects. Dependent Blueprints must already be compiled. Unmanaged
classes, unsupported referencing assets, World Partition/streaming worlds and
affected live PIE/game instances cause refusal before property writes.

For component changes, referencing editor maps must be open/initialized. A map
loaded only as an asset may still have pre-construction component data; the
preflight refuses that state instead of running user construction scripts. Open
the named map normally before retrying. The same guard currently applies to a
safe reimport of a class containing component defaults even when those defaults
have not changed. An explicit body-only `Refuse` reimport remains available under
its existing CDO validation rules.

The merge does not compile or reinstance Blueprints or rerun construction. It
preserves object/class identity and keeps idle persistent VM sessions attached.
Active calls and queued events block the update. A later runtime reload accepts
the approved default-only declaration; structural changes remain guarded.

Snapshots cover reflected values, CDO source contracts, Blueprint variable
defaults, import metadata and package dirty flags. A value-stage or metadata-stage
failure restores these snapshots. Native template edit notifications and the
StaticMesh setter keep native caches consistent without reconstructing actors.
This is not a sandbox for arbitrary side effects in custom native editor hooks.

Merged CDO values also update `FBPVariableDescription::DefaultValue`, so a later
Blueprint compile keeps manual CDO edits. Cooked source-default fingerprints on
the declaring CDO let runtime validation distinguish an approved editor override
from an unapplied source default change. These fingerprints are not JSON IR.

Changed packages are marked dirty; the importer does not save user assets
automatically. Save affected Blueprints/maps after reviewing the merge. This path
provides failure rollback; it does not add a coordinated editor Undo transaction.

## Verification

```powershell
.\Scripts\TestDefaultsMerge.ps1
.\Scripts\TestDefaultsMerge.ps1 -Package
```

The fixture starts with old defaults, makes independent CDO, child Blueprint,
inherited component template and map instance overrides, then reimports. Checks
cover member-wise struct merging, class/asset references, sibling component
isolation, two injected rollback stages, unchanged class/context identity,
reflected invocation through an existing VM, a repeated import, fresh spawn,
save/reopen, Blueprint recompile, live-game and layout refusal, and refusal of
uninitialized component maps. The optional Win64 Development NullRHI packaged
probe checks saved overrides, fresh spawn and cooked source-contract validation.

Example declaration: `Plugins/DasForUE/Content/Das/Scripts/dsl_defaults_merge.das`.

Verified 2026-10-16: Editor build; Create/Reopen/Recompile/Unopened commandlet phases; automatic and explicit console reimport; two rollback injection stages; Editor and packaged foundation (zero failures); Win64 Development NullRHI packaged defaults probe without errors/ensures; legacy PreserveInstances Create/Reopen and real DefaultMigrationPIE (one success, zero warnings/failures).
