# Natural authored property values validation — 2026-10-10

Source Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: PASS in Create, Reopen, actual PIE and packaged game.
The ordinary exercise_values function verifies:

- api.Settings has MethodSettings, including nested MethodPoint/native math
  fields and supported array/map/set members.
- Local edits and whole assignment transfer vector components, unicode text,
  authored struct array values, double map entries and string set entries.
- An earlier getter result retains original independent fields and containers.
- Editing another getter's Point.Count without assigning back leaves the native
  property unchanged (api.Settings.Point.Count still equals 2).
- Readonly authored struct getters allow natural nested reads.
- api.State and api.LockedState expose MethodState; a writable enum can be set.
- OtherMethodState with the same numeric values rejects during compilation.
- Runtime source converters check exact reflected snapshot identities before
  reading fields or casting enum numbers. Direct guard probes reject a Vector
  snapshot where Rotator is expected and a MethodState snapshot where another
  enum is expected; both failures are recovered.
- Wrong whole struct types and readonly whole enum/struct writes reject during
  compilation: PASS source_property_wrong_type_and_readonly_compile_errors.
- Existing self writes, nonfinite update recovery, native math/object and
  enum/struct/container/ref/out method probes remain passing.

PIE Automation: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
Packaged marker: DAS_METHOD_VALUES_RESULT=PASS.

Regressions: TestActorComponent.ps1 Create/Reopen PASS;
TestDSLDirectTypesAsset.ps1 Create/Reopen PASS;
TestEditor.ps1 DAS_FOUNDATION_RESULT=PASS failures=0;
TestBindingsGenerator.py 84 tests OK;
GenerateBindings.ps1 -Check PASS; git diff --check PASS.
ThirdParty/daScript unchanged.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log, actor-component-Create.log,
actor-component-Reopen.log, direct-types-asset-Create.log,
direct-types-asset-Reopen.log and smoke-editor.log.
Automation report: logs/method-values-pie-report/index.json.

Source descriptors retain names/scopes only within a compile chain; exact source
types are resolved in the caller's imported module group, with asset validation.
There is one generated property accessor path. Explicit low-level source field
types remain explicit wrappers. In-place foreign nested writes and automatic
imports remain separate work.
