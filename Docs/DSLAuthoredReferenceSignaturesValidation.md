# Authored reference signature validation — 2026-10-10

Source Unreal Engine 5.8.3, pinned daScript 0.6.4, Win64 Development.

- TestMethodValues.ps1 -Cooked: Create/Reopen/actual PIE/packaged PASS.
  ValueTarget declares methods taking/returning its own shared ValueTargetRef
  before its Blueprint exists. Echo, ref/out and null values execute correctly.
  A forged view containing the driver's same-host but wrong authored instance
  rejects during packing and is recovered by the caller.
- The type model preserves authored_id=methodvalues.ValueTarget alongside native
  host storage through JSON roundtrip; the commandlet verifies both the argument
  and return descriptor. Native/model type identities remain distinct.
- Actual PIE Automation: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
- TestActorComponent.ps1 -Cooked: Create/Reopen/packaged PASS, including
  ScoreComponent as parameter/result and access to the returned view's Bonus.
- TestEditor.ps1: DAS_FOUNDATION_RESULT=PASS, failures=0.
- TestBindingsGenerator.py: 84 tests, OK.
- GenerateBindings.ps1 -Check: PASS; native reflection IR/module remain current.
- git diff --check: PASS. ThirdParty/daScript unchanged.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log, actor-component-Create.log,
actor-component-Reopen.log, actor-component-cooked.log, smoke-editor.log and
bindings-export.log. Report: logs/method-values-pie-report/index.json.
One staging attempt required UAT's automatic Zen readiness restart/retry; it
completed successfully. No Windows Zen service was installed or modified.

Limit: Blueprint pins use the common native DAS host class, with logical authored
identity enforced by source converters. Concrete Blueprint-class pins/catalog
bootstrap, authored UPROPERTY ownership and general migration remain separate.
