# Accumulated validation — 2026-10-11

UE 5.8.3, Win64 Development, source engine. Baseline: f838a0d.
The final checks ran after concurrent writes to the repository were paused.

## Results

| Check | Result | Evidence |
| --- | --- | --- |
| Editor build and foundation/DSL | PASS, failures=0 | logs/accumulated-final-editor.log |
| Gameplay DSL source/model collection | PASS, 19 fixtures | logs/accumulated-2026-10-11-TestGameplaySources.log |
| Bindings generator | PASS, 90 tests | logs/accumulated-bindings-generator.log |
| Native component templates/defaults | PASS | logs/native-components-report/index.json |
| Gameplay real PIE: timers, captures, extended events, scene operations, latent/async, cancellation/Destroy/EndPIE | PASS | logs/gameplay-runtime-pie-report/index.json |
| Dynamic components real PIE and fresh-process class reload | PASS | logs/dynamic-components-pie-report/index.json; logs/class-property-reload-report/index.json |
| Actor spawn and typed queries real PIE | PASS | logs/actor-spawn-pie-report/index.json |
| Actor/component lifecycle real PIE | PASS | logs/actor-component-pie-report/index.json |
| Authored dispatchers, Blueprint handler, asset create/reopen | PASS | logs/accumulated-final-authored-events.log |
| Managed delegates: real PIE, reload, nested dispatch, Destroy/EndPIE | PASS | logs/accumulated-final-events-pie.log |
| Own method/Blueprint event dispatch real PIE | PASS | logs/accumulated-final-methods-pie.log |
| Function libraries and WorldContext real PIE | PASS | logs/function-libraries-report/index.json |
| Packaged Development build, cook, stage, archive | PASS | logs/accumulated-cooked-final.log |
| Packaged foundation/DSL | PASS, failures=0 | logs/smoke-packaged.log |
| Packaged gameplay on cooked map and real game ticks | PASS | logs/cooked-gameplay-runtime.log |

Each final Unreal Automation suite reported succeeded=1, warnings=0, failed=0,
notRun=0. PIE tests use separate unattended editor processes; they enter and exit
actual PIE rather than only creating a game UWorld. Packaged tests launch the
Development executable from Build/Packaged and verify explicit PASS markers.

The cooked gameplay probe checks repeating and one-shot timers, owned array capture,
Vector2D next-tick capture, cancellation and owner isolation, scalar/Unicode string/
array/const-ref Vector2D events, native/authored SCS components, template defaults,
root and attachments, component BeginPlay/Tick and owner-event delivery, latent and
async completion, authored multicast handlers and individual removal, Destroy of an
owner with pending work, and final context cleanup.

## Fixes

1. Class-reference fields previously fell through the editor pin factory as
   PC_Real with no float/double subcategory. DynamicComponents and SpawnPIE crashed
   in KismetCompiler. Class fields now use PC_Class with the declared MetaClass.
   Existing class defaults are checked and preserved on unchanged reimport; class
   default migration remains explicitly deferred. Both PIE suites and fresh-process
   class reload pass after this fix.
2. Cooked map actors kept the native ProbeRoot although a declared SCS root existed.
   The root promotion had only run from OnConstruction. It now also runs after
   PostInitializeComponents, using the same idempotent helper. The packaged probe
   reports root=Pivot, correct attachments/defaults and working component lifecycle;
   native, actor/component, dynamic and spawn PIE suites passed again afterward.
3. Packaged test receipts could omit scripts added since the cached rules evaluation.
   TestPackaged now forces rules compilation and verifies that every DAS script is
   present before staging. It accepts additional cooked maps while retaining the
   foundation smoke test. Generated cooked-map assets are ignored by Git; their
   source commandlet/fixtures and deterministic test runner are committed.

## Reproduction and limits

Run Scripts/TestEditor.ps1, Scripts/TestGameplaySources.ps1, the PIE scripts named
above, and Scripts/TestCookedGameplay.ps1. The last command exports an isolated
GUID-named map, packages Entry plus that map, runs foundation, then checks gameplay
in the packaged executable. It does not alter other user projects.

This validation does not claim all pending roadmap features or all historical
migration suites. Physical collision/network outcomes, input-device injection,
timer pause/resume/reload combinations, every async failure path, structural event
migration, and packaged function-library WorldContext calls require further targeted
tests. NullRHI tests do not verify rendering or an interactive UI.
