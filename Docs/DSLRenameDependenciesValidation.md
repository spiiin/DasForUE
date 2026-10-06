# Dependency-aware rename validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, Win64 Development.

- TestDSLRenameScopeAsset.ps1: Create/Migrate/Reopen in three separate editor processes, PASS. Initially unloaded child Blueprint/map; transitive child/grandchild and external Blueprint; parent/derived CDO and placed actor overrides; external getter reference rename; inherited DSL ProcessEvent; dirty packages without API autosave; explicit harness save and fresh-process reopen; idempotence.
- Negative preflight: isolated policy refuses saved dependencies; occupied destination in a dependent child refuses before root class/property mutation.
- TestDSLRenameScopePIE.ps1: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0. Live grandchild overrides/defaults, persistent method before/after reinstance, fresh actor defaults, GC and EndPIE. A child override does not bypass the guard for an unapplied parent default.
- TestEditor.ps1: 373 foundation checks, PASS, failures=0.
- TestPackaged.ps1: build/cook/stage/archive/launch, 373 foundation checks, PASS, failures=0. Existing Zen readiness retry recovered during staging.
- TestDSLFieldRenameAsset.ps1: previous isolated rename/Create/Reopen and dependency refusal regression PASS.
- Final scope asset/PIE tests rerun after loaded-dependency bounds and validity guards; PASS. git diff --check passed.

Tests use GUID-scoped disposable assets under DasFrameAssetProbe in the isolated smoke project. Temporary script files are cleaned up. No user projects were migrated.

Scope supports ordinary UBlueprint and non-partitioned non-streaming map assets. Unsupported saved asset types, World Partition/external actors, streaming levels and separate dependent DSL sources are rejected. Runtime owner validation now compares defaults with the declaring property's class CDO. Full rollback after an unexpected post-mutation compiler/restore failure remains future work.