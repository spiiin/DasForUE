# Authored method call validation — 2026-10-10

Engine: source Unreal Engine 5.8.3, pinned daScript 0.6.4, Win64 Development.

- TestActorComponent.ps1 -Cooked: PASS. New GUID Blueprint/Component assets were
  created, saved, reopened and cooked. The packaged executable passed the same
  26 shared runtime checks as the Create and Reopen commandlets.
- Typed calls cover result/void, ref/out copyback, chained actor.Component.Method,
  primitive values, Name, enum, struct snapshots and object handles.
- An ordinary DAS function invokes the same method overload. Actor/Component
  mutual calls and same-context calls preserve the interpreter and output frame.
- Failure recovery preserves ref values. 300 recovered failures do not exhaust
  call frames. Recursion limit rejection is followed by a successful call.
- A destroyed Component method call rejects safely. World cleanup and existing
  instance/world isolation, callbacks and subscription checks continue to pass.
- A positive compile-only method fixture passes. Four negative fixtures produce
  actual compiler diagnostics for unknown methods, type/count mismatch and an
  immutable ref argument; they do not merely fail an entry-point contract check.
- TestActorComponentPIE.ps1: PASS. Actual PIE runs method/ref/out/free-function,
  value marshalling and mutual reentry checks, then destroys the objects and ends
  PIE. Automation report: succeeded=1, warnings=0, failed=0, notRun=0.
- TestEditor.ps1: DAS_FOUNDATION_RESULT=PASS, failures=0.
- TestBindingsGenerator.py: 84 tests, OK.
- GenerateBindings.ps1 -Check: PASS; reflected native IR and generated native
  module are current without regeneration changes.
- git diff --check: PASS. ThirdParty/daScript is unchanged.

Logs: logs/actor-component-Create.log, actor-component-Reopen.log,
actor-component-cooked.log, actor-component-pie.log, smoke-editor.log and
bindings-export.log. PIE report: logs/actor-component-pie-report/index.json.
The commandlet logs retain ambient engine ACL asset warnings and the existing
callback-only synthetic-world cleanup warning; the shared runtime checks pass.

Container parameters and automatic natural source-value wrapper mappings remain
explicitly outside this adapter. General structural migration remains deferred.
