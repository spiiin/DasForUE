# Source map/set validation — 2026-10-09

UE 5.8.3 source build, Win64 Development, pinned daScript 0.6.4.

- `C:\Python312\python.exe Scripts\TestBindingsGenerator.py`: 82 tests PASS.
  Checks cover nested source tables, declaration dependencies, const input
  iteration, invalid native type markers, unsupported object values and source
  Name keys. Existing native generation remains deterministic.
- `Scripts\TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked`: PASS.
  Create/save and editor reopen succeeded, including actual source-to-native
  and native-to-source calls against an independent Blueprint receiver.
  Wrong source ID is rejected before invocation. Game build, cook, archive and
  actual packaged game invocation succeeded.
- The generated authored-interface fixture exercises map/set fields in a nested
  struct and array elements, distinct entries, Unicode keys/elements, signed int
  keys, double and Name values, empty defaults and independent copies/mutation.
- `Scripts\TestEditor.ps1`: 520 PASS, zero failures. Six new native checks cover
  missing nested headers/values, duplicate map keys/set elements, parent-array
  resize removing pending hash slots and native output with holes after removal.
  Existing array/hash/object/GC and DSL checks remain green.
- `Scripts\GenerateBindings.ps1 -Check`: PASS. Fresh native IR and generated
  bindings match the checked-in artifacts.
- `git diff --check`: PASS. ThirdParty is unchanged; no commit was made.

Scope: source int/string map keys and set elements, scalar/Name map values,
inside mapped structs and struct-array elements. Hash values containing objects,
structs or containers and direct arrays-of-hashes remain unsupported by source
authoring/generation. General migrations remain deferred.

Evidence: logs/smoke-editor.log, logs/type-registry-Create.log,
logs/type-registry-Reopen.log, logs/type-registry-cooked.log,
logs/bindings-export.log. Temporary GUID-specific script probes were removed by
the test script; unrelated user projects/files were not changed.
