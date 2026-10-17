# Field retirement and restoration

Explicit reimport policies retain deleted field values for later restoration:

```text
Das.DSL.Reimport /Game/Folder/BP_Name fields=archive_remove_with_dependencies
Das.DSL.Reimport /Game/Folder/BP_Name fields=archive_restore_with_dependencies
```

For an isolated Blueprint use `archive_remove` or `archive_restore`. API:
`ReimportArchivedFields(Blueprint, Restore, IncludeDependencies, Error)`.

Remove property annotations and declarations from the source, then apply the removal
policy. All other declarations must stay unchanged, including methods. Remove direct
Blueprint graph references to the property first; the service refuses them before
mutation. Restoration requires the original declaration, including stable ID, name,
type, default, flags and metadata. A changed ID describes another field and cannot
retrieve the archive. Preserve explicit `ue_id` if a previous rename changed its name.

Each native Actor adapter stores values in the hidden persistent
`RetiredDasValues` UPROPERTY, keyed by class ID and field ID. Values are native
`FDasScalarValue` entries, including owned `FInstancedStruct` snapshots; they are
not runtime JSON descriptions. Root and derived CDOs retain their defaults, while
existing actors retain their own values. New actors created while fields are absent
inherit archived values from their CDO. Blueprint post-construction caches are
refreshed after archive defaults change.

The Blueprint package's editor metadata retains the original authoring declaration
and Unreal variable GUID. Restoration recreates the original property and GUID,
copies values into replacement objects and consumes the archive only after success.
Save changed Blueprint and map packages to persist the operation. Reimport marks
packages dirty; it does not save them or rewrite source files.

Supported fields: int32, float32, double, byte, bool, string, localized FText,
name, enum, typed object references, supported native reflected structs and
containers accepted by the shared type descriptor. Replicated/RepNotify properties are refused. Existing archive
entries cannot be overwritten. Missing values, incompatible kinds/struct snapshots,
changed restoration declarations and occupied destination names are refused.

Dependency discovery follows the existing bounded ordinary Blueprint/map policy:
up to 256 saved `/Game` packages, parent-first compilation and snapshots of loaded
instances, including PIE. World Partition, streaming maps, other asset types and
separately imported dependent DSL classes need additional migration support.

Post-compilation failure restores original variable descriptors, graphs, native
instance data, archive maps and prior package dirty flags when rollback succeeds.
Current model and archived declaration metadata commit only after validation.
This is bounded rollback of this operation, not a general editor transaction engine.
Idle method, lifecycle and managed delegate contexts survive success and rollback
through the shared preservation scope; see DSLLifecycleMigration.md. Call frames
remain invocation-local.

Example source: `Plugins/DasForUE/Content/Das/Scripts/dsl_retired_fields.das`.
Test commands: `Scripts/TestDSLArchiveFieldsAsset.ps1` and
`Scripts/TestDSLArchiveFieldsPIE.ps1`. See DSLFieldArchiveValidation.md for results.


Existing-Blueprint structural reimport and stable-ID rename now have bounded
rollback too; see DSLStructuralRollback.md. Expanded asset scopes and arbitrary
Unreal transactions remain separate work.


Typed UObject fields are now supported too. Native snapshots pin referenced
objects during migration and remap them through replacements; see DSLObjectFields.md.

Restoration now uses the common Blueprint pin adapter and the same default-literal
builder as ordinary class import. Container pins are native array/set/map pins,
while retained values use the existing owned carrier. Pin/default conversion
finishes before variables or instance data are changed. Parsed defaults remain
GC-rooted through the operation. Delegate/event classes, class reference fields,
authored class hierarchies and component migrations retain their existing guards.
