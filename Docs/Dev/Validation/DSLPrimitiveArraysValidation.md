# Primitive array DSL validation — 2026-10-08

- TestDSLTypeRegistry.ps1 -PrimitiveArray -Cooked: PASS. Arrays of int, float,
  double, bool, string and UENameValue become correctly typed native fields with
  empty defaults, then preserve populated state through save/reopen/reimport.
- Native and typed return assertions preserve [-3,7], float 1.25, double
  1.23456789012345, true/false, Unicode, quote/comma text and name CaseKey.
- Primitive pack helpers use element iteration. UENameValue setters bind a const
  reference; this avoids managed handle R2V during daScript simulation. Repeated
  owned snapshot get/with operations preserve values and do not expose raw aliases.
- Actual packaged game: 14 checks PASS, including all six native primitive array
  values, typed roundtrip, previous struct arrays, nested callbacks and GC.
- Final -PrimitiveArray rerun PASS with explicit deferred migration assertion:
  catalogs containing primitive arrays reject structural migration before reading
  candidate/mutating native layout. Count remains Count, Score does not appear,
  and valid source is restored after the refusal test.
- -AuthoredArray regression: PASS for existing struct array loops and types.
- Editor foundation: 420 checks, failures=0, PASS.
- DirectTypesPIE: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
- git diff --check PASS; GUID source/rules files cleaned; no commit created.

Evidence: work/primitive-array-cooked.log, primitive-array-final.log,
primitive-array-struct-regression.log, primitive-array-foundation.log,
primitive-array-pie.log; logs/type-registry-cooked.log, smoke-editor.log and
dsl-direct-types-pie-report/index.json. The final guard change affects editor
migration refusal only; the validated cooked interop implementation is unchanged.

Scope and deferred work: DSLPrimitiveArrays.md. Migration expansion is postponed
by user decision. Set/map, standalone array properties/signatures, enum/object
elements, nonempty defaults and direct indexed self setters remain later features.
