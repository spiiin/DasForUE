# DasForUE2

New, independent daScript integration for Unreal Engine. The old `C:\src\DasForUE`
proof of concept is a reference only; none of its generated bindings are used here.

## Foundation scope

- UE runtime module with a pinned daScript 0.6.4 static compiler/runtime.
- Isolated editor commandlet and a separate C++ smoke game.
- Game-thread execution, independently owned program/context per call, checked
  entry-point signature, compile/runtime diagnostics, UTF-8 copies across the boundary.
- File access through Unreal's platform file system, including staged UFS/Pak content.
- Native C++ call from interpreted daScript and a generated AOT function. The AOT
  test requires `SimFunction::aot == true`; interpreter fallback does not pass.
- An experimental structure macro emits two successive type descriptions in the
  same process. Annotation spelling is disposable and is **not** the chosen DSL.

The foundation tests alone do **not** implement structural Unreal hot reload; see the separate editor probe below. The macro produces plain owned
declarations; it does not create `UClass`, modify CDOs, migrate actors or reinstance
Blueprints. The separate editor probe now tests these through a Blueprint backend. UObject bindings, GC handles,
delegates, gameplay state migration, Shipping, replication and a production sandbox
are also outside this foundation.

## Reproduce on Windows

Prerequisites: source-built UE 5.8.3, VS 2022 17.14 / MSVC 14.44, Windows SDK
10.0.22621.0, Git, CMake. The currently tested host supports AVX2; upstream CMake
selects SIMD based on the build host. Broader CPU support has not been validated.
See `dependencies.json` for the exact upstream commit. Existing user daScript
checkouts are never used or modified.

```powershell
.\Scripts\BuildDependencies.ps1
.\Scripts\TestEditor.ps1
.\Scripts\TestPackaged.ps1
```

The default engine is `C:\src\unreal\UnrealEngine`; test scripts accept `-Engine`.
BuildDependencies accepts `-VisualStudio`. The isolated project is
`Tests\DasForUESmoke\DasForUESmoke.uproject`, and only this plugin is explicitly
enabled. Build and test logs live under `logs/`, packaged output under
`Build/Packaged/Windows/`.

## Important integration choices

- daScript uses `/MD`, no RTTI, and exceptions enabled in its UE module.
- `DAS_NO_GLOBAL_NEW_AND_DELETE` prevents daScript from replacing UE's global
  allocation functions. Do not suppress duplicate symbols with linker overrides.
- `DAS_FREE_LIST`, `DAS_FUSION`, assertion and exception defines match the pinned
  Release library. Debug CRT is not supported by this configuration.
- Unreal's `verify`, `ensure`, `check` macros are temporarily hidden while including
  daScript headers. Upstream headers are not patched.
- No C++ parser over Unreal headers and no generated engine-wide binding set.
- Runtime module dynamic unloading is disabled: compiled code and module lifetime
  need a dedicated reload design. Script compilation uses local owned contexts.
- Native type-description emission is allowed only in the active runtime context;
  a compiler/macro context cannot be blindly cast to the Unreal context subclass.

## Editor reload feasibility probe

See [the implementation report](Docs/ReloadProbe.md). Run:

```powershell
.\Scripts\TestReload.ps1
.\Scripts\TestReloadPIE.ps1
```

The schema macro feeds an internal Blueprint backend that creates a real
UBlueprintGeneratedClass with int/float fields and a fixed script-backed method.
It has passed additive structural reload, Blueprint descendant/instance migration,
CDO and instance overrides, references, transforms, GC and fresh-process reopening.
A real PIE automation test also passes, preserving different values in editor and
play worlds and verifying the editor instance after EndPIE. This is not a finished
DSL, arbitrary native UClass synthesis or a general migration system.

Next: extract a typed IR/editor service, add safe self/property bindings, and extend
coverage to lifecycle callbacks, subscriptions, metadata changes and cook parity.
## Third-party notices

daScript is BSD-3-Clause, copyright Gaijin Games Kft.; its license and dependency
notices remain in `ThirdParty/daScript`. Distribution must preserve applicable notices.

## Roadmap

See [the audited feature plan](Docs/Plan.md) for the NimForUE comparison, priorities and acceptance gates.

## Validation — 2026-10-05

UE 5.8.3 / Win64 Development: all 12 foundation checks passed in both the
DasSmoke editor commandlet and the packaged game; both processes exited with 0.
The game log identifies the archived RemappedPlugins content root and mounted Pak.
The staging manifest contains the macro library, invalid-schema and UTF-8 probes.

Checks cover native roundtrip, independent contexts, syntax/signature/runtime/missing
file errors, successive macro schemas, native AOT execution without fallback,
unsupported schema rejection, UTF-8 ownership after context destruction, and recovery.
Logs: logs/smoke-editor.log, logs/smoke-packaged.log, logs/package-final.log.
This validates a headless Development foundation, not Shipping, renderer output,
UObject interoperability, actual structural reload or other platforms.
