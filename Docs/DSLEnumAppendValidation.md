# Enum append validation — 2026-10-07

Source UE 5.8.3, pinned daScript 0.6.4, isolated DasForUESmoke,
Win64 Development Editor and cooked game.

- TestDSLTypeRegistry.ps1 -EnumAppend -Cooked: PASS.
- Adds Paused=200 while retaining Idle=7/Active=42, the same UEnum object,
  stable ID and catalog reference. Existing Actor Mode=42/Config values persist.
- Active context, old MAX=43 in instance data, Switch on Enum graph, changed class
  declaration and changed existing enum number all reject. Scope requires compiled
  Blueprints; the test compiles its fixture before migration.
- Failure after native enum mutation and after model publication restores old
  enum entries, metadata/model, package dirty flags and Actor Mode=42.
- The updated direct DSL callback accepts 200; idempotent reapply PASS.
- Migrated enum and sibling type models saved and reopened in a new editor process.
  The old saved map still has Mode=42/Count=21 before calls; updated callbacks and
  idempotent reapply PASS. No graph/layout/value rewrite is required for this slice.
- Actual cooked game: 18 checks PASS, including the appended definition/member
  callback, saved pre-call map overrides, Blueprint ProcessEvent, owned Unicode
  struct results, invalid/wrong enum refusal and GC.
- Editor foundation regression: 416 checks, failures=0, PASS.
- DirectTypesPIE regression: one successful automation test, zero warnings/failures/
  not-run, including typed callbacks, structural rollback and EndPIE cleanup.
- git diff --check PASS. Temporary GUID source files cleaned up; no commit created.

Evidence: logs/type-registry-Create.log, type-registry-Reopen.log,
type-registry-cooked.log, smoke-editor.log, dsl-direct-types-pie-report/index.json.
Transcripts: work/enum-append-cooked.log, enum-append-editor.log, enum-append-pie.log.

This policy requires stopped PIE and no script contexts. Stable enum-member IDs,
enum node reconstruction, rename/removal/renumber and struct definition migration
are not implemented by this step. See DSLEnumAppend.md and Plan.md.
