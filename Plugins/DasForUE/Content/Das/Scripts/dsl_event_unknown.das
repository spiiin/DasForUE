options gen2
require ue_native_gameplay public

// Negative fixture: misspelled native delegate name must fail at compile time.
[ue_class(Blueprintable)]
class EventUnknownActor : Actor {
    [ue_event(name="OnTakeAnyDamagee")]
    def OnDamage(damaged:ActorRef; amount:float; kind:ObjectRef; instigator:ObjectRef; source:ActorRef):void {}
}
