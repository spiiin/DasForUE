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

## Focused GAS cue assets

Submit `test-suite <unique-id> gas-cue-assets repo=C:\src\DasForUE2-main` through the main checkout build-agent queue described above. The suite builds Editor Development and runs one DasGASCueAssets commandlet. It imports the sample into `/Game/Tests/GASAuthored/CueAssets`, checks repeat import and native route-conflict preflight, applies/removes real GameplayEffects and checks owned DAS cue snapshots after GC. The fixed generated test folder avoids creating duplicate cue assets for the same registered tags on every run. It is not a PIE or packaged suite.

`gas-cue-hooks` is the corresponding focused suite for scripted cue assets and extended owned payloads. Queue it through the same build agent; the single DasGASCueHooks commandlet uses `/Game/Tests/GASAuthored/CueHooks` and verifies callback-return/GC ownership, per-target state and cleanup. Regenerate native gameplay artifacts through the agent's `gas-native-generate` suite after changes to these reflected payloads.

## Focused GAS cue cosmetics

The gas-cue-playground suite builds first, then runs one offscreen commandlet with real sound/Niagara assets. Its Tests/suites.json entry opts into rendering=true: the commandlet runner uses -AllowCommandletRendering, -AllowCommandletAudio and -RenderOffscreen instead of -NullRHI. Niagara's native IsReadyToRun rejects NullRHI. Other commandlet suites keep their default NullRHI mode. This is component/lifecycle verification, not full PIE, packaged or perceptual audio/visual QA.

The gas-ability-playground suite uses the same opt-in offscreen mode and demo assets, but runs a single end-to-end ability scenario: BeginAim, cancel, confirm a real physics hit, apply damage plus timed aura, expire the native effect, reject a freshly aimed miss, and release all contexts on EndPlay. It does not require running gas-cue-playground first; the asset preparation is shared and each suite remains independently runnable through the build agent.


## GAS playable map PIE

Queue `test-suite <unique-id> gas-ability-pie repo=C:\src\DasForUE2-main`
through the shared build agent. This builds Editor Development and runs only
`DasForUE.GAS.AbilityPlaygroundPIE`, with real rendering/audio and a real PIE
world. It generates the saved `/Game/DasExamples/GASCues/GASAbilityMap`, feeds
mapped input to the local PlayerController, checks ability/cue lifetimes and
EndPIE cleanup, and captures `logs/gas-ability-pie.png`. The test requires a
non-null RHI and is excluded from NullRHI runs. Reports are under
`logs/gas-ability-pie-report`. No packaged suite is selected.

Automation suites can opt into `rendering: true` (or Invoke-DasAutomation
`-Rendering`). The default remains NullRHI/no audio for existing suites.


## GAS map in packaged Development

Queue `test-suite <unique-id> gas-ability-packaged repo=C:\src\DasForUE2-main`
through the shared agent. The suite builds Editor, exports the map with
`DasGASPlaygroundMap`, builds Win64 Development, cooks/stages/archives it and
runs the archived game with real rendering/audio. It does not run broad editor
or Foundation suites. Logs: `gas-ability-package-export.log`,
`build-packaged.log`, agent/UAT output, `gas-ability-packaged.log`;
viewport: `gas-ability-packaged.png`.

PIE and commandlet export share `DasGASPlaygroundMap.h`. Input/GAS setup is
reproducible from source; generated assets remain in the dedicated demo folder.
`TestPackaged.ps1 -AdditionalCookDirectories` passes explicit physical cook roots
to UAT, and `-SkipSmoke` skips its default Foundation launch only when requested.
Other callers retain their existing behavior. The GAS runner cooks the demo
folder and engine sounds because cue script soft paths and discovery-only
Blueprints are not guaranteed dependencies of the map. The receipt must still
contain every DAS script before packaging.

The packaged-only probe checks cooked runtime identity, bundled script paths,
saved input/GAS/mesh references, a live audio device, mapped movement/look/jump,
aim/cancel/confirm, native damage, playing sound and ready Niagara systems,
native aura expiry/Removed and actor/session cleanup. Inputs traverse
PlayerController/Enhanced Input; no ability methods are invoked directly by the
probe. Startup/runtime has an outer process deadline as well as a bounded
in-game probe deadline. The probe is enabled only by `-DasGASAbilityPackaged`
and omitted from ordinary Shipping; the opt-in Shipping acceptance build is described below.


## GAS map in Shipping and AOT scope

Queue `test-suite <unique-id> gas-ability-shipping repo=C:\src\DasForUE2-main`
through the shared build agent. The runner exports the same map, builds Win64
Shipping, cooks/stages/archives to `Build/PackagedShipping`, then launches the
archived Shipping executable with real rendering/audio and mapped input.
Development output is retained separately. The test does not change engine
Shipping logging/optimization settings.

Shipping ignores the positional startup-map override by default. The runner
therefore stages GASAbilityMap as GameDefaultMap in the isolated smoke project
only during packaging, then restores the config's exact original bytes and
timestamp in finally. The archive retains the GAS default map; source project
defaults stay on Entry. Do not enable engine map-override flags for this test.

`DAS_GAS_SHIPPING_VALIDATION=1` is a process-scoped build opt-in for a probe in
the DasRuntime module only. The runner restores the previous environment value
after packaging. Ordinary Shipping excludes the probe. At runtime the test flag
and `-DasGASReport=<absolute path>` select its file report, because ordinary
Shipping logging is disabled. Do not ship the instrumented acceptance binary
as an end-user release. A future normal Shipping build must run without this
opt-in and regenerate its target makefile (the common runner already does so).

`logs/gas-ability-shipping.txt` records the compiled configuration, bundled DAS
paths, input/GAS/cue checks, expiry and cleanup. The strict native AOT check runs
the linked `aot_smoke.das` function with AOT enabled and requires `bUsedAot`;
success through VM fallback fails this check. Screenshot capture is omitted
in Shipping and does not affect the Development screenshot check.

**Full GAS AOT is not supported yet.** UE externs deliberately use `noAot` to
prevent emitted raw C++ calls from bypassing execution guards. Authored
gameplay/cue callbacks use the VM path. A Shipping map pass plus the pure AOT
check proves those two capabilities separately; it does not prove that the
ability, character or cue hooks have been translated to native code. Adding
guard-preserving AOT wrappers and an authored-callback AOT build/link pipeline
remains a separate implementation milestone. Networking is not tested here.
