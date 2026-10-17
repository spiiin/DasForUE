# Multiple arguments in events and timers

Annotated instance event handlers and managed class timers accept 2–16 typed
arguments, using the same ownership rules as existing zero/one-argument calls.

## Syntax

Inside a `ue_class`, with aliases from the explicit type registry:

```das
def Schedule():void {
    var weights:array<int> = [3,7]
    let title=ue_text("Example","Saved","Saved")
    ue_timer_next_tick("update", "UpdateScore", 17, title, weights)
    ue_timer_start("repeat", 0.1f, "UpdateScore", true, 17, title, weights)
    clear(weights)
}

[ue_function(BlueprintCallable)]
def UpdateScore(score:int;title:FText;var weights:array<int>):void {
    self.Score=score
    clear(weights) // Only this delivery's copy is changed.
}

[ue_event(name="OnTakeAnyDamage")]
def OnDamage(damaged:CallbackActorRef;amount:float;kind:CallbackObjectRef;
             instigator:CallbackObjectRef;source:CallbackActorRef):void {
    self.Score+=int(amount)
}
```

The timer handler is a literal name of an annotated void instance method in the
same authored class. Captures follow the existing timer arguments; their order
and types match the method. The examples above use `CallbackActorRef` (AActor)
and `CallbackObjectRef` (UObject) aliases from `ue_callback_container_signals.das`.
Native event objects can widen to their declared parent class/interface. Null
references remain null. Enum and struct identities must match exactly.

Runnable coverage: `dsl_callback_containers.das` and
`ue_callback_container_signals.das` under `Plugins/DasForUE/Content/Das/Scripts`.
The fixture has a six-argument authored multicast, repeated/next-tick timers,
three-argument object captures, a vector/name/map/set tuple, cancellation,
rejected replacement preservation and a native five-argument event.

## Types and ownership

Supported argument types follow the shared callback/property services: scalar
values, FText, uint8, registered enums/value structs, canonical math aliases,
typed object references and supported standalone arrays/maps/sets. Existing
container restrictions apply. Class/delegate values, mutable ref/out and default
arguments are rejected. Script handlers receive values; native source parameters
may be by value or const-ref.

Each queued invocation owns a complete snapshot. Changes to a source collection
or one delivery do not change later deliveries. UObject identities are shared;
their mutable state is not cloned. The existing GC root service retains captured
objects; destroyed references invalidate the whole capture even after GC has
cleared native pointers. Cancellation, replacement, owner release, EndPlay and
accepted reload release captures through existing timer/event cleanup. Rejected
reload preserves the live session. The existing combined element/depth/text/byte
budgets apply to the entire argument list.

## Internal representation

The DSL generates one private `UEStructValue` bridge with a registered transient
native struct containing `arg_0`…`arg_N`. Its internal path is derived from ordered
storage identities and its live layout is verified by the common fingerprint
service. Containers use their existing canonical carriers inside that struct.
Users do not name or store these paths. This reuses the existing queue,
marshalling, GC and lifecycle implementation instead of introducing a second
multi-argument dispatch engine.

Timer construction uses the existing argument setters and releases the temporary
call frame in a finalizer. Signature/payload validation completes before replacing
an existing timer. Immutable snapshot reuse preserves FText localization history even in editor,
where Unreal property equality may compare display strings. The native property
iterator also checks map keys, so reordered storage cannot hide changed key/text
associations.

Native event receivers clone the complete source signature
and preserve computed native property traits.

## Verification

Verification uses the isolated `C:\Users\sanya\Documents\Codex\cccheck`
snapshot, because another chat is changing/building the main repository.
See `logs/multi-callbacks-validation.json` for the recorded result and source
manifest. No commit is made by this step.


Completed: Editor/Game Development builds; editor/packaged Foundation (zero
failures); strict real PIE (one success, no warnings/failures/skips); export,
cook/stage/package; packaged gameplay and playable-map probes. The recorded
1298 source files stayed unchanged through verification. Main checkout
differences are listed in the JSON report because another chat continues work.
