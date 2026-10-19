# Universal accessor validation — 2026-10-10

UE 5.8.3 Win64 Development, pinned daScript 0.6.4.

- Standalone property syntax probe confirmed = setter and += dispatch with a const
  let-bound receiver through native daScript property operators.
- Imported ue_reference_functions ordinary functions now execute the six primitive/
  Name scalar operations, enum/struct/object value assignments and destroyed-reference
  read. Functions receive nominal references as normal DAS parameters; no ue_class
  or annotation is applied to those functions.
- Old FDSLAuthoredRefVisitor is removed, including local-name type tracking and
  field assignment/compound rewriting. Property generation uses inferred receiver
  types; factories are shared call macros. self component conversion remains a
  context bridge and uses the same resulting receiver operators.
- Compile-chain declaration scope handles recursive source dependencies and clears
  declaration cache at the outer boundary. Shared type-call normalization fixes
  short enum names in ordinary functions as well as methods.
- Create/Reopen/cooked PASS, including 14 runtime probe checks, metadata/recompile,
  readonly/unknown fields, wrong/forged/null/destroyed handles and own-class peers.
- Actual PIE: one success, zero warnings/failures/skips.
- TestEditor.ps1: 524 PASS, failures=0.
- GenerateBindings.ps1 -Check: PASS; native IR/module unchanged.
- TestBindingsGenerator.py: 84 PASS.
- git diff --check: PASS; ThirdParty unchanged; no commit made.

Scope: all currently supported reference property kinds in regular functions and
DSL bodies. self remains an Unreal-context bridge; nested/self value conversion
logic and low-level native interop APIs are not removed. New field-kind adapters,
authored method dispatch and general structural migrations remain separate work.

Evidence: actor-component Create/Reopen/cooked and PIE logs/report, smoke-editor.log,
bindings-export.log under logs/. Standalone probe and tool logs are in workspace work/.
