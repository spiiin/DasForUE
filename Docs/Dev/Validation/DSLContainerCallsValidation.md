# Standalone container calls validation — 2026-10-08

The first direct container-signature milestone is complete: six primitive array
shapes, two int/string set shapes and twelve scalar/name map shapes.

- Scripts/TestDSLTypeRegistry.ps1 -ContainerCalls -Cooked: PASS, including
  creation, save/reopen and packaged execution. The cooked probe passed 115 checks.
- All 20 shapes expose real native container UFunction input/return properties,
  preserve populated values through typed frames and ProcessEvent, return
  independent owned storage and accept empty inputs/results.
- Mixed array/set/map/scalar input ordering and a scalar double result pass
  both typed and ProcessEvent calls. Wrong carrier and oversized input reject.
- Unicode, empty strings, FName identities, bool values and float/double precision
  are covered by the shared editor/cooked matrix.
- Final editor/reopen matrix passes after the nested-container write guard change;
  direct self nested-container assignment still rejects at compile time.
- Scripts/TestEditor.ps1: PASS, 430 checks, zero failures, including compilation
  of dsl_container_calls.das and authoring IR/native adapter roundtrip.
- Scripts/TestDSLDirectTypesPIE.ps1: PASS, one success, zero warnings/failures
  (shared typed DSL/rollback regression).

Evidence transcripts in the Codex workspace work directory:
container-calls-cooked.log, container-calls-final-editor.log,
container-calls-foundation.log and container-calls-pie.log. Shared registry
Create/Reopen logs reflect the final editor run. The final runtime guard change
only restores rejection of direct nested-container writes; the positive packaged
matrix was verified before that guard change.

The pinned daScript sources were not modified. Migration expansion and an
automatic commit were not performed. See DSLContainerCalls.md for syntax,
native array flag conventions, owned carrier transport, bounds and scope.
Next: explicit DSL out/ref semantics and interfaces.
