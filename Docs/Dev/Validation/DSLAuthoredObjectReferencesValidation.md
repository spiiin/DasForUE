# Authored reference validation — 2026-10-10

UE 5.8.3 source build, Win64 Development, pinned daScript 0.6.4.

- Actor/Component Create/Reopen/cooked: PASS. Component Tick uses a typed owner
  reference and actor.Score += 1; the shared 12-check probe verifies the live Actor
  score, lifecycle, methods/ref parameters, isolation, delegates and cleanup.
- Six additional final Create checks PASS: existing imported class receives cooked
  identity without recompilation; all six scalar field kinds roundtrip with aliases
  and compound writes; wrong class and manually forged view reject; null/destroyed
  references reject (including a stored typed global); readonly/unknown fields
  reject at compilation; another instance of the same authored class changes while
  the receiver's own Score remains unchanged.
- TestActorComponent.ps1 -Cooked: PASS with the final runtime/macro code. Subsequent
  editor-only legacy identity initialization was checked with final Create/Reopen.
- TestActorComponentPIE.ps1: actual PIE PASS, one test, no warnings/failures/skips.
- TestEditor.ps1: 524 PASS, failures=0.
- GenerateBindings.ps1 -Check: PASS; native IR/module unchanged.
- TestBindingsGenerator.py: 84 PASS.
- git diff --check: PASS; ThirdParty unchanged. Changes are not committed.

The Name test found an incorrect copy-return binding for a uint64-backed token;
the final implementation uses the same POD return ABI as existing Name getters.
Reference source reads are captured in compilation source snapshots. Current-class
field declarations are exposed during rewrite so own-type references avoid eager
recursive source compilation; reference definitions are resolved on use.

Limits: direct primitive/Name fields in DSL class methods/callbacks. Reference
parameters in reflected signatures, object/enum/struct/container fields, nested
mutation, component member access, method dispatch and native views remain separate.
General migration policies remain deferred.

Evidence: logs/actor-component-Create.log, actor-component-Reopen.log,
actor-component-cooked.log, actor-component-pie.log/report, smoke-editor.log and
bindings-export.log. GUID test assets and package output belong to the test project.
