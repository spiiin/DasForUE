# Callback Arguments

Queued callbacks, meaning managed timers (interval, repeat, next-tick) and `ue_event` handlers bound to native or authored multicast events, can receive typed arguments. This page covers which argument types are allowed, how many, and how the captured values are owned. The timer API itself is in [Timers](Timers.md).

## Handler rules

- A named managed handler is a non-static instance method of the same authored class, passed as `self.Name`. It may be declared later in the class. No annotation is needed: the compiler generates its checked entry point and marks it `BlueprintInternalUseOnly`, hiding it from the Blueprint palette. An annotation-only `[ue_function]` also stays hidden when used this way.
- Explicit `[ue_function(BlueprintCallable)]` retains public Blueprint visibility. String callback names still require an explicitly reflected method; use `self.Name` for automatic hidden exports. Ordinary helpers that are never used as callbacks remain script-only.
- Lifecycle and `ue_event` methods keep their own entry points and cannot also be managed callback targets. Automatic promotion currently resolves methods declared on the same class; inherited method references are not added by this step.
- It returns `void` and takes **0 to 16** by-value arguments, in the same order and types as the capture (timers) or the event signature (events).
- `var` on a parameter allows editing the delivery's local copy. `&`, ref/out parameters and argument defaults are rejected.
- Handler signature and payload are validated **before** an existing named timer is replaced. A mismatch keeps the previous timer.

```das
[ue_class]
class Counter : Actor {
    @ue_property=(BlueprintReadWrite)
    Score:int = 0
    [ue_function(BlueprintCallable)]
    def Start():void { ue_timer_start("pulse", 1.0f, self.Pulse, true) }
    def Pulse():void { self.Score += 1 } // Hidden from the Blueprint palette.
}
```

Hidden handlers still have reflected transport functions for widget binding and runtime dispatch. They use the same payload validation, hot reload and owner cleanup as explicit handlers. Reapplying after adding/removing an explicit `BlueprintCallable` updates the visibility.

Plain exported (non-class) DAS functions can use the scalar timer overloads with their exported callback name; array packing, typed enums/structs and multiple arguments are generated only for authored class callbacks.

## Inline managed callbacks

Inside a `[ue_class]` instance method, managed callback arguments also accept a `$` block:

```das
ue_timer_start("pulse",1.0f,$ { self.Score+=1 },true)
ue_enhanced_bind(self.MoveAction,TriggerEvent.Triggered,$(value:float) {
    self.Speed=value
})
```

The block is lifted into a generated hidden `__ue_cb_<hash>` instance method and then uses the same
signature checks, queuing and cleanup as a named callback. Parameters need explicit types,
and the result must be `void`. Locals declared **inside** the block are allowed.
This form applies to the managed timer, input, Enhanced Input, latent, async asset/soft-load,
widget click, effects, montage and AI move callback arguments. It does not change ordinary
daScript function/block arguments or the `[ue_event]` declaration syntax.

Only `self` is captured implicitly. Capturing an enclosing local variable, method parameter
or loop iterator is a compile error, including `let` constants. Supply a timer payload instead:

```das
let amount=3
ue_timer_start("award",1.0f,$(payload:int) {
    self.Score+=payload
},false,amount)
```

Here `amount` is evaluated when the timer starts and copied through the existing checked payload
transport; the block receives it as `payload`. No closure retaining a daScript stack frame is created.
Module globals remain ordinary global accesses, with their existing reload/state semantics.

Names are derived from the class, containing method, ordinal of the managed callback call in that
method, and canonical block AST encoded as UTF-8. They contain no memory addresses, absolute file
paths or line numbers. Moving the file, adding comments/blank lines or editing unrelated code keeps
the callback identity. Editing the block or inserting/reordering earlier callback calls may change
its identity and require Apply/reimport of the generated Blueprint methods. Compatible runtime
reload preserves existing timer/input recipes; an invalid candidate keeps the working revision.

## Supported argument types

| Type family | Timers | `ue_event` handlers |
| --- | --- | --- |
| `uint8`, `int`, `float`, `double`, `bool`, `string`, `UENameValue`, `FText` | Yes | Yes |
| `UEVector2D`, `UEVector`, `UERotator`, `UETransform` | Yes | Yes |
| Concrete native/authored enum | Yes, exact type | Yes, exact type |
| Concrete native/authored value struct | Yes, registered supported layout | Yes, registered supported layout |
| Typed object/interface reference | Yes | Yes |
| `array<T>` of the primitive types above | Yes | Yes (native const-ref array) |
| `array<T>` of enum, struct or object reference | Yes | Yes |
| `table<K;V>` (map) and set | Supported carrier shapes | Supported carrier shapes |
| Class reference, delegate | No | No |
| Nested containers, fixed C++ arrays, pointers | No | No |

Details:

- **Enums** keep their exact UEnum identity and member number. Only the byte-sized domain (members 0..254) is supported; wrong types and undefined members are rejected.
- **Structs** keep their exact UScriptStruct identity and are captured as an owned native copy. Nested supported fields, enums, strings, FText, object/interface references and containers are allowed. Structs with class/delegate fields, unsupported fields, fixed arrays and opaque/recursive layouts are rejected. Structural similarity is not enough: the source type must match exactly.
- **Map/set keys** can be `int`, `uint8`, `string`, name, enum or native object. `FText` is a value, never a key. Interface object keys/sets are not supported.
- **Numbers** must be finite. Strings may not contain NUL.
- A raw untyped `UEStructValue`/`UEEnumValue` declaration does not replace a concrete type. Names resolve through the class's type folder/registry (`type_folder=`/`type_registry=` on `ue_class`) or native type annotations.

Native enum-class delegate properties use their canonical enum/byte underlying storage (as Kismet does). Assets saved with an older native enum delegate layout need a normal DSL reimport. Authored multicast array inputs use native const-ref signatures; DAS handlers still receive by-value copies.

For native events, the source parameter may be by-value or native `const ref`; mutable native ref/out sources are rejected for subscriptions. A native event object may widen to the handler's declared parent class/interface, with a checked runtime conversion.

## Examples

Single typed values (registered `GameMessage` struct and `GameState` enum):

```das
[ue_class(type_registry="/Game/GameTypes/Registry")]
class Notices : Actor {
    def BeginPlay():void {
        var message=GameMessage()
        message.Score=17
        ue_timer_start("message",1.0f,"Remember",true,message)
        ue_timer_next_tick("state","ChangeState",GameState.Active)
        message.Score=99   // does not affect the capture
    }
    [ue_function(BlueprintCallable)]
    def Remember(value:GameMessage):void {
        // Every repeat receives an independent snapshot with Score=17.
    }
    [ue_function(BlueprintCallable)]
    def ChangeState(value:GameState):void {}
    [ue_event(name="MessageChanged")]
    def OnMessageChanged(value:GameMessage):void {}
}
```

Containers:

```das
def ScheduleStates():void {
    var states:array<CallbackState> = [CallbackState.Active,CallbackState.Idle]
    ue_timer_next_tick("states","ReceiveStates",states)
    clear(states)   // the timer owns its own snapshot
}
[ue_function(BlueprintCallable)]
def ReceiveStates(var value:array<CallbackState>):void {
    clear(value)    // changes only this delivery's copy
}
[ue_event(name="RecordsChanged")]
def OnRecords(var value:table<string;CallbackRecord>):void {
    print("records: {length(value)}\n")
}
```

Multiple arguments, including a native five-argument event:

```das
def Schedule():void {
    var weights:array<int> = [3,7]
    let title=ue_loctext(Saved, "Saved")
    ue_timer_next_tick("update","UpdateScore",17,title,weights)
    ue_timer_start("repeat",0.1f,"UpdateScore",true,17,title,weights)
    clear(weights)
}
[ue_function(BlueprintCallable)]
def UpdateScore(score:int;title:FText;var weights:array<int>):void {
    self.Score=score
}
[ue_event(name="OnTakeAnyDamage")]
def OnDamage(damaged:CallbackActorRef;amount:float;kind:CallbackObjectRef;
             instigator:CallbackObjectRef;source:CallbackActorRef):void {
    self.Score+=int(amount)
}
```

Object references:

```das
[ue_function(BlueprintCallable)]
def Schedule(target:CallbackObjectRef):void {
    ue_timer_next_tick("notify","Notify",target)
}
[ue_function(BlueprintCallable)]
def Notify(target:CallbackObjectRef):void {}
```

Example scripts in `Plugins/DasForUE/Content/Das/Scripts/`: `dsl_callback_containers.das` with `ue_callback_container_signals.das` (containers, multi-argument, object captures, native five-argument event), `dsl_text_byte_timers.das` with `ue_callback_value_types.das` / `ue_callback_value_signals.das` (enums, structs, FText, uint8, objects).

## Ownership and copy semantics

- Each queued invocation owns a complete native snapshot. Changing the source variable, or editing one delivery's copy, never changes later deliveries or repeats.
- `FText` keeps its native localization history (namespace/key, invariant text). Display text can still update by Unreal's normal localization rules.
- Object identities are shared, not deep-copied: the handler sees the same UObject, whose state may have changed.
- Captured objects are kept alive against GC until the capture is released. Releasing a capture does not destroy the object.
- Lifetime is checked at scheduling and again before delivery. Null is a valid reference. If a referenced Actor/component (or its owner) is destroyed, the whole payload is invalid even after GC cleared the pointer: a stale event is dropped and a stale timer, including a repeating one, is removed. A destroyed object never turns into a silently-accepted null.
- Cancellation, replacement, owner/session release, EndPlay, accepted reload and runtime shutdown release captures. A capture stays valid through a callback that is already executing, so a handler may cancel its own timer safely. A rejected reload keeps the live session and its captures.

## Limits

| Limit | Value |
| --- | --- |
| Arguments per handler | 0..16 |
| Array elements | 4096 per array |
| String | 4096 TCHARs per string; 1,048,576 total in a string array |
| FText | 65,536 TCHARs each for source and display; 1024 for namespace/key; 1,048,576 combined in a text array |

Element, depth, text and byte budgets are applied recursively and to the whole argument list.

See also: [Timers](Timers.md), [Delegates and events](../DSL/DelegatesAndEvents.md), [Containers](../DSL/Containers.md), [Struct snapshots](../../Reference/StructSnapshots.md), [Type support](../../Reference/TypeSupport.md)
