# Enum rename validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, isolated DasForUESmoke Win64 Development.

- TestDSLTypeRegistry.ps1 -EnumAppend -EnumGraphs -EnumNodes -EnumRename -Cooked PASS.
- Active -> Running keeps number 42, ID direct.State.Active, enum/catalog identity
  and old Actor/map values. The old name no longer resolves.
- Switch branch connection/result 420, Select option default 420, enum Literal and
  function-entry Input default rename correctly. Count/cast/ForEach results remain
  unchanged. CDO stays 42; stored class default becomes Running.
- Injected failures at all four stages restore old names, pin defaults/links,
  executable graph results, class model, Actor value and package dirty flags.
- Changing the member ID or number rejects before mutation. Idempotent reapply PASS.
- Saved migrated types and Blueprint reopen in a new editor process; old saved map
  values, normal DSL reimport, graphs/defaults/member identity and callbacks PASS.
- Actual cooked game: 27 checks PASS, including renamed number/ID and CDO default,
  saved values before calls, all enum nodes, typed frames, Unicode and GC.
- Editor foundation: 420 checks, failures=0, PASS.
- DirectTypesPIE regression: one success, zero warnings/failures/not-run; callbacks,
  rollback, GC and EndPIE cleanup PASS.
- git diff --check PASS. GUID sources cleaned up; no commit created.

Evidence: logs/type-registry-Create.log, type-registry-Reopen.log,
type-registry-cooked.log, smoke-editor.log, dsl-direct-types-pie-report/index.json.
Transcripts: work/enum-rename-cooked.log, enum-rename-editor.log, enum-rename-pie.log.

Current scope/limitations: DSLEnumRename.md. Removal/number-change policies, struct
migrations, multiple independent roots and type-definition hot reload remain later.
