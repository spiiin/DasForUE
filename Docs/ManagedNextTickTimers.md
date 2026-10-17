# Named next-tick timers and schedule queries

Current cross-surface support and shared callback policy:
[TypeSupportMatrix.md](TypeSupportMatrix.md). All eight primitive array families
are supported by authored class timers; the lists below describe earlier stages.

Current FText/uint8 extension and integration evidence:
[ManagedTextByteTimers.md](ManagedTextByteTimers.md). The initial validation notes
below describe the earlier implementation stage.

Added 2026-10-10. **Not built or tested**. No UHT/build, source import, scheduling,
ABI/callback, pause/query, reentry, PIE or packaged execution was run.

```das
ue_timer_next_tick("refresh","Refresh")
ue_timer_next_tick("score","AddScore",5)
```

Next-tick timers use UE TimerManager::SetTimerForNextTick. They run once according
to the world's TimerManager tick semantics; they are not wall-clock/worker-thread
tasks and do not force a world tick. Callback IDs use the existing idle dispatch
queue rather than directly entering an already active VM.

Same owner/script name means replacement, shared with interval timers. Calling
next_tick twice under one name cancels the first schedule. Other owners/sessions
are unaffected. The same handler/count/world/capture validation occurs before
replacement. Starting during EndPlay or bind_events remains refused.

Supported arguments match previous timer stages: zero arguments, one scalar/name/
math value, and int/double/string arrays for class callbacks. Example declarations:
```das
[ue_function(BlueprintCallable)]
def Refresh():void { self.Score += 1 }
[ue_function(BlueprintCallable)]
def AddScore(amount:int):void { self.Score += amount }
```
Short literal names and array packing use one shared class lowering helper for
interval and next-tick calls. Captures keep their owned-copy semantics. Next-tick
records are removed before invocation; a callback may schedule the same name again.

Additional APIs:
- ue_timer_is_paused(name): false when absent; forwards UE pause state.
- ue_timer_elapsed(name): UE elapsed seconds, -1 when absent.
- ue_timer_cancel_all(): cancels current owner/script session timers, returns count.

Queries follow UE TimerManager's active/pending/paused semantics, including any
special behavior of next-tick handles. Cancellation invalidates pending delivery
IDs through the existing timer-record lookup; no cancelled callback body is invoked.
Calls require a live persistent session. Querying a fired one-shot sees no record.
Cancel-all does not erase previously registered callback signature contracts.

Existing EndPlay/release/world shutdown/reinstancing/accepted reload cancel schedules;
rejected reload keeps old schedules. There is no new migration or auto-restart policy.
No additional async/task, input, coroutine or closure integration is implied.

Example: dsl_next_tick_timers.das, including zero/int/array requests, same-name
replacement and session-local cancellation/state queries. Example output has not
been observed in a running editor.

Pending checks: zero/all captured value overloads; source forward declarations;
real next-tick timing, callback rearm and interval/next-tick replacement; pause and
elapsed behavior for pending handles; cancel-all owner/world isolation; cancelled
queued IDs; limits/error preservation; reload/teardown, PIE and packaged Development.

2026-10-11 validation: cancellation, typed Vector2D capture and owner-Destroy cleanup passed in actual PIE and packaged Development. See [AccumulatedValidation.md](AccumulatedValidation.md) for the exact scope. The initial untested status above is historical.
