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
Single delegate fields on `ue_class`, by-value `ue_function` parameters and results
are supported. There is no raw VM/function pointer in a delegate binding.

```das
[ue_class(module_id="example", Blueprintable)]
class DelegateOwner {
    @ue_property=(BlueprintReadWrite)
    Callback:FScoreQuery

    [ue_function(BlueprintCallable)]
    def Bind():void {
        self.Callback=ue_bind(type<FScoreQuery>, ue_self_object(), "Handler")
    }
    [ue_function(BlueprintCallable)]
    def Handler(value:int):int { return value+1 }
    [ue_function(BlueprintCallable)]
    def Invoke(value:int):int { return self.Callback.Execute(value) }
    [ue_function(BlueprintCallable)]
    def Clear():void { self.Callback=FScoreQuery() }
}
```

`FScoreQuery` is declared in `ue_delegate_examples.das`. These fields are real UE
`FDelegateProperty` properties. Reading returns a binding snapshot; assigning copies
the binding into the property. Copying to another field or DAS variable is independent
of subsequent assignments. Use assignment with `ue_bind` to bind and an empty value
to clear a field; mutable `BindUFunction`/`Unbind` methods operate on local variables,
not accessor temporaries. Typed foreign references use the same syntax, for example
`actor.Callback=actor.Copy` and `actor.Callback.Execute(5)`.

BlueprintReadOnly rejects DAS writes as for existing fields. Initial bindings and
EditAnywhere are unsupported and rejected. A generated sibling asset named
`<Blueprint>_DelegateSignatures` stores signature functions in a separate package;
the owner references it, and cooking includes it as a dependency. Keep this asset
with the authored Blueprint. No authoring JSON is required by the cooked signature
asset. Structural/default migrations involving delegate fields are deferred.

## Delegate-valued method parameters and results

```das
[ue_function(BlueprintCallable)]
def ExecuteCallback(callback:FScoreQuery, value:int):int {
    return callback.Execute(value)
}
[ue_function(BlueprintCallable)]
def EchoCallback(callback:FScoreQuery):FScoreQuery { return callback }
[ue_function(BlueprintCallable)]
def StoreCallback(callback:FScoreQuery):void { self.Callback=callback }
```

An ordinary DAS function can use typed method calls:

```das
let callback=actor.GetCallback()
let copy=actor.EchoCallback(callback)
actor.StoreCallback(copy)
let answer=actor.ExecuteCallback(actor.Callback,2)
```

These are reflected delegate pins (`FDelegateProperty`), not object/string surrogate
parameters. Generated Blueprint adapters pack/unpack a transient native binding
value. Crossing a reflected call copies that binding and creates a token in the
receiving VM context; no sender token crosses the boundary. Empty bindings roundtrip
as empty values. A target in another actor's context is supported and remains weak.
Delegate signatures and parameter/result contracts are checked before dispatch.

Only single-cast values and by-value method inputs are included. Multicast method
values, delegate-valued ref/out parameters, delegate containers and authored
interface declarations with delegate methods are not included. Ref/out payloads inside a single-cast delegate signature are supported as described
below. These method contracts
use the normal callable Actor/Component adapter; they do not add pure/static support.

## Ref/out payloads in delegate signatures

Use an empty method declaration to preserve parameter metadata:

```das
[ue_delegate]
struct FValueMutation {
    def signature(var Count:int&; var Text:string&;
                  @ue_out var Accepted:bool&; @ue_out var Description:string&):int {}
}

let callback=ue_bind(type<FValueMutation>,target,"Mutate")
var count=5
var text="start"
var accepted=false
var description="old"
let answer=callback.Execute(count,text,accepted,description)
```

`var T&` means inout/ref; `@ue_ref` is an optional explicit marker. `@ue_out var T&`
means output-only. Markers are on the parameters themselves, with the same direction
validation as `ue_function`. `signature` is a declaration, not an implementation;
its body must be empty and direct calls trap. Default arguments, overloaded/static
signature methods and return references are rejected. The compact
`signature:function<(Value:int):int>` form remains available for by-value payloads.
References require the method form: pinned daScript erases parameter annotations
when it converts function-type syntax to TypeDecl, so treating `@ue_out` there as
meaningful would silently produce a different contract. The parser is unchanged.

Single-cast Execute supports ref/out with a return value or void. All outputs and
the return are read/validated before any caller variable is updated. Failed calls
do not publish partial outputs; this does not roll back handler side effects.
An unbound void ExecuteIfBound preserves caller ref/out variables. Native property
flags distinguish ref (`CPF_OutParm|CPF_ReferenceParm`) from out (`CPF_OutParm`).
Binding checks types, direction, parameter storage size and offsets before invoking
through a native delegate. Newly generated authored Blueprint adapters allocate
out-only parameters before the return to match the declared signature layout.
Old saved graphs with another layout are rejected at binding; this step does not
migrate or silently rebuild those graphs.

Multicast supports mutable ref payloads, including authored Event Dispatcher fields.
Each listener sees the current shared frame, and the final values are published to
the caller after Broadcast. Listener ordering is unspecified. An empty signal
preserves ref values. Pure out payloads are rejected for multicast, whose invocation
list may be empty. Multicast return values remain forbidden.

Examples include scalar/string ref/out, output-only optional execution, and
array<double> ref/out. Existing checked value adapters also supply object/name,
math/struct/enum payloads where the reflected handler has an exact native contract;
this step does not expand the existing payload type set. Internal generated value
converters for delegate declarations are private to their declaring module.

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
authored-dispatch error propagation. Worker-thread delivery,
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

2026-10-11 accumulated validation: editor/create/reopen, actual Blueprint handlers and packaged authored dispatcher add/broadcast/remove all passed. See [AccumulatedValidation.md](AccumulatedValidation.md).


Single-cast field validation (2026-10-11): TestEditor foundation passed with zero
failures. TestAuthoredDelegates created and reopened assets in separate processes,
then dispatched through actual reflected handlers; assignment, independent copying,
clearing and foreign accessors passed. TestCookedGameplay built/cooked/staged
Development and executed the same field scenario in the packaged game successfully.

TestGameplayRuntimePIE also passed: the authored actor was duplicated into a real
PIE world; field invocation, independent copies, clearing, foreign accessor writes,
Destroy cleanup and EndPIE context release passed. The Automation report contains
one success, zero warnings, zero failures and zero unrun tests (NullRHI).

Delegate-valued methods (2026-10-11): editor Create/Reopen tests passed for native
reflected parameter/result roundtrip, empty bindings, typed DAS methods, and passing
a binding into a second actor's VM while invoking the first actor's handler. The
foundation suite and actual PIE method parameter/result scenario passed as well.

Packaged Development validation also passed after the final source changes:
TestCookedGameplay built/cooked/staged/archived the map, then executed GetCallback,
EchoCallback, StoreCallback and ExecuteCallback in the packaged game. The same
scenario passed in real PIE; its Automation report contains one success, zero
warnings, failures and unrun tests. Rendering was disabled with NullRHI.

Ref/out implementation validation (2026-10-11): native scalar/string ref/out,
output-only Execute, unbound ExecuteIfBound preservation and direction mismatch
rejection passed in foundation. Create/Reopen tests passed for authored ref/out,
saved signature assets, multicast ref dispatch and array<double> ref/out.

Final ref/out checks passed: TestEditor (DAS_FOUNDATION_RESULT=PASS failures=0), TestMethodDispatchPIE, TestGameplayRuntimePIE and TestCookedGameplay. Actual PIE and packaged Development executed scalar/string ref/out, multicast ref and array<double> ref/out. Rendering was disabled with NullRHI.
