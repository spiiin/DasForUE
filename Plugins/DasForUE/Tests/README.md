# Test-only runtime sources

`Runtime/` is an additional UBT module directory, not an unconditional source directory.
`DasRuntime.Build.cs` adds it only for Editor targets (test commandlets/automation) or a
project with `Config/DasIncludeTestFixtures.txt` containing `include-plugin-test-fixtures`.
The smoke project opts in, including packaged Development and Shipping checks.
Ordinary Game/Client/Server targets do not compile these sources or reflect these types.

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
Editor builds still include the probes: separating editor test tooling is a separate
boundary from excluding tests from game runtime builds.

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
not in the plugin runtime. Editor builds still include the authoring test tooling.
Run this suite through the shared build agent; output is `Build/IsolationPackaged`
and `logs/isolation-packaged.log`. Generated Blueprint assets are ignored by Git.
