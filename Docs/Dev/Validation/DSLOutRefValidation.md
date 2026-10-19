# Explicit out/ref validation — 2026-10-08

The initial primitive/standalone-container out/ref milestone is complete.

- Scripts/TestDSLTypeRegistry.ps1 -OutRef -Cooked: PASS. Editor creation,
  save/reopen and packaged execution pass. The cooked probe completed 73 checks.
- Six primitive and 20 container types preserve modified ref values and out
  values through typed frames and native ProcessEvent copy-back. Unicode,
  numbered FName, bool and exact float/double values are covered.
- Native out/ref flags match their declared directions. Mixed parameter order,
  normal return alongside outputs and void methods pass. Out inputs initialize
  to defaults even when a caller supplies existing data.
- Script panic publishes no output frame. Native failure preserves original ref
  values and resets output-only values to defaults.
- Late packing failure (oversized output after a primitive slot was prepared)
  discards every output; native copy-back preserves the original ref value.
- Wrong direction contract, unknown parameter name, missing reference type and
  duplicate/overlapping direction annotations reject.
- Scripts/TestDSLTypeRegistry.ps1 -ContainerCalls: PASS, regression for all
  20 previous in-only container signatures and their ProcessEvent calls.
- Scripts/TestEditor.ps1: PASS, 433 checks and zero failures. Includes compiled
  source example, explicit directions and IR/native contract roundtrip.
- Scripts/TestDSLDirectTypesPIE.ps1: PASS, one success, zero warnings/failures
  (shared typed DSL/rollback regression).

Evidence in the Codex workspace work directory: out-ref-cooked.log,
out-ref-foundation.log, out-ref-container-regression.log and out-ref-pie.log.
Shared registry Create/Reopen logs were overwritten by the in-only container
regression; the workspace transcripts retain the out/ref evidence.

See DSLOutRef.md for ownership, copy-in/copy-out, aliasing and failure semantics.
Objects/enums/authored structs/math out/ref remain later extensions. Parameter
output publication is transactional; earlier arbitrary script side effects are
outside that guarantee. Migration development remains deferred. No automatic
commit was performed. Next planned step: interfaces.
