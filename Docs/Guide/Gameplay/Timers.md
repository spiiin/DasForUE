# Timers and Latent Delays

Scripts schedule deferred work through owner-managed, named timers backed by the world's TimerManager, and through a latent delay backed by the Latent Action Manager. Both call an ordinary method of your class, clean up automatically with the owner, and never hold pointers into the VM.

## Interval and repeating timers

```das
options gen2
require ue_dsl

[ue_class(module_id="timers",Blueprintable)]
class TimerCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=0

    def BeginPlay():void {
        ue_timer_start("score",1.0,"Pulse",true)
    }
    [ue_function(BlueprintCallable)]
    def Pulse():void {
        self.Score+=1
    }
    [ue_function(BlueprintCallable)]
    def Stop():bool { return ue_timer_cancel("score") }
}
```

Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_managed_timers.das`. For plain exported functions (outside `ue_class`) pass the exported `handler():void` name directly; see `managed_timer_functions.das`.

| Function | Result |
| --- | --- |
| `ue_timer_start(name, interval, handler, repeat [, args...])` | Create or replace a named timer. |
| `ue_timer_next_tick(name, handler [, args...])` | Run once on the next TimerManager tick. |
| `ue_timer_cancel(name)` | `true` if cancelled, `false` if absent. |
| `ue_timer_cancel_all()` | Cancel every timer of this owner/script session; returns the count. |
| `ue_timer_pause(name)` / `ue_timer_resume(name)` | `false` if absent. |
| `ue_timer_is_active(name)` | `false` for absent or paused timers. |
| `ue_timer_is_paused(name)` | `false` when absent; otherwise UE pause state. |
| `ue_timer_remaining(name)` | Seconds, or `-1` when absent. |
| `ue_timer_elapsed(name)` | UE elapsed seconds, or `-1` when absent. |

- The handler is an annotated instance method returning `void`, named by a short string literal; methods declared later in the class are resolved. No extra callback annotation is needed.
- `interval` must be a positive finite float.
- Queries follow TimerManager semantics, including those of pending next-tick handles. A fired one-shot timer has no record.

## Passing arguments

Trailing arguments after `repeat` (or after `handler` for next-tick) are captured at start and delivered to the handler:

```das
ue_timer_start("score",1.0,"AddScore",true,5)
ue_timer_next_tick("priority","SetPriority",uint8(255))   // uint8 needs an explicit cast

[ue_function(BlueprintCallable)]
def AddScore(amount:int):void { self.Score+=amount }
[ue_function(BlueprintCallable)]
def SetPriority(value:uint8):void { self.Priority=value }
```

Up to 16 arguments of scalars, FText, math types, enums, structs, object references and supported arrays/maps/sets are allowed. Each delivery, including every repeat, receives a fresh copy of the original capture; to change the captured value, start the timer again under the same name. A plain integer literal selects the `int` overload, so write `uint8(255)` for a byte. See [Callback arguments](CallbackArguments.md) for the full type table, ownership rules and limits.

Examples: `dsl_timer_arguments.das`, `dsl_timer_array_arguments.das`, `dsl_next_tick_timers.das`, `dsl_text_byte_timers.das`.

## Names and replacement

- Names are scoped to one owner/script VM session; other owners or scripts can reuse a name. Interval and next-tick timers share the namespace.
- Starting a name that exists replaces it. Rate, handler, world, count and payload validation run first, so an invalid request leaves the old timer running.
- One-shot and next-tick records are removed before the handler runs, so a handler can re-arm the same name.

| Limit | Value |
| --- | --- |
| Named timers per session | 64 |
| Named timers globally | 1024 |
| Name length | 128 characters |
| Queued callback IDs / dispatches per drain | 1024 |

Repeated queued ticks of one timer coalesce.

## Execution and errors

The TimerManager owns the schedule; the adapter keeps only weak owner/world references and opaque IDs (no public `FTimerHandle`). Callbacks that fire while a script is running are queued and dispatched when the VM is idle. Next-tick timers use `SetTimerForNextTick`: they run on the world's tick, not on wall-clock or a worker thread, and do not force a tick.

A callback error cancels that timer, logs the error, and updates the Actor/Component `LastDasError` when applicable.

## Lifetime and hot reload

- Timers require a persistent owner session and its live game world. Standalone `RunFile` and global initializers cannot create timers; starting timers during EndPlay or event binding is refused.
- EndPlay, owner/session release, Destroy, world cleanup, shutdown and editor reinstancing cancel timers. Structural reinstancing cancels them even if contexts are preserved; schedules are not migrated. In EndPlay, rely on automatic cancellation rather than querying the session.
- **Accepted** logic reload cancels existing timers and their captures; a callback tick that discovers new code may invoke the new handler once. **Rejected** reload keeps the old VM and timers.
- `BeginPlay` is not replayed after reload. Provide an explicit method (for example `Start` or `ResumeBrain`) to recreate timers.

## Latent delay

`ue_latent_delay` uses Unreal's Latent Action Manager to call a method after a delay. It is not coroutine suspension, `await` syntax or a worker-thread task.

```das
[ue_class(module_id="latent",Blueprintable)]
class LatentActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Completed:int=0

    def BeginPlay():void {
        ue_latent_delay("intro",0.5,"OnIntroFinished")
    }
    [ue_function(BlueprintCallable)]
    def OnIntroFinished():void { self.Completed+=1 }
}
```

| Function | Result |
| --- | --- |
| `ue_latent_delay(name, seconds, completion)` | Add or replace a named pending delay. |
| `ue_latent_cancel(name)` | `true` if the managed record was cancelled, `false` if absent. |
| `ue_latent_cancel_all()` | Number of latent records removed in this session. |
| `ue_latent_is_pending(name)` | `true` while pending, including a queued, not yet delivered completion. |

- Names are per owner/script session and separate from timer and input names. Up to 64 active delays per session.
- Duration must be finite and non-negative. Zero completes on a later latent-manager update, which is not guaranteed to be a separate rendered frame; use `ue_timer_next_tick` when you need next-tick semantics. Timing follows world delta/time dilation, not wall-clock.
- The completion is a `method():void` with no arguments. Validation happens before replacing an existing record. Completion is consumed before user code runs, so the handler can re-arm the same name.
- Each operation has its own private callback target; cancellation removes only that action and never touches unrelated Blueprint/native latent actions, timers or input handlers.
- EndPlay, release, world cleanup, shutdown, reinstancing and accepted reload remove pending delays; a failed reload keeps them. `BeginPlay` is not replayed.

Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_latent_delay.das`.

## Limitations

- No closures or variable-reference captures, and no ref/out handler parameters.
- No `await`/coroutine syntax, latent results/arguments, or general Blueprint latent node invocation. For asset loading see [Asset loading](../Data/AssetLoading.md).

See also: [Callback arguments](CallbackArguments.md), [Lifecycle](../DSL/Lifecycle.md), [Apply and hot reload](../Workflow/ApplyAndHotReload.md), [Delegates and events](../DSL/DelegatesAndEvents.md)
