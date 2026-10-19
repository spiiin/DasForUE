# Interface method layer validation — 2026-10-09

- GenerateBindings.ps1: PASS; native IR exported and facade module regenerated.
- TestBindingsGenerator.py: PASS, 67 tests. Additional cases verify nominal method
  receiver signatures, actual mutable ref/out parameters and result copyback,
  container result moves and deterministic deduplication of selected alias methods.
- TestEditor.ps1: PASS, 509 checks, zero failures. Existing generated interface
  null/forged/wrong-type guards and the broader runtime/type/annotation suite remain.
- TestDSLTypeRegistry.ps1 -Interfaces -Cooked: PASS. Native fixtures now call Query,
  Label, Stats, Weights, EchoMap/EchoSet and Notify/ResetCounter with dot syntax.
  Real Blueprint invocation verifies caller inout/out updates, owned container
  results, Unicode and actual void event state changes in editor and packaged game.
  Existing signature mismatch and VM reentry checks remain passing.
- TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS. Isolated module from actual
  BPI reflection now uses api.Query/Label/Stats/EchoState/EchoSettings, including
  automatic caller copyback, in editor reopen and packaged runtime. Existing scoped
  registry/native frame/ProcessEvent signature checks remain passing.
- GenerateBindings.py --check and git diff --check: PASS. ThirdParty unchanged.

Natural authored vs generated enum/struct representation, generic reference factory,
reflected interface storage and broader authoring-side ref/out remain separate work.
Code/API limits: DSLInterfaceMethodCalls.md. Evidence: workspace
work/interface-facades-*.log. Changes are not committed.
