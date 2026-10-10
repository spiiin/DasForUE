# Native object/interface and math method validation — 2026-10-10

Source Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: PASS in Create, Reopen, actual PIE and packaged game.
The ordinary exercise_values function additionally checks:

- ActorRef argument/result and ref/out: previous preserves a live reference and
  current becomes null; nullable Actor echo returns an empty reference.
- Native DasEventInterface argument/result and ref/out, with a target Blueprint
  implementing Notify and ResetCounter. Previous remains valid, current is null.
- UEVector, UERotator and UETransform input/result and ref/out values, including
  vector components, rotator angles, transform translation and scale.
- Recovered failed vector call preserves the caller's current vector.
- Wrong nominal ObjectRef argument for ActorRef and UERotator for UEVector produce
  actual compiler errors; valid calls compile and execute.
- The preceding enum, nested struct and container checks continue to pass.

PIE Automation: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0;
EndPIE releases persistent contexts. Packaged marker: DAS_METHOD_VALUES_RESULT=PASS.

Regression checks: TestActorComponent.ps1 Create/Reopen PASS;
TestEditor.ps1 DAS_FOUNDATION_RESULT=PASS, failures=0;
TestBindingsGenerator.py 84 tests, OK; git diff --check PASS;
ThirdParty/daScript unchanged. GenerateBindings.ps1 -Check: PASS; the native
reflection IR and generated module need no replacement.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log, actor-component-Create.log,
actor-component-Reopen.log and smoke-editor.log.
Automation report: logs/method-values-pie-report/index.json.

These are native generated reference wrappers, not authored ue_object_ref types
inside reflected parameter declarations. That identity-resolution step,
automatic import/reference discovery and general migration remain separate work.
