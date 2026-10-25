# Test-only plugin extensions

`Runtime/` is an additional UBT module directory, not an unconditional source directory.
`DasRuntime.Build.cs` adds it only for an Editor target whose project explicitly enables
editor tests (see below), or a project with `Config/DasIncludeTestFixtures.txt` containing `include-plugin-test-fixtures`.
The smoke project opts in, including packaged Development and Shipping checks.
Ordinary Editor/Game/Client/Server targets do not compile these sources or reflect these types.

`DAS_WITH_TEST_PROBES` gates the few white-box includes that must share a translation
unit with private VM/physics state. The implementations live here, not in production
sources. This avoids exporting VM internals just to make a separate test DLL.
The directory belongs to DasRuntime only when selected, so test reflection remains
in `/Script/DasRuntime`. There is no production dependency on a test module.

`ADasActor` is the production DSL actor host. `ADasProbeActor` derives from it here
and contains regression-only delegates, container storage and `Peer`. Actor DSL
declarations resolve to `ADasActor` in every build, including Editor. Fixtures opt
in explicitly with `ProbeActor` from `ue_probe_actor` or
`parent="/Script/DasRuntime.DasProbeActor"`. The latter parent is rejected when the
test extension is absent. The legacy `ue_subscribe_int` and `ue_emit_signal`
fixture helpers are registered only with the test extension; runtime code uses
the general object/delegate APIs.

Old assets whose native parent was `ADasProbeActor` must be recreated from their DSL sources
with the clean host before shipping. Reimport does not migrate native parents,
and no compatibility redirect is installed.
Production generated bindings and native registries exclude fixture types.
`GenerateRuntimeBindings.py` derives them from the same reflection IR using the explicit
`ReflectedTypes.json` test inventory. Production staging selects `Content/Das/Runtime`
variants; smoke/editor targets retain the full test catalog in `Content/Das/Scripts`.
Ordinary editor builds exclude probes as well; test projects opt in explicitly.

Validate through the shared build agent: `actor-host-regression,foundation,native-defaults,runtime-isolation`.
`actor-host-regression` prepares method-value assets, then checks clean/test host selection,
authored references, spawning and gameplay callbacks in PIE.
`runtime-isolation` links a minimal ordinary game and checks the actual UHT manifest
and build response files for test-source leakage. It does not replace packaged tests.

`runtime-isolation-packaged` creates a fresh Blueprint from the project-owned
`Tests/DasRuntimeIsolation/Content/Das/Scripts/isolation_actor.das`, cooks it and
runs the archived ordinary Development game. The project never opts into runtime
fixtures. Its game module checks fixture-class absence, spawning the authored
actor, BeginPlay/Tick, the delayed callback, its root component and reflected
Unreal-to-daScript method dispatch. Validation code lives in the test project,
not in the plugin runtime. This project explicitly opts into the authoring test tooling for its Editor target only.
Run this suite through the shared build agent; output is `Build/IsolationPackaged`
and `logs/isolation-packaged.log`. Generated Blueprint assets are ignored by Git.

## Editor tests

`Editor/` contains regression commandlets, automation/PIE tests and their fixtures.
`DasEditor.Build.cs` adds this conditional module directory only when the project
has `Config/DasIncludeEditorTests.txt` containing `include-plugin-editor-tests`.
This also enables runtime fixtures for that project's Editor target. It does not
enable test fixtures or test scripts in the project's Game target. The smoke
project separately opts into game fixtures for its packaged regression suites.

Tests retain `/Script/DasEditor` class paths and can exercise private services in
the same module; no extra public exports or production dependency on a test DLL
are necessary. `DAS_WITH_EDITOR_TESTS` also gates failure-injection hooks and the
test-only modes of the production exporter commandlets. The regular exporter,
source catalog, asset preflight, native API generator, DSL editing, reimport and
migration services remain in `Source/DasEditor`.

`editor-isolation` builds and starts `Tests/DasEditorIsolation`, which has no opt-in.
It checks UHT/build inputs for both test directories and checks loaded classes and
production editor commands, then imports a project-owned DSL class on ADasActor.
Run it last in a suite batch because it changes the
shared engine Editor module build ID. All builds run through the shared agent.

The test build helper passes `-ForceHeaderGeneration`: these projects share plugin
UHT output paths, and switching their conditional source sets must refresh generated
reflection. Use the same flag for manual target switches outside the helper.
