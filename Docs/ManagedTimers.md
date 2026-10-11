# Owner-managed named timers

Implementation added 2026-10-10. **Not built or tested**. No commandlet, PIE,
runtime example, cook or packaged test was run for this batch.

```das
def BeginPlay():void {
    ue_timer_start("score",1.0,"Pulse",true)
}
[ue_function(BlueprintCallable)]
def Pulse():void {
    self.Score += 1
}
```

Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_managed_timers.das`.
Callbacks are ordinary annotated instance methods returning explicit void and
accepting no arguments. Short literal method names lower to existing exported
entries in ue_class bodies, including references to methods declared later.
No new callback/delegate annotation is required.

In ordinary exported DAS functions, use an exported handler():void name directly;
see `managed_timer_functions.das`. Timers require a persistent owner session and
its live world. Standalone RunFile and candidate global initialization cannot
create timers. Closures, captured arguments and next-tick callbacks are not supported
by this initial adapter. Intervals must be positive finite float values.

API:
- `ue_timer_start(name, interval, handler, repeat)` creates/replaces a named timer.
- `ue_timer_cancel(name)` returns false when absent, true when cancelled.
- `ue_timer_pause(name)` / `ue_timer_resume(name)` return false when absent.
- `ue_timer_is_active(name)` is false for absent/paused timers.
- `ue_timer_remaining(name)` returns seconds or -1 when absent.

Names are scoped to one owner/script VM session. Different owners or scripts can
reuse names. A successful replacement cancels the old timer before installing the
new one; rate/handler/world/count validation precedes that cancellation. Limits:
64 named timers per session, 1024 globally, 128 characters per name, 1024 queued
callback IDs and dispatches per drain. Repeated queued ticks of one ID coalesce.

The UE TimerManager owns the schedule. Adapters keep weak owner/world references
and opaque internal IDs; no public raw FTimerHandle or dangling VM callback pointer.
Callbacks are queued while a script is active and run through persistent exported
void dispatch when idle. One-shot records are removed before their callback so
that callback can rearm the same name. Callback errors cancel that timer, log the
error, and update the Actor/Component LastDasError when applicable.

Timer handlers participate in the session's existing required-entry contracts.
Accepted logic reload cancels existing timers; rejected reload keeps the old VM
and timers. A callback tick that discovers and accepts new code can invoke the new
handler once while future old scheduling is cancelled. BeginPlay is not replayed:
call Start or another explicit setup method to recreate timers after reload.
Cancellation does not remove the session's previously recorded method contracts.

EndPlay, owner/session release, world cleanup, shutdown, and editor reinstancing
cancel timers. Structural reinstancing cancels timers even when contexts are
preserved; schedules are not migrated. No timer start during EndPlay or bind_events.
EndPlay code should rely on automatic cancellation instead of querying a closed
session. Broad structural migration remains deferred.

Pending validation: build; handler signature/name errors; forward method resolution;
owner isolation and multi-world timing; repeating/one-shot/rearm/replace; pause/resume;
expiry/limits; reentry queues; errors and cancellation; rejected/accepted reload;
EndPlay before BeginPlay, Destroy/GC, world cleanup, reinstancing and shutdown;
DSL import/reopen, real PIE and packaged Development.

One captured scalar/name/math value was added subsequently, without build/tests;
see ManagedTimerArguments.md. Earlier no-argument limits describe the initial slice.

Named next-tick scheduling, paused/elapsed queries and session cancel-all were
added subsequently (validation pending); see ManagedNextTickTimers.md.

2026-10-11: repeating, one-shot array capture, next-tick Vector2D capture, cancellation, owner isolation and Destroy/EndPIE cleanup passed in real PIE and packaged Development. Other earlier pending timer edge cases remain targeted future checks; see [AccumulatedValidation.md](AccumulatedValidation.md).
