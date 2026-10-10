# Native math class field validation — 2026-10-10

Source Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: PASS in Create, Reopen, actual PIE and packaged game.

- UEVector and UERotator literal defaults survive Blueprint save and reopen.
- UETransform defaults to identity; an uninitialized vector defaults to zero.
- Self vector component assignment/addition and rotator component addition work.
- Whole transform assignment preserves translation and scale.
- Typed reference getters and whole setters preserve native math values.
- Assigning UERotator to a UEVector property and writing BlueprintReadOnly
  math fields produce compiler errors (math_field_wrong_type_and_readonly_compile_errors).
- Earlier enum, struct, container, object and authored-reference method checks pass.

PIE Automation: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
Packaged marker: DAS_METHOD_VALUES_RESULT=PASS.

Regression checks: TestActorComponent.ps1 Create/Reopen PASS;
TestEditor.ps1 DAS_FOUNDATION_RESULT=PASS, failures=0;
TestBindingsGenerator.py: 84 tests, OK;
GenerateBindings.ps1 -Check: DAS_BINDINGS_RESULT=PASS.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log, actor-component-Create.log,
actor-component-Reopen.log and smoke-editor.log.
Automation report: logs/method-values-pie-report/index.json.

Scope: ue_class fields only. Through another object's typed view, modify a
local value and assign the complete value back; in-place component writes
through that view are not claimed. Math members of authored ue_struct remain
a separate step.
