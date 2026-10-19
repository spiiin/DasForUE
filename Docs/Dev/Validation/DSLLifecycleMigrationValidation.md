# Lifecycle/delegate migration validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, Win64 Development.

- TestDSLLifecycleMigrationPIE.ps1: succeeded=1, succeededWithWarnings=0,
  failed=0, notRun=0. Root Blueprint Actor plus independent native publisher and
  listener. Both self and peer subscription declarations, including unchanged
  listener ownership with a replaced publisher.
- Remove and restore each exercised successful migration and injected failure
  after compilation. Both rollback paths retained lifecycle/handler state, remapped
  sources and installed one proxy per declaration without duplicate delivery.
- Field rename and ordinary structural reimport retained the same lifecycle,
  BeginPlay/EndPlay flags, peer links and independent handler counters.
- Explicit idle preservation scope detached proxies, blocked persistent and
  one-shot VM calls, suppressed compilation teardown, refused nested scopes and
  resumed without lifecycle replay or duplicate subscriptions.
- Call frames created in a callback expired on callback exit; a later invocation
  refused the stale token. There is no cross-invocation cache to migrate.
- A later editor frame delivered real Tick to the replacement Actor. GC retained
  valid proxies. Real Destroy emitted script EndPlay once to the external listener,
  removed the destroyed owner's subscriptions and left the remaining listener alive.
  EndPIE released all remaining contexts/proxies and native end notifications ran once.
- Existing TestDSLPIE, TestDSLRenameScopePIE, TestLifecyclePIE, TestDelegatesPIE,
  TestDSLArchiveFieldsPIE and TestDSLNumericConvertPIE regressions PASS. Each reported
  exactly one successful automation test without warnings.
- TestDSLArchiveFieldsAsset and TestDSLNumericConvertAsset: five-process
  save/reopen migrations PASS with the shared preservation scope enabled.
- TestEditor: 397 foundation checks, failures=0, PASS.
- TestPackaged: native rebuild/cook/stage/archive/launch, 397 foundation checks,
  failures=0, PASS. Editor preservation hooks remain editor-only.

Disposable GUID source files were cleaned up; fixture assets are confined to the
smoke project. No user projects were migrated. Arbitrary external compilation,
general rollback for structural reimport/rename and expanded asset scopes remain
separate tasks; this stage does not claim general transactions or persistent
UFunction frame caching.
