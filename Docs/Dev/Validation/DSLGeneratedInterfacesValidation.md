# Generated native interface validation — 2026-10-09

- GenerateBindings.ps1 and GenerateBindings.ps1 -Check: PASS. Actual reflection
  export and deterministic regeneration of the eight selected native methods.
- TestBindingsGenerator.py: PASS, 58 tests. Interface kind marker, native identity,
  declared-signature call generation, nominal receivers and alias/helper collision
  policy, plus existing classes/enums/structs/containers/defaults coverage.
- TestEditor.ps1: PASS, 499 checks, zero failures. New source cases cover null
  reference membership, null dispatch, nonimplementer conversion, forged references
  and compile-time rejection of another interface's nominal type.
- TestDSLTypeRegistry.ps1 -Interfaces -Cooked: PASS. Editor asset creation, saved
  class/map reopening and packaged game. Generated calls execute against a plain
  Blueprint receiver in editor creation and in the packaged game: Query, Unicode
  Label, owned double arrays, multiple out/ref results, Unicode map, set, Notify
  and ResetCounter. EventCount proves the void events actually execute. Exported
  interface signature mismatch and active VM reentry reject. Existing manual
  typed frames and native C++ interface dispatch checks remain passing.
- TestBindingsPolicy.py: PASS, 9 exports. Explicit/rule selection, exclusion,
  defaults, inherited methods, deprecated/internal policy and malformed selections.
- TestBindingsExport.ps1: PASS. Unsupported required signature preserves the
  previous output instead of publishing partially validated bindings.
- git diff --check: PASS. ThirdParty remains unchanged.

Evidence: workspace work/generated-interfaces-*.log, including the copied game
log. Source API and limitations: DSLGeneratedInterfaces.md. General migrations,
VM reentry, authored BPI wrapper generation and reflected interface storage remain
separate work. Changes are not committed.
