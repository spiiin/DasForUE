# Nested self math writes validation — 2026-10-10

Source Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: PASS in Create, Reopen, actual PIE and packaged game.
ValueTarget.Settings is an authored MethodSettings struct property with a nested
MethodPoint.Position vector and an Orientation rotator. The probes verify:

- self.Settings.Point.Position.x += 4 commits x=5 to the real property.
- self.Settings.Point.Position.y = 8 commits y=8.
- self.Settings.Orientation.yaw *= 2 commits yaw=40.
- Independent reads in ReadNestedMathFields observe the changes and preserve
  other defaults (Total=3, Point.Count=2, identity Pose and readonly struct).
- FailNestedMath multiplies the finite x value by 1.0e308; the resulting infinity
  is rejected by recursive snapshot validation. After DAS recover, independent
  reads observe the previous valid x/y/yaw values.
- A readonly self struct root rejects nested vector component writes during
  compilation (PASS nested_math_readonly_self_compile_error).
- String assignment to a vector scalar component rejects during compilation
  (PASS nested_math_wrong_scalar_compile_error).
- Earlier fields/methods/native object/enum/struct/container/ref/out probes pass.

PIE Automation: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
Packaged marker: DAS_METHOD_VALUES_RESULT=PASS.

Regressions: TestDSLDirectTypesAsset.ps1 Create/Reopen PASS;
TestEditor.ps1 DAS_FOUNDATION_RESULT=PASS failures=0;
TestBindingsGenerator.py: 84 tests, OK;
GenerateBindings.ps1 -Check PASS; git diff --check PASS.
ThirdParty/daScript unchanged.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log,
direct-types-asset-Create.log, direct-types-asset-Reopen.log, smoke-editor.log.
Automation report: logs/method-values-pie-report/index.json.

Scope: nonindexed self Vector/Rotator scalar components inside authored structs.
Transform internals and indexed self container writes remain unsupported.
Natural source-type mapping for authored struct properties through another
object's typed view remains separate; these getters currently return UEStructValue.
A rejected individual snapshot update preserves the field; earlier successful
statements in the same function are not rolled back.
