# Additional reference field values validation — 2026-10-10

UE 5.8.3 Win64 Development, pinned daScript 0.6.4.

- TestActorComponent.ps1 -Cooked: PASS. Existing owner-reference example still
  succeeds in Create/Reopen and actual packaged execution with 12 shared checks.
- Additional enum/struct/object exercise PASS: direct field reads/assignments,
  enum member Active=7, struct Count=31, object reference to an Actor and independent
  original struct snapshot retaining Count=10.
- Incompatible struct/enum/object writes all reject. Final editor checks verify
  Count remains 31, Mode remains 7 and Target remains the original Actor after
  every rejected write. These test assertions were added after the successful cook;
  the runtime code is the same final version used in the packaged executable.
- Earlier scalar, alias, own-class peer, identity/lifetime, readonly and callback
  tests remain PASS. No new per-class runtime adapters are required.
- Actual Actor/Component PIE: one successful test, no warnings/failures/skips.
- TestEditor.ps1: 524 PASS, failures=0.
- GenerateBindings.ps1 -Check: PASS; native IR/module unchanged.
- TestBindingsGenerator.py: 84 PASS.
- git diff --check: PASS; ThirdParty unchanged; no commit made.

Scope: whole enum/struct/object fields with existing UE value representations.
Natural source-value mappings, nested mutation, containers and method dispatch
remain separate work. General structural migration stays deferred.

Evidence: logs/actor-component-Create.log, actor-component-Reopen.log,
actor-component-cooked.log, actor-component-pie.log/report, smoke-editor.log and
bindings-export.log. Only GUID test assets and ignored package output were generated.
