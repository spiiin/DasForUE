# Type registry validation — 2026-10-07

Environment: source UE 5.8.3 / pinned daScript 0.6.4, isolated DasForUESmoke,
Win64 Development Editor and packaged game.

- TestDSLTypeRegistry.ps1 -Cooked: PASS. Create and Reopen use independent editor
  processes; types, runtime catalog, Blueprint and map are saved and reopened.
- Source PlayState/GameSettings declarations resolve by direct.State/direct.Settings
  to the original State/Settings assets. Catalog publication is idempotent.
- Duplicate IDs, unknown IDs, wrong asset kind and wrong struct GUID refuse.
  Unknown source ID fails compilation. Registry hard references survive GC.
- The map persists Mode=42, Count=21 and Title="Saved 世界" before script calls.
  Reopened compilation/reimport and direct Bump return 24 without losing identity.
- Build/cook/stage/archive/launch PASS. The map's native Actor catalog reference
  supplies the cook dependency; no CookDir/CookAll is needed. A prior test without
  that reference correctly exposed a missing catalog package.
- Real cooked game: all 16 checks PASS. Catalog/native type identity, sparse enum,
  loaded map values, direct leaf write, mixed frame result 142, struct setter,
  Blueprint ProcessEvent results/setters, Unicode ownership, invalid enum/wrong
  identity refusal, missing ID and GC/repeated values are verified.
- A transient Zen stage connection refusal recovered through UAT's readiness
  retry. The successful final run uses the normal project cook settings.
- TestEditor: 416 foundation checks, failures=0, PASS.
- TestDSLDirectTypesPIE regression: exactly one success, zero warnings/failures/
  not-run; typed callbacks, rollback, GC and EndPIE cleanup PASS.
- Packaged foundation regression in the same cooked fixture game/map: 416 checks,
  failures=0, PASS.
- git diff --check PASS. Temporary GUID source files cleaned up. No commit created.

Evidence: logs/type-registry-Create.log, type-registry-Reopen.log,
type-registry-cooked.log, type-registry-foundation-packaged.log, smoke-editor.log,
dsl-direct-types-pie-report/index.json. Workspace transcript:
work/type-registry-cooked-run.log; regressions: work/type-registry-editor.log and
work/type-registry-pie.log.

Catalog bindings and authored type definitions remain immutable. Source type
renaming with a preserved ID is supported; this is not a data/layout migration.
Asset moves, registry rebinding, enum-member migrations, struct-field migration
and dependent Blueprint/map repair remain explicit future work.
