# Handler references

Inside an authored class, pass a method reference instead of its name:

```das
let query = ue_bind(type<FScoreQuery>, ue_self_object(), self.QueryScore)

var stored = FScoreQuery()
stored.BindUFunction(ue_self_object(), self.QueryScore)

var changed = FScoreEvent()
changed.AddUFunction(ue_self_object(), self.OnChanged)
changed.RemoveUFunction(ue_self_object(), self.OnChanged)
```

`self.QueryScore` and `self.OnChanged` name the nearest instance method in the authored class hierarchy, including required script modules. Their Unreal entry points are generated and hidden from the Blueprint palette automatically. A delegate handler may return a value and have the ref/out arguments allowed by its delegate contract; managed timer/input/event callbacks retain their own void/signature rules. An explicit `BlueprintCallable` keeps a handler visible in Blueprint.

The delegate signature is checked at compilation: argument types, result and ref/out directions must match exactly. An unknown method fails before Apply. The explicit target must resolve to the current self object; a different object is rejected at runtime. String and FName forms remain available for existing reflected methods on other targets.

Single-cast `BindUFunction` mutates a local delegate variable. For a reflected single-cast field use assignment: `self.Stored = ue_bind(type<FScoreQuery>, ue_self_object(), self.QueryScore)`. Multicast field views support Add/Remove. This is the existing direct UFunction delegate transport; use [dynamic event operators](DelegatesAndEvents.md#dynamic-subscriptions-without-strings) when the subscription should be owned and removed by the script session.

## Inherited handlers

```das
[ue_class(Blueprintable)]
class BaseCounter {
    @ue_property=(BlueprintReadWrite)
    Score:int=0
    def Pulse():void { self.Score+=1 }
    [ue_function(BlueprintCallable)]
    def Start():void { ue_timer_next_tick("pulse",self.Pulse) }
}

[ue_class(Blueprintable)]
class ChildCounter : BaseCounter {
    def override Pulse():void {
        Super.Pulse()
        self.Score+=10
    }
}
```

`Start` inherited by `ChildCounter` delivers to `ChildCounter.Pulse`, including when registration executes in the base script's VM. `Super.Pulse()` calls the parent implementation once. The nearest declaration wins; a mismatched override is an error, not a reason to fall back to an ancestor.

Hidden callback methods are internal, non-final Unreal events, so overrides do not need their own annotation. A referenced callback is virtual by default while explicit visibility stays intact; `BlueprintEvent=false` preserves an explicitly final callback. Visible reflected methods not used as callbacks keep their declared policy. A previously script-only inherited helper referenced from a child receives a child-owned hidden forwarding adapter; the imported parent's annotations and IR stay unchanged.

Existing Blueprints with previously final hidden handlers need new Blueprint assets: Apply currently rejects changing callable/event roles before modifying the class. Apply the base declaration to a new asset path, then its descendants. Ordinary body-only reload of the new virtual handlers is supported.

The same lookup covers timer payloads and multi-capture timers, input, widget clicks, effects, latent/async callbacks, dynamic events and typed delegate Bind/Add/Remove. Compile-time signature and lifecycle/static-method restrictions still apply. The existing rule requiring separate files for lifecycle inheritance is unchanged; same-file method hierarchies are supported without lifecycle hooks. Accepted reload uses the current override body through reflected dispatch; existing cancellation, input restoration and owner cleanup policies remain in force.

## Free functions

Use daScript's ordinary function-address expression outside a class:

```das
def QueryScore(value:int):int { return value + 10 }
def OnChanged(value:int):void { /* update script state */ }
def Pulse():void { /* timer work */ }

[export]
def main(input:int):int {
    let query = ue_bind(type<FScoreQuery>, ue_self_object(), @@QueryScore)
    var stored = FScoreQuery()
    stored.BindUFunction(ue_self_object(), @@QueryScore)
    var changed = FScoreEvent()
    changed.AddUFunction(ue_self_object(), @@OnChanged)
    changed.Broadcast(5)
    changed.RemoveUFunction(ue_self_object(), @@OnChanged)
    ue_timer_next_tick("pulse", @@Pulse)
    return query.Execute(input)
}
```

Delegate references generate a stable exported ABI wrapper, reusing the reflected class-method adapter. They support the delegate surface's value/result/ref/out types, including typed object references and supported arrays. Bind/Add/Remove share one checked contract and use the same function identity for removal. The target is the current owner session. A free function does not need its own `ue_function` annotation or Unreal UFunction.

Managed APIs accepting callback names also accept `@@function` in that argument. Their existing callback ABI and payload restrictions remain: a timer callback without a payload has no arguments and returns void, for example. These managed references currently require a free function in the current script module, and are exported automatically. Delegate wrappers may call visible free functions from a required module. Use an unambiguous function-address literal; arbitrary function variables and closures are outside this form.

## Lifetime

A free-function delegate uses a transient UObject proxy with a weak owner and VM generation lease. It stores a stable wrapper name and script identity, never a VM function pointer. Native caller input/outputs use the common value codec, with result and ref/out values staged before publication. Calls run synchronously on the game thread; worker-thread delivery is ignored.

The context owns its proxies, and event-property registrations made through free references are tracked independently of temporary delegate values. Release, EndPlay, Destroy and accepted reload revoke the generation and detach those registrations. Rejected reload preserves the working VM and proxies. After successful reload, bind the free delegate again; an externally retained old proxy cannot call the replacement VM. Signature changes of previously used exports retain the normal preflight policy and may require releasing the session.

See [delegates and events](DelegatesAndEvents.md), [input](../Gameplay/Input.md), and [callback arguments](../Gameplay/CallbackArguments.md).
