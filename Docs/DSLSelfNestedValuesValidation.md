# Whole nested self value validation — 2026-10-10

Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: Create/Reopen/actual PIE/packaged PASS.

ValueTarget.ReplaceSelfValues performs:
- self.Settings.Point = point (authored struct);
- self.Settings.Point.Position = vector;
- self.Settings.Orientation = rotation;
- self.Settings.Pose = transform;
- self.Envelope.Payload := settings (authored struct with containers).

Self readback and independent typed-view readback verify native math values,
Point.Note, enum member values, struct-array contents, map entries and set keys.
Root Total=3 and sibling Envelope.Sentinel=91 remain intact. Mutating a getter
copy's array element does not alter the native property.

Wrong authored struct, wrong vector/rotator/transform types, readonly root writes
and compound struct arithmetic fail compilation:
PASS self_nested_value_wrong_type_readonly_arithmetic_compile_errors.
Previous enum/reference/scalar/math/property/ref/out/destruction checks pass.

The first run correctly rejected plain = for a container-bearing source struct
under daScript's noncopyable-type rules. The final fixture uses := and the DSL
handles ExprClone through the common typed/snapshot commit path.

PIE report: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
Packaged marker: DAS_METHOD_VALUES_RESULT=PASS.
Regressions: TestActorComponent.ps1 Create/Reopen PASS;
TestDSLDirectTypesAsset.ps1 Create/Reopen PASS;
TestEditor.ps1 DAS_FOUNDATION_RESULT=PASS failures=0;
GenerateBindings.ps1 -Check PASS; git diff --check PASS.
ThirdParty/daScript unchanged.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log, actor-component-Create.log,
actor-component-Reopen.log, direct-types-asset-Create.log,
direct-types-asset-Reopen.log and smoke-editor.log.
Report: logs/method-values-pie-report/index.json.

Indexed self mutations and foreign-view nested writes remain separate.
Native snapshot identity/math validity checks and ordinary Unreal ownership
remain unchanged; there is no alternate nested struct storage path.
