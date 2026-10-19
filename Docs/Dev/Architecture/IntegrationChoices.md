# Integration Choices

How daScript is embedded in Unreal Engine and why. These choices apply to UE 5.8.3,
daScript 0.6.4 (pinned upstream commit in `dependencies.json`) and Win64.

## Foundation scope

- A UE runtime module links a pinned daScript static compiler/runtime. An
  isolated editor commandlet and a separate C++ smoke game run the same checks.
- Scripts execute on the game thread. Each compilation uses an independently
  owned program and context; entry-point signatures are checked; compile and
  runtime diagnostics are returned to UE as results. Strings cross the boundary
  as owned UTF-8 copies.
- Files are read through Unreal's platform file system, so staged UFS/Pak content
  works in packaged games.
- Native C++ functions are callable from interpreted daScript, and generated AOT
  functions run without interpreter fallback (verified by `SimFunction::aot == true`).
- C++ exceptions are contained within the runtime module; ordinary engine code
  never sees them. UE `check`/fatal errors and access violations are not recoverable.

## Build configuration

- daScript is built with `/MD`, without RTTI, and with exceptions enabled in its
  UE module.
- `DAS_NO_GLOBAL_NEW_AND_DELETE` prevents daScript from replacing UE's global
  allocation functions. Do not suppress duplicate symbols with linker overrides.
- `DAS_FREE_LIST`, `DAS_FUSION`, assertion and exception defines match the pinned
  Release library. The Debug CRT is not supported by this configuration.
- Unreal's `verify`, `ensure` and `check` macros are temporarily hidden while
  daScript headers are included. Upstream headers are not patched.
- The upstream CMake build selects SIMD from the build host (AVX2 on the tested
  host); broader CPU support is not validated.
- Toolchain: VS 2022 17.14 / MSVC 14.44, Windows SDK 10.0.22621.0, Git, CMake.
  Existing user daScript checkouts are never used or modified.

## Bindings

- There is no C++ parser over Unreal headers and no engine-wide binding set.
  Bindings are generated from UE reflection (UHT data) for selected packages and
  discovered receivers, plus explicit managed adapters.

## Module and context lifetime

- Dynamic unloading of the runtime module is disabled: compiled code and module
  lifetime would need a dedicated reload design.
- Script compilation uses local owned contexts; gameplay uses persistent
  per-owner contexts that are replaced only after a candidate compiles.
- Native type-description emission is allowed only in the active runtime context;
  a compiler/macro context cannot be blindly cast to the Unreal context subclass.

## Third-party notices

daScript is BSD-3-Clause, copyright Gaijin Games Kft.; its license and dependency
notices remain in `ThirdParty/daScript`. Distribution must preserve them.

See also: [AuthoringModel](AuthoringModel.md), [Interop](Interop.md),
[Testing](../Testing.md).
