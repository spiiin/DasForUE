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

`self.QueryScore` and `self.OnChanged` must name instance methods declared on the current class. Their Unreal entry points are generated and hidden from the Blueprint palette automatically. A delegate handler may return a value and have the ref/out arguments allowed by its delegate contract; managed timer/input/event callbacks retain their own void/signature rules. An explicit `BlueprintCallable` keeps a handler visible in Blueprint.

The delegate signature is checked at compilation: argument types, result and ref/out directions must match exactly. An unknown method fails before Apply. The explicit target must resolve to the current self object; a different object is rejected at runtime. String and FName forms remain available for existing reflected methods on other targets.

Single-cast `BindUFunction` mutates a local delegate variable. For a reflected single-cast field use assignment: `self.Stored = ue_bind(type<FScoreQuery>, ue_self_object(), self.QueryScore)`. Multicast field views support Add/Remove. This is the existing direct UFunction delegate transport; use [dynamic event operators](DelegatesAndEvents.md#dynamic-subscriptions-without-strings) when the subscription should be owned and removed by the script session.

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
