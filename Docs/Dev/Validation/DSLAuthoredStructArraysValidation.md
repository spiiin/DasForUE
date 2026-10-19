# Standalone authored struct-array validation — 2026-10-09

UE 5.8.3 source build, Win64 Development, pinned daScript 0.6.4.

- `C:\Python312\python.exe Scripts\TestBindingsGenerator.py`: 84 tests PASS.
  New tests verify natural-source array argument/result conversion and ref copyback
  after output conversion. Existing deterministic bindings remain unchanged.
- `Scripts\TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked`: PASS.
  The authored interface now has 20 methods. Create/save, editor reopen, metadata
  updates, source identity checks, game build/cook/archive and packaged execution
  all succeeded.
- Ten native array checks PASS in Create, Reopen and packaged run: Actor/ProcessEvent
  array return, independent native copy, return without input, out/ref copyback,
  exact reflected carrier type, direct runtime transport, wrong snapshot identity,
  a different authored struct element type and a 4,097-element snapshot rejection.
- Generated interface calls against an independent plain Blueprint receiver verify
  empty and two-element source arrays, distinct values, nested map/set data, Name,
  UObject handles and independent mutation of returned storage. A wrong source ID
  still rejects during compilation before invocation.
- `Scripts\TestEditor.ps1`: 520 PASS, zero failures.
- `Scripts\GenerateBindings.ps1 -Check`: PASS. Fresh native IR and generated module
  match checked-in artifacts. `git diff --check` PASS; ThirdParty unchanged.
- No commit was made. Temporary GUID-specific daScript probes were removed by the
  existing test script; other user projects/files were not touched.

Scope is authored ue_struct array parameters/results and out/ref, using the existing
registry/folder and bounded snapshot contracts. Cached carriers reject reflected
layout drift; structural migrations are still deferred. Wider standalone enum/object
arrays and reflected interface storage remain subsequent work.

Evidence: logs/type-registry-Create.log, logs/type-registry-Reopen.log,
logs/type-registry-cooked.log, logs/smoke-editor.log, logs/bindings-export.log.
