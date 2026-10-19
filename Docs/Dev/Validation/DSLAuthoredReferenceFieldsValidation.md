# Authored reference field validation — 2026-10-10

Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: Create/Reopen/actual PIE/packaged PASS.

- ValueTarget has Partner:ValueTargetRef and readonly LockedPartner, implicit null.
- External typed-view writes store a self reference; its getter returns a usable
  ValueTargetRef and EchoState dispatch succeeds through it.
- SetSelfPeer uses direct self.Partner assignment with a source-typed parameter;
  null clearing and live re-assignment work.
- A forged view carrying a live ValueDriver handle (same native host, wrong ID)
  rejects before setter publication; the previous Partner remains valid.
- Wrong ActorRef wrapper writes through self/foreign views and readonly Partner
  writes fail compilation (authored_field_wrong_wrapper_and_readonly_compile_errors).
- The shared native probe assigns ValueDriver directly to Partner via FProperty,
  bypassing the typed setter. A subsequent typed getter rejects its authored ID;
  recovery clears the field and native readback confirms null:
  PASS native_assigned_wrong_authored_field_rejected_and_cleared.
- Previous container/math/struct/enum/reference/method and destruction probes pass.

PIE report: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
Packaged marker: DAS_METHOD_VALUES_RESULT=PASS.
Regressions: TestActorComponent.ps1 Create/Reopen PASS;
TestEditor.ps1 DAS_FOUNDATION_RESULT=PASS failures=0;
GenerateBindings.ps1 -Check PASS; git diff --check PASS.
ThirdParty/daScript unchanged.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log, actor-component-Create.log,
actor-component-Reopen.log and smoke-editor.log.
Report: logs/method-values-pie-report/index.json.

Scope: nullable class fields using common native host storage and authored_id,
with ordinary Unreal property ownership. Physical pins are generic DAS hosts;
concrete generated Blueprint class pins and authored struct reference members
remain separate. Identity migration remains deferred.
