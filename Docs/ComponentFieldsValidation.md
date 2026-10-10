# Component fields validation — 2026-10-10

UE 5.8.3 Win64 Development, pinned daScript 0.6.4.

- @ue_component field creation: Create/Reopen/cooked PASS with all 12 shared runtime
  checks. Positive Actor and dual-component examples use imported field types.
- Field metadata/flags PASS: Category=Components, Unicode ToolTip, BlueprintVisible,
  BlueprintReadOnly, Edit and EditConst. Explicit Blueprint compilation retains
  these values; saving and loading in a new process retains them too.
- Negative metadata/type checks PASS: EditAnywhere/VisibleAnywhere conflict,
  BlueprintReadOnly/ReadWrite conflict, repeated marker, unsupported field type,
  and explicit initializer all reject.
- Compiler extension restores metadata from persistent SCS nodes after each layout
  generation. Component metadata participates in the declaration transport/equality;
  existing structural reimport guard remains. Legacy class annotations are preserved
  as explicitly labelled compatibility rejection fixtures.
- TestActorComponentPIE.ps1: actual PIE, one success, no warnings/failures/skips.
- TestEditor.ps1: 524 PASS, failures=0.
- GenerateBindings.ps1 -Check: PASS; native IR/module unchanged.
- TestBindingsGenerator.py: 84 PASS.
- git diff --check: PASS; ThirdParty unchanged; no commit made.

The final editor-only metadata/negative assertions were added after the cooked run;
runtime, collector and compiler extension are the same validated implementation.
Scope: component field declarations and property metadata, not component member
dispatch, per-instance defaults, native SceneComponents or structural migration.

Evidence: logs/actor-component-Create.log, actor-component-Reopen.log,
actor-component-cooked.log, actor-component-pie.log/report, smoke-editor.log,
bindings-export.log. Generated GUID assets/package output stay in the test project.
