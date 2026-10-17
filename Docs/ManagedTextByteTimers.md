# FText and uint8 timer arguments

`ue_timer_start(name, seconds, callback, repeat, value)` and
`ue_timer_next_tick(name, callback, value)` accept `uint8` and `FText` values.
Authored class callbacks additionally accept `array<uint8>` and `array<FText>`,
using the same lowering as the existing int/double/string arrays. Scalar overloads
are also available to ordinary exported DAS callbacks.

```das
options gen2
require ue_text

[ue_class(Blueprintable)]
class Notice : Actor {
    properties=ue_properties(BlueprintReadWrite) {
        var Title:FText=ue_text_invariant("unset")
        var Priority:uint8=0
        var Total:int=0
    }
    def BeginPlay():void {
        var title=ue_text("MyGame","Reminder","Remember this")
        ue_timer_start("reminder",1.0f,"Remember",true,title)
        title=ue_text_invariant("This reassignment does not change the timer")
        ue_timer_next_tick("priority","SetPriority",uint8(255))
        var values:array<uint8> = [uint8(0),uint8(255)]
        ue_timer_start("values",1.0f,"Apply",true,values)
        clear(values)
        var messages:array<FText> = [ue_text("MyGame","Array","Message")]
        ue_timer_next_tick("messages","ShowMessages",messages)
    }
    [ue_function(BlueprintCallable)]
    def Remember(value:FText):void { self.Title=value }
    [ue_function(BlueprintCallable)]
    def SetPriority(value:uint8):void { self.Priority=value }
    [ue_function(BlueprintCallable)]
    def Apply(var values:array<uint8>):void {
        for (value in values) { self.Total+=int(value) }
        clear(values)
    }
    [ue_function(BlueprintCallable)]
    def ShowMessages(values:array<FText>):void {
        if(length(values)>0) { self.Title=values[0] }
    }
}
```

The handler returns void and accepts exactly one matching by-value argument.
`var values:array<T>` permits editing that delivery's local array; `&` and out/ref
parameters are rejected. Use an actual uint8 value (for example `uint8(255)`),
since an ordinary integer literal selects the int timer overload.

Captured FText retains native localization history, including namespace/key and
invariant text. Timers own FText and FInstancedStruct values rather than VM tokens
or pointers into DAS arrays. Every repeat unpacks a fresh owned array. Changing
the original variable or clearing the handler's array does not change later calls.
Localization can still update the display text according to Unreal's normal rules.

Existing naming, replacement, cancellation, pause/resume and owner lifecycle
semantics are unchanged. Validation runs before replacement: a mismatched handler
or invalid new payload leaves the previous named timer intact. Session release,
reload and EndPlay clear native captures along with the timer. An active host Tick
can initialize a released session again, so native tests stop host ticking before
checking that release prevents future delivery. `FDasRuntimeModule::ScriptTimerCount`
provides the same kind of read-only ownership diagnostic as SubscriptionCount.

Array snapshots retain the 4096-element limit. FText source and display strings
are limited to 65536 TCHARs each; namespace/key to 1024 each. Text arrays additionally
limit the combined source/display character count to 1048576. The existing owner,
interval and timer-count checks still apply. The later enum/registered value-struct
extension is documented in ManagedCallbackValues.md. Object captures, nested
containers and closures remain unsupported.

The asynchronous fixture is `dsl_text_byte_timers.das`. A saved actor runs actual
TimerManager callbacks in PIE and packaged gameplay. It tests 0/255, localized and
invariant text, next-tick and repeats, copy isolation, mutation of callback arrays,
replacement/rejection, pause/resume, cancellation and release with pending captures.
Ref callbacks and unsupported array<UEVector> captures have negative DSL fixtures.

## Validation — 2026-10-17

Editor and packaged Win64 Development builds passed. The real PIE automation
reported 1 success, 0 warnings, 0 failures and 0 skipped tests. Saved fixture export,
packaged foundation smoke, cooked gameplay and the playable map all passed under
NullRHI. This verifies runtime behavior, rather than rendering or Shipping builds.

Both runners keep the native fixture carrier alive until the delayed release check;
destroying it earlier makes the checker lose its observation target. Pending timer
records are removed immediately on release, and counters remain unchanged on later
ticks. Unsupported ref and array<UEVector> callbacks are rejected during fixture export.

Commands: `Scripts/TestGameplayRuntimePIE.ps1 -LogPrefix text-byte-timers` and
`Scripts/TestCookedGameplay.ps1`. Evidence is preserved in
`logs/text-byte-timers-report/index.json`, `logs/text-byte-timers-runtime.log`,
`logs/text-byte-timers-foundation.log` and `logs/text-byte-timers-map.log`.
The cooked map is `/Game/DasCookedGameplay/bc5f26dba63e463e8a0aa0901baa615b/Map`.
Export/cook emitted the existing missing ACL compression-settings asset warnings;
build and runtime checks completed successfully.


## Shared callback policy extension

The shared capability table now also supports `array<float>`, `array<bool>` and
`array<UENameValue>` in both interval and next-tick class callbacks, as well as
native event subscriptions. All eight primitive array families use owned snapshots
and the same event/timer payload validator. Infinite/NaN numeric payloads are rejected.
See [TypeSupportMatrix.md](TypeSupportMatrix.md) for the complete surface comparison
and the remaining grouped work. The validation above describes the preceding step;
the shared-policy validation is recorded in that matrix document.
