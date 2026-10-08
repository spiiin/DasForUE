# Enum member identities and Switch repair

Enum members now have stable IDs. By default, the ID is `EnumStableId.MemberName`.
Explicit IDs are optional and use an argument on the enum annotation:

```das
[ue_enum(module_id="memberids", BlueprintType,
         member_id_Active="memberids.State.running")]
enum State {
    Idle = 7
    Active = 42
}
```

The pinned daScript enum entry AST/grammar has no per-entry annotation list, so
this uses ordinary enum annotation arguments without reader macros or parser
changes. Unknown member keys, non-string/invalid IDs and duplicate IDs are rejected.
Example: Plugins/DasForUE/Content/Das/Scripts/dsl_enum_member_ids.das.

The reflected type transport writes schema_version=2 with entry id/name/value.
Version 1 is still readable and derives IDs automatically. Type import compares
canonical models so an unchanged old v1 description can be reapplied. Newly created
UDasDefinedEnum assets serialize their numeric-value-to-ID map as binary UPROPERTY
data; successful append migrations update it atomically. Direct type compilation
checks the map when present. Legacy assets without a map remain readable; the
first successful append persists it. Existing member ID changes are rejected.

Append migration can now explicitly repair Switch on Enum graphs:

```text
Das.DSL.Types.ExtendEnums Script.das /Game/Registry.Registry graphs=repair
```

The default policy still rejects nodes requiring reconstruction. The repair policy
refreshes each supported Switch's enum entries, reconstructs its pins and compiles
the Blueprint plus ordinary dependent Blueprint classes. Existing links are
verified after compilation; appended members receive new unconnected execution
outputs. The same enum UObject, member numbers and old data remain intact.

Scope is deliberately bounded to one imported DSL Actor root containing the
affected Switch nodes and its ordinary Blueprint/map dependencies. Multiple
independent roots, unrelated DSL sources, other enum-dependent nodes (Select,
ForEachEnum, enum literal/count/cast nodes), World Partition/streaming and unsupported
asset kinds are rejected. Existing scope limits remain in DSLEnumAppend.md.
PIE must be stopped and the runtime must have no retained script contexts.

The rollback snapshot covers graphs, pin links/defaults, class metadata, Actor
fields/retired values and package dirty flags. After restoring old enum entries
and member IDs, Switch pins are rebuilt for the old definition before graph/class
restore. Failure injection tests both reconstruction and compilation. Successful
repair marks Blueprint/map dependencies dirty; save the enum/types, Blueprint and
affected maps explicitly. Source files are never rewritten by the service.

This does not yet enable member rename/removal/renumber or struct migration.
Stable member IDs provide the identity needed for later rename policies.
Test: Scripts/TestDSLTypeRegistry.ps1 -EnumAppend -EnumGraphs -Cooked.


The supported enum-node family has since expanded to literal/select/count/safe
cast/iteration; current bounds and semantics are in DSLEnumNodeRepair.md.
