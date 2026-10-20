# Operation identities and event contracts — Phases 3.4 / 3.5

Worktree: `C:/src/DasForUE2-ui`, branch `widget-layout`.

## Operation identities

```das
ue_timer_start(1.0f, self.Pulse, true)
ue_timer_next_tick(self.ApplyScore, 5)
ue_timer_pause(self.Pulse)
ue_timer_resume(self.Pulse)
ue_timer_cancel(self.Pulse)
ue_latent_delay(0.5f, self.Pulse)
ue_latent_cancel(self.Pulse)
ue_effect_stop(self.EffectFinished)
ue_montage_cancel(self.MontageUpdate)
```

The compiler supplies the existing string transport with an internal UTF-8 hash of family, stable class ID and callback method name. Bodies, source line numbers and VM addresses are not identity inputs. Named starts accept the keyless form; corresponding queries and cancellation accept the callback reference. The effect, sound and Niagara APIs share one family. Timer, latent and montage families remain separate. Existing explicit/dynamic key forms are unchanged; self properties used as keys or intervals are not mistaken for method references. Inline blocks retain their explicit keys.

One method identifies one operation per family. Independent instances sharing a callback still use distinct explicit keys. No new runtime handle type or timer-reload preservation policy is introduced. The static unknown-key lint warns on cancel/query literals without a matching statically declared start; dynamically created names may still exist at runtime.

## Event contracts

Native delegates and authored self/component declarations are checked for name, parameter count, exact value identity, compatible object/class widening and supported queued parameter direction. Both compile checking and queued frame transport use the same property compatibility function. Source type declarations provide authored signatures before generated Blueprint assets exist. Mutable ref/out sources and narrowing are rejected. Native enum storage differences retain the existing canonical enum identity rules.

`ue_event(source=Health, name=Changed)` accepts identifiers. `source=owner` now requires `source_type=OwnerRef`; the generated binding checks the actual native/authored owner type before subscription installation. Seven existing owner scripts were updated with explicit contracts. The shared internal `ue_probe_actor` module describes the custom signal on the test host; general gameplay owners should use their own correct reference type.

```das
def BindEvents():void {
    self.OnDasSignal += self.OnSignal
    self.Health.Changed += self.OnHealthChanged
}
```

This reserved method is a declaration block containing only top-level `+=` entries. It lowers to the existing `ue_event` collection pass, not an imperative multicast mutation. Arbitrary logic, conditional declarations and pre-annotated/multiply assigned handlers are rejected. Instance-method `with self` wrappers added by daScript are unwrapped before reading the declarations. Normal gameplay-method `+=` / `-=` subscriptions are outside this form. Handler runtime marshalling, replacement and lifecycle cleanup are unchanged.

## Targeted validation

The final run used the single build agent, with this request in the main checkout's queue:

```text
test-suite dsl3435-ui-abe548864e76 dsl-operation-keys,dsl-event-checks,dsl-inline-callbacks repo=C:\src\DasForUE2-ui
```

`C:/src/DasForUE2/logs/agent/dsl3435-ui-abe548864e76.done`: **0**. All suites passed:

- **26 operation checks:** keyless starts for all four families, typed pause/resume/query/cancellation, timer/latent isolation, dynamic self key/interval compatibility, expected unknown-key warning, shared hidden-callback Apply/palette/runtime/reload checks.
- **27 event checks:** native and authored self events, native/authored component signatures, typed owner declarations, wrong names/types/handlers, syntax-invalid declarations, four existing event/component scripts; Apply, native scalar and authored tuple delivery, accepted/rejected reload, cleanup; actual compatible owner binding and incompatible owner rejection without subscriptions.
- **26 inline regression checks:** stable identities, capture rejection, real timer/input delivery and reload/cleanup.

Receipts: `logs/operation-key-validation.json`, `logs/event-check-validation.json`, `logs/inline-callback-validation.json`. The focused runners freeze source/script and local DLL hashes during execution. The event and operation runners additionally freeze their fixtures, runner, extern baseline and engine/project module manifests.

The suite manifest exposes these focused jobs without adding them to broad presets. The generic script step now permits a script that has no persisted probe-ID file. Adding the new commandlet changed unity compilation groups; two existing translation units gained their missing direct includes (`DasAuthoring.h`, `DasScriptComponent.h`).

Earlier development builds ran directly with this worktree's `-Project` and `-NoEngineChanges`. After the new shared-agent rule arrived, all subsequent build/editor jobs went through the queue. No second agent was started. PIE, packaging and full suites were not run. Staged formatting and `git diff --check` passed.
