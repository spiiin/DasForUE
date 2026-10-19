# Direct authored types validation — 2026-10-07

Environment: pinned daScript 0.6.4, source Unreal Engine 5.8.3,
DasForUESmoke Win64 Development / Editor. No commit was created for this step.

- TestDSLDirectTypesAsset: Create and Reopen both PASS. The same source declares
  types and the class. Typed Blueprint fields, enum sparse value 42, ProcessEvent
  setters/results, mixed frame result 142, Unicode strings and defaults PASS.
- Direct `self.Config.Count += amount` updates the native property and returns
  103. Direct string leaf write PASS. A RHS that changes Title preserves that
  change while updating Count; validated in both processes.
- Saved enum 42 and struct Count=100/Title="Owned 世界" are checked before new
  calls after reopening the map. GC and idempotent class reimport PASS.
- Enum value drift, struct field type drift, struct default drift, nonempty class
  constructor defaults and readonly struct writes reject during compilation.
  Invalid enum 255 rejects before property mutation.
- TestDSLDirectTypesPIE: one success, zero warnings/failures/not-run. Mixed frame
  result 119, typed field setters, wrong enum identity refusal, post-compile
  structural rollback, last-good callback, GC and EndPIE cleanup PASS.
- TestEditor: 416 foundation checks, failures=0, PASS.
- TestPackaged: build/cook/stage/archive/launch PASS; 416 foundation checks,
  failures=0. The game target compiles the direct adapters and stages the example
  and ue_dsl helpers. The generic packaged foundation map does not cook/load the
  GUID direct-types fixture: direct authored-type invocation is verified in
  editor commandlets and PIE, not yet end-to-end in a cooked game.
- git diff --check PASS. Disposable GUID source files are removed by test scripts.

Evidence: logs/direct-types-asset-Create.log, direct-types-asset-Reopen.log,
dsl-direct-types-pie-report/index.json, smoke-editor.log, smoke-packaged.log.
Workspace transcripts: work/direct-types-asset-final.log, direct-types-pie.log,
direct-types-editor.log and direct-types-packaged.log.

Bounds and next work: DSLDirectTypes.md and Plan.md. Explicit type_folder and
loaded, immutable authored enum/struct assets remain required.

Stable-ID resolution and actual cooked direct-type invocation are now validated
in DSLTypeRegistryValidation.md.
