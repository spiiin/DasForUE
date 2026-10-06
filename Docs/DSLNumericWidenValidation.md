# Exact numeric widening validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, Win64 Development.

- TestDSLNumericWidenAsset.ps1: Create/Migrate/Reopen in separate processes PASS. int32 extremes and float 0.1/-13.25 preserve exact promoted values; root/child CDO overrides and saved map actor overrides; initially unloaded dependencies; inherited double-returning DSL method; explicit save/reopen and idempotence.
- Preflight negatives: implicit conversion, isolated policy with dependencies, combined default change, lossy int->float, raw Blueprint property nodes and nonfinite actor values refused before mutation.
- Development fault after scope compilation: rollback restores int/float descriptors, original instance and child CDO values and canonical metadata; DAS_NUMERIC_ROLLBACK=PASS.
- TestDSLNumericWidenPIE.ps1: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0. Old callback, double reinstance rollback, last-good callback after rollback, successful new double callback, child defaults/new spawn, source-declaration removal guard, idempotence, GC and EndPIE all PASS.
- TestEditor.ps1: 373 foundation checks, failures=0, PASS.
- TestPackaged.ps1: build/cook/stage/archive/launch; 373 foundation checks, failures=0, PASS.
- TestDSLRenameScopeAsset.ps1: dependency-aware rename Create/Migrate/Reopen regression PASS.
- Test helper worlds are explicitly cleaned up after saving; temporary GUID script files are removed. No user projects were migrated.

Numeric rollback is scoped descriptor/native-value restoration, not recovery from engine crashes or arbitrary user effects. Idle method-only contexts with no lifecycle/subscription/cached-call state are preserved. General lifecycle/delegate preservation, Blueprint pin/link adaptation, narrowing and removals remain separate stages.