# Enum member rename by stable ID

Preserve the old member ID and numeric value while changing its source name:

```das
[ue_enum(id="direct.State", module_id="direct", BlueprintType,
         member_id_Running="direct.State.Active")]
enum PlayState {
    Idle = 7
    Running = 42
    Paused = 200
}
```

Update source references, including enum property defaults, to PlayState.Running.
Then run in the editor console:

```text
Das.DSL.Types.RenameEnumMembers Script.das /Game/Registry.Registry
```

The policy requires the same types/member count, member order, IDs and numbers.
Addition/removal/renumber/reorder, changed struct definitions and unrelated class
declaration changes are refused. A changed class enum default symbol is allowed
only when the rename map preserves the same numeric value. An old automatic ID
must be explicit under the new member name; omitting it derives a different ID and
is rejected. Reapplying an already migrated source is idempotent.

The enum UObject/catalog identity stays unchanged. The service updates native
member names, short/native-qualified enum defaults, Blueprint variable defaults,
enum-typed pin defaults and Switch/Select option names. Existing case/option pins
are renamed before native reload/reconstruction so their links/defaults survive.
Supported enum nodes are compiled and checked; stored type and class descriptions
are published together. Numeric CDO/Actor/map/container values are not remapped.
Blueprint compilation may clear NewVariables.DefaultValue; the persisted authoring
model carries the renamed default symbol and the CDO retains its numeric value.

Rollback restores enum names/member IDs, graph links/defaults/classes, Actor data,
stored type/class models and dirty flags. Failure injection covers enum mutation,
model publication, node reconstruction and compilation. Source files are never
rewritten by the service. Save changed type assets, Blueprint and affected maps
explicitly; no automatic save or disk-wide migration is performed.

Current scope: one imported DSL Actor root and its ordinary Blueprint/map/catalog
dependencies, within existing bounds. PIE must be stopped and retained script
contexts absent. Other independent DSL roots, unsupported graph nodes/assets and
World Partition/streaming maps require later coordinated policies.

Test: Scripts/TestDSLTypeRegistry.ps1 -EnumAppend -EnumGraphs -EnumNodes -EnumRename
(add -Cooked). It migrates Active -> Running with value 42 / ID direct.State.Active,
checks class and function-input defaults, all enum-node results, rollback,
save/reopen and an actual packaged game. See DSLEnumRenameValidation.md.
