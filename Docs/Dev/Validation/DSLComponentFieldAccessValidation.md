# Component field access validation — 2026-10-10

UE 5.8.3 Win64 Development, pinned daScript 0.6.4.

- Create/Reopen/cooked: 14 shared checks PASS. Actor Blueprint methods invoke real
  component field getters/setters, write Bonus=20 then +=3, read 23 through a local
  alias and retain the other Actor's component default Bonus=10. Default is restored
  before existing lifecycle/world/delegate checks. Destroyed component access rejects.
- Compile-only negatives PASS: replacing BlueprintReadOnly component reference and
  accessing an unknown component field reject with explicit diagnostics.
- Component metadata/flags still survive explicit recompile and save/reopen.
- Actual Actor/Component PIE: one success, no warnings/failures/skips.
- TestEditor.ps1: 524 PASS, failures=0.
- GenerateBindings.ps1 -Check: PASS; native IR/module unchanged.
- TestBindingsGenerator.py: 84 PASS.
- git diff --check: PASS; ThirdParty unchanged; no commit made.

The synthetic game-world probe initially skipped Actor ProcessEvent because
AreActorsInitialized was false. InitializeActorsForPlay now establishes the normal
execution state, without bypassing AActor::ProcessEvent. The generated AST itself
then passed the field read/write checks. Compound receivers use one generated local
before evaluating the right-hand side.

Scope: component fields and local aliases in DSL bodies; method dispatch, rich
nested mutations and structural migration remain separate work.

Evidence: logs/actor-component-Create.log, actor-component-Reopen.log,
actor-component-cooked.log, actor-component-pie.log/report, smoke-editor.log and
bindings-export.log. Generated assets/package output remain in the test project.
