# Reflection call macros — Phase 3.8

Worktree: `<repo>-ui`, branch `widget-layout`.

Implemented forms (no reader macro or parser changes):

```das
ue_static(GameplayStatics.GetTimeSeconds)
ue_static(DasFunctionLibraryFixture.Add, 20, 22)
ue_call(type<DasProbeInterface>, Query, target, 5)
ue_struct_get(payload, Count)
```

The planned curried `ue_static(...)(...)` / `ue_call(...)(...)` is not in daScript's grammar. A negative probe pins this finding. Direct macro arguments retain normal overload checking and generated parameter defaults.

Static wrappers carry `ue_reflected_call(owner, method, signature)` descriptors emitted by `GenerateBindings.py`. These survive dependency compilation and optimization; no AST body scanning or alias-name convention is needed. The macro compares the recorded signature with the loaded UFunction and selects a unique wrapper. Existing static artifacts have only the new descriptor lines (1,172 wrappers); transport bodies are unchanged. Instance/interface calls reuse the existing typed reference-method emitter, including native registries, authored contracts, ref/out and container adapters. Struct reads lower to ordinary typed field access; raw UEStructValue is deliberately rejected because it has no compile-time identity.

## Targeted validation

```powershell
./Scripts/TestReflectionMacros.ps1 -SkipBuild
C:/Python312/python.exe Scripts/TestBindingsGenerator.py GeneratorTests.test_static_reflection_macro_descriptor GeneratorTests.test_static_library_has_no_receiver GeneratorTests.test_world_context_is_hidden_from_wrapper_inputs GeneratorTests.test_deterministic_and_current
```

28 compile/runtime checks and 4 generator checks: PASS.

- Scalar static calls select the original Unreal name despite different generated aliases.
- Automatic WorldContext uses a real owning Actor; missing owner fails at runtime.
- Real native interface dispatch, mutable ref/out copyback and owned array results.
- Typed native struct and nested struct reads.
- Unknown functions/methods/fields, unsupported transport, wrong argument types/counts, raw receiver and untyped snapshot rejected during compilation.
- Stale and ambiguous static descriptors rejected during compilation.
- Macro inside authored method compiles into its authoring model.
- World, Math, Collision and UMG Input generated packages parse with their new descriptors.
- Existing actor_location frame fixture executes; array_gc_exception_fixture compiles. Neither was edited.

The runner freezes SHA-256 of plugin C++ sources, all script modules, its probe, generator, runner, runtime/editor DLLs, and engine/project module manifests. Receipt: `logs/reflection-macro-validation.json`; execution: `logs/reflection-macro-runtime.log`.

Build uses only this worktree's project and DasRuntime/DasEditor/DasForUESmoke modules with `-NoEngineChanges`. A concurrent engine build changed the engine BuildId during development. After UBT confirmed all local modules current with `-NoEngineChanges`, only this worktree's ignored project module manifest was synchronized to that engine BuildId. Shared engine manifests and other worktrees were not edited by this operation.

PIE, packaging and full suites were not run. Public Guide/Reference examples use the macro forms; raw frame/snapshot contracts are retained under `Docs/Dev/Runtime`.
