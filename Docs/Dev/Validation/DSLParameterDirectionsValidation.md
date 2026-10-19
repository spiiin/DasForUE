# Parameter-local directions validation — 2026-10-09

- TestEditor.ps1: PASS, 488 checks, zero failures.
- Mutable references infer inout; ue_out marks output-only; optional ue_ref has
  an identical contract. Single-reference scalar methods retain source parameter
  name/stable ID instead of legacy Input rewriting.
- Value/const-ref markers, repeated/conflicting/false/non-boolean/unknown metadata
  and parameter-versus-function direction conflicts reject.
- Preferred parameter-local example and the existing legacy CSV fixture produce
  equivalent canonical contracts. The new policy also permits an unmarked
  mutable reference in an ordinary interface stub.
- TestDSLTypeRegistry.ps1 -OutRef -Cooked: PASS. The actual generated class source
  uses parameter-local directions, with scalar/container copy-back, output-only
  initialization, failure publication behavior, save/reopen and packaged runtime.
- TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS. Parameter-local Stats,
  ordinary functions/events, metadata, native dispatch, live body reload and game
  launch remain compatible.
- Primitive/container capability and existing native transport/ownership limits
  are unchanged. No parser/reader/ThirdParty changes were needed.

Workspace evidence: work/parameter-directions-foundation.log,
work/parameter-directions-cooked.log and work/parameter-directions-interfaces.log.
