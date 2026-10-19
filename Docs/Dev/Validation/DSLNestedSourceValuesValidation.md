# Nested source values validation — 2026-10-09

- TestBindingsGenerator.py: PASS, 74 tests. New cases cover recursive owned nested
  source struct/array conversion, missing child mapping and rejected direct nested
  arrays. Existing generation/defaults/directions/interface/factory cases remain.
- TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS. Registry now contains
  enum + two structs. InterfaceSettings contains InterfaceDetails and its array.
  Natural interface call roundtrips nested int/Unicode values, distinct two-element
  arrays, mutations confined to the returned copy, repeat calls and empty arrays
  preserving nested defaults. Actual BPI export, registry ID checks, source converters,
  save/reopen and packaged execution all participate.
- TestEditor.ps1: PASS, 509 checks, zero failures. Existing required array-leaf,
  type/bounds/expiry, enum, struct and interface transport cases remain passing after
  bounded recursive call-path validation. No nested hash/object capability was added.
- GenerateBindings.ps1 -Check: PASS; existing native module remains reproducible.
- git diff --check: PASS; ThirdParty unchanged, transient generated modules cleaned.

Runtime signature description now permits arrays in reflected structs. Invocation
validates concrete wildcard paths with separate input/output traversal budgets and
retains pre-call object guards and post-call output checks. Generated readers/converters
own container data and move structures with arrays. Limits and remaining capabilities:
DSLNestedSourceValues.md. Evidence: workspace work/nested-source-*.log.
Changes are not committed.
