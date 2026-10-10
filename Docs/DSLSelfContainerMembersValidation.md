# Whole self container member validation — 2026-10-10

Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: Create/Reopen/actual PIE/packaged PASS.

ReplaceSelfContainers replaces Items (array<MethodPoint>), Weights
(table<string;double>) and Tags (table<string> set) through self clone assignment.
- Native readback matches source struct-array values, map entries and set keys.
- Mutating a getter copy's array/map/set does not change the property or inputs.
- Empty replacements clear all three members while siblings remain intact.
- Wrong container kinds, wrong array element types, map/set mismatch and readonly
  root writes reject during compilation:
  PASS self_container_wrong_type_element_readonly_compile_errors.

ReplaceLocalSamples uses an array<double> member and local arrays; no reflected
method signature uses array<double>. Its standalone carrier converters are
generated from the member assignment. Changing the source array after commit
leaves stored values independent. A candidate of 4097 elements rejects under
the existing 4096-element bound; recover observes the prior two values intact.
Previous struct/math/enum/reference/property/method/ref/out probes pass.

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

Runtime replacement uses the existing nested snapshot adapter with an exact
native carrier/Values SameType check, owned CopyCompleteValue and whole-root
snapshot validation before commit. There is no live mutable container view.
Indexed mutation, new container shapes and direct class containers remain
separate work.
