# Reflected interface-reference validation — 2026-10-09

UE 5.8.3 source build, Win64 Development, pinned daScript 0.6.4.

- `Scripts\TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked`: PASS.
  Create/save, editor reopen, game build/cook/archive and actual packaged run
  succeeded. Native interface fields have FInterfaceProperty and real interface
  pins. ProcessEvent and direct runtime roundtrips preserve Blueprint-only
  implementers whose native interface address is null.
- Eight native reference checks PASS in Create, Reopen and packaged run: exact
  UPROPERTY type, BP-only roundtrip/null pointer, direct runtime roundtrip,
  DSL field get/set, null values, nonimplementer rejection without write and
  destroyed Actor reads after GC.
- Four authored-interface-holder checks PASS in Reopen: Blueprint application,
  exact authored interface field class, runtime roundtrip and DSL field get/set.
  The generated ue_interface_ref stable-ID guard remains active. This holder is
  an editor probe; packaged reference coverage uses the native event interface.
- Generated source calls use an independent Blueprint receiver implementing the
  native event interface. EchoEvents nonnull/null calls and interface fields in
  nested structs/array elements succeed. Wrong source identity still rejects.
- `Scripts\TestEditor.ps1`: 524 PASS, zero failures. New GC tests verify native
  interface pointers, strong reflected ownership, weak VM wrappers and rejection
  of nonimplementers before field writes.
- `C:\Python312\python.exe Scripts\TestBindingsGenerator.py`: 84 tests PASS.
- `Scripts\GenerateBindings.ps1 -Check`: PASS. Fresh selected native IR and
  generated bindings match checked-in artifacts. `git diff --check` PASS.
- ThirdParty unchanged; no commit was made. Temporary GUID-specific scripts
  (including the authored holder) were removed by the test script.

Scope: typed native/authored interface fields and input/results, including
interface fields within authored structs. Interface out/ref, standalone interface
collections and VM reentry remain deferred. General migrations are unchanged.

Evidence: logs/smoke-editor.log, logs/type-registry-Create.log,
logs/type-registry-Reopen.log, logs/type-registry-cooked.log and
logs/bindings-export.log.
