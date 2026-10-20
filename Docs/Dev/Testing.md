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

## Regression suites

Suites are listed in `Tests/suites.json` and run by `RunDasTests.ps1`:

```powershell
.\Scripts\RunDasTests.ps1 -List
.\Scripts\RunDasTests.ps1 -Preset quick        # build, Python, Foundation, automation except MethodValues (~5 min)
.\Scripts\RunDasTests.ps1 -Preset editor       # quick + MethodValues create/reopen and all automation
.\Scripts\RunDasTests.ps1 -Preset full         # editor + native/computed defaults, cooked export, packaged maps
.\Scripts\RunDasTests.ps1 -Suite method-values,automation -SkipBuild
```

A suite's `requires` are added automatically (`automation` needs `components-prepare`
and `method-values`); `-NoDependencies` uses the probe ids of the previous run
instead. Each suite prints `DAS_SUITE_<NAME>=PASS seconds=<n>`, the run ends with
`DAS_SUITES=PASS`. A new suite is one entry in the manifest; the automation entries
pin the expected minimum test count.

`TestCombined.ps1` (`full`), `TestCombined.ps1 -SkipEditor` and
`TestCombinedEditor.ps1 [-SkipBuild]` (`editor`) remain as wrappers.

## Build agent and parallel work

All checkouts share one engine, and every editor build rewrites the engine's module
BuildId: a build from another checkout during a test run makes that run fail with
"The game module ... could not be found". Run builds and tests of every checkout
through the single build agent (`Scripts\DasBuildAgent.ps1`, started once from the
main checkout); it runs one job at a time.

A request is one line in a new file under `logs\agent\queue\` of the main checkout
(any name ending in `.txt`; oldest first) or in `logs\agent\request.txt`:

```text
test-quick q1
test-suite s1 foundation,method-values
test-suite ui1 automation-quick repo=C:\src\DasForUE2-ui
```

`test-suite` always builds first. `repo=` must be the main checkout or one of its
`git worktree`s. Output goes to `logs\agent\<id>.log` and the exit code to
`logs\agent\<id>.done` in the main checkout; the scripts' own logs stay in the
worktree's `logs\`.

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

## Shared script helpers and validation index

`Test*.ps1` runners import `DasTest.psm1`. Use `Invoke-DasBuild` for builds;
`-BuildArguments` preserves a specialized game target or receipt-refresh invocation,
while `-Modules` and `-NoEngineChanges` cover module-only editor builds. Use
`Invoke-DasCommandlet`, `Invoke-DasAssetPhases`, `Invoke-DasAutomation` or
`Invoke-DasPackaged` for standard runs. Keep scenario-specific setup and negative
exit-code expectations explicit. `Test-DasResult` retains Select-String's literal,
regex and Quiet behavior; `Get-DasResult` extracts custom result fields;
`Assert-DasResult` additionally checks an exact count. `Invoke-DasValidation`
combines a scoped source/binary freeze, a commandlet, count/lint checks and a receipt.
Its path provider is evaluated before and after the probe, including additions/removals.
The ten common DSL receipt runners are about 20 lines each.

`RunDasTests.ps1` regenerates [the validation index](Validation/index.md) after its
selected suites finish and then checks it. Standalone commands:

```powershell
C:/Python312/python.exe Scripts/RenderValidation.py
C:/Python312/python.exe Scripts/RenderValidation.py --check
```

The generator reads top-level `logs/*.json` receipts, supports state/result and
numeric/list check counts, and ignores unrelated probe JSON. It uses an explicit
receipt timestamp or the JSON file's UTC modification time. Missing counts and
log paths remain unknown. A source-content fingerprint catches receipt changes
that do not alter the visible table. `--check` does not write; missing, changed,
added or removed receipts make it fail. Invalid validation JSON is an error.
Logs are local artifacts: a fresh checkout with no receipts generates an empty
index. Recorded PASS rows describe those receipts, not the current checkout's
source or an unperformed PIE/packaged run. Existing manual reports are preserved.

Candidates for later removal of duplicated result sections: `WidgetLayouts.md`,
`ReflectionMacros.md`, `IdentityDSL.md`, `EnumNames.md`, `HiddenCallbacks.md`,
`InlineCallbacks.md`, `HandlerReferences.md` and `InheritedHandlers.md` under
`Docs/Dev/Validation`. Whole-file removal is conditional on moving their unique
contract/migration explanations to the Guide and retaining the corresponding JSON
receipts. Historical aggregate reports are not automatically superseded by a new
narrow run and should not be deleted on that basis.
