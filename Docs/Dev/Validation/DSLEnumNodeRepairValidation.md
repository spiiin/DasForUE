# Standard enum-node validation — 2026-10-07

Source UE 5.8.3 / pinned daScript 0.6.4, isolated DasForUESmoke Win64 Development.

- TestDSLTypeRegistry.ps1 -EnumAppend -EnumGraphs -EnumNodes -Cooked: PASS.
- Executable Blueprint functions verify count 2->3, literal Active=42, Select old
  options 70/420, safe casts of 42/200 and invalid 255 -> reserved MAX 43/201.
- ForEach uses explicit enum values and excludes MAX; its function-local accumulator
  returns 49 before append, 249 afterwards, including after reopening and cook.
- Native graph links and defaults are checked after reconstruction/compilation.
  Prior Switch/member-ID/refusal checks remain covered.
- Failure after reconstruction and compilation restores execution of every enum
  probe to its pre-append result, not only its geometry/links.
- New schema/data/model source remains compatible with DSL reimport after reopen.
  The fixture local variable is Blueprint-writable and does not add Actor fields.
- Real cooked game: 25 checks PASS, including all five new enum-node assertions.
- Editor foundation regression: 420 checks, failures=0, PASS.
- DirectTypesPIE regression: one success, zero warnings/failures/not-run; callbacks,
  rollback, GC and EndPIE cleanup PASS.
- git diff --check PASS. Temporary GUID source files cleaned up; no commit created.

Evidence: logs/type-registry-Create.log, type-registry-Reopen.log,
type-registry-cooked.log, smoke-editor.log, dsl-direct-types-pie-report/index.json.
Transcripts: work/enum-nodes-cooked.log, enum-nodes-editor.log, enum-nodes-pie.log.

Scope/semantics: DSLEnumNodeRepair.md. Unsafe casts, member rename/removal/renumber,
multiple independent DSL roots, struct migration and type hot reload remain later
work. The safe cast follows Unreal's MAX sentinel behavior; the binding rejects MAX.
