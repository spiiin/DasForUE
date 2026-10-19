# Shared type and annotation rules validation — 2026-10-08

- TestEditor.ps1: PASS, 476 checks, zero failures.
- TestDSLTypeRegistry.ps1 -TypeSyntax -Cooked: PASS. Source aliases and qualified
  stable IDs resolve through the selected registry; native parent shorthand,
  save/reopen/reimport and raw enum/struct runtime roundtrips pass in editor/game.
- Wrong kinds, missing scoped names, ambiguous registry short names and direct
  enum/struct annotation conflicts reject before applying a changed declaration.
- TestDSLTypeRegistry.ps1 -OutRef -Cooked: PASS, native tuple direction lists and
  existing copy-back/ownership/error behavior pass in editor and packaged runtime.
- Tuple and legacy CSV definitions produce identical canonical method contracts.
- Invalid/empty/unknown/overlapping direction list items and repeated ue_function,
  scalar constraint/callable options reject in compiler probes.
- TestDSLTypeRegistry.ps1 -AuthoredInterface: PASS. Tuple method annotations,
  metadata updates, multiple interfaces, stable identities and reopen pass.
- Explicit registry storage/hard references/GUID validation are retained.
- No ThirdParty modifications or commits were made. General migration work remains
  deferred and the parent backend still requires direct native Actor inheritance.

Workspace evidence: work/dsl-rules-foundation-final.log,
work/dsl-rules-registry-final.log, work/dsl-rules-outref.log and
work/dsl-rules-interfaces.log. Registry logs in the repo are shared/overwritten.
