# Authored delegates and Blueprint events

Added 2026-10-11. Reference: [AngelScript delegates and events](https://angelscript.hazelight.se/scripting/delegates/).

The reference distinguishes a single reflected handler, multicast handlers and
Blueprint-visible Event Dispatchers. DAS declarations use its existing function
type syntax inside a declaration struct; the annotation consumes `signature` and
generates typed methods. No reader macro or parser modification is involved.

```das
require ue_dsl

[ue_delegate]
struct FExampleDelegate {
    signature:function<(Object:UObjectHandle; Value:float):void>
}

[ue_multicast]
struct FExampleEvent {
    signature:function<(Counter:int):void>
}

var storedDelegate=FExampleDelegate()
```

`signature` is a declaration marker, not a callable field or runtime function
pointer. Native UObject wrappers can be used instead of `UObjectHandle` in payloads.
Each parameter must have a name; at most 16 inputs are accepted. Multicast signatures
must return void. `type_folder`/`type_registry` options resolve authored enum/struct
payloads using the existing explicit type registry. Unsupported native signature
shapes fail at compilation or binding, rather than being coerced.

## Single delegates

```das
// Equivalent to the bound-constructor use case in the reference page.
var ready=ue_bind(type<FExampleDelegate>, ue_self_object(), "OnDelegateExecuted")
storedDelegate.BindUFunction(ue_self_object(), "OnDelegateExecuted")
if(storedDelegate.IsBound()) {
    storedDelegate.Execute(ue_self_object(), 1.0)
}
storedDelegate.ExecuteIfBound(ue_self_object(), 1.0)
storedDelegate.Unbind()
```

The target must have a reflected, local instance UFunction with the matching types,
directions and frame layout. BlueprintCallable is not required for native handler
functions: ordinary UFUNCTIONs work. Static/network/delegate signature functions
cannot be used as handlers. Method parameter names do not have to match the delegate
parameter names. DAS authored handlers currently use the normal `ue_function`
annotation, which produces a reflected adapter method.

Single delegates can return values through `Execute`. `ExecuteIfBound` is generated
for void-returning signatures; an unbound `Execute` fails. Copying a single delegate
produces a value: unbinding/rebinding one copy does not alter the other copy.

Stored globals belong to the existing context for an owner and script, so a stored
delegate can be bound in one method/lifecycle callback and invoked in another.
Single delegate fields on `ue_class` and delegate-valued reflected function parameters
are not added in this step. Use a local
or stored DAS value plus BindUFunction or the typed `ue_bind` factory. There is no raw VM/function pointer in a
delegate binding.

## Multicast values and authored Event Dispatchers

```das
[ue_class(module_id="delegateexample", Blueprintable)]
class DelegateExample {
    // Broadcasts each change to Blueprint and DAS handlers.
    @ue_property=(BlueprintAssignable)
    @Category="Events"
    OnExampleEvent:FExampleEvent

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score:int=0

    [ue_function(BlueprintCallable)]
    def FirstHandler(counter:int):void { self.Score+=counter }

    [ue_function(BlueprintCallable)]
    def BindHandlers():void {
        self.OnExampleEvent.AddUFunction(ue_self_object(), "FirstHandler")
    }

    [ue_function(BlueprintCallable)]
    def Emit(counter:int):void { self.OnExampleEvent.Broadcast(counter) }
}
```

The field creates a real Blueprint multicast delegate property and a saved delegate
signature graph, with BlueprintAssignable/BlueprintCallable flags, category and
comment-derived tooltip. It is available as an Event Dispatcher through normal UE
reflection. The same accessor syntax works through an authored object reference,
e.g. `actor.OnExampleEvent.Broadcast(5)` in an ordinary DAS function.

The event property also supports the existing grouped declaration syntax:

```das
properties=ue_properties(BlueprintAssignable,Category="Events") {
    // Broadcasts score changes.
    var OnExampleEvent:FExampleEvent
}
```

Standalone multicast values use the same methods:

```das
var event=FExampleEvent()
event.AddUFunction(target, "FirstHandler")
event.AddUFunction(target, "SecondHandler")
event.Broadcast(4)
event.RemoveUFunction(target, "SecondHandler")
event.Clear()
```

Repeated addition of the same object/method is unique. RemoveUFunction removes only
that pair, preserving other handlers. Clear removes all handlers. Standalone event
copies are references to the same signal, whereas single delegates copy their
binding value. Broadcast uses a snapshot of the invocation list; changes during a
broadcast affect later broadcasts. Do not depend on multicast handler ordering.

`FExampleEvent_from_property(object, "OnExampleEvent")` gives a checked view of an
existing reflected event. Binding requires BlueprintAssignable; broadcasting also
requires BlueprintCallable. `ue_event` remains the earlier managed subscription
annotation for existing UE events, and is different from declaring `ue_multicast`.

## Lifetime and current limits

Bindings hold weak UObject references. Expired targets are not invoked. Checked
handles and frames belong to their VM context and are not transferable to another
owner/context. Unused delegate values are collected at VM boundaries alongside
snapshot roots; persistent globals remain rooted. Limits are 4096 live values and
256 signature contracts per context, with the existing 256 pending-call limit.
The bound factory accepts a checked UObjectHandle or a typed object reference, and
a string or UENameValue method name.

Calls run on the game thread and use the existing typed frame marshaller and
authored-dispatch error propagation. Ref/out signatures, worker-thread delivery,
native non-UFUNCTION delegates and script lambda bindings remain separate work.
Scalar/object/name/math/struct/enum and supported array signatures use the existing
value adapters, but an actual handler must have a compatible UE signature. Const
reference/native enum representations are not silently converted.

Initial dispatcher creation and body/metadata-only reimport are supported.
Structural/default migrations of classes containing events are explicitly refused
before mutation, pending preservation of dependent graphs and live bindings.

The VS Code handler-generation code action from the reference page is an editor
extension feature; it is not implemented in this repository.

## Examples and validation

Public examples: `ue_delegate_examples.das` and `dsl_delegate_actor.das`.
Foundation fixtures cover return values, independent single-delegate copies,
unbinding, two-argument object/float handlers, optional execution, multicast
add/remove/clear, invalid signature rejection, unbound Execute and declaration
tooltips. Additional fixtures exercise stored values across VM boundaries and weak
targets through GC. `TestAuthoredDelegates.ps1` checks import, actual Unreal dispatch,
typed foreign accessors and asset reopening in a new process. Validation results
are recorded after the final run below.

Final validation (2026-10-11, UE 5.8.3): TestEditor.ps1 — build succeeded, DAS_FOUNDATION_RESULT=PASS failures=0, including all six new delegate checks and both bound-factory target forms. TestAuthoredDelegates.ps1 — Create and Reopen in separate processes passed; actual Blueprint handler execution, DAS/native dispatch, unique registration, individual removal, native Broadcast and typed foreign accessors passed. Packaged execution, a Level Blueprint binding-node graph, structural migration and replacement-specific regression tests were not run for this step. Existing object-replacement handling also remaps stored delegate weak targets/views.
