# Authored registry/interface binding validation — 2026-10-09

- TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS. Isolated value assets,
  registry, 16-method authored BPI and DSL implementation, save/reopen/recompile,
  metadata/body reimport and packaged execution. EchoState and EchoSettings use
  concrete declaration types and registered identities; native frame and Blueprint
  ProcessEvent verify the enum and Unicode struct values in editor and game.
- The same script exports five actual authored interface methods, generates an
  isolated public module and tests Query/Label, multiple inout/out results, enum
  symbol conversion and owned struct transfer on a plain Blueprint receiver after
  editor reopen and in the packaged game. Registry is retained by an Actor hard
  reference; implementing/receiver class signatures retain type/interface references.
- TestEditor.ps1: PASS, 509 checks, zero failures, including the previous native
  interface, type, annotation, direction, lifetime and marshalling regressions.
- TestBindingsGenerator.py: PASS, 64 tests. Six additional cases cover authored
  identity metadata, missing/invalid IDs, short names, module isolation/injection
  and refusal of authored identity on a native interface marker.
- GenerateBindings.ps1 -Check: PASS; native checked-in generation stays identical.
- TestBindingsPolicy.py: PASS, 9 exports. Existing native selection policy preserved.
- git diff --check: PASS; ThirdParty unchanged; generated temporary sources removed.

Compiler placeholders now use a real declared enum member (no zero requirement)
and normal struct initialization. Saved BPI signature UFunctions are reconstructed
in memory by the editor commandlet before reflection export; no Blueprint save or
contract mutation is performed by export. Authored binding annotations validate
the actual loaded generated class and its stable ID.

Evidence: workspace work/authored-interface-registry*.log. Supported workflow and
remaining limits: DSLAuthoredInterfaceRegistryBindings.md. Changes are not committed.
