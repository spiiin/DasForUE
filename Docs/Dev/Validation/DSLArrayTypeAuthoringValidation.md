# Authored struct arrays: validation — 2026-10-08

- TestDSLTypeRegistry.ps1 -AuthoredArray -Cooked: PASS. Three authored types include
  Tree.Items : array<Branch>. Native creation is child-first with a typed struct
  array inner property and empty default. Idempotent apply and stable ID aliasing
  remain valid; saved state reopens and Blueprint reimports.
- Generated statement/loop helpers recursively pack/unpack dynamic arrays and
  nested structs, maintaining owned snapshot boundaries. Native seed produces two
  elements with values 31/45; unpacked iteration returns sum=76 and saved length=2.
- A populated typed input/output frame returns both elements and Unicode title
  "Array 世界". Existing scalar/nested ProcessEvent and deep scalar self writes pass.
- daScript clone/move rules are honored: source uses local clone/move returns and
  whole-self move assignment. Unsupported direct indexed self mutation rejects
  explicitly rather than changing a temporary; valid source is restored after test.
- Actual packaged game: 12 checks PASS, including stable IDs/native layout, saved
  state, existing nested calls/GC, declared array layout, unpacked iteration and
  populated frame/return. The native container state survives map serialization.
- Regression -StructArray -StructNumeric: PASS for ordered snapshots, numeric
  conversions, rollback and typed accessors after reimport/reopen.
- Regression -AuthoredNested: PASS after switching all owned struct helpers to
  statements and recursive nested get/with APIs.
- Editor foundation: 420 checks, failures=0, PASS.
- DirectTypesPIE: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
- git diff --check PASS; GUID source/rules files cleaned; no commit created.

Evidence: work/dsl-array-cooked.log, dsl-array-migration-regression.log,
dsl-array-nested-regression.log, dsl-array-foundation.log, dsl-array-pie.log.
Shared registry Create/Reopen logs reflect the later nested regression; array output
remains in its transcript. Runtime evidence: logs/type-registry-cooked.log,
smoke-editor.log, dsl-direct-types-pie-report/index.json.

Scope: array<authored struct> fields in ue_struct, enclosing typed values and whole
root-property commits. Empty defaults and existing snapshot bounds apply. Primitive
arrays, standalone array properties/signatures, direct indexed mutations, set/map
and outer-container definition migration remain later work. Helpers currently
compose owned snapshots with repeated copies; bulk packing optimization is separate.
See DSLArrayTypeAuthoring.md and dsl_array_authoring.das.
