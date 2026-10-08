# Typed UObject DSL validation — 2026-10-08

- Scripts/TestEditor.ps1: PASS, 446 checks, zero failures.
- Scripts/TestDSLTypeRegistry.ps1 -TypedObjects -Cooked: PASS. Creation, typed
  saved reference, reopen/reimport and actual packaged game launch pass.
- Actor/Object constraints follow imported macro aliases. Null CDO/input/result,
  property assignment, ProcessEvent, wrong-class input, forged wrong-class return,
  expired Actor and clearing values are verified in editor and cooked execution.
- A native ObjectRef UPROPERTY keeps a transient curve alive through GC; clearing
  it permits GC despite VM capability handles. World/owner are explicitly pinned
  during this test; the test curve is not independently rooted.
- Source compile rejects incompatible nominal types, conflicting native class
  annotations, non-object macro arguments and fixed-array wrappers.
- Scripts/TestBindingsGenerator.py: PASS, 54 tests, including native metadata,
  deterministic short aliases, ambiguous names and generated-symbol collisions.
- Scripts/TestBindingsPolicy.py: PASS, nine actual reflection exports.
- GeneratedBindings --check and git diff --check pass. No ThirdParty modifications.
- Unresolved type macros in class method parameters remain a parser limitation;
  use a separately compiled alias module. The implementation does not modify
  daScript grammar or add a reader/preprocessor.

Workspace evidence: work/typed-objects-foundation-final.log,
work/typed-objects-cooked-final.log. The packaged log is logs/type-registry-cooked.log
(shared by subsequent registry probes).
