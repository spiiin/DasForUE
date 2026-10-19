# Initial map DSL validation — 2026-10-08

- TestDSLTypeRegistry.ps1 -AuthoredMap -Cooked: PASS. Tree.Scores is a
  table<int;int>/TMap<int32,int32> field with an implicit empty default. Native
  idempotent creation, registered identity, Blueprint reimport and map reopen pass.
- Seed inserts key -5=10, key 7=20 and updates -5=25. Two native entries remain;
  cloned readonly values iteration sums to 45. Typed frame/return preserves the
  keys and duplicate-update result. Missing key 999 returns a script error.
- Generated pack code uses parallel readonly keys/values iterators on the same
  live input table. Unpack creates an owned table from checked native pairs.
- keys/values over temporary self snapshots reject at compile time; local clone
  usage passes. The fixture restores valid source after the negative assertion.
  Table indexing during locked iteration is avoided in favor of values iteration.
- Actual packaged game: 16 checks PASS, including saved native map layout/entries,
  sum=45, missing-key rejection, typed return, prior nested/array interop and GC.
- PrimitiveArray regression: PASS for six array scalar types and deferred-migration
  refusal; prior array behavior remains available.
- Editor foundation: 420 checks, failures=0, PASS.
- DirectTypesPIE: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
- git diff --check PASS; GUID sources/rules cleaned; no commit created.

Evidence: work/dsl-map-cooked.log, dsl-map-array-regression.log,
dsl-map-foundation.log, dsl-map-pie.log. Shared Create/Reopen logs are overwritten
by the later primitive-array regression; map evidence remains in its transcript.
Runtime evidence: logs/type-registry-cooked.log, smoke-editor.log,
dsl-direct-types-pie-report/index.json.

Scope: int keys and int values in authored struct fields, empty defaults, bounded
owned snapshots and typed enclosing-struct interop. Other map types, set, standalone
container signatures, nonempty defaults, native aliases and schema/key migration
remain outside this milestone. See DSLMapAuthoring.md and dsl_map_authoring.das.
