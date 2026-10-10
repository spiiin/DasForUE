# Source struct reference validation — 2026-10-09

UE 5.8.3 source build, Win64 Development; pinned daScript 0.6.4.

- `C:\Python312\python.exe Scripts\TestBindingsGenerator.py`: 78 tests PASS.
  New checks cover Name/object conversions, missing/wrong object class,
  interface rejection, module-qualified internal references and preservation
  of shadowing locals. Existing generation remains deterministic.
- `Scripts\TestEditor.ps1`: 514 PASS, zero failures. Five new snapshot checks
  cover nested structs/array elements, strong UObject retention across GC,
  independent copy/update, incompatible class rejection, reclamation releasing
  references and destroyed Actor reads after GC.
- `Scripts\TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked`: PASS.
  Types and registry create/save, independent editor reopen, generated source
  facade, deliberate wrong-source-ID rejection, game build, cook and actual
  packaged game run all succeeded. The fixture checks suffixed Name values,
  None/null defaults and Actor/Object fields at nested/array positions.
- `Scripts\GenerateBindings.ps1 -Check`: PASS. Fresh native reflection IR
  and generated module match the checked-in artifacts.
- `git diff --check`: PASS; ThirdParty remains unchanged. No commit was made.

The tested object fields are explicit reflected native UObject properties.
Source variables carry weak typed handles; context snapshots/native fields
retain object identity rather than deep-copying UObjects. Unimplemented weak,
soft, lazy, class and interface property adapters remain rejected, as do native
opaque reference callbacks and object arrays inside authored source structs.

Relevant logs: logs/smoke-editor.log, logs/type-registry-Create.log,
logs/type-registry-Reopen.log, logs/type-registry-cooked.log and
logs/bindings-export.log. Temporary GUID-specific daScript probe files were
removed by the test script; unrelated projects/files were not touched.
