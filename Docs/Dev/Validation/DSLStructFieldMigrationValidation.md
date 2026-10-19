# Struct add/rename validation — 2026-10-07

Source UE 5.8.3 / pinned daScript 0.6.4, isolated DasForUESmoke Win64 Development.

- TestDSLTypeRegistry.ps1 -StructFields -Cooked: PASS.
- Count -> Score preserves ID direct.Settings.Count and native field GUID; the
  UUserDefinedStruct UObject/catalog GUID remain unchanged. Enabled=true is added.
- Existing Actor value 24, Unicode Title="Saved 世界" and CDO value 15 persist.
  New Enabled default applies; MakeStruct count/default 88 and linked BreakStruct
  getter preserve their behavior. Updated direct DSL Bump returns 27.
- Failure injection after structure compilation, node reconstruction, data restore
  and model publication restores old fields/layout, Actor value 24, BreakStruct
  execution and original map dirty flag.
- Changed field ID and field type reject; valid candidate is restored explicitly.
  Normalized struct metadata changes are outside the policy and refused.
- Idempotent application and save/reopen PASS. Reopened Score=27, Enabled field,
  BreakStruct execution and ordinary DSL reimport remain valid.
- Actual cooked game: 12 checks PASS, including renamed friendly field lookup,
  saved scalar/Unicode/new default, CDO, Make/Break and direct callback/GC.
- Editor foundation: 420 checks, failures=0, PASS.
- DirectTypesPIE regression: one success, zero warnings/failures/not-run.
- git diff --check PASS. Temporary GUID sources cleaned up; no commit created.

Evidence: logs/type-registry-Create.log, type-registry-Reopen.log,
type-registry-cooked.log, smoke-editor.log, dsl-direct-types-pie-report/index.json.
Transcripts: work/struct-fields-cooked.log, struct-fields-editor.log,
struct-fields-pie.log; final preflight confirmation: work/struct-fields-final.log.

Scope and future work: DSLStructFieldMigration.md. Removal/type/default changes,
nested/container/archive state, split/custom operation pins, independent roots
and active-PIE schema reload are not implemented in this step.


## Explicit field drop validation — 2026-10-08

- TestDSLTypeRegistry.ps1 -StructDrop -Cooked: PASS. Create/reopen and actual
  packaged game retain Score=27, Title="Saved 世界", CDO Score=15 and the
  Make/Break graph behavior while Enabled is absent. DSL Bump returns 30 in
  the cooked game and the value survives GC. Twelve cooked checks pass.
- Final TestDSLTypeRegistry.ps1 -StructDrop: PASS after extending rejection
  and graph rollback assertions. Missing/unknown/stale rules, wrong schema version,
  unsupported operations, duplicate IDs, unknown JSON fields, null entries and
  nonlocal rules paths reject. A connected Break Struct field cannot be dropped.
- Modified boolean and empty string Make Struct defaults reject before mutation.
  The test explicitly shows the new optional Enabled input and checks its default
  after all four failure stages. No obsolete member/orphan pins remain after drop.
- Four injected stages restore the old layout including Enabled, Actor Score=27,
  Break Struct result, Make Struct Enabled default and map dirty flag. Successful
  drop intentionally discards a nondefault Actor Enabled=false value; remaining
  identity and data persist. Saved assets reimport/reopen successfully.
- Editor foundation: 420 checks, failures=0, PASS.
- DirectTypesPIE: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
- git diff --check PASS; GUID source/rules files cleaned up; no commit created.

Evidence: logs/type-registry-Create.log, type-registry-Reopen.log,
type-registry-cooked.log, smoke-editor.log and
dsl-direct-types-pie-report/index.json. The final editor-only run and cooked run
use separate isolated GUID maps. Transcripts:
work/struct-drop-cooked.log, struct-drop-final.log, struct-drop-foundation.log,
struct-drop-pie.log.

Removal is now implemented within the documented direct Actor/CDO and Make/Break
scope. Numeric field type/default conversion remains the next step. Wider storage,
graphs, multiple roots and active-PIE schema reload remain future work.


## Numeric field conversion validation — 2026-10-08

- TestDSLTypeRegistry.ps1 -StructNumeric -Cooked: PASS. Actor Score and CDO
  are preserved through int -> double, double -> int (Ceil), int -> float
  (RoundNearest). The saved final float Score=28 and CDO=15 survive reopen.
- Missing conversion policy and an independent new default reject. Exact
  rejects fractional data, int32 overflow, nonfinite data and float32 precision
  loss. Explicit float rounding maps 16777217 to 16777216; final test state is
  intentionally restored to 28 before saving. Stable type/member GUIDs and
  Unicode Title remain unchanged.
- Four widening failure stages restore int layout, Actor=27, Make default=88,
  Break result, adapter count and package dirty flag. The final editor-only
  probe additionally injects all four narrowing stages with double value
  27.123456789012345: exact original double, CDO, Make behavior and original
  adapter count are restored. Eight injected conversion failures pass.
- Make default and autogenerated default are converted separately. Make default
  88 remains a user override rather than being reset to native default 15.
  The final editor probe additionally verifies a connected Make Struct input
  carrying 91 through the adapters at each type change and after reopen.
- Cooked game: 11 checks PASS, including float native layout, saved scalar/Unicode,
  CDO, checked Break graph, Make literal, direct float leaf compound write returning
  31 and state after GC. The cooked run precedes adding the extra connected-input
  editor assertion; numeric implementation/default handling is the same.
- Stale conversion rules reject; repeat without rules is idempotent.
- Regression TestDSLTypeRegistry.ps1 -StructDrop: PASS.
- Regression TestDSLNumericConvertAsset.ps1: Create/Migrate/Reopen/NarrowFloat/
  ReopenFloat PASS for the shared numeric graph conversion code.
- Editor foundation: 420 checks, failures=0, PASS.
- DirectTypesPIE: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
- git diff --check PASS. GUID source/rules files cleaned up; no commit created.

Evidence: work/struct-numeric-cooked.log, struct-numeric-final.log,
struct-numeric-drop-regression.log, struct-numeric-convert-regression.log,
struct-numeric-foundation.log, struct-numeric-pie.log. The shared
logs/type-registry-Create.log and Reopen.log are overwritten by the later drop
regression; numeric output is retained in the final transcript. Cooked evidence:
logs/type-registry-cooked.log. Runtime/PIE evidence: logs/smoke-editor.log and
logs/dsl-direct-types-pie-report/index.json.

Scope: one numeric field per transaction in the documented direct Actor/CDO
and Make/Break scope. Conversion cannot combine add/drop/rename; default changes
must be the declared conversion, and graphs can retain composed checked adapters.
Nested/container/archive state, split/custom nodes, multiple roots and active-PIE
type reload remain future work.


## Nested storage validation — 2026-10-08

- TestDSLTypeRegistry.ps1 -StructNested: add/rename Create/Reopen PASS.
  Existing Bundle -> Left/Right Envelope -> Inner Settings gives two independent
  inner occurrences. Their values 61/97, Unicode titles, sibling labels and Actor
  Ratio=9.87654321098765 remain exact. Inner CDO values=15 and native wrapper
  defaults remain consistent. Added Enabled=true applies to both inner values.
- The mismatched-root-CDO test rejects before mutation. A numeric native wrapper
  default that loses precision through Unreal editor's export/import rejects too.
  Test native default Ratio=1.25 is deliberately round-tripping; the high-precision
  Actor override demonstrates independent state precision without relying on
  unsupported default text serialization.
- TestDSLTypeRegistry.ps1 -StructNested -StructNumeric: PASS. Includes ordinary
  add/rename, int -> double -> int/Ceil -> float/RoundNearest, default/value refusal,
  widening/narrowing rollback and save/reopen with the nested trees intact.
- TestDSLTypeRegistry.ps1 -StructNested -StructDrop: PASS. Drops Enabled in all
  occurrences, preserves remaining values/defaults, exercises graph-loss refusals
  and four-stage rollback, then reopens successfully.
- TestDSLTypeRegistry.ps1 -StructNested -StructNumeric -Cooked: PASS.
  Thirteen actual game checks pass, including nested loaded type identity,
  occurrence-specific data, Unicode, siblings, exact Actor Ratio and CDO=1.25,
  float layout, Make/Break, callback returning 31 and subsequent GC.
- Loaded wrapper structs seed the bounded asset scope. This fixes dependencies
  missing from Asset Registry immediately after new wrapper packages are saved.
- TestDSLAuthoredValuesAsset.ps1 regression: PASS for precise Blueprint struct
  default serialization, registered type binding, native snapshots and reimport.
- Editor foundation: 420 checks, failures=0, PASS.
- DirectTypesPIE: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
- git diff --check PASS; temporary GUID source/rules files cleaned; no commit.

Evidence: work/struct-nested-editor.log, struct-nested-numeric.log,
struct-nested-drop.log, struct-nested-cooked.log, struct-nested-defaults-regression.log,
struct-nested-foundation.log, struct-nested-pie.log. Runtime evidence:
logs/type-registry-cooked.log, smoke-editor.log and
dsl-direct-types-pie-report/index.json. Shared registry Create/Reopen logs reflect
the final cooked numeric fixture; earlier mode output remains in transcripts.

This is storage migration of existing user-struct wrappers exposed through
UEStructValue. Nested `[ue_struct]` field declarations/recursive direct DSL helpers
are not implemented here. Containers, explicit nested defaults, split/wrapper
operation nodes, retired nested snapshots, multiple roots and active-PIE reload
remain outside this policy. Details: DSLStructFieldMigration.md.
