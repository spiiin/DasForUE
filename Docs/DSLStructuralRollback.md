# Bounded rollback for declaration reimport and field rename

Existing DSL Blueprint declaration reimport and stable-ID field rename now restore
their pre-operation state when a failure occurs after mutation. No additional
console policy is needed. Source files are never rewritten or restored by the
service: rejected source remains available for correction and retry.

The shared editor `FDasBlueprintRollback` captures:

- Blueprint variable descriptors, original GUIDs, function/ubergraph/delegate/macro
  graph lists and display/description/category values.
- Graph nodes, pins, defaults and links; variable/call member references and
  function entry metadata. Added method graphs are removed and marked transient.
- Complete Blueprint, graph, generated class and existing function metadata,
  including the stored DSL model and stable identities.
- Existing supported authored field values on root/derived CDOs and loaded actors,
  plus native retirement maps. Replacement-object notifications keep snapshots
  attached to current objects throughout failure and rollback reinstancing.
- Original package dirty flags. They restore only after complete rollback succeeds.

Rollback restores definitions and graphs, compiles parents before descendants,
restores original pins/links and values, reapplies metadata and refreshes Blueprint
post-construction caches. The shared runtime preservation scope keeps last-good
method/lifecycle state and managed subscriptions while both forward and rollback
replacements happen. Failed operations provide no new declaration approval.

Ordinary structural reimport now discovers the same bounded ordinary Blueprint/map
dependency scope: root plus loaded dependencies and saved `/Game` referencers, up
to 256 packages. Dependencies load before value snapshots, compile before success
and remain dirty for review/save. Independent dependent DSL sources, invalid scoped
Blueprints, unsupported asset types and partitioned/streaming maps are refused.
Existing rename dependency policy and isolated rename refusal remain unchanged.

Development-only fault tests exercise rename immediately after member mutation,
after compilation and after metadata/model update, and structural reimport after
compilation and metadata/model update. A successful rollback returns false with
`Structural migration rolled back:` or `Rename migration rolled back:`. An
incomplete rollback is reported explicitly; packages stay dirty for inspection.

This covers the declarations currently accepted by DSL reimport: additive fields
and methods, supported metadata/adapter updates, and explicit field rename. Existing
numeric and retirement operations keep their dedicated rollback implementations.
It is not an arbitrary Unreal transaction/undo engine: external construction-script
side effects, unrelated asset edits, native type/module reload and unsupported
migrations are outside this snapshot. First creation of a new imported Blueprint
is also outside the existing-Blueprint rollback contract.

Tests: TestDSLLifecycleMigrationPIE.ps1, TestDSLRenameScopeAsset.ps1 and
TestDSLStructuralRollbackAsset.ps1. See DSLStructuralRollbackValidation.md.


Typed UObject fields are now supported too. Native snapshots pin referenced
objects during migration and remap them through replacements; see DSLObjectFields.md.
