# Nested authored DSL validation — 2026-10-08

Source UE 5.8.3 / pinned daScript 0.6.4, isolated DasForUESmoke Win64 Development.

- TestDSLTypeRegistry.ps1 -AuthoredNested -Cooked: PASS. Three authored structs
  Leaf -> Branch -> Tree compile to native assets in child-first order, with
  repeated branches and inherited child defaults. Idempotent type application
  preserves the same assets. Missing and recursive model references reject.
- Empty child constructors are necessary for safe daScript initialization;
  explicit overrides are rejected by the annotation. Plain uninitialized child
  fields are rejected by daScript itself.
- Deep self compound write changes native Left.Inner.Count from 10 to 13.
  Typed input/output snapshots recursively unpack/modify/repack Right.Inner.Count
  to 15 and Title to "Nested 世界". ProcessEvent returns the nested saved value.
- Direct whole nested-member self assignment rejects explicitly instead of changing
  a temporary; whole self.Config assignment packs and commits the nested value.
- Source Leaf -> RenamedLeaf alias preserves id nested.Leaf and binds the same
  native asset through the registry. Blueprint reimport and typed setter PASS.
- Saved map/registry/classes/types reopen and reimport successfully with Count=13.
- Actual packaged game: 9 checks PASS for registry, stable IDs, native layout,
  saved value, deep self write returning 16, recursive frame/value/Unicode and GC.
- Final editor-only rerun after correcting test-world shutdown: PASS, with no
  Ensure/assertion/test-failure output. This cleanup affects the test harness only;
  the cooked nested implementation is unchanged.
- TestDSLTypeRegistry.ps1 -StructNested -StructNumeric regression: PASS.
- Editor foundation: 420 checks, failures=0, PASS.
- DirectTypesPIE: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
- git diff --check PASS; temporary GUID sources/rules cleaned; no commit created.

Evidence: work/authored-nested-cooked.log, authored-nested-final.log,
authored-nested-migration-regression.log, authored-nested-foundation.log,
authored-nested-pie.log. Shared Create/Reopen logs are overwritten by the later
migration regression; new DSL evidence remains in its transcripts. Cooked and
runtime evidence: logs/type-registry-cooked.log, smoke-editor.log,
dsl-direct-types-pie-report/index.json.

Scope/bounds and syntax: DSLNestedTypeAuthoring.md. Containers, explicit nested
defaults, whole nested-member self setters, enum/object members, multiple modules,
outer nested-child definition migration and active-PIE type reload remain future
work. Whole root-property assignment and scalar-leaf self paths are supported.
