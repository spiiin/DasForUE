# Stable ID field rename validation — 2026-10-07

Unreal Engine 5.8.3 / pinned daScript 0.6.4, Win64 Development.

- TestDSLFieldRenameAsset.ps1: Create and Reopen in separate processes PASS. int and native struct rename; scalar/struct/string overrides and untouched defaults; variable GUID and local getter reference preservation; stable ID persistence; old property absence; updated self ProcessEvent; save/reopen and repeat idempotence.
- Negative preflight: implicit rename, occupied destination, changed automatic ID, simultaneous default change, and loaded dependent child Blueprint are refused before class/value mutation.
- TestDSLFieldRenamePIE.ps1: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0. Persistent method before rename, actor reinstance and value restore, updated script method, fresh actor defaults, repeat/GC/EndPIE all PASS.
- TestEditor.ps1: 373 foundation checks, PASS, failures=0.
- TestDSLDefaultMigrationAsset.ps1: existing default policy Create/Reopen regression PASS.
- Temporary GUID source files were cleaned up. Disposable asset fixtures are scoped under DasFrameAssetProbe. No user projects were modified.

This step changes editor tooling and adds a DSL fixture. Runtime implementation is unchanged; packaged rebuild was not required. Migration of dependent Blueprints/saved referencing maps and assets, field type conversion/removal and full rollback remain separate stages.