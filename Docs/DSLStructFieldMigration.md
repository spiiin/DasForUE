# Authored struct field migration

Preserve a field's stable ID when changing its name. New fields receive their own
automatic IDs and literal defaults:

```das
[ue_struct(id="direct.Settings", module_id="direct", BlueprintType)]
struct GameSettings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    @ue_id="direct.Settings.Count"
    Score : int = 15

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Enabled : bool = true

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title : string = "Initial"
}
```

Update source field accesses from Count to Score, then run:

```text
Das.DSL.Types.MigrateStructFields Script.das /Game/Registry.Registry
```

The policy handles one scalar-only authored struct at a time. Existing field IDs,
types, defaults and metadata must remain unchanged; their names may change and new
fields may be added. Removal requires explicit drop rules below. Numeric type changes require the separate conversion transaction below.
Independent default/metadata changes remain unsupported.
Other reflected type definitions and the owning class declarations must stay the
same. Source type names may differ from Unreal asset names through the catalog.

The native UUserDefinedStruct UObject and GUID stay stable, as do old field GUIDs.
Renamed fields retain the GUID suffix while changing their friendly/internal name;
the cooked friendly-name fallback therefore recognizes the new name. New fields
receive fresh GUIDs. The service uses a single structure compilation after editing
the validated variable descriptions, then repairs supported Make/Break Struct
pins and compiles the Blueprint/dependent classes.

Before compilation, existing values are copied into independent scalar text
snapshots keyed by stable ID. No old-layout FInstancedStruct snapshot is retained
for the changing type. After compilation, direct Actor/CDO struct fields start
from the new native defaults, then old values are restored by ID. Object replacement
is tracked; current CDOs are resolved through the owning Blueprint. Added fields
therefore get defaults while renamed fields retain instance overrides and strings.

The existing Blueprint graph/class/value rollback is used with changing struct
fields excluded; their independent snapshots handle restore after recompiling the
old definition. Four failure stages verify layout, graph links/results, Actor data,
stored models and package dirty flags. Successful assets/maps remain dirty until
explicitly saved. Source files are not rewritten and no automatic save occurs.

Scope: one imported DSL Actor root and ordinary dependent Blueprint/map/catalog
assets (256 packages), stopped PIE and no retained script contexts. Direct struct properties and scalar-leaf struct trees on its Actor/CDO instances
are supported. Container/unrelated state, retired snapshots of the changing type, split pins, Set Members/custom
struct-operation nodes, World Partition/streaming maps and nonempty text default
overrides are refused. Multi-root coordinated migration remains later work.

Test: Scripts/TestDSLTypeRegistry.ps1 -StructFields -Cooked. It renames Count to
Score, adds Enabled=true, preserves type/field identity, old scalar/Unicode/CDO
data and Make/Break behavior, and verifies rollback/reopen/cooked invocation.

## Explicit field removal

Remove the declaration and its source accesses, then supply a JSON file located
beside the daScript sources:

```json
{
  "schema_version": 1,
  "rules": [
    {"field_id": "direct.Settings.Enabled", "operation": "drop"}
  ]
}
```

```text
Das.DSL.Types.MigrateStructFields Script.das /Game/Registry.Registry Rules.json
```

`drop` deliberately discards that field's Actor/CDO data, including instance
overrides. It does not archive deleted data or supply a replacement field. Only
the deleted fields need explicit rules; surviving IDs retain their values/GUIDs.
The original add/rename command remains supported without a rules file.

A removed Make/Break Struct member must have no links, no orphan/split state,
and no overridden Make Struct input default. Literal defaults are compared to
the old native struct default using the property's parser and equality operation.
Actor/CDO overrides may be discarded, while graph logic/overrides require an
explicit source/graph edit before migration. The rules cannot waive these checks.

The JSON schema rejects unknown fields, duplicate IDs, unsupported operations,
wrong versions, unused/stale rules and absent rules for a removal. Limits are
64 KiB of decoded text and 128 rules. Rules use a local filename. After success,
remove the used rules from subsequent invocations; a repeat without rules is
idempotent. No source, Blueprint, registry or map is automatically saved.

Independent scalar snapshots retain the old removed data until the transaction
finishes, so all four injected failure stages restore the full original layout,
values, graph links/defaults and dirty flags. Forward compilation removes member
pins before Make/Break reconstruction to avoid keeping obsolete orphan pins.

Test: `Scripts/TestDSLTypeRegistry.ps1 -StructDrop -Cooked` first adds/renames
fields, then rejects missing/invalid rules, connected deletion and graph default
loss, exercises four rollback stages, drops Enabled with an Actor override,
and verifies remaining Score/Title/CDO/Make/Break data after save/reopen/cook.

Nested/container state and wider graph/scope support remain separate later work.

## Numeric type conversion

Change one existing field between int, float and double, preserving its stable ID
and name. A double literal uses daScript's `lf` suffix:

```das
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    @ue_id="direct.Settings.Count"
    Score : double = 15.0lf
```

Adapt source operations to the new type with explicit casts where daScript requires
them; keep the owning ue_class declarations and method signatures unchanged.
The declared new field default must equal the old default converted using the rule.
Use the same command with a local rules JSON file:

```json
{
  "schema_version": 1,
  "rules": [
    {
      "field_id": "direct.Settings.Count",
      "operation": "convert",
      "integer_rule": "Exact",
      "float_rule": "Exact"
    }
  ]
}
```

Integer rules: Exact, Truncate, Floor, Ceil, NearestAway. Float rules: Exact and
RoundNearest. Both must be explicitly declared. These are the existing checked
ConvertDasNumber policies: reject nonfinite values and overflow; Exact rejects
fractional int32 conversion and float32 precision loss. RoundNearest permits
float32 rounding; it does not permit float32 overflow. NearestAway rounds integer
ties away from zero. Widening to double preserves the source numeric value.

The first scope is one numeric field per transaction, with no simultaneous field
add/drop/rename, no metadata changes and no independent changes of defaults. Other
fields and types remain unchanged. The old struct/member GUIDs stay stable. New
defaults and all direct Actor/CDO values are checked before native mutation.
Numeric snapshots use round-trip precision rather than property display text;
forward converted snapshots are separate from originals used by rollback.

Make/Break member pins are selected explicitly in the shared numeric graph service.
Unconnected numeric Make defaults and their autogenerated defaults are converted
separately so a user literal such as 88 is retained when the new native default is
15. Connected pins retain their interfaces through checked UDasNumericLibrary
conversion nodes. Reference/container/wildcard/orphan/split conversions reject.
Adapters are installed before native compilation, then Make/Break reconstruction
and Blueprint compilation follow. Rollback removes added adapters and restores
the old pins, defaults, links, layout and data.

Repeated conversions can compose existing adapters. These preserve earlier graph
interfaces/rules rather than being optimized away. At later graph execution, a
value that violates an adapter's rule aborts that Blueprint execution through the
existing Unreal script-exception mechanism; it is not silently truncated. All
current values/defaults are checked upfront, but arbitrary future graph values
cannot be prevalidated.

After a successful transaction, omit used rules in subsequent idempotent commands;
stale convert rules reject. The service does not automatically save assets/source.
Run `Scripts/TestDSLTypeRegistry.ps1 -StructNumeric -Cooked` to exercise widening,
explicit narrowing/rounding, precision/nonfinite/overflow rejection, four-stage
rollback, saved map/CDO, native Make/Break execution and updated daScript callbacks.

Next: nested/container state and wider Blueprint node/scope support. Active PIE
type-definition reload and multi-root migration remain separate broader stages.

## Nested struct storage migration

Changing the authored scalar struct now also migrates copies stored inside existing
user struct wrappers on the same DSL Actor/CDO family. For example, a Bundle may
contain Left/Right envelopes, each with an Inner Settings value and unrelated
Label fields. Count -> Score, adding Enabled, dropping Enabled and numeric changes
of Score apply to both Inner values while their independent overrides persist.

This section describes storage migration. Nested `[ue_struct]` declarations
are now documented separately in DSLNestedTypeAuthoring.md. Wrappers can be existing Unreal user structs. The class exposes one
as the existing owned token type, with an implicit native default:

```das
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Bundle : UEStructValue = ue_struct_value("/Game/Types/Bundle.Bundle")
```

All leaves in a wrapper tree must satisfy ValidateDasValueStruct: no object,
container, fixed-array, opaque or recursive storage, maximum depth 16, 1024 leaves
and 1 MiB per struct. The changed authored type remains scalar-only. Existing
outer member names/types/IDs and structure GUIDs do not change in this transaction.

Snapshots flatten each tree into scalar text entries. Outer native member paths
identify unchanged wrapper fields; stable IDs identify fields inside the changing
type. Two occurrences have different paths, so their values cannot collide. Numeric
leaves use round-trip precision and converted values remain separate from originals.
Restoration starts each changing inner occurrence from its new native defaults,
then applies old values by ID; added members therefore receive the new defaults.
Unrelated sibling strings/numbers are restored as well.

Dependent wrapper native defaults are snapshotted independently before compilation,
restored and written back to variable descriptions after compilation. Wrapper
GUIDs and old descriptions are retained for rollback; scalar default text is
regenerated with precise numeric formatting when restoring native defaults. Changing-type and wrapper
values are excluded from the generic old-layout FInstancedStruct rollback snapshots.
Loaded user structs containing the type also seed the bounded package scope, since
Asset Registry references for newly saved packages can lag behind loaded assets.

Root CDO defaults must match the implicit native wrapper defaults before migration.
This follows the current DSL owner/default contract; mismatches reject before
mutation. Numeric wrapper defaults must also round-trip through Unreal editor's
property text export/import. Some high-precision defaults are normalized by that
serializer; such defaults reject instead of making later reimport inconsistent.
Existing instance numeric overrides use independent precise snapshots and are not
subject to this default-only restriction. A new non-round-tripping wrapper default
causes transaction rollback. No engine source modification is required.

Blueprint class default text emitted by the bridge now uses precise recursive
numeric export and delimited strings, avoiding its own loss of double precision.
This does not override Unreal's user-struct editor serialization behavior above.

Whole wrapper property pins without explicit defaults are allowed. Split pins,
Make/Break/Set/custom operations on wrappers themselves, explicit nested pin/default
overrides, containers and retired snapshots containing the changed type reject.
Make/Break operations on the changing scalar type retain the earlier repair policy.
Independent roots and active PIE remain outside this scope.

Tests: TestDSLTypeRegistry.ps1 -StructNested; add -StructNumeric or -StructDrop
to verify those policies across repeated nested occurrences. -StructNested
-StructNumeric -Cooked verifies the saved nested trees in an actual packaged map.

Nested declarations/pack/unpack are now supported: DSLNestedTypeAuthoring.md.
Next: container storage migration and split/custom graph repair.


## Array storage extension

Arrays of supported structs inside wrapper trees now preserve length, element
order and per-ID values through field migrations. This extends the earlier
scalar-only wrapper storage bounds; details and stricter array/default/graph limits
are in DSLStructArrayStorage.md. Map/set, direct Actor container fields and DSL
array declarations are still separate work.
