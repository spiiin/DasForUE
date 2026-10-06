# Field rename migration by stable identity

Explicit editor command:

```text
Das.DSL.Reimport /Game/MyFolder/BP_Counter fields=rename_by_id
```

API: `FDasDSLService::ReimportRenamedFields(Blueprint, Error)`.
Normal reimport still refuses a rename.

Edit the existing source file in place and retain the old property ID:

```dascript
// Before, in [ue_class(module_id="example")] class Counter
@ue_property=(EditAnywhere,BlueprintReadWrite)
Score : int = 5

// After: keep the former automatically generated ID explicitly
@ue_property=(EditAnywhere,BlueprintReadWrite)
@ue_id="example.Counter.Score"
Total : int = 5
```

Update `self.Score` usages to `self.Total` in method bodies. IDs normally remain
optional; keeping the old ID is required when a renamed property would otherwise
receive a new automatic ID. An already explicit ID stays unchanged.

This policy accepts only property name changes. Class/method identities,
parameter contracts/order, property type/default/flags/metadata must remain
unchanged. Multiple fields may rename to distinct vacant names in one operation.
Occupied destinations, swaps/cycles, case-only names and replication/RepNotify
are refused before mutation. Automatic ID changes are not guessed as renames.
Default migration must be a separate operation using its own explicit policy.

Preflight compiles the candidate source and validates the Blueprint against the
stored model. A comparison with names normalized to the old names ensures the
remaining declaration is unchanged. Loaded Blueprint dependencies/classes and
saved package referencers from a completed Asset Registry scan are checked.
This first policy requires an isolated Blueprint: dependent Blueprints and saved
referencing maps/assets need a future migration scope and are refused.

Unreal's RenameMemberVariable updates variable descriptors and local graph
references while retaining variable GUIDs. The class is compiled/reinstanced.
Native copies of renamed field values are captured before mutation and restored
onto replacements reported by OnObjectsReplaced, including the selected CDO,
untouched instances and overrides. Int/float/bool/string/double/name and supported
generic structs are covered by the value transport. No FProperty pointers are
retained across reinstance. Struct strings/nested values are owned copies.

The canonical import model advances after successful restore and validation.
Renamed fields/IDs/defaults/graph references persist through asset Save/Reopen;
repeat migration is idempotent. Updated script methods can run against the new
fields in the same PIE session. Assets remain dirty until explicitly saved.

Compiler/restore failures after mutation are reported; complete rollback/Undo for
the whole import remains unimplemented. This is not a project-wide offline
migration and does not rewrite source files or saved user maps automatically.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_rename_fields.das.
Tests: Scripts/TestDSLFieldRenameAsset.ps1 and TestDSLFieldRenamePIE.ps1.
