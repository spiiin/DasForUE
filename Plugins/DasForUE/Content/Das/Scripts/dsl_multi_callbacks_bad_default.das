options gen2
require ue_callback_value_signals
[ue_class]
class BadMulti : Actor {
[ue_event(name="Signal")]
 def Bad(a:int;b:int=2):void {}
}
