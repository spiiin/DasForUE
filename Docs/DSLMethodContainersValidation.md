# Container method validation — 2026-10-10

Source Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: PASS in Create, Reopen, actual PIE and packaged game.
The ordinary exercise_values function checks:

- int array input/result, ref growth, out replacement and an empty array result.
- string/double map input/result and ref/out values, including a deleted key.
- string set input/result, insert/erase and separate previous values.
- authored struct array input/result/ref/out with independent element contents.
- nested authored struct fields containing a struct array, map and set.
- nested fields/Unicode strings and the previous enum/struct direction checks.
- failed array and struct callees preserve caller containers after recover.
- wrong container element types produce actual compiler diagnostics, alongside
  the existing wrong enum and struct argument cases.

The runtime probe checks the ordinary function's full predicate; failure reports
identify array/map/set/struct-array/nested-struct groups and runtime errors.
PIE report: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
Packaged marker: DAS_METHOD_VALUES_RESULT=PASS through staged Pak/UFS sources.

The reflected input array initially appeared as inout/in. Unreal's Blueprint
physical container reference flags are now normalized for declared DAS inputs;
the call owns a separate snapshot and only explicit ref/out values are published.

Regression checks:

- TestActorComponent.ps1: Create/Reopen PASS, retaining ordinary property/method,
  scalar/opaque value, reentry, recursion/recovery and destruction checks.
- TestEditor.ps1: DAS_FOUNDATION_RESULT=PASS, failures=0.
- TestBindingsGenerator.py: 84 tests, OK.
- GenerateBindings.ps1 -Check: PASS; native IR/generated module are current.
- git diff --check: PASS; ThirdParty/daScript unchanged.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log, actor-component-Create.log,
actor-component-Reopen.log, smoke-editor.log and bindings-export.log.
Report: logs/method-values-pie-report/index.json.

Existing carrier type/size policies remain in force. No natural native-object/math
signature adapter, automatic import/reference discovery or general migration is
added by this stage.
