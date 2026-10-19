# Native math authored struct validation — 2026-10-10

Source Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: PASS in Create, Reopen, actual PIE and packaged game.
The ordinary exercise_values function checks native members of MethodSettings
and its nested MethodPoint:

- Position vector defaults (1,2,3), Orientation rotator defaults (10,20,30).
- Pose without a source initializer becomes identity in both ordinary DAS
  construction and the reflected Unreal default.
- Math members inside array<MethodPoint> survive reflected input/result copying.
- ref/out changes Position.x and Orientation.yaw and publishes a new transform;
  previous values retain the original math values.
- Existing recovered-failure checks preserve caller struct values.
- Assigning UERotator to MethodPoint.Position produces an actual compiler error:
  PASS struct_math_wrong_type_compile_error.
- Existing primitive/enum/container/native-object/authored-reference checks pass.

PIE Automation: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
Packaged marker: DAS_METHOD_VALUES_RESULT=PASS.

Regressions: TestDSLTypesAsset.ps1 Create/Reopen PASS;
TestDSLDirectTypesAsset.ps1 Create/Reopen PASS;
TestEditor.ps1 DAS_FOUNDATION_RESULT=PASS failures=0;
TestBindingsGenerator.py 84 tests OK;
GenerateBindings.ps1 -Check PASS; git diff --check PASS.
ThirdParty/daScript unchanged.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log, types-asset-Create.log,
types-asset-Reopen.log, direct-types-asset-Create.log,
direct-types-asset-Reopen.log and smoke-editor.log.
Automation report: logs/method-values-pie-report/index.json.

The editor struct API uses a different default representation for builtin math
types than schema ImportText/ExportText. Creation converts at that boundary;
saved native default comparison uses the schema representation. An unchanged
editor default counts as success rather than requiring a mutating API call.

Scope excludes direct containers of math types and nested self math component
writes. Whole struct assignments and local native math member updates work.
