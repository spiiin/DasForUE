# Extended out/ref validation — 2026-10-09

UE 5.8.3 source build, Win64 Development, pinned daScript 0.6.4.

- `Scripts\TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked`: PASS.
  Interface collection (28 methods), Create/save, Reopen, game build/cook/archive
  and actual packaged game invocation all succeeded.
- Nineteen new native checks PASS in Create, Reopen and packaged execution:
  ProcessEvent and direct runtime pairs for Actor/interface references, authored
  enum/struct, vector/rotator/transform; original struct input remains unchanged;
  wrong enum/struct outputs publish no output set; panic publishes none; Blueprint
  abort before copyback preserves both caller fields after local mutations.
- Natural source facades for RefState/RefSettings are called through an independent
  Blueprint receiver. Mapped outputs are converted before caller updates, owned
  containers survive copyback, and modifying the previous struct does not affect
  the inout struct or original input. Existing wrong source identity checks remain.
- `Scripts\TestEditor.ps1`: 524 PASS, zero failures. Previous object/math reference
  rejection fixtures are now positive annotation/direction tests; remaining invalid
  constraint/type/default/body fixtures still reject.
- `C:\Python312\python.exe Scripts\TestBindingsGenerator.py`: 84 tests PASS.
- `Scripts\GenerateBindings.ps1 -Check`: PASS. Selected native IR and generated
  module remain unchanged. `git diff --check` PASS; ThirdParty unchanged.
- No commit was made. The existing test script removed temporary GUID-specific
  scripts; unrelated projects/files were not changed.

Output slots are independent with declaration-order copyback. VM reentry and
general structural migrations remain deferred; container schema limits are unchanged.

Evidence: logs/type-registry-Create.log, logs/type-registry-Reopen.log,
logs/type-registry-cooked.log, logs/smoke-editor.log and logs/bindings-export.log.
