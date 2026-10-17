# Container callback arguments

Timers (next-tick, interval and repeat) and queued event handlers accept supported
standalone arrays, maps and sets. This extends the enum/struct/object capture
implementation; it uses the same owned payloads, budgets and lifetime checks.

## Syntax

Inside a `ue_class`, using the aliases and event fields from the executable fixture:

```das
def ScheduleStates():void {
var states:array<CallbackState> = [CallbackState.Active,CallbackState.Idle]
ue_timer_next_tick("states", "ReceiveStates", states)
// The timer owns a snapshot; changing the source does not change the capture.
clear(states)
}

[ue_function(BlueprintCallable)]
def ReceiveStates(var value:array<CallbackState>):void {
    // By value: the handler may change its private copy.
    clear(value)
}

[ue_event(name="RecordsChanged")]
def OnRecords(var value:table<string;CallbackRecord>):void {
    print("records: {length(value)}\n")
}
```

Executable examples: `dsl_callback_containers.das` and
`ue_callback_container_signals.das`. Their aliases and reflected declarations
come from the existing explicit type registry. No new user-facing carrier paths
or container wire names are needed.

## Supported shapes

- Arrays: existing primitives/FText, registered enums, registered value structs
  (including canonical native math aliases), and typed native object references.
- Sets: supported hash keys: int, uint8, string, name, enum, native object.
- Maps: the same key policy; supported primitive, enum, struct and native object
  values. FText is a value, not a hash key.

The canonical container registry remains authoritative. Direct nested containers,
class/delegate captures and interface object keys/sets are unsupported. A struct
element may contain its own supported reflected fields. Native object class
requirements and the registered struct policy are unchanged.

## Transport and ownership

Containers use the existing struct snapshot ABI internally with the exact
canonical carrier identity. Equal-looking but distinct carriers do not match.
Native event receivers use the shared reflected value reader and accept by-value
or native const-ref source parameters. Mutable native ref/out subscriptions remain
unsupported; script handlers consume by-value snapshots.

Each delivery reconstructs an independent DAS value. Clearing the source or one
callback copy does not mutate queued captures or subsequent repeat deliveries.
Object identities inside the copy remain shared Unreal identities; UObject state
is not deep-copied. The common reflected GC root service retains referenced
objects until the queued event/timer releases its capture. Weak original identities
detect destroyed objects even after GC clears a reflected pointer: stale events
are dropped and stale repeating timers are removed. The whole payload is rejected
when a referenced object becomes invalid.

Existing cancellation, replacement, owner release, reload, EndPlay and PreExit
cleanup apply. Recursive element/text/string/finite-number budgets also apply.

## Validation

Validation runs in an isolated source copy at
`C:\Users\sanya\Documents\Codex\cccheck` because another chat continues to
change/build the main repository. Its editor/game outputs are separate. The copy
reuses the existing daScript third-party libraries through an adjusted Build.cs;
it does not replace the main project's DLLs. Results verify that recorded snapshot,
not an unrestricted claim about concurrent changes in the main checkout.

The fixture checks enum/struct/math arrays, struct/text maps, enum/int sets,
next-tick and repeated delivery, source/per-delivery mutation isolation, a native
const-ref Actor array, and object array/map capture survival across forced GC.
Destroyed Actor captures must deliver no callbacks and leave no live timers.
Foundation checks cover carrier identity and native reflected container frames.


Native signature duplication preserves the source computed property traits after
linking (including array ZeroConstructor), before the exact layout/flag check.
Automatic native enum storage is resolved by the shared DasResolvedEnumStorage
service in both the property factory and the container registry. Native enum-array
fixtures exercise that consistency in addition to authored enum arrays.

PIE and fixture export must run sequentially within one validation project: both
create/update shared reflected type assets. Separate source checkouts alone do not
make concurrent asset writers within the same project safe.


### Completed validation

Editor and Game Win64 Development builds passed. Strict real PIE returned exactly
one success, zero warnings/failures/skips. Final editor and packaged foundation
both returned PASS with failures=0, including native UHT array/struct receiver
flags, exact snapshot identities, by-value/const-ref frames and mutable-ref rejection.
Saved fixture export, packaged gameplay and the playable map also passed.

Positive object array/map captures delivered three repeats each, two next-tick
values and two events, with zero errors and pending timers. Destroyed Actor
captures delivered zero timer callbacks and left zero pending timers. All eight
nonreference container shapes passed copy isolation and delivery checks.

The final source manifest covered 1286 files with no changes during validation.
This guarantee applies to the isolated snapshot; the main checkout remained
editable by the other chat. Final editor processes used UE_SKIP_UBT_SDK_SETUP=1
to reuse the configured SDK without waiting for a concurrent UBT build.
Runs used NullRHI; Shipping and physical rendering were not tested.

Evidence: `logs/callback-containers-validation.json`,
`logs/callback-containers-source-sha256.json`,
`logs/callback-containers-report/index.json`,
`logs/callback-containers-editor-foundation.log`,
`logs/callback-containers-foundation.log`,
`logs/callback-containers-export.log`, `logs/callback-containers-runtime.log`,
`logs/callback-containers-map.log` and the corresponding build logs.

Cooked map: `/Game/DasCookedGameplay/91398d3fa0754efbb0f4e01bd1729362/Map`.

Authored multicast input arrays use native const-ref signatures to match Kismet;
DAS callbacks still receive independent by-value copies.


Multiple typed arguments are supported by authored class timers/events; see
[MultiArgumentCallbacks.md](MultiArgumentCallbacks.md).
