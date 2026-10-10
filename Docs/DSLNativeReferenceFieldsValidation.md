# Typed native reference field validation — 2026-10-10

Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: Create/Reopen/actual PIE/packaged PASS.

- NativeActor:ActorRef and NativeEvents:DasEventInterface begin null.
- Natural getters retain wrapper types; valid Actor/interface assignments and
  readback work in the ordinary exercise_values function.
- default<ActorRef> and default<DasEventInterface> clear the native properties.
- ObjectRef assignments to ActorRef/interface fields and readonly ActorRef
  writes fail compilation (native_reference_property_wrong_type_and_readonly_compile_errors).
- Internal getter/setter probes supply a mismatched declared native class path.
  Both fail, including a null setter; the existing live property remains intact.
- A shared C++ probe assigns a fresh Actor to NativeActor, destroys that Actor,
  then invokes ExpiredNativeFields on the still-live owner. Its getter returns
  an empty wrapper and the property can be cleared:
  PASS destroyed_native_reference_property_is_null.
- Earlier enum/struct/math/container/signature/ref/out checks remain passing.

PIE report: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
Packaged marker: DAS_METHOD_VALUES_RESULT=PASS.

Regressions: TestActorComponent.ps1 Create/Reopen PASS;
TestEditor.ps1 DAS_FOUNDATION_RESULT=PASS failures=0;
TestBindingsGenerator.py 84 tests OK;
GenerateBindings.ps1 -Check PASS; git diff --check PASS.
ThirdParty/daScript unchanged.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log, actor-component-Create.log,
actor-component-Reopen.log and smoke-editor.log.
Report: logs/method-values-pie-report/index.json.

There is one object property getter/setter implementation. Its generated calls
carry the declared class path. Generated component properties retain their
existing logical authored identity check because their physical property type
is a generated Blueprint component class. Ordinary authored-reference fields
remain a separate ownership/bootstrap stage.
