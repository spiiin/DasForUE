# Rename migration with dependent Blueprints and maps

```text
Das.DSL.Reimport /Game/Folder/BP_Name fields=rename_with_dependencies
```

API: `FDasDSLService::ReimportRenamedFields(Blueprint, Error, true)`.
The isolated `fields=rename_by_id` policy keeps its previous dependency refusal.
The original stable IDs must still be retained in source via `@ue_id`; only names
may change. Types/defaults/flags and method contracts stay unchanged.

The editor completes the Asset Registry scan, walks saved package referencers
transitively and loads the dependent assets **before** capturing native values.
Already loaded dependents and derived classes seed the same walk, including saved
maps referencing those packages. Scope is bounded to 256 queued packages and
saved dependencies must be `/Game` Blueprint or ordinary map assets. Package names
are logged as `DAS_RENAME_SCOPE` before the rename; active loaded maps are also
reported. No user asset is automatically saved.

Supported scope includes ordinary child/grandchild Blueprints, other Blueprint
graphs referencing the renamed field, saved non-partitioned non-streaming maps,
their placed actors, and already loaded direct/derived instances. Preflight rejects
destination collisions in dependents, invalid classes/Blueprints, unsupported asset
types, separate DSL import sources in dependent Blueprints, and unsupported maps.
World Partition/external actors, streaming levels, non-Game saved dependencies and
arbitrary data assets require another migration scope. Dependency discovery follows
the Asset Registry and loaded Blueprint dependency graph, not arbitrary source text.

Unreal RenameMemberVariable rewrites local and dependent variable references while
retaining variable GUIDs. The selected parent compiles first, then other scoped
Blueprints compile in inheritance order. Native snapshots cover actual direct and
derived objects/CDOs and follow OnObjectsReplaced through multiple compilations;
SKEL_/REINST_ and superseded compiler classes are excluded. Every renamed value is
restored on the final replacement, preserving inherited CDO overrides, map actor
overrides and unmodified values. Struct values own their nested/string storage.

All affected Blueprints, assets and instance packages are marked dirty. Save the
reported packages after review. Canonical DSL metadata advances after successful
restore/validation; repeat migration is idempotent. Full rollback/Undo after an
unexpected compiler/restore failure remains unimplemented, as in isolated rename.

Inherited DSL methods now validate defaults against the CDO of the class declaring
the reflected field. They read/write the actual receiver instance. A child CDO
override therefore does not falsely reject the inherited method, while an unapplied
new parent default still fails owner validation.

Tests:
- TestDSLRenameScopeAsset.ps1: three separate processes Create/Migrate/Reopen;
  parent/child/grandchild, external getter, saved map, CDO and actor overrides,
  initially unloaded dependencies, collision preflight and explicit save/reopen.
- TestDSLRenameScopePIE.ps1: live grandchild actors, inherited method before/after
  reinstance, preserved defaults/overrides, fresh spawn, default guard, GC and EndPIE.

No source files are rewritten by the migration service. Update source field names
and `self` usages in place before invoking the explicit policy.
