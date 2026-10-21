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

- Fixture generator: a base declaration per entity kind and a mutation per change kind (add, remove, rename,
  retype, default, metadata, editability) applied to a placed instance with an overridden value, in the
  editor and in PIE.
- Each cell asserts one of: migrated (values and overrides kept), refused (message names the policy, the
  Blueprint and instances unchanged), restart.
- Today's behaviour becomes the baseline; later phases flip cells from refused to migrated.
- New suite `reload-matrix`.

### S1. Generic snapshot and rollback

- Replace scalar capture with tagged property serialization of every instance of the class scope (CDO,
  placed and PIE instances, component templates and component instances, assets), the same mechanism UE
  reinstancing uses, plus object reference roots for GC.
- Drop the `ADasProbeActor` restriction; keep the scope limits (/Game, ordinary packages, 256 packages, no
  World Partition) as preflight refusals.
- Unblocks containers, Pawn/Character, ActorComponent, SaveGame, DataAsset and component templates in the
  rollback layer. Each kind's gate in `Apply` is removed only in its own phase.

### S2. Containers

- Remove the container gates (`DasDSLService.cpp:228`, `DasBlueprintRollback.cpp:232`).
- Add/remove container fields; element type conversion element-wise through the adapter; map key conversion
  only when keys stay unique; struct-element arrays reuse struct field migration.
- Container defaults merge: replace when the instance value equals the old default, keep overrides.

### S3. Class kinds: Pawn/Character and ActorComponent

- Pawn/Character: reinstancing during PIE keeps possession, controller, movement state and input bindings
  (check the playable example: WASD and HUD must keep working after a live field add).
- ActorComponent classes: reinstance component instances inside their actors and the SCS templates that use
  the class; the owning actors' composition stays as is.

### S4. Removal, rename and signatures

- Member removal archives values automatically; the Blueprint graphs that reference the member are refused
  with a list of the nodes (or fixed up when the member was renamed).
- Field and method rename by stable id without an explicit policy when the id is unchanged.
- Method signature changes: add/remove/reorder parameters with defaults; call sites in Blueprints are
  reconstructed and broken pins reported.
- Callable <-> BlueprintEvent role changes through the same dependency fix-up.

### S5. Components and defaults during PIE

- Component composition: add/remove components (new SCS nodes, removed nodes with their instance data
  archived), template default merge with per-instance overrides, rename by component id, attachment changes.
- Optional live default changes during PIE: update non-overridden values of play-world instances, using the
  editor instance as the reference for "overridden".

### S6. Delegates, events and interfaces

- Delegate/event fields: add/remove; signature change rebinds compatible handlers and drops incompatible ones
  with a report; bindings of live instances are restored after reinstancing.
- Interfaces: add/remove an implemented interface (Blueprint interface list, generated events); interface
  definition changes migrate implementers through S4 signature rules.

### S7. Hierarchy

- Parent change between authored classes and to a native parent: reparent, migrate shared fields by id,
  archive fields that disappear.
- Authored hierarchies in every explicit migration (today refused at `:634`, `:1066`, `:1583`): scope covers
  the whole subtree.
- Replacing an inherited SCS component type stays refused (engine constraint).

### S8. Asset kinds

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
