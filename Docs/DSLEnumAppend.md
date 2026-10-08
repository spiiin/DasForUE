# Append-only authored enum migration

Edit the type source by appending new members with explicit, unique values 0..254.
Existing names, numbers and order must stay unchanged. For example:

```das
[ue_enum(id="direct.State", module_id="direct", BlueprintType)]
enum PlayState {
    Idle = 7
    Active = 42
    Paused = 200
}
```

Then run in the editor console:

```text
Das.DSL.Types.ExtendEnums Script.das /Game/Folder/TypesRegistry.TypesRegistry
```

This explicit policy resolves the existing types through the catalog. It updates
the existing UDasDefinedEnum in place, keeping its UObject identity and stable ID.
Existing byte values and graph defaults keep their meanings; no object/class
reinstancing, Blueprint compilation or map data rewrite occurs inside the migration.
The generated MAX member stays reserved. Related stored type models are updated
together; affected type assets remain dirty until explicitly saved. The service
does not write source files or save assets automatically.

Preflight validates old enum values/order and stored models, unchanged struct
fields/defaults/IDs, and the catalog binding set. It discovers ordinary /Game
Blueprint, map and catalog dependencies within 256 packages, checks loaded
Blueprint graphs, and checks current reflected enum values (including arrays,
sets/maps, owned FInstancedStruct snapshots and FDasScalarValue enum carriers).
Unknown/reserved values are rejected before mutation. The updated DSL must compile
after the enum update and its class declarations must match the imported model.
Source type names may differ from native asset names when their stable IDs match.

The editor must be idle, PIE stopped and persistent contexts/subscriptions absent.
Compile dependent Blueprints first. Independent dependent DSL sources require a
later coordinated migration. Enum-specific nodes, including Switch on Enum,
ForEachEnum, literal/select/count nodes using enum-dependent interfaces, and
unsupported enum-bearing graph nodes are rejected: reconstructing them safely is
a later stage. World Partition/streaming maps and other dependency asset kinds
are outside this scope. Ordinary typed variables and the existing typed method
wrappers are supported.

On failure after mutation, the operation restores all old enum entries, enum
metadata, stored type models and original package dirty flags. Since graphs,
class layouts and values are untouched, no dependent graph/value rollback is
needed in this append-only slice. Failure injection covers both enum mutation and
model publication. Idempotent reapplication is supported.

This is the first type-definition migration, not full enum/struct hot reload.
Rename/removal/renumber/reorder, new type definitions, enum metadata changes,
struct changes, stable enum-member IDs and enum-node reconstruction remain later
policies. Test: Scripts/TestDSLTypeRegistry.ps1 -EnumAppend (add -Cooked to verify
the saved addition and its callback in a packaged game).


Extension: stable member IDs and optional `graphs=repair` for Switch on Enum are
now covered by DSLEnumMembersAndSwitch.md. The default policy retains the original
node refusal; the explicit repair policy compiles and snapshots its bounded scope.
