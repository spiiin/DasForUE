# Default migration validation — 2026-10-06

Win64 Development, Unreal Engine 5.8.3, pinned daScript 0.6.4.

- TestDSLDefaultMigrationAsset.ps1: Create/Reopen PASS in separate processes. Explicit scalar/struct default migration, no class replacement, untouched and overridden existing actors preserved, new actor defaults, invalid default and combined structural change refused before mutation, later Blueprint compile and saved defaults, repeat migration idempotence. GUID source files cleaned up.
- TestDSLDefaultMigrationPIE.ps1: real PIE; succeeded=1, succeededWithWarnings=0, failed=0, notRun=0. Defaults update without reinstance; existing/new actor values; updated script promotes after migration; later reinstance preserves old-default and overridden values; GC/EndPIE. Temporary source cleaned up.
- TestEditor.ps1: 373 foundation checks, failures=0, PASS.
- TestDSLStructFieldsAsset.ps1: previous struct Save/Reopen and no-policy refusal regressions PASS.
- git diff --check: PASS.

Only editor module/API changed in this step; runtime implementation and packaged target were unchanged. No packaged rebuild was required. Earlier struct-field stage passed 373 packaged checks.

Policy preserves every existing value and changes only selected Blueprint CDO/default descriptors. It does not rename, remove, convert field types or propagate old-default values to existing instances/derived Blueprint CDOs. Tests cover disposable assets in the isolated smoke project, not user projects.