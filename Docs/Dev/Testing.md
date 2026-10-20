# Testing

How to run the plugin's checks on Windows. All scripts live in `Scripts/` and
accept `-Engine <path>` (default `C:\src\unreal\UnrealEngine`). The isolated test
project is `Tests/DasForUESmoke/DasForUESmoke.uproject`; build and test logs are
written under `logs/`, packaged output under `Build/Packaged/Windows/`.

## Policy

- **During iteration:** build what is needed and run the focused checks for the
  changed path.
- **Full suites** (Foundation, PIE, packaged) run at the end of a major milestone,
  on request, or when a change plausibly affects several systems.
- Every report lists separately which checks were run and which were deferred.
- NullRHI runs and programmatically injected input do not prove rendering quality
  or physical-device input.

## Full regression

```powershell
.\Scripts\TestCombined.ps1            # editor suite + packaged maps
.\Scripts\TestCombined.ps1 -SkipEditor
.\Scripts\TestCombinedEditor.ps1 [-SkipBuild]
```

`TestCombined.ps1` runs `TestCombinedEditor.ps1` (all Python tests, editor
Foundation, Unreal Automation tests including PIE, MethodValues create/reopen),
then native/computed defaults, a cooked export and `TestPackaged.ps1` with the
gameplay, playable, native-defaults, computed-defaults and method-values maps.

## Foundation

```powershell
.\Scripts\BuildDependencies.ps1   # once; accepts -VisualStudio
.\Scripts\TestEditor.ps1          # Foundation checks in the editor commandlet
.\Scripts\TestPackaged.ps1        # same checks in a packaged Development game
```

`TestPackaged.ps1` also accepts `-AdditionalMaps`, `-SkipZenStore` and `-DDC`.

## Focused checks

Each feature area has its own `Scripts/Test*.ps1` runner, for example
`TestDSL.ps1`, `TestLifecyclePIE.ps1`, `TestMontage.ps1`, `TestAI.ps1`,
`TestInputUI.ps1`, `TestWidgetEvents.ps1`, `TestDataTables.ps1`,
`TestGameplayTags.ps1`. Many accept `-SkipBuild` when the Editor target is
already built. API packages have `TestWorldApiPackage.ps1`,
`TestCollisionApiPackage.ps1`, `TestPawnAIPackage.ps1`,
`TestAnimationAudioVFXPackage.ps1` and `TestUMGInputPackage.ps1`.
For a standalone project use `TestStandalone.ps1 -Project <path.uproject>`.

## Generator tests and freshness

Python unit tests (`Scripts/Test*.py`) cover the binding, native gameplay and API
package generators and the coverage audits:

```powershell
python -m unittest discover -s Scripts -p "Test*.py" -v
```

Generated artifacts are checked for staleness without rewriting them:

```powershell
.\Scripts\GenerateBindings.ps1 -Check
.\Scripts\GenerateNativeGameplay.ps1 -Check
```

`GenerateApiPackage.ps1 -Profile <profile.json>` regenerates a package and runs a
whole-module preflight; it installs the module only with `-Publish`.

See also: [Roadmap](Plans/Roadmap.md), [validation records](Validation/).

## Writing test scripts

`Scripts/DasTest.psm1` holds the shared steps; a test script is a short list of them:

```powershell
Import-Module (Join-Path $PSScriptRoot 'DasTest.psm1') -Force
$Das = Get-DasContext -Engine $Engine
Invoke-DasBuild $Das -Name 'My feature'
Invoke-DasCommandlet $Das -Run DasFrame -Name 'my-feature-Create' -Marker 'DAS_MY_RESULT=PASS' -Arguments @('-Phase=Create')
Invoke-DasAutomation $Das -Test 'DasForUE.DSL.MyPIE' -Name 'my-feature-pie' -Marker 'DAS_MY_PIE_RESULT=PASS'
```

| Function | Does |
| --- | --- |
| `Get-DasContext` | Repository, logs, smoke project and editor/packaged executable paths |
| `Invoke-DasBuild` | Builds the editor target |
| `Invoke-DasCommandlet` | Runs `UnrealEditor-Cmd -run=...`, checks the exit code, PASS marker and optional failure pattern; log in `logs/<Name>.log` |
| `Invoke-DasAssetPhases` | Runs Create/Reopen/... phases of an asset probe with one probe id and returns the id |
| `Invoke-DasAutomation` | Runs Automation tests and requires exactly `-Count` successes without warnings |
| `Invoke-DasPackaged` | Runs the packaged smoke game with a map/switch and checks its marker |

Every helper throws on failure. `TestEditor.ps1`, the native/computed defaults tests and the
`*Asset.ps1` / `*PIE.ps1` scripts use it.

## Formatting

C++ sources follow `.clang-format` (4 spaces, Allman braces, 120 columns).
Generated files (`*.generated.h`, `Private/Generated/*`) are excluded.

```powershell
python Scripts\CheckFormat.py           # check all plugin sources
python Scripts\CheckFormat.py --fix     # reformat in place
python Scripts\CheckFormat.py --staged  # check staged files; new lines must be <= 400 chars
```

The script uses `clang-format` from PATH or from the Visual Studio LLVM tools.
