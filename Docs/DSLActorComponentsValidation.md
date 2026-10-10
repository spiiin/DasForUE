# Authored Component validation — 2026-10-09

UE 5.8.3 source build, Win64 Development, pinned daScript 0.6.4.

- TestActorComponent.ps1 -Cooked: PASS. BP_Behavior is emitted from
  ue_class(parent="ActorComponent"), attached to the DSL Actor's SCS and saved
  with its map. Create/Reopen/actual packaged execution all pass 12 runtime checks.
- New checks cover the generated Component class, reflected Bonus default=10,
  Evaluate(5)=15 and Accumulate(ref 7)=17 with return=17 via ProcessEvent.
  Existing world isolation, owner delegates, Tick delta, destruction, new sessions
  and world cleanup checks remain green. Create also proves unchanged declaration
  reimport keeps the same generated UClass without Blueprint recompilation.
- TestActorComponentPIE.ps1: PASS. One actual Unreal Automation PIE test succeeded
  with no failures, skipped tests or warnings, using the authored component class.
- TestEditor.ps1: 524 PASS, failures=0.
- GenerateBindings.ps1 -Check: PASS; native IR/module unchanged.
- TestBindingsGenerator.py: 84 PASS.
- git diff --check: PASS; ThirdParty unchanged. Changes are not committed.

The first packaged runs exposed an omitted new script in the dependency receipt.
DasRuntime.Build.cs now explicitly enumerates Content/Das files and registers them
as runtime/external dependencies. The packaged probe checks that its new script is
in the receipt before cooking. The final packaged run succeeded with staged script
loading, rather than relying on the source tree.

Current reimport support is body-only. The normal import path rejects Component
declaration/default migration before mutation; Actor migration behavior is unchanged.
Authored-parent inheritance, SceneComponent specialization, component composition
DSL, arbitrary virtual overrides and VM reentry remain separate work.

Evidence: logs/actor-component-Create.log, actor-component-Reopen.log,
actor-component-cooked.log, actor-component-pie.log and its report, smoke-editor.log,
bindings-export.log. GUID test assets stay under the ignored test project's
Content/DasActorComponent; package output is Build/ActorComponentPackaged/Windows.
