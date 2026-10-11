# Gameplay runtime PIE validation — 2026-10-10

`Scripts/TestGameplayRuntimePIE.ps1` builds the editor target, starts a separate
unattended NullRHI editor and runs `DasForUE.Gameplay.RuntimePIE`.

Result: **PASS**. Unreal Automation report: succeeded=1,
succeededWithWarnings=0, failed=0, notRun=0.
Log: `logs/gameplay-runtime-pie.log`; report: `logs/gameplay-runtime-pie-report/index.json`.

The fixture `dsl_gameplay_runtime_probe.das` is imported into an unsaved Blueprint
in a unique test package. Two actors execute it in a real PIE world. The test
checks:

- A repeating timer runs; cancellation prevents subsequent increments.
- An array capture keeps its original 3+4 values after the caller appends 99.
- A next-tick Vector2D capture delivers 6+7 through the snapshot ABI.
- Cancelled next-tick and latent callbacks do not execute.
- Latent delay and async loading of the engine DefaultMaterial each complete once.
- Scene components support creation, attachment, relative transform, detachment
  and destruction. Cyclic attachment and reading a destroyed component are rejected.
- Float, void, Unicode string, int-array and const-reference Vector2D delegate
  payloads reach the first actor without affecting the second actor.
- Destroy removes the second actor's session/subscriptions and cancels pending
  next-tick, latent and repeat work. The test temporarily retains that actor only
  to inspect counters, then releases it before ending PIE.
- EndPIE releases all sessions and subscriptions without retaining the world.

The scene fixture uses GameplayRoot/GameplayChild: the native probe actor already
has its own ProbeRoot. No map or Blueprint packages are saved. The test runs in
its own editor process and does not close an interactive editor.

Runtime hardening: SceneComponent access/validity now rejects IsBeingDestroyed,
including the interval before garbage collection invalidates its weak reference.

Remaining validation: legacy/Enhanced Input dispatch, mapping-context ownership
and restoration, accepted/rejected reload of these managed operations, async
failure/class-mismatch/cancellation, and cooked gameplay execution. This test
does not turn GameplayDemo's source/setup manifest into a playable map.

2026-10-11: rerun passed after class-pin and cooked-root fixes. Packaged gameplay coverage now also passes; see [AccumulatedValidation.md](AccumulatedValidation.md).
