# Self container operations validation — 2026-10-10

Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: Create/Reopen/actual PIE/packaged PASS.

MutateSelfContainers verifies direct self operations:
- insert into a string set and a string/double map;
- table/set erase returns true for existing keys, false for a missing set key;
- array erase removes index 0 and preserves the former second double value;
- clear empties the array, map and set while root Total=3 is preserved.

Readonly root, incompatible set key and incompatible map value reject during
compilation: PASS self_container_operation_readonly_key_value_compile_errors.
Existing clone replacement, local-only converter generation, limit rejection,
struct/math/enum/reference/property/ref/out/destruction probes remain passing.

The helper invokes standard DAS operations on an owned source container, then
commits through existing native carrier/root snapshot adapters. Arguments are
evaluated before the native container read. Bool results publish after commit.

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

Scope excludes indexed assignments, push/pop, int64 array overloads, iterators
and foreign-view mutations. Earlier successful statements are not transactional.
