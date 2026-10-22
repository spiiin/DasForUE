# Structural changes for every entity kind

Plan for the v1 "reload matrix" gate ([Roadmap](Roadmap.md#v1--first-release-win64)): every declaration change
of every entity kind ends in a live migration, a controlled refusal with a stated next step, or a documented
restart. Drafted 2026-10-21 from an inventory of the current refusals.

## Where we are

Supported today (editor and PIE, through `FDasDSLService::Reimport` and auto-reimport):

- body-only changes (hot reload, no reimport);
- additive changes of Actor classes: new fields, methods, metadata; also composed actors with unchanged
  components and classes with soft reference fields;
- default values: merge with per-instance overrides (`PreserveOverrides`), deferred to the end of PIE;
- explicit migrations: `rename_by_id`, `rename_with_dependencies`, `widen_numeric`, `convert_numeric`,
  `archive_remove` / `archive_restore`, enum append and value remap, struct field drop/convert.

Everything else is refused. The refusals fall into three groups.

1. **Snapshot limits** (`FDasBlueprintRollback`). Rollback captures instance values through
   `ReadDasPropertyValue` and only on `ADasProbeActor` instances. That alone refuses containers, Pawn and
   Character, ActorComponent, SaveGame and DataAsset classes, and makes component templates and SCS
   untouchable.
2. **Missing transforms.** No code maps old values or graph references to the new shape: member removal and
   rename without an explicit policy, type changes beyond numeric, method signature changes, delegate and
   event fields, interfaces, parent class, component composition.
3. **Class-kind gates** in `FDasDSLService::Apply`: DataAsset (`:132`), ActorComponent (`:298`),
   Pawn/Character (`:404`), GAS schemas (`DasGASAssetService.cpp:227`), and the explicit migrations that
   refuse authored hierarchies, components and delegates (`:629-640`, `:1061-1072`, `:1578-1589`).

Group 1 blocks most of the others, so it comes first.

## Principles

- **One migration pipeline.** Compare old and new declaration -> a migration plan (a list of member changes
  with their transform) -> preflight (scope, PIE, idle runtime) -> snapshot -> apply -> reinstance -> remap
  values -> finish or roll back. The explicit migrations above become plan steps instead of separate entry
  points, so they also compose (rename + add in one save).
- **Transforms come from the type adapters** (`IDasTypeAdapter`, phase 2): an adapter that can convert a value
  between two declared types (numeric, enum by name, struct by field, container element-wise) offers that
  conversion; a change without a conversion is a controlled refusal that names the explicit policy to use.
- **Never lose data silently.** Removing a member archives its values (`RetiredDasValues`) by default instead
  of refusing; `archive_restore` brings them back. Destructive drops stay explicit.
- **PIE.** Changes that only add or rebuild run live (reinstancing). Changes that rewrite existing values
  (defaults, conversions) run on the editor world and are deferred for the play world until Play stops, as
  defaults already are.
- **Every cell has a test.** A data-driven matrix test (entity kind x change kind, editor and PIE) asserts the
  outcome and generates the reload matrix document (`Docs/Reference/ReloadMatrix.md`).

## Phases

Each phase ends with its matrix rows green, `foundation` + `automation-quick` + the new suites passing, and the
matrix document updated.

### S0. Matrix harness and generated matrix document

**Done 2026-10-21.** `DasForUE.ReloadMatrix.Editor` / `.PIE` (`DasReloadMatrix.cpp`, suite `reload-matrix`):
96 cells (11 field types x 6 field changes on an Actor; 9 field and method changes on Actor, composed
Actor, Pawn and Character) in the editor and during PIE, compared with `Tests/ReloadMatrix/expected.json`;
[Reload matrix](../../Reference/ReloadMatrix.md) is generated from it by `Scripts/RenderReloadMatrix.py`
(`--from-actual` takes the last run, `--check` for staleness). Baseline: 24 migrated, 72 refused, none
lossy or broken, editor and PIE alike. Next phases flip cells and update the expected file.

- Fixture generator: a base declaration per entity kind and a mutation per change kind (add, remove, rename,
  retype, default, metadata, editability) applied to a placed instance with an overridden value, in the
  editor and in PIE.
- Each cell asserts one of: migrated (values and overrides kept), refused (message names the policy, the
  Blueprint and instances unchanged), restart.
- Today's behaviour becomes the baseline; later phases flip cells from refused to migrated.
- New suite `reload-matrix`.

### S1. Generic snapshot and rollback

**Done 2026-10-21 (rollback layer).** `FDasBlueprintRollback` snapshots every instance of the class (any host:
Pawn, Character, components, the CDO), not only `ADasProbeActor`; values the owned-value codec does not cover
(containers, delegates) are kept as property text. The container rollback guard is gone; the container gate in
`Apply` stays until S2. Tagged serialization of whole objects was not needed for the current cells.

- Replace scalar capture with tagged property serialization of every instance of the class scope (CDO,
  placed and PIE instances, component templates and component instances, assets), the same mechanism UE
  reinstancing uses, plus object reference roots for GC.
- Drop the `ADasProbeActor` restriction; keep the scope limits (/Game, ordinary packages, 256 packages, no
  World Partition) as preflight refusals.
- Unblocks containers, Pawn/Character, ActorComponent, SaveGame, DataAsset and component templates in the
  rollback layer. Each kind's gate in `Apply` is removed only in its own phase.

### S2. Containers

**Gate removed 2026-10-21.** Classes with array/map/set fields migrate like other classes (rollback keeps
container values as property text); only explicit `PreserveInstances` default migration is refused for them.
Matrix: array/map/set `add_field` and `change_default` migrated. Still open (with S4): removing, renaming and
retyping container fields, element-wise conversion.

- Remove the container gates (`DasDSLService.cpp:228`, `DasBlueprintRollback.cpp:232`).
- Add/remove container fields; element type conversion element-wise through the adapter; map key conversion
  only when keys stay unique; struct-element arrays reuse struct field migration.
- Container defaults merge: replace when the instance value equals the old default, keep overrides.

### S3. Class kinds: Pawn/Character and ActorComponent

**Pawn/Character done 2026-10-21.** The Pawn/Character gate in `Apply` is removed; added fields and methods
migrate in the editor and during PIE (matrix cells flipped). A possessed Pawn keeps its controller across the
reinstance and a rollback (`FKeepPossession`, test `DasForUE.ReloadMatrix.PossessedCharacterPIE`).
ActorComponent classes remain.

- Pawn/Character: reinstancing during PIE keeps possession, controller, movement state and input bindings
  (check the playable example: WASD and HUD must keep working after a live field add).
- ActorComponent classes: reinstance component instances inside their actors and the SCS templates that use
  the class; the owning actors' composition stays as is.

### S4. Removal, rename and signatures

**Done 2026-10-21 (isolated Blueprints).** The automatic reimport dispatches a pure rename / removal /
numeric retype to the explicit migrations (exact numeric rules; archive and rename keep the controller of a
possessed Pawn). Archive works on Actor, Pawn and Character hosts; the explicit migrations no longer refuse
composed actors. Editability flags are applied. Removed or renamed methods drop their adapter graph,
changed signatures rebuild it; Blueprints that still call them fail the scope compile and roll back. Matrix:
92 migrated, 10 refused (non-numeric retypes). Still open: call-site fix-up in dependent Blueprints,
non-numeric conversions, dependency scopes in the automatic path, Callable <-> BlueprintEvent role changes.

- Member removal archives values automatically; the Blueprint graphs that reference the member are refused
  with a list of the nodes (or fixed up when the member was renamed).
- Field and method rename by stable id without an explicit policy when the id is unchanged.
- Method signature changes: add/remove/reorder parameters with defaults; call sites in Blueprints are
  reconstructed and broken pins reported.
- Callable <-> BlueprintEvent role changes through the same dependency fix-up.

### S5. Components and defaults during PIE

**Done 2026-10-21.** Component composition changes apply (add, remove, rename by component id, re-attach,
template defaults and metadata of changed components only); a changed component class or root is refused.
Rollback snapshots the SCS tree and serialized templates. Default merges include the running PIE world
(values still equal to the old default follow the new one). Matrix: 5 component cells migrated; 97
migrated, 10 refused.

- Component composition: add/remove components (new SCS nodes, removed nodes with their instance data
  archived), template default merge with per-instance overrides, rename by component id, attachment changes.
- Optional live default changes during PIE: update non-overridden values of play-world instances, using the
  editor instance as the reference for "overridden".

### S6. Delegates, events and interfaces

**Done 2026-10-21 (except interface removal and signature changes).** Classes with delegate/event fields
migrate like others (also through the explicit migrations); delegate/event fields can be added and removed
(variable and dispatcher graph); a changed delegate/event signature is refused. Implemented interfaces can
be added; removal is refused (it would cut links of nodes other Blueprints may use). Rollback restores
`ImplementedInterfaces` and removes nodes added to existing graphs (`DasForUE.ReloadMatrix.Rollback`).
Matrix: 109 migrated, 10 refused.

- Delegate/event fields: add/remove; signature change rebinds compatible handlers and drops incompatible ones
  with a report; bindings of live instances are restored after reinstancing.
- Interfaces: add/remove an implemented interface (Blueprint interface list, generated events); interface
  definition changes migrate implementers through S4 signature rules.

### S7. Hierarchy

**Done 2026-10-21 (except a native-root change).** A parent change keeping the native root reparents the
Blueprint inside the rollback snapshot (which now records `ParentClass`); same-named fields keep values.
Explicit migrations accept hierarchies (children are part of the rollback scope); only a parent change in
the same save is refused. Saving a parent script reimports its children too. Matrix kind `child`
(`parent_add_field`, `reparent`, field and method changes on a child of a DSL parent).
The runtime approval now carries the class with its authored ancestors (a child-only model failed
validation, so every structural reimport of a child was refused). Matrix: 120 migrated, 10 refused.

- Parent change between authored classes and to a native parent: reparent, migrate shared fields by id,
  archive fields that disappear.
- Authored hierarchies in every explicit migration (today refused at `:634`, `:1066`, `:1583`): scope covers
  the whole subtree.
- Replacing an inherited SCS component type stays refused (engine constraint).

### S8. Asset kinds

**S8a done 2026-10-21 (DataAsset, PrimaryDataAsset, SaveGame classes).** The DataAsset gate is gone; saved
DataAsset instances of the class family are part of the migration scope (`FDasBlueprintRollback::InstanceAsset`,
also in the explicit migrations), migrated and marked dirty. The automatic path passes "with dependencies" when
the only saved referencers are such instances. DataAsset, PrimaryDataAsset and SaveGame hosts got
`RetiredDasValues`, so removed fields are archived on them. Matrix kinds `data_asset`, `primary_data_asset`,
`save_game`: every field and method change migrates. SaveGame file versioning stays out (decision 2026-10-21).

**S8b done 2026-10-21 (DataTables).** `MigrateStructFields` takes DataTables whose row struct is the migrated
struct into scope (registry referencers and loaded tables), captures every row by field ID and restores it
after the struct compiles (and on rollback). A struct used only as rows needs no DSL Actor root.
Test `DasForUE.Types.StructDataTable` (rename + add, injected failure, drop with rule).

**S8c done 2026-10-22 (GAS).** `FDasGASAssetService::Apply` into the same folder rewrites the CDO of every
asset whose definition hash changed (same code as creation) and adds new attributes to AttributeSets;
removed/renamed attributes, cue tag and identity changes are refused. Snapshots (variables + CDO bytes)
restore every updated asset on failure. Assets remember `DasGASSource`/`DasGASFolder`; auto-reimport
updates them on save, on map open and before Play, deferring during Play. Test `DasForUE.Reload.GASInPlace`.

**Dependency scopes in the automatic path (2026-10-22).** `FDasDSLService::PlanReimport` computes the change
without applying it (added/removed/renamed/retyped fields, new defaults, saved maps and Blueprints outside the
class). Auto-reimport asks with an Apply / Details / Cancel notification when a field migration would modify
them, and applies with `IncludeDependencies`. New defaults in the same save go first
(`DefaultsBeforeMigration`), so a removal plus a default change no longer fails. Test
`DasForUE.Reload.ConfirmDependencies`.

- DataAsset / PrimaryDataAsset: instances are assets; migrate and re-save them in scope.
- SaveGame: no live instances to migrate; covered by the save-game schema item (v1) — versioned field ids
  and conversions applied on load.
- DataTables: row struct migration through struct field rules; rows re-imported in place.
- GAS (AttributeSet, Effect, Ability, MMC, Execution): replace "import into a new folder" with in-place update
  for additive changes; contract changes stay refusals.

## Order and size

| Phase | Unblocks for users | Size |
|---|---|---|
| S0 matrix | visibility of every outcome | M |
| S1 snapshot | prerequisite for S2-S8 | L |
| S2 containers | array/map/set fields in gameplay classes | M |
| S3 Pawn/Character, components | the playable character, component classes | M |
| S4 removal/rename/signatures | everyday refactoring | L |
| S5 components, PIE defaults | composed actors, tuning during Play | M |
| S6 delegates, interfaces | event-driven classes | L |
| S7 hierarchy | base-class refactoring | M |
| S8 assets | DataAsset, DataTable, GAS, SaveGame | L |

S0-S3 are what the playable example needs (the character and the pickup); S4 is the largest everyday win.
Networking (replicated fields, schema mismatch between clients) stays outside this plan.
