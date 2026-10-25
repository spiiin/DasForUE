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

`ADasProbeActor` is the actual DSL actor host and remains in production despite its
historical name (including its legacy regression fields; splitting that host is a
separate class-hierarchy change). Production generated bindings and native registries exclude fixture types.
`GenerateRuntimeBindings.py` derives them from the same reflection IR using the explicit
`ReflectedTypes.json` test inventory. Production staging selects `Content/Das/Runtime`
variants; smoke/editor targets retain the full test catalog in `Content/Das/Scripts`.
Editor builds still include the probes: separating editor test tooling is a separate
boundary from excluding tests from game runtime builds.

Validate through the shared build agent: `foundation,ue-aot,runtime-isolation`.
`runtime-isolation` links a minimal ordinary game and checks the actual UHT manifest
and build response files for test-source leakage. It does not replace packaged tests.
