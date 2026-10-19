# Struct array storage validation — 2026-10-08

- TestDSLTypeRegistry.ps1 -StructArray: Create/Reopen PASS for add/rename and
  four rollback stages. Three ordered envelope elements with inner values 31/38/45,
  separate Unicode titles and labels survive. Empty array and CDO defaults stay empty.
- -StructArray -StructNumeric: PASS for int -> double -> int/Ceil -> float/
  RoundNearest, refusal policies and widening/narrowing rollback. All indexed
  element values are converted independently. Source accessors are updated to
  match the new field type, keeping the method signatures unchanged.
- -StructArray -StructDrop: PASS; final rerun verifies indexed access after drop,
  graph-loss refusals, rollback, removal in every array element and save/reopen.
- Indexed owned read/write changes the first value 31 -> 44 -> 31. Index 99 rejects
  without corrupting state. A 4097-element owned snapshot rejects the element limit.
  Restoration also checks allocations against the new element layout byte limit.
- -StructArray -StructNumeric -Cooked: PASS. Sixteen actual cooked checks include
  array length/order, zero-length array/CDO, element values/Unicode, indexed read,
  out-of-range rejection, nested/root values, float layout, callback and GC.
- -AuthoredNested regression: PASS for the previous declaration/recursive helpers.
- Editor foundation: 420 checks, failures=0, PASS.
- DirectTypesPIE: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
- git diff --check PASS; GUID source/rules files cleaned; no commit created.

Evidence: work/struct-array-editor.log, struct-array-numeric.log,
struct-array-drop.log, struct-array-drop-final.log, struct-array-cooked.log,
struct-array-authoring-regression.log, struct-array-foundation.log,
struct-array-pie.log. Shared registry Create/Reopen logs are overwritten by the
authoring regression; array output remains in transcripts. Runtime evidence:
logs/type-registry-cooked.log, smoke-editor.log,
dsl-direct-types-pie-report/index.json.

Scope: existing native/user wrappers exposed through UEStructValue, with arrays of
supported struct elements and empty native/CDO array defaults. DSL array declarations,
primitive arrays, map/set, direct Actor container fields, nonempty defaults and
container graph repair remain separate work. See DSLStructArrayStorage.md.
