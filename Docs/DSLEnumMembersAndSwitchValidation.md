# Enum member IDs and Switch repair validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, isolated DasForUESmoke, Win64 Development.

- TestDSLTypeRegistry.ps1 -EnumAppend -EnumGraphs -Cooked: PASS.
- Automatic member IDs persisted for Idle=7/Active=42/Paused=200. Source member
  identity drift refused by direct adapter compilation; existing IDs remain stable.
- Foundation verifies explicit member_id_Active, automatic IDs, duplicate ID refusal
  and v1 read/v2 write with automatic legacy IDs.
- Switch RouteEnum preserves its linked Idle/Active outputs, returns 70/420 and gains
  a Paused output after append. Saved graph reopens and still executes Active=420.
- Failure injection after reconstruction and after root compilation restores the
  old enum, removes the new output, restores old links, executable Active branch
  and Actor Mode=42. Earlier enum/model rollback and refusal checks also PASS.
- Source class changes, reserved old MAX data, active contexts and default-policy
  Switch repair refusal remain covered. The explicit graph repair policy is used
  only inside its supported single DSL Actor root scope.
- Real cooked game: 20 checks PASS, including binary member IDs and reconstructed
  old Switch branches, appended value 200, typed callbacks, saved overrides, owned
  Unicode results, invalid enum/wrong identity refusal and GC.
- Editor foundation: 420 checks, failures=0, PASS.
- Packaged foundation in the same cooked map: 420 checks, failures=0, PASS.
- DirectTypesPIE regression: exactly one success, zero warnings/failures/not-run,
  typed callbacks, rollback, GC and EndPIE cleanup PASS.
- git diff --check PASS. Temporary GUID sources cleaned up. No commit created.

Evidence: logs/type-registry-Create.log, type-registry-Reopen.log,
type-registry-cooked.log, smoke-editor.log, enum-switch-foundation-packaged.log,
dsl-direct-types-pie-report/index.json. Transcripts: work/enum-switch-cooked.log,
enum-switch-editor.log and enum-switch-pie.log.

Bounds: DSLEnumMembersAndSwitch.md. Member rename/removal/renumber, other enum node
kinds, multiple independent repair roots, struct migration and type hot reload
remain future work.
