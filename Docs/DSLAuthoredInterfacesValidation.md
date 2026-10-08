# DSL-authored interfaces validation — 2026-10-08

The initial abstract-signature/Blueprint Interface asset milestone is complete.

- Scripts/TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS. Creation,
  identical reapplication, Actor implementation, save/reopen and cook pass.
  The cooked probe completed six checks.
- StableId and ContractJson persist in UDasAuthoredInterfaceClass across cook.
  Independent interface compilation preserves them via the registered compiler.
- Membership, typed int query, Unicode ProcessEvent result, native out/ref
  copy-back and double array return precision/ownership pass in editor/cooked.
- Changed contract, unknown output slot and method body in an interface definition
  reject. Changed contracts are refused before modifying existing interface assets.
- Scripts/TestDSLTypeRegistry.ps1 -Interfaces: PASS, native-interface implementation
  and typed dispatch regression after authored /Game interface path support.
- Scripts/TestEditor.ps1: PASS, 439 checks and zero failures, including abstract
  source compilation, IR identity and direction checks.
- Scripts/TestDSLDirectTypesPIE.ps1: PASS, one success, zero warnings/failures
  (shared typed DSL/rollback regression).

Evidence transcripts in the Codex workspace work directory:
authored-interfaces-cooked.log, authored-interfaces-foundation.log,
authored-interfaces-native-regression.log and authored-interfaces-pie.log.
Shared registry Create/Reopen logs reflect the later native regression.
The cooked marker remains in logs/type-registry-cooked.log.

The pinned parser forbids annotations on abstract functions. Contract declarations
therefore use ue_interface class annotations, abstract signatures, inferred inout
references and interface-level Method.Parameter out names. See DSLAuthoredInterfaces.md.
No reader macro, pinned daScript source modification or automatic commit was used.

Next: event-only methods, additional authored type references and generated typed
wrappers. Structural contract/membership migrations remain deferred.


## Repeatable implements annotations — 2026-10-08

- Scripts/TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS.
- Short names, complete stable IDs, explicit native paths, two repeatable
  annotations, real multiple-interface Blueprint membership and native set
  dispatch pass. Annotation order is independent.
- Saved searchable identities resolve before loading assets on reopen.
- Missing/duplicate references, non-interface paths, unknown annotation arguments
  and overlapping method names reject. Method conflicts reject before mutation.
- Const native map/set inputs are accepted by the DSL owner contract check.
- Cooked query, Unicode, out/ref and array tests pass (six runtime checks).
- Scripts/TestEditor.ps1: 439 checks, zero failures.
- git diff --check passes. No ThirdParty changes; no commit was created.

Workspace evidence: work/repeated-interfaces-final.log and
work/repeated-interfaces-foundation-final.log.


## Annotated empty method declarations — 2026-10-08

- Scripts/TestEditor.ps1: PASS, 459 checks, zero failures.
- Empty ordinary methods compile for scalar/string/container results and void
  out/ref signatures. Per-method directions and metadata reach authoring IR.
- Nonempty bodies, missing/repeated annotations, unknown flags/slots, overlapping
  directions, missing ref direction, defaults, fields and static methods reject.
- A real direct script invocation reaches the declaration-only runtime trap;
  it cannot return a fictional default result.
- Legacy abstract source remains readable.
- Scripts/TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS. Native method
  Category/DisplayName/ToolTip, metadata-only reapplication without reinstance,
  independent compilation, save/reopen/reimport and actual packaged game pass.
- Existing signature changes still reject before mutation. Array ownership,
  Unicode and out/ref implementation dispatch pass in editor and cooked runtime.
- Evidence: workspace work/interface-stubs-foundation.log and
  work/interface-stubs-cooked.log. Shared packaged log: logs/type-registry-cooked.log.
