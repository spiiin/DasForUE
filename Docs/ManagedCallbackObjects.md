# Object references in managed callbacks

Typed native UObject/interface wrappers can be captured by class interval/repeat
timers and next-tick timers, and consumed by `ue_event` handlers. The user-facing
syntax is the same as for other typed method arguments:

```das
[ue_function(BlueprintCallable)]
def Schedule(target:CallbackObjectRef):void {
    ue_timer_next_tick("notify","Notify",target)
}
[ue_function(BlueprintCallable)]
def Notify(target:CallbackObjectRef):void {}
```

`CallbackObjectRef` is the native UObject wrapper from `ue_callback_value_types.das`.
Ordinary low-level exported functions use the internal object timer adapters;
the class collector generates packing and the expected class/interface path.
No context-owned VM token is stored across deferred delivery.

Registered native/authored structs can contain typed object/interface fields,
including references within supported nested structs, arrays and maps. The same
snapshot property validation and carrier registry used for synchronous methods
remains authoritative. Supported object-hash sets/maps in native snapshots are
traversed as well. Standalone object-array/map/set callback signatures are covered
in [ManagedCallbackContainers.md](ManagedCallbackContainers.md). New container
shapes, class references and delegate captures remain outside this extension.
Opaque native layouts, fixed arrays, recursive structs and mutable callback
ref/out parameters remain rejected.

## Ownership and destruction

`FDasCallbackPayload` is one stable heap owner per queued payload. It reuses
the shared `FDasPropertyValueRoots` reflected-value GC service, which owns a native
`FDasScalarValue` and traverses its references, including FInstancedStruct ARO.
No separate object-reference walker is introduced for callbacks.
The timer record and in-flight delivery share that owner; removing/cancelling a
timer from its own handler cannot free its arguments during execution. Repeated
delivery unpacks a fresh script value from the unchanged native snapshot.

The event queue uses the same owner, including generic native struct receivers.
Native object/interface event signatures use the existing generic receiver
mechanism, with the exact native parameter layout. The handler's concrete
object/interface type is retained in the declaration; incompatible runtime values
are not delivered. UObject events with a broader native source type retain
runtime checked conversion to a narrower handler wrapper.

Type identity, class/interface membership and actor/component lifetime are checked
at scheduling/enqueue and again before delivery. Null is a valid reference.
Destroying an Actor, destroying a component, or destroying its owner invalidates
the pending payload even though GC ownership still holds the allocation. A stale
event is dropped; a stale timer is removed (including a repeating timer).
Destroying one referenced member invalidates the whole struct payload. Original
non-null identities are retained as weak pointers through the same reflected GC
traversal. Thus a GC-cleared destroyed object cannot silently turn into an allowed
null argument. The identity collector does not implement a second field walker.

Cancellation, replacement, session release, reload, EndPlay and runtime shutdown
release their capture owners through the existing callback cleanup paths. A
shared owner lasts through an already executing callback. Objects may still be
retained by other Unreal references or live script snapshots; releasing a capture
does not forcibly destroy the object. Receiver function roots and PreExit teardown
from the enum/struct extension remain in place.

## Verification

The shared capability probe checks forced-GC survival of a nested object snapshot,
collection after releasing its only capture, mismatched scalar classes and a
destroyed component. Gameplay fixtures schedule a previously unowned UObject by
value and inside an authored struct (scalar/array/string-map fields), force GC,
and check next-tick/repeated delivery and per-delivery copy isolation. Existing
timer cancellation, owner release and late-delivery checks remain enabled.

See `dsl_text_byte_timers.das`, `ue_callback_value_types.das` and
`ue_callback_value_signals.das` for executable syntax. Validation results are
recorded below.

### Completed validation

Editor and Win64 Development game builds passed. Strict real PIE returned
1 success with 0 warnings, failures or skipped tests. Saved fixture export and
packaged foundation (`failures=0`), gameplay and playable map all passed, including
forced GC and normal process exit. Reference identity/configuration probes include
native interface signatures, valid implementers, nonimplementers and null values.
Positive object captures delivered three repeated values plus one independent
next-tick value; captures of a destroyed Actor delivered neither. Both owners had
zero pending timers at the delayed assertion and were then released/destroyed.
The dynamically spawned object fixtures cancel the placed fixture's unrelated
paused timer; the original placed owner retains its full pause/resume checks.

Copies preserve reference identities; UObject state itself is not deep-copied.
All runs used NullRHI; Shipping and physical rendering were not tested.
The cook retained the pre-existing missing ACL compression-settings warnings.

Commands: `Scripts/TestGameplayRuntimePIE.ps1 -LogPrefix callback-objects` and
`Scripts/TestCookedGameplay.ps1`. Evidence: `logs/callback-objects-validation.json`,
`logs/callback-objects-report/index.json`, `logs/callback-objects-foundation.log`,
`logs/callback-objects-runtime.log`, `logs/callback-objects-map.log`,
`logs/callback-objects-export.log` and `logs/callback-objects-game-build.log`.

Cooked map: `/Game/DasCookedGameplay/7a65caee22a1490e928bb6141dd0e721/Map`.


Standalone array/map/set callback signatures are now covered separately in
[ManagedCallbackContainers.md](ManagedCallbackContainers.md).
