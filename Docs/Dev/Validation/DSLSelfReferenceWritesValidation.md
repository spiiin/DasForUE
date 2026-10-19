# Direct self reference member write validation — 2026-10-10

Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: Create/Reopen/actual PIE/packaged PASS.

- MethodPoint contains native Owner:ActorRef and Events:DasEventInterface,
  with implicit null defaults.
- ValueTarget.SetSelfReferences assigns its typed parameters directly to
  self.Settings.Point.Owner/Events. Self and external-view readback confirm
  valid Actor/interface values, then empty wrappers after assigning null.
- Existing Point.Count and Position.x remain intact.
- Wrong nominal ObjectRef values for ActorRef/interface members, readonly root
  writes and reference arithmetic fail compilation:
  PASS self_reference_wrong_type_readonly_arithmetic_compile_errors.
- The shared C++ probe writes a fresh Actor into the NativeActor property and
  the nested Point.Owner field, destroys it, and invokes ExpiredNativeFields.
  Both reads normalize to empty wrappers; the self setter clears the nested
  reference successfully. PASS destroyed_native_reference_property_is_null.
- Previous enum typed-leaf negative checks and all method/property/container/
  math/ref/out probes remain passing.

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

Object/interface and enum writes share a generated source-typed leaf helper.
Runtime reference transfer remains the existing snapshot member adapter; no
additional ownership or alternate property backend is introduced.
Scope excludes authored reference members, indexed mutations and foreign-view
nested writes.
