# Authored struct enum member validation — 2026-10-10

Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: Create/Reopen/actual PIE/packaged PASS.

MethodPoint.Mode uses MethodState.Active=42; MethodState.Idle=7 and no zero member.
- Enum default survives native struct creation, Blueprint save/reopen and cook.
- MethodSettings contains MethodPoint directly and in array<MethodPoint>.
- EchoSettings preserves Active in both places; ChangeSettings ref/out publishes
  Idle in current values while previous values retain Active.
- A natural property getter/local edit/whole setter roundtrip transfers Mode
  independently of previous copies and struct array elements.
- OtherMethodState with identical values rejects on assignment to Mode:
  PASS struct_enum_same_values_wrong_type_compile_error.
- Previous math/container/object/interface/property/ref/out and destruction
  probes remain passing.

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
Report: logs/method-values-pie-report/index.json.

The initial implementation exposed an editor API issue: AddVariable compiled
an enum field with zero before its named default was assigned, logging INVALID
for a 7/42 enum. Final creation seeds an enum variable description, metadata and
the named default before its first compile. Final tests complete with zero
commandlet errors; the intermediate failure is not counted as a passing run.

Scope: same-module authored enums, explicit named defaults and whole-value/local
member operations. Direct self enum leaf writes and standalone enum containers
remain separate work.
