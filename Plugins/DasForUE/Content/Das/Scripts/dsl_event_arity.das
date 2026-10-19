options gen2
require ue_native_gameplay public

// Negative fixture: OnTakeAnyDamage delivers five values, the handler takes one.
[ue_class(Blueprintable)]
class EventArityActor : Actor {
    [ue_event(name="OnTakeAnyDamage")]
    def OnDamage(amount:float):void {}
}
