# Field archive validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, Win64 Development.

- TestDSLArchiveFieldsAsset.ps1: Create/Remove/ReopenRemoved/Restore/ReopenRestored,
  five separate processes, PASS. Saved parent/child CDO defaults, map overrides,
  new child spawned while fields were absent, save/reopen and idempotence.
- Seven field kinds: int32, float32, double, bool, Unicode string, name and owned
  native reflected struct. Exact scalar overrides and variable GUID survive restore
  and subsequent reopen; consumed instance archives remain empty.
- Isolated policy refuses saved dependency scope. Direct property graph references
  refuse removal. Changed restoration default/type/stable ID refuse before mutation.
- Development-only failure immediately after recompilation: removal and restoration
  rollback PASS, old layout/data/model preserved and archive retained as appropriate.
- TestDSLArchiveFieldsPIE.ps1: succeeded=1, succeededWithWarnings=0, failed=0,
  notRun=0. Live actor reinstance, child defaults, fresh retired actor, restoration,
  original GUID, GC and EndPIE PASS. Missing or incompatible per-instance archives
  refuse restoration before mutation. The explicit daScript state counter advances
  through both failures and both successful migrations without resetting.
- TestEditor.ps1: 397 foundation checks, failures=0, PASS.
- TestPackaged.ps1: build/cook/stage/archive/launch; 397 foundation checks,
  failures=0, PASS after rebuilding the native Actor adapter.
- TestDSLNumericConvertPIE.ps1: regression PASS, exactly one successful test without
  warnings. No changes to the existing numeric conversion policies.

GUID-named temporary source files are removed. Disposable assets remain confined
to DasFrameAssetProbe in the smoke project. No user project assets were migrated.
Archive migration is editor functionality; packaged tests validate runtime
compatibility, not editor migration commands. General rollback, lifecycle/delegate
contexts and World Partition/streaming/other asset scopes remain future work.

## Shared pin/default conversion validation — 2026-10-17

Editor build and final up-to-date build passed. Expanded archive asset phases
Create / Remove / ReopenRemoved / Restore / ReopenRestored passed. In addition to
previous fields, the fixture checks byte 255, localized FText namespace/key/source,
native int arrays and sets, and string-to-int maps. Parent/child CDOs, actor
manual values and actors spawned while retired survive restore and reopening.
Injected failures on removal/restoration still roll back; GUID and repeat checks
also pass. The fixture keeps test marker comments separate from tooltip comments.

ArchivePIE and Types.PinAdapter each report exactly 1 success, 0 warnings,
failures or skipped tests. The pin adapter compiles a 134-type Blueprint matrix.
Computed defaults Create / Reopen / Recompile passed after extracting the shared
Blueprint literal builder. Runs used NullRHI. No packaged build was repeated for
this editor migration change.

Evidence: logs/archive-pins-editor-build.log,
logs/archive-pins-editor-final-build.log, logs/archive-pins-suite.log,
logs/archive-pins-{Create,Remove,ReopenRemoved,Restore,ReopenRestored}.log,
logs/archive-pins-pie-report/index.json, logs/archive-pins-matrix-report/index.json
and logs/archive-pins-defaults-{Create,Reopen,Recompile}.log.
No commit was created.