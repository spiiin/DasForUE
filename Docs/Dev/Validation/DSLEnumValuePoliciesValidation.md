# Enum value/removal validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, isolated DasForUESmoke Win64 Development.

- TestDSLTypeRegistry.ps1 with -EnumAppend -EnumGraphs -EnumNodes -EnumRename
  -EnumValues -Cooked: PASS.
- Running 42->50: Actor and current CDO become 50, stable member ID remains
  direct.State.Active; Switch/Select branch result stays 420, Literal becomes 50,
  ForEach sum becomes 257. Updated direct DSL callback accepts 50.
- Wrong replacement rule rejects. Four failure stages restore old number 42,
  executable graphs, Actor/CDO and package dirty flags.
- Paused deletion with 200->50 replaces old instance value. Linked Paused Switch
  case refuses before enum mutation. Empty case/option removal succeeds, count
  becomes 2 and ForEach sum 57.
- Save/reopen preserves remapped Blueprint, enum/member metadata, map values and
  normal DSL reimport. The refreshed CDO is explicitly included in remap.
- Actual cooked game: 12 checks PASS, including definition/ID, mapped map Actor/CDO,
  graphs, count/iteration, direct callback, deleted-value refusal and GC.
- Editor foundation regression: 420 checks, failures=0, PASS.
- DirectTypesPIE regression: one success, zero warnings/failures/not-run; callbacks,
  rollback, GC and EndPIE cleanup PASS.
- git diff --check PASS. GUID source/rule files cleaned up; no commit created.

Evidence: logs/type-registry-Create.log, type-registry-Reopen.log,
type-registry-cooked.log, smoke-editor.log, dsl-direct-types-pie-report/index.json.
Transcripts: work/enum-values-cooked.log, enum-values-editor.log, enum-values-pie.log.

Storage/graph bounds: DSLEnumValuePolicies.md. Containers/nested/archive data,
independent root coordination and active-PIE schema reload remain future policies.
