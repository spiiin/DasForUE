# Structural reimport/rename rollback validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, Win64 Development. Editor-only change.

- TestDSLLifecycleMigrationPIE: exactly one successful test, zero warnings/failures.
  Rename faults after member mutation, after compile and after model/metadata write
  restore layout/model and preserve lifecycle state, self/peer subscriptions and an
  unchanged external listener. Structural faults after compile and metadata write
  undo an added field, added method and changed function metadata. Retry creates
  the added method using its exact intended name. Real Tick, GC, Destroy and EndPIE
  continue passing after repeated forward/rollback reinstancing.
- TestDSLRenameScopeAsset: Create/Migrate/Reopen PASS. Three fault checkpoints
  before successful rename restore root/child/grandchild defaults, saved map values,
  external getter references, stored model and original root package dirty state.
  Successful retry and subsequent save/reopen preserve the migrated scope.
- TestDSLStructuralRollbackAsset: Create/Rollback/Reopen PASS in three separate
  processes. Dependencies start unloaded; both fault checkpoints restore original
  model/schema, CDO and saved map values and external graph reference. Added field
  and method are absent after rollback, save and reopen. The fixture restores its
  disposable source explicitly; the production service never edits source files.
- TestDSLPIE and TestDSLRenameScopePIE regressions PASS without automation warnings.
- TestDSLArchiveFieldsAsset and TestDSLNumericConvertAsset: five-process regressions
  PASS. Existing dedicated archive/numeric rollback remains intact.
- TestEditor: 397 foundation checks, failures=0, PASS. Runtime implementation did
  not change in this stage; packaged runtime checks were already validated in the
  preceding lifecycle stage.
- git diff --check PASS. Disposable GUID sources cleaned up; only smoke-project
  fixture assets were migrated.

The snapshot is bounded to currently supported DSL declarations and ordinary
Blueprint/map scopes. First asset creation, external construction-script side
effects, native reload, unrelated edits and broader asset types are outside this
rollback contract. An incomplete rollback is reported distinctly from a successful
restoration and does not reset package dirty flags.
