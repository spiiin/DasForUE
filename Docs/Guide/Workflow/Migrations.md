# Migrations

When a DSL declaration changes in a way that affects existing assets and saved data (new defaults,
renamed or removed fields, changed numeric types, changed enums or structs), each migration policy
below preflights the change, updates Blueprints, dependent assets and instance values, and rolls back on
failure.

The automatic reimport (saving the `.das` file, or `Das.DSL.Reimport <bp>` without an option) picks the
policy itself when one save makes only one kind of field change: defaults (`preserve_overrides`), renames
by stable ID (`rename_by_id`), removals (`archive_remove`) or numeric type changes (`convert_numeric`
with exact rules, so a value that the new type cannot hold refuses the change). Dependency scopes and
lossy conversions stay explicit. What each change does is listed in the
[reload matrix](../../Reference/ReloadMatrix.md).

## Policy overview

Class policies are options of `Das.DSL.Reimport <asset path>`. Type policies are separate
`Das.DSL.Types.*` commands that take the script and the type catalog (registry) asset.

| Change | Command / option | Keeps | Scope |
| --- | --- | --- | --- |
| Defaults only (automatic) | `Das.DSL.Reimport <bp>` or `defaults=preserve_overrides` | Manual overrides (values ≠ old default) | Blueprint, children, component templates, map instances |
| Defaults only, keep every instance | `defaults=preserve_instances` | Every existing instance and derived CDO value | This Blueprint's CDO |
| Strict defaults | `defaults=refuse` | — (refuses any default change) | — |
| Rename fields by stable ID | `fields=rename_by_id` | Values, overrides, variable GUIDs, graph references | Isolated Blueprint only |
| Rename with dependents | `fields=rename_with_dependencies` | Same, plus child Blueprints, other graphs, saved maps | Dependency scope |
| Exact numeric widening | `fields=widen_numeric` / `fields=widen_numeric_with_dependencies` | Exact values (int32→double, finite float→double) | Isolated / dependency scope |
| Numeric conversion with graph repair | `fields=convert_numeric` / `fields=convert_numeric_with_dependencies` + `integers=…` `floats=…` | Values converted by explicit rules; graph links via checked adapters | Isolated / dependency scope |
| Remove a field, archive its values | `fields=archive_remove` / `fields=archive_remove_with_dependencies` | Values stored in a hidden archive | Isolated / dependency scope |
| Restore an archived field | `fields=archive_restore` / `fields=archive_restore_with_dependencies` | Original property, GUID and values | Isolated / dependency scope |
| Append enum members | `Das.DSL.Types.ExtendEnums <script> <registry> [graphs=repair]` | Enum identity, byte values, graph defaults | Single DSL Actor root + dependencies |
| Rename enum members | `Das.DSL.Types.RenameEnumMembers <script> <registry>` | Member IDs and numbers; graph pins/links | Single DSL Actor root + dependencies |
| Renumber or remove enum members | `Das.DSL.Types.MigrateEnumValues <script> <registry> <rules.json>` | Values remapped by explicit rules | Scalar enum fields on the DSL Actor |
| Add/rename struct fields | `Das.DSL.Types.MigrateStructFields <script> <registry>` | Struct identity, field GUIDs, values by ID | Single DSL Actor root + dependencies |
| Drop or convert struct fields | `Das.DSL.Types.MigrateStructFields <script> <registry> <rules.json>` | Remaining values; converted values by rules | Same |

Shared rules for all policies:

- Edit the **existing** source file in place first; the services never rewrite source files.
- Each policy accepts only its own kind of change. Combine nothing else in the same operation
  (e.g. rename and default change are two separate operations).
- Preflight happens before any mutation: the candidate is compiled and the Blueprint is validated
  against its stored model.
- Changed packages are marked dirty and never saved automatically. Save the reported Blueprints,
  maps and type assets after review.
- Repeating a successful migration is idempotent.
- The canonical DSL model advances only after the whole operation succeeds.

### Dependency scope

The `_with_dependencies` policies, ordinary structural reimport, and the type commands use the same
bounded scope: the root Blueprint, loaded dependencies and derived classes, and saved `/Game`
referencers found through a completed Asset Registry scan, walked transitively (up to 256 packages;
defaults merge also limits staged objects to 8192). Dependents are loaded before values are captured,
and the parent compiles before children. Supported dependents are ordinary Blueprints and ordinary
(non-partitioned, non-streaming) maps with their placed actors. Package names are logged as
`DAS_RENAME_SCOPE`.

Refused before mutation: World Partition / external actors, streaming levels, non-`/Game` or
unsupported asset types, dependent Blueprints with their own separate DSL import source, Blueprints
that are not successfully compiled (compile pending edits first), and replicated/RepNotify
properties.

## Defaults

### Automatic merge (`defaults=preserve_overrides`)

Plain `Das.DSL.Reimport` selects this policy when only defaults changed. The previous DSL model is
the baseline. For each owned CDO property or declared component default:

- actual value equals the previous DSL default → the new default is applied;
- actual value differs → kept as a manual override;
- supported structs merge per reflected member (a quaternion is atomic);
- children, archetypes and editor instances compare with their own archetype's previous value and
  inherit the merged value where unchanged.

Example: `Score: 10 -> 20` updates an unchanged `10`, while an edited `15` stays `15`. This covers
inline initializers, `self.Property` entries in `ue_defaults`, Blueprint children, SCS component
templates, inheritable component override templates and placed editor instances. Component default
keys may be added or removed. Computed defaults and inherited property defaults resolve to the same
values. It is a value comparison, not an edit history: a manual value equal to the old default
follows the new default. Existing non-empty containers and delegate bindings are preserved.

The merge does not recompile, reinstance or rerun construction; object/class identity and idle VM
sessions stay. During PIE it updates the class and the editor world; actors of the running game keep
their current values (game state) and pick up the new default only when spawned again. It refuses:
layout, metadata, hierarchy, method signature or callback contract changes; unmanaged classes. For
component defaults, referencing maps must be
open in the editor (a map loaded only as an asset is refused — open it and retry). On failure, values,
CDO source contracts, Blueprint variable defaults, metadata and dirty flags are restored.

Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_defaults_merge.das`.

### Keep every instance (`defaults=preserve_instances`)

```text
Das.DSL.Reimport /Game/MyFolder/BP_Counter defaults=preserve_instances
```

Updates only this Blueprint's CDO and variable default descriptors, without recompiling. Every
existing instance keeps its value, even one equal to the old default; derived Blueprint CDOs keep
theirs. Changing `Count` from 10 to 11 leaves an existing `Count=10` actor at 10 and `Count=77` at
77; a newly spawned actor gets 11. Supported: int, float, bool, string, double, name and supported
reflected structs. Body-only changes may accompany it; declaration changes may not.

## Field rename

Keep the old stable ID while changing the name:

```das
// Before, in [ue_class(module_id="example")] class Counter
@ue_property=(EditAnywhere,BlueprintReadWrite)
Score : int = 5

// After: keep the former automatically generated ID explicitly
@ue_property=(EditAnywhere,BlueprintReadWrite)
@ue_id="example.Counter.Score"
Total : int = 5
```

Update `self.Score` to `self.Total` in method bodies, then:

```text
Das.DSL.Reimport /Game/MyFolder/BP_Counter fields=rename_by_id
Das.DSL.Reimport /Game/Folder/BP_Name fields=rename_with_dependencies
```

- Without `@ue_id`, a renamed field would get a new automatic ID; automatic ID changes are never
  guessed as renames. An already explicit ID stays.
- Only names may change: class/method identities, parameters, types, defaults, flags and metadata
  must stay the same. Several fields may be renamed at once to distinct free names.
- Refused: occupied destinations, swaps/cycles, case-only renames, replication/RepNotify.
  `rename_by_id` also refuses any dependent asset — use `rename_with_dependencies`.
- Unreal's `RenameMemberVariable` updates variable descriptors and graph references (local and in
  dependent Blueprints) and keeps variable GUIDs. Values of renamed fields are restored on the
  replacement objects, preserving CDO overrides, map actor overrides and untouched values.
- Updated methods can run against the new fields in the same PIE session.

Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_rename_fields.das`.

## Numeric type changes

### Exact widening

```text
Das.DSL.Reimport /Game/Folder/BP_Name fields=widen_numeric_with_dependencies
```

```das
// Before
Score : int = 5
Rate : float = 1.5
// After
Score : double = 5.0lf
Rate : double = 1.5lf
```

Supports int32 → double and finite float32 → double. The new default must equal the exact promotion
of the old one (an old float `0.1` is `0.10000000149011612`). Refused: int → float, narrowing,
non-numeric types, NaN/Inf instance values, and direct Blueprint property nodes on the converted field
(even disconnected). Method signatures can stay the same by converting in the body
(`double(self.Score)`). Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_widen_numeric.das`.

### Conversion with rules and graph repair

```text
Das.DSL.Reimport /Game/Folder/BP_Name fields=convert_numeric_with_dependencies integers=nearest_away floats=round
```

All directed conversions between int32, float32 and double. Both rules default to `exact`; duplicate
or unknown options are refused.

| Rule | Values | Meaning |
| --- | --- | --- |
| `integers=` | `exact` | Refuse fractions |
| | `truncate` / `floor` / `ceil` | Toward zero / down / up |
| | `nearest_away` | Nearest integer, ties away from zero |
| `floats=` | `exact` | Refuse any float32 precision loss |
| | `round` | Allow float rounding (including underflow to zero) |

NaN, Inf and out-of-range values always fail; there is no implicit clamping or wrapping. The new
default must equal the policy-converted old default.

Getter/setter pins update in parent and dependent graphs. Compatible links stay connected; between
different numeric types the migration inserts a checked `UDasNumericLibrary` adapter using the same
rules. These are lasting graph semantics: after changing `Score` from int to double, an integer graph
gets a double→int adapter; with `exact` a future fractional value aborts that Blueprint execution,
with `nearest_away` 2.5 becomes 3 and -2.5 becomes -3. Setter literals are converted too. Refused:
wildcard, container, split, orphan or by-reference links; non-scalar types.

## Field archive (remove and restore)

```text
Das.DSL.Reimport /Game/Folder/BP_Name fields=archive_remove_with_dependencies
Das.DSL.Reimport /Game/Folder/BP_Name fields=archive_restore_with_dependencies
```

**Remove:** delete the property declaration from the source (everything else unchanged), remove
direct Blueprint graph references to it, then run the removal policy. Values are stored in the hidden
persistent `RetiredDasValues` property of the actor adapter, keyed by class ID and field ID: CDOs keep
their defaults, actors their own values, and actors created while the field is absent inherit the
archived CDO value.

**Restore:** put back the original declaration exactly (stable ID, name, type, default, flags,
metadata); a different ID is a different field. The original property and variable GUID are
recreated, values copied back, and the archive entry consumed.

Supported field types: int32, float32, double, byte, bool, string, FText, name, enum, typed object
references, supported reflected structs and supported containers. Refused: replicated/RepNotify,
overwriting an existing archive entry, missing values, incompatible kinds, occupied names.
Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_retired_fields.das`.

## Enum migrations

Authored enums are migrated through their type catalog. All enum commands need an idle editor,
stopped PIE, no retained script contexts, compiled dependent Blueprints, and one imported DSL Actor
root with ordinary dependencies. Enum members have stable IDs (default `EnumStableId.MemberName`,
override with `member_id_<Name>="..."` on `[ue_enum]`).

**Append** members with explicit unique values 0..254 (existing names, numbers, order unchanged):

```das
[ue_enum(id="direct.State", module_id="direct", BlueprintType)]
enum PlayState {
    Idle = 7
    Active = 42
    Paused = 200
}
```

```text
Das.DSL.Types.ExtendEnums Script.das /Game/Registry.Registry
Das.DSL.Types.ExtendEnums Script.das /Game/Registry.Registry graphs=repair
```

The enum asset is updated in place; no reinstancing or data rewrite. Without `graphs=repair`, enum
nodes needing reconstruction are refused. With it, Switch on Enum, Enum Literal, Select, Get Num Enum
Entries, safe Byte to Enum and For Each Element in Enum nodes are reconstructed and compiled; new
options get Unreal's normal unconnected defaults (no logic is invented).

**Rename** members, keeping ID and number:

```das
[ue_enum(id="direct.State", module_id="direct", BlueprintType,
         member_id_Running="direct.State.Active")]
enum PlayState {
    Idle = 7
    Running = 42
    Paused = 200
}
```

```text
Das.DSL.Types.RenameEnumMembers Script.das /Game/Registry.Registry
```

Updates native names, enum defaults, Blueprint variable and pin defaults and Switch/Select option
names; links survive.

**Renumber or remove** members with a rules file beside the scripts:

```json
{
  "schema_version": 1,
  "rules": [
    { "enum_id": "direct.State", "from": 42, "to": 50 }
  ]
}
```

```text
Das.DSL.Types.MigrateEnumValues Script.das /Game/Registry.Registry Rules.json
```

Every changed or deleted value needs exactly one rule targeting a value in the new enum; unknown,
duplicate and unused rules are refused. Not combined with addition, rename or reorder. Removing a
connected Switch output or a Select option with a link/default override is refused. Value remapping
covers scalar typed enum fields on the DSL Actor, its instances and CDOs; container/nested enum
storage is refused.

## Struct migrations

```das
[ue_struct(id="direct.Settings", module_id="direct", BlueprintType)]
struct GameSettings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    @ue_id="direct.Settings.Count"
    Score : int = 15

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Enabled : bool = true
}
```

```text
Das.DSL.Types.MigrateStructFields Script.das /Game/Registry.Registry
Das.DSL.Types.MigrateStructFields Script.das /Game/Registry.Registry Rules.json
```

- **Add / rename** (no rules file): one authored struct at a time; existing IDs, types, defaults and
  metadata unchanged, names may change, new fields get their defaults. The struct asset and field
  GUIDs stay stable; Make/Break Struct pins are repaired.
- **Drop** fields: remove the declaration and accesses, and list them in the rules file:
  `{"field_id": "direct.Settings.Enabled", "operation": "drop"}`. Data is discarded, not archived.
  A removed Make/Break member must be unlinked with no overridden default.
- **Convert** one numeric field (int/float/double):
  `{"field_id": "...", "operation": "convert", "integer_rule": "Exact", "float_rule": "Exact"}`.
  Integer rules: `Exact`, `Truncate`, `Floor`, `Ceil`, `NearestAway`; float rules: `Exact`,
  `RoundNearest`. Connected pins get checked `UDasNumericLibrary` adapters.

Rules files: `schema_version` 1, local file name, at most 64 KiB and 128 rules; unknown fields,
duplicates, stale or missing rules are refused. Remove used rules for later runs.

Values are migrated in direct struct properties on the DSL Actor/CDO family, inside existing user
struct wrappers (nested occurrences each keep their own overrides), and inside arrays of structs in
wrappers (length, order and per-element values preserved; array defaults must be empty). Refused:
map/set or direct Actor container storage, retired snapshots of the changing type, split pins,
Set Members/custom struct nodes, active PIE, multiple independent roots.

## Rollback

Every policy restores its pre-operation state when a failure happens after mutation: variable
descriptors and GUIDs, graphs (nodes, pins, defaults, links; inserted adapters removed), generated
class metadata and stored DSL model, CDO and instance values, retirement archives, enum/struct
definitions, and package dirty flags. Successful rollback is reported (e.g.
`Rename migration rolled back:`); an incomplete rollback is reported explicitly and packages stay
dirty for inspection. Rollback is bounded to the operation's scope; it is not a general editor Undo,
and does not cover engine crashes, external construction-script side effects or the first creation of
a new Blueprint.

## Runtime state during migrations

Reimport, rename, numeric and archive migrations wrap Blueprint reinstancing in a runtime
preservation scope. It requires an idle runtime (no running VM, no pending events). Live sessions are
suspended, then remapped to replacement objects: owners, `self`, typed object handles and
subscription sources. Subscriptions are reinstalled with new IDs (old IDs cannot dispatch). Lifecycle
delivery flags stay — BeginPlay is not replayed, Tick resumes. Timers are cancelled by reinstancing
and not migrated. A successful migration approves exactly the applied declaration for the next
source validation; a rollback approves nothing, so rejected source cannot replace the last-good code.
Call frames remain local to one VM invocation.

Try it: `Scripts/TestDefaultsMerge.ps1`, `Scripts/TestDSLRenameScopePIE.ps1`,
`Scripts/TestDSLTypeRegistry.ps1 -StructFields`.

See also: [Apply and hot reload](ApplyAndHotReload.md) · [Defaults](../DSL/Defaults.md) ·
[Enums](../DSL/Enums.md) · [Structs](../DSL/Structs.md) · [Classes](../DSL/Classes.md)
