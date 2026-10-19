# Direct self enum member writes validation — 2026-10-10

Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: Create/Reopen/actual PIE/packaged PASS.

ValueTarget.SetSelfEnum takes a MethodState parameter and assigns it directly
to self.Settings.Point.Mode. Tests verify Idle and Active readback through
both self and the external typed view, while Point.Count and Position.x retain
their values. Other existing snapshot/property/method/destruction checks pass.

Negative compilation probes reject:
- OtherMethodState.Idle, despite having the same numeric value as MethodState.Idle;
- the integer literal 7;
- a nested write through LockedSettings (readonly root);
- compound arithmetic on a nested enum member.
Marker: PASS self_enum_wrong_type_integer_readonly_arithmetic_compile_errors.

The source-typed generated helper checks the RHS nominal type before packing
to UEEnumValue. Runtime update reuses __ue_struct_with_enum_value and the
latest-snapshot whole-property commit; no alternate enum storage is introduced.

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

Scope: nonindexed self authored enum members with assignment. Foreign-view
nested writes, indexed self container mutations and enum arithmetic remain
outside this stage.
