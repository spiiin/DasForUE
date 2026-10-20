# Delegates and Events

The DSL has three related features: `ue_delegate` declares a single-cast delegate
type (one bound handler, may return a value), `ue_multicast` declares a multicast
signal that becomes a Blueprint Event Dispatcher when used as a field, and
`ue_event` subscribes a class method to an existing reflected multicast delegate
on the object, its owner or a component.

## Declaring delegate types

```das
options gen2
module ue_delegate_examples public
require ue_dsl public

[ue_delegate]
struct FScoreQuery {
    signature:function<(Value:int):int>
}

[ue_delegate]
struct FObjectValueDelegate {
    signature:function<(Object:UObjectHandle; Value:float):void>
}

[ue_multicast]
struct FScoreEvent {
    signature:function<(Counter:int):void>
}

// Method form: required when parameters carry direction markers.
[ue_delegate]
struct FValueMutation {
    def signature(var Count:int&; var Text:string&;
                  @ue_out var Accepted:bool&; @ue_out var Description:string&):int {}
}
```

- `signature` is a declaration marker, not a callable field. In the method form the
  body must be empty; calling it directly traps.
- Every parameter needs a name; at most 16 parameters. Multicast signatures must
  return `void`.
- `var T&` is inout (ref), `@ue_out var T&` is output only, `@ue_ref` is an optional
  explicit marker. Use the method form for these: the compact
  `function<...>` form cannot carry parameter annotations. Defaults, overloads,
  static signatures and reference returns are rejected.
- `type_folder`/`type_registry` options resolve authored enum/struct payloads.
- Unsupported shapes fail at compile or bind time; nothing is silently coerced.

Payload types: `int`, `float`, `double`, `bool`, `string`, `UENameValue`, `uint8`,
`FText`, native object wrappers (`ActorRef`, `ObjectRef`) or `UObjectHandle`,
class values (`ActorClass`), `UEVector`/`UERotator`/`UETransform`, native and
authored enums/structs, and supported arrays (including `array<uint8>` and
`array<FText>`). Handlers must have an exactly compatible UE signature; const
reference and native enum representations are not converted silently.

## Single-cast delegates

```das
var stored = FObjectValueDelegate()

var ready = ue_bind(type<FObjectValueDelegate>, ue_self_object(), "OnDelegateExecuted")
stored.BindUFunction(ue_self_object(), "OnDelegateExecuted")
if (stored.IsBound()) {
    stored.Execute(ue_self_object(), 1.0)
}
stored.ExecuteIfBound(ue_self_object(), 1.0)
stored.Unbind()
```

- The handler is a reflected instance `UFUNCTION` of the target with matching
  types, directions and layout (parameter names may differ). Native handlers do not
  need `BlueprintCallable`; DSL handlers are normal `ue_function` methods. Static,
  network and signature functions cannot be handlers.
- The target may be a `UObjectHandle` or a typed object reference; the method name
  a string or `UENameValue`.
- `Execute` returns the handler's result; executing an unbound delegate fails.
  `ExecuteIfBound` exists for `void` signatures.
- Delegates are values: copying one and then rebinding or unbinding a copy does
  not affect the other.
- A delegate stored in a global belongs to the owner's script context, so it can be
  bound in one method and invoked in another.

### Delegate fields, parameters and results

```das
[ue_class(module_id="example", Blueprintable)]
class DelegateOwner {
    @ue_property=(BlueprintReadWrite)
    Callback:FScoreQuery

    [ue_function(BlueprintCallable)]
    def Bind():void { self.Callback = ue_bind(type<FScoreQuery>, ue_self_object(), "Handler") }
    [ue_function(BlueprintCallable)]
    def Handler(value:int):int { return value + 1 }
    [ue_function(BlueprintCallable)]
    def Invoke(value:int):int { return self.Callback.Execute(value) }
    [ue_function(BlueprintCallable)]
    def Clear():void { self.Callback = FScoreQuery() }

    [ue_function(BlueprintCallable)]
    def ExecuteCallback(callback:FScoreQuery; value:int):int { return callback.Execute(value) }
    [ue_function(BlueprintCallable)]
    def EchoCallback(callback:FScoreQuery):FScoreQuery { return callback }
}
```

Fields are real `FDelegateProperty` properties. Reading returns a snapshot of the
binding; assigning copies it into the property. Bind a field by assigning
`ue_bind(...)` and clear it by assigning an empty value — `BindUFunction`/`Unbind`
work on local variables, not on property accessors. `BlueprintReadOnly` blocks
script writes; initial bindings and `EditAnywhere` are rejected. The same syntax
works through authored references: `actor.Callback = actor.Copy`,
`actor.Callback.Execute(5)`.

Delegate parameters and results are by value. Crossing a reflected call copies the
binding into the receiving VM context; a target in another Actor's context works and
stays weak. Empty bindings round-trip.

The owning Blueprint gets a sibling asset `<Blueprint>_DelegateSignatures` that
stores the signature functions. Keep it next to the Blueprint; it is referenced by
the owner and cooked as a dependency.

### Ref/out payloads

```das
let callback = ue_bind(type<FValueMutation>, target, "Mutate")
var count = 5
var text = "start"
var accepted = false
var description = "old"
let answer = callback.Execute(count, text, accepted, description)
```

All outputs and the result are validated before any caller variable changes; a
failed call publishes nothing (handler side effects are not rolled back). An unbound
`ExecuteIfBound` leaves ref/out variables unchanged. Binding checks types,
directions and layout; old saved handler graphs with a different layout are
rejected at bind time.

## Multicast delegates and Event Dispatchers

```das
require ue_delegate_examples

[ue_class(module_id="delegateexample", Blueprintable)]
class DelegateExample {
    properties = ue_properties(BlueprintAssignable, Category="Events") {
        // Broadcasts each change to Blueprint and DAS handlers.
        var OnExampleEvent:FScoreEvent
    }
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score:int = 0

    [ue_function(BlueprintCallable)]
    def FirstHandler(counter:int):void { self.Score += counter }
    [ue_function(BlueprintCallable)]
    def BindHandlers():void { self.OnExampleEvent.AddUFunction(ue_self_object(), "FirstHandler") }
    [ue_function(BlueprintCallable)]
    def Emit(counter:int):void { self.OnExampleEvent.Broadcast(counter) }
}
```

A multicast field (`@ue_property=(BlueprintAssignable)` or the grouped form above)
becomes a real Blueprint Event Dispatcher with a saved signature graph, flags,
category and a tooltip taken from the comment. Blueprints bind to it normally. The
same accessors work through authored references:
`actor.OnExampleEvent.Broadcast(5)`.

Standalone values use the same methods:

```das
var event = FScoreEvent()
event.AddUFunction(target, "FirstHandler")
event.AddUFunction(target, "SecondHandler")
event.Broadcast(4)
event.RemoveUFunction(target, "SecondHandler")
event.Clear()
```

- Adding the same object/method twice registers it once. `RemoveUFunction` removes
  only that pair; `Clear` removes all handlers.
- Copies of a multicast value refer to the **same** signal (unlike single delegates).
- `Broadcast` iterates a snapshot of the handler list; changes take effect on the
  next broadcast. Handler order is unspecified.
- Mutable `var T&` payloads are shared by all listeners and published to the caller
  after the broadcast; an empty signal leaves them unchanged. Output-only
  (`@ue_out`) payloads and return values are rejected.
- `FScoreEvent_from_property(object, "OnExampleEvent")` gives a checked view of an
  existing reflected multicast property. Binding requires `BlueprintAssignable`;
  broadcasting also requires `BlueprintCallable`.

## Declarative subscriptions without strings

```das
def BindEvents():void {
    self.OnDasSignal += self.OnSignal
    self.Health.Changed += self.OnHealthChanged
}
def OnSignal(value:int):void { self.Score += value }
def OnHealthChanged(current:float; maximum:float):void { self.RefreshHUD() }
```

`BindEvents` is a reserved, unannotated declaration method with no parameters or result. Its body accepts only top-level `+=` declarations. These become ordinary `ue_event` contracts and are collected against the candidate VM before it is accepted. They are not imperative subscriptions executed by calling `BindEvents`. Conditions, loops, arbitrary statements, `-=` and handlers already carrying annotations are rejected. Each handler names one source; use separate forwarding handlers for multiple sources. Native self events and declared native/authored components are supported.

This keeps the existing queued delivery, reload replacement, rejection and owner/source cleanup rules. It does not introduce direct native delegate bindings that outlive the script context. An `+=` expression in an ordinary gameplay method is not this declaration syntax.

## Subscribing to events: `ue_event`

`ue_event` binds a class method to an existing reflected dynamic multicast
delegate (`BlueprintAssignable`), native or authored:

```das
[ue_class(module_id="events", Blueprintable)]
class ScalarEventActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Label:string = ""
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Pulses:int = 0

    [ue_event(source="self", name="OnDasStringSignal")]
    def OnLabel(value:string):void { self.Label = value }
    [ue_event(source="self", name="OnDasVoidSignal")]
    def OnPulse():void { self.Pulses += 1 }
    [ue_event(name="OnTakeAnyDamage")]
    def OnDamage(damaged:ActorRef; amount:float; kind:ObjectRef;
                 instigator:ObjectRef; source:ActorRef):void { }
}
```

- `source=self` is the default; `source=owner` is available in ActorComponents and requires `source_type=OwnerRef`;
  a component name subscribes to one of the object's own components.
- When the source is native (self of a class with a native parent, or a native
  component), the delegate is checked while the script compiles: a misspelled name or
  a handler with the wrong number of parameters is a compile error. Owner and authored
  sources are checked when `bind_events` subscribes.
- Handlers return `void` and take 0–16 by-value parameters (`var` allows local
  mutation of the copy); no `ref`/`out`/defaults, and not combined with
  `BlueprintCallable`. Several handlers per class are allowed; duplicates are
  rejected.
- Delivery is **queued** on the game thread: payloads are copied into owned storage
  when the event fires and delivered afterwards. Calls from other threads are
  dropped.

Supported payloads (native side by value or `const&`):

| Native parameter | Handler parameter |
| --- | --- |
| `int32`, `float`, `double`, `bool`, `uint8` | `int`, `float`, `double`, `bool`, `uint8` (numeric types are exact: `float` is not `double`) |
| `FString`, `FName`, `FText` | `string`, `UENameValue`, `FText` |
| `FVector`, `FRotator`, `FTransform` | `UEVector`, `UERotator`, `UETransform` (non-finite or non-normalized values are dropped) |
| `UObject*` and derived classes | native object wrapper, native/authored interface view or authored view; narrowing is checked before the body runs |
| `UClass*` / `TSubclassOf` | `ActorClass` etc.; widening allowed, narrowing rejected at subscription |
| Enums and structs | Registered native/authored enum/struct types |
| `TArray`/`TMap`/`TSet` | Supported arrays/maps/sets, e.g. `array<double>` |

Object payloads are kept alive until delivery; a destroyed object invalidates the
queued event. Strings are limited to 4096 characters (no NUL), and container
payloads to the usual snapshot limits (4096 elements / 16 MiB). At most 1024 queued
or recursive events and 256 declarations per class. Full details on payload
ownership, multiple arguments and timers are in
[CallbackArguments.md](../Gameplay/CallbackArguments.md).

Names may be identifiers: `[ue_event(source=Health, name=Changed)]`. Quoted names remain accepted. Every declared event is checked before Apply: name, arity, payload types, enum/struct/container identity and source parameter direction. Compatible object/class widening follows the queued callback-frame policy; narrowing and mutable ref/out sources are rejected. Native sources use reflected signatures; authored sources use their source declarations, so they do not require an already-generated Blueprint.

An owner is dynamically assigned to a component, so state its contract explicitly:

```das
[ue_event(source=owner, source_type=ActorRef, name=OnTakeAnyDamage)]
def OnDamage(damaged:ActorRef; amount:float; kind:ObjectRef;
             instigator:ObjectRef; source:ActorRef):void { /* ... */ }
```

`source_type` names a native or authored object reference type available in the module. The actual owner is checked against that type during binding. Existing owner annotations must add this option; an unknown owner contract is no longer deferred to PIE. `ProbeActorRef` in internal examples describes the test host's custom `OnDasSignal`; it is not a general Actor contract.

Subscriptions are removed on EndPlay, owner or source destruction, world cleanup
and runtime context release. An accepted hot reload re-binds; a rejected reload keeps
the old bindings. Handler errors are reported but do not unsubscribe.

## Lifetime and limits

- Bindings hold weak references; expired targets are not invoked. Delegate values
  and frames belong to their VM context and cannot be moved to another owner.
- Per context: 4096 live delegate values, 256 signature contracts, 256 pending
  calls. Nested delegate, interface and authored calls share a depth limit of 32.
- Calls run on the game thread only.
- Not supported: multicast values as method parameters or results; delegate-valued
  ref/out parameters; delegates in containers or captured by timers/events;
  delegate parameters in interface methods; native non-UFUNCTION delegates; script
  lambda bindings; worker-thread delivery.
- Structural or default migrations of classes containing delegate or event fields
  are refused; body and metadata-only reimports work.

## Examples

`Plugins/DasForUE/Content/Das/Scripts/`: `ue_delegate_examples.das`,
`dsl_delegate_actor.das`, `ue_text_byte_delegates.das`, `dsl_text_byte_events.das`,
`ue_class_callback_signals.das`, `dsl_class_callbacks.das`, `dsl_scalar_events.das`,
`dsl_value_events.das`, `dsl_array_events.das`, `dsl_math_events.das`,
`dsl_interface_object_events.das`, `dsl_authored_object_events.das`.

Try it: `Scripts/TestAuthoredDelegates.ps1`.

See also: [Lifecycle.md](Lifecycle.md) ·
[CallbackArguments.md](../Gameplay/CallbackArguments.md) ·
[Timers.md](../Gameplay/Timers.md) · [Interfaces.md](Interfaces.md) ·
[AuthoredReferences.md](AuthoredReferences.md) ·
[ObjectAndClassReferences.md](ObjectAndClassReferences.md)
