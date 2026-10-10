# Gameplay validation — 2026-10-10

Engine: source Unreal Engine 5.8.3, Win64 Development, daScript 0.6.4.

Editor and game target builds passed after compilation fixes. `Scripts/TestEditor.ps1` passed.
`DAS_FOUNDATION_RESULT=PASS failures=0`.

The following regression scripts passed:

- `TestMethodValues.ps1`: create, reopen, real PIE; without `-Cooked`.
- `TestActorComponent.ps1` and `TestActorComponentPIE.ps1`.
- `TestDSLDirectTypesAsset.ps1` and `TestDSLMathAsset.ps1`.
- `TestDelegatesPIE.ps1`.
- `TestBindingsExport.ps1`: expected rejection preserves prior output.
- `GenerateBindings.ps1 -Check`: generated bindings match their inputs.

`TestGameplaySources.ps1` checks 19 new source examples through catalog generation:
timers, captured scalar/array values, next-tick timers, scalar/value/math/array
events, native interface and authored object events, legacy and Enhanced Input,
mapping contexts, Vector2D, scene components, latent delay, async assets, and both
GameplayDemo actor/component sources. All 19 passed; authored object events were
rerun separately after fixing explicit void return types in their struct-map
dependency. Per-example logs are `logs/source-<example>.log`.

`DasGameplaySourceTest` prepares required struct-map type fixtures in memory.
It does not save packages. The script deletes only its uniquely named catalog
and generated module. It tests compiler/model validity, not callback delivery.

Fixes found by validation:

- Unreal smart pointer access and header ordering required by UBT.
- Catalog commandlet no longer treats omitted source lists as empty filenames.
- Vector2D method transport resolves its native type path.
- Timer captures are lowered in original source methods as well as exported clones,
  so captured arrays type-check in both places.
- New examples use supported BlueprintCallable annotations; BlueprintPure is not
  supported by the current class adapter.
- Correct gen2 loop syntax and explicit void returns in affected examples.

Follow-up: [GameplayRuntimePIEValidation.md](GameplayRuntimePIEValidation.md) records successful real PIE delivery/cancellation checks for timers, events, scene components, latent delay and async success. The remaining limits below describe the earlier compiler/regression run.

Limits: this run does not validate delivery/cancellation/reload of the new timers,
input bindings, mapping contexts, latent actions, or async loads in PIE. The old
delegate/component PIE suites cover their existing fixtures. GameplayDemo is a
source example, not a tested playable map. Cooking/packaging and migration suites
were not rerun. Deprecation warnings for reflection property constructors remain.
