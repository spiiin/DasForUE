# Authored enum/struct class binding validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, Win64 Development.

- TestDSLAuthoredValuesAsset: separate Create/Reopen processes PASS. Type assets,
  typed DSL class and map are saved. Enum=42 and struct Count=100/Unicode Title
  overrides are checked before new calls in Reopen. Exact model reimport is
  idempotent; declaring defaults remain Idle=7 and Count=15.
- Real Blueprint ProcessEvent SetMode/GetMode transport explicit sparse enum values
  through typed byte pins and native custom thunks. GetConfig returns owned native
  user-struct data. Mixed enum/struct frame returns Count=142 from input Count=100
  plus enum 42; friendly field access works despite native GUID property names.
- Invalid enum 255 refuses before write; type identity, GC and model validation PASS.
- TestDSLAuthoredValuesPIE: exactly one successful test without warnings. Typed
  setters/mixed frame, different enum identity refusal, structural post-compile
  failure restores enum/struct overrides and last-good callback, GC and EndPIE PASS.
- TestDSLObjectFieldsPIE and TestDSLLifecycleMigrationPIE regressions PASS.
- TestEditor: 416 foundation checks, failures=0, PASS. New checks validate native
  sparse enum values, reserved/unknown values/type, POD enum ABI and unknown member.
- TestPackaged: build/cook/stage/archive/launch, 416 foundation checks,
  failures=0, PASS. Native value functions are runtime-compatible; editor-only enum
  metadata lookup is excluded from the game build. Authored fixture asset save/
  reopen and methods are tested in editor/PIE; the packaged foundation map does not
  independently cook/load the GUID authored fixture map.
- GUID source files cleaned up; only the isolated smoke project's fixtures changed.

Exact loaded asset paths are required. UEEnumValue/UEStructValue wrappers carry
checked native identity and copy data rather than exposing layout-dependent memory.
Direct plain daScript declared-type conversion is now covered by
DSLDirectTypesValidation.md. Stable-ID resolution, nested/complex authored
structs and type-change/data migrations remain separate stages.

Stable-ID resolution and actual cooked direct-type invocation are now validated
in DSLTypeRegistryValidation.md.
