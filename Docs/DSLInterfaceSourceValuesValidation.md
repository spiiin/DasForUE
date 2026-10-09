# Source value/factory validation — 2026-10-09

- TestBindingsGenerator.py: PASS, 71 tests. Typed factory receiver selection and
  explicit source manifest registry/module/identity validation are covered alongside
  all existing generation/alias/default/owned-container/method facade tests.
- TestEditor.ps1: PASS, 509 checks, zero failures. Generated type-tag factory
  overloads compile and the existing runtime/interface/type/lifetime suite remains.
- TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS. Actual BPI reflection IR
  generates a mapped public module. The caller uses ue_interface(type<CounterContract>,
  target), InterfaceState.Active and normal InterfaceSettings.Count/Title fields.
  Query/Label/Stats and natural enum/Unicode struct roundtrip work after editor reopen
  and in the packaged game. Deliberately wrong stable ID rejects at module compilation
  before invocation; restoring the original source resumes successful calls.
- TestDSLTypeRegistry.ps1 -Interfaces -Cooked: PASS. Native reference construction
  uses the new typed factory; dot-call, copyback, owned containers, events and existing
  signature/reentry failure checks remain passing in editor and packaged game.
- GenerateBindings.ps1 -Check: PASS; native generation remains deterministic.
- git diff --check: PASS; ThirdParty unchanged; temporary test modules cleaned.

This stage maps source enums and primitive/string-field structs through an explicit
manifest and existing registry. Nested/container/name/object source struct fields and
container element conversions remain later extensions. Generic implicit conversion,
reflected interface storage, VM reentry and migrations are unchanged.

Evidence: workspace work/source-values-*.log. Syntax/configuration:
DSLInterfaceSourceValues.md. Changes are not committed.
