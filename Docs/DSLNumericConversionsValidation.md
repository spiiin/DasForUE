# Combined numeric graph/conversion/rollback validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, Win64 Development.

- TestDSLNumericConvertAsset.ps1: five separate processes Create/Migrate/Reopen/NarrowFloat/ReopenFloat PASS. Parent/child defaults, saved map values, float->int nearest rounding, double/int->float explicit precision rounding, precision refusal without policy, connected getter/setter and literal conversion, existing adapter preservation across subsequent migrations, save/reopen and idempotence.
- Injected failure after graph mutation/compilation: original descriptors, native values, nodes, pin types/defaults/links and metadata restored. Both initial conversion and subsequent float narrowing rollback PASS; original graph execution still works.
- TestDSLNumericConvertPIE.ps1: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0. Live graph execution, negative-half rounding, runtime overflow abort, failed incoming setter adapter leaves destination unchanged, old callback after rollback, new callback after success, fresh child defaults, declaration removal guard, GC and EndPIE PASS. Expected Unreal temporary-reinstance numeric cast diagnostics are explicitly recorded; final values come from the policy-converted snapshots.
- TestEditor.ps1: 397 foundation checks, failures=0, PASS. Includes integer rules/bounds, floating precision/range, NaN/Inf, unknown policy rejection and ProcessEvent execution of all six native conversion thunks.
- TestPackaged.ps1: build/cook/stage/archive/launch; 397 foundation checks, failures=0, PASS, including all six runtime thunks.
- TestDSLNumericWidenAsset.ps1 and TestDSLRenameScopeAsset.ps1: existing widening and dependency-aware rename regressions PASS.
- Disposable GUID source files cleaned up. Probe assets stay scoped under DasFrameAssetProbe in the isolated smoke project. No user projects were migrated.

Supported pin domain is standard scalar numeric getter/setter links. Wildcard/container/split/orphan/by-reference adaptation, other property types, World Partition/streaming, lifecycle/delegate sessions and general rollback of other migration policies remain separate work. Numeric conversion rules are explicit; no silent clamp or saturation is used.