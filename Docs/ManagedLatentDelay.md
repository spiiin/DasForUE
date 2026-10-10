# Managed latent delay callbacks

Added 2026-10-10. **Not built or tested**. UHT/build, actual latent action updates,
completion/abort, reentry, lifecycle, GC, PIE and packaging remain unverified.

```das
ue_latent_delay("intro",0.5,"OnIntroFinished")

[ue_function(BlueprintCallable)]
def OnIntroFinished():void { self.Completed += 1 }
```

This is a world-owned Unreal Latent Action Manager adapter with an exported void
completion callback. It is not coroutine suspension, await syntax or worker-thread
execution. Native pending action state holds elapsed duration and a weak callback
target; no DAS stack/closure or context address is kept by the pending action.

API:
- ue_latent_delay(name, seconds, completion): add/replace a named pending delay.
- ue_latent_cancel(name): false if absent, true if its managed record was cancelled.
- ue_latent_cancel_all(): number of latent records removed in the current session.
- ue_latent_is_pending(name): includes a queued completion not yet delivered.

Names are scoped to owner/script session and separate from timer/input namespaces.
Duration must be finite/nonnegative. Zero completes on a later latent-manager update,
which is not guaranteed to be a separate rendered frame; use next-tick timers for
that API's scheduling semantics. Delay uses Unreal world update delta/time behavior,
not wall-clock promises. Runtime requires owner's live game world and persistent
session, refuses start in bind_events/EndPlay, and caps active records at 64/session.

Short class names resolve to annotated non-static method():void exports, including
forward declarations. Plain exported handlers may use their exported names. Callback
signature/rate/world/count validation happens before replacing the previous record.
No captured argument/closure form is added here.

Each operation gets a private transient UObject callback proxy and its own latent
UUID scope. Cancellation removes actions only for that target; unrelated Blueprint/
native latent targets are not touched. Existing managed proxy queue handles delivery
when the VM is idle. Completion is consumed before entering user code, allowing
same-name rearm. The proxy is strongly held across its native completion trampoline
even if callback dispatch removes its session record and triggers GC.

Session binding teardown removes latent actions on EndPlay/release/world cleanup/
shutdown/reinstancing/accepted reload. Failed candidate reload retains the old
operation. No latent recipe restoration is introduced; accepting new code cancels
pending delay, and BeginPlay is not replayed. Required completion method contracts
remain registered until session release. Callback errors use existing event reporting.

Example: dsl_latent_delay.das. Source import and observed completion counts have
not been run. Timers remain their distinct TimerManager API; latent cancellation
does not affect timers, input handlers or unrelated managed event subscriptions.

Still separate: await/coroutine DSL, arguments/results, async asset/task adapters,
thread handoff, named operation migration and general Blueprint latent invocation.

Pending checks: UHT Complete(int32) signature and latent manager linkage; once/rearm/
replace; queued cancelled IDs; manager removal while dispatching; zero/paused/time
dilation behavior; exact target cleanup; invalid callback preserves previous work;
GC during completion; accepted/rejected reload; actor/component/multi-world ownership;
EndPlay/world shutdown/reinstancing; real PIE and packaged Development.
