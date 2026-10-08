# Interface-constrained typed dispatch validation — 2026-10-08

ue_begin_interface_call creates an owned UCallHandle constrained to a declared
native interface method. Membership and reflected signatures validate both when
the frame is created and immediately before invocation. Existing typed marshalling
APIs handle the frame's supported types and directions.

- Scripts/TestDSLTypeRegistry.ps1 -Interfaces -Cooked: PASS, including creation,
  save/reopen and packaged execution. The cooked probe passed 32 checks.
- Typed int dispatch, exact double array input/return, several out/ref values,
  Unicode map values and set elements dispatch to ordinary Blueprint receivers.
  Results are read/copied before frame release; input values retain ownership.
- Missing input, wrong scalar setter and use after frame release reject.
- Existing native C++ Execute_* -> DSL, membership and initial interface contract
  rejection probes remain passing.
- Scripts/TestEditor.ps1: PASS, 436 checks, zero failures. This includes regression
  of existing scalar/array/hash marshalling and the extended source example.
- Scripts/TestDSLDirectTypesPIE.ps1: PASS, one success, no warnings/failures
  (shared typed DSL/rollback regression).

Evidence in the Codex workspace work directory: typed-interfaces-cooked.log,
typed-interfaces-foundation.log and typed-interfaces-pie.log. Runtime cooked
markers are in logs/type-registry-cooked.log. No pinned daScript changes or
automatic commit were performed.

See DSLInterfaces.md for the API, limits and retained native-fallback/reentrant
VM limitations. This stage extends the checked call frame; it does not yet generate
new wrapper source automatically. Next: DSL-authored interface definitions.
Migration development remains deferred.
