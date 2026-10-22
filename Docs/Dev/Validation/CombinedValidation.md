# Combined binding validation — 2026-10-17

Baseline: `b7da031597723e0dacfbe9cd2ce7caeb76d9e05b`, plus the fixes below.
The exact tracked commit was exported into `<user folder>`. Stale validation-copy
source files were backed up and removed there. The main repository's temporary
files, generated assets and parallel changes made after the commit were preserved.
The copy uses existing main-repository daScript third-party libraries through a
Build.cs path adjustment; editor/game outputs are separate.

## Findings and fixes

The first aggregate run reported 31 successes and four failures:

- ObjectFields: the strict property codec rejected destroyed references before
  VM getters could return null. A shared nullable getter now handles direct
  fields, nested struct/array/map-value reads and native getter returns. Invalid
  class assignments are rejected before changing the field, with the original
  field-class diagnostic. Immutable/queued capture validation remains strict;
  destroyed event/timer captures still prevent delivery.
- MethodDispatch: its ordinary helper was named `Initialize`, which is now an
  automatically recognized lifecycle method. The example helper is `ResetScore`.
- ClassReload and MethodValues require saved fixtures from earlier processes.
  The aggregate runner now prepares those assets, reopens MethodValues and passes
  its generated probe ID. Test assertions were not weakened.

The expanded preparation also caught the destroyed-reference case in nested
struct and array getters; it is covered by the existing method-value regression.

## Reproduce

```powershell
.\Scripts\TestCombinedEditor.ps1
.\Scripts\TestCombined.ps1
```

The second command includes Editor validation, native/computed defaults in
Create/Reopen/Recompile processes, gameplay export, one Game Development
build/cook/stage/package, packaged Foundation and five packaged map probes.
Use `TestCombined.ps1 -SkipEditor` only after the Editor runner has passed in the
same checkout. Existing generated method-value sources must remain for packaging.

## Verified result

- 116 Python generator/policy tests passed.
- All 35 Unreal Automation tests passed, zero warnings/failures/skips. This
  includes gameplay, DSL, migrations/reload, components, method dispatch/values,
  delegates, library calls, pin adapters and the 106-combination value matrix.
- Editor and Game Win64 Development builds passed; Editor/packaged Foundation
  returned PASS with zero failures.
- Saved method-value fixtures passed Create/Reopen. Native and computed defaults
  passed Create/Reopen/Recompile.
- Export, cook, staging and packaging passed. Packaged gameplay, playable map,
  native defaults, computed defaults and method values all passed and exited.
- 848 recorded build/test source files stayed unchanged through
  the final verification. The JSON report records main-checkout differences;
  newer parallel changes are not covered by this result. The exact tested fixes
  are retained in `logs/combined-tested-fixes.patch` for the recorded baseline.

Runs use NullRHI and Win64 Development. Shipping and visual rendering were not
tested. Known missing ACL compression-settings warnings are retained in cooker
logs. Evidence: `logs/combined-validation.json` and the `combined-*` reports/logs.
Fixes are not committed by this validation step.
