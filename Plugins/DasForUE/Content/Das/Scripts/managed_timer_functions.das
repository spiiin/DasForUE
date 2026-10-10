options gen2
require ue_timer_counter_refs

[export]
def pulse():void {
    let actor = ue_ref(type<TimerCounterRef>,ue_self_object())
    actor.Score += 1
}
[export]
def start_timer():void { ue_timer_start("score",1.0,"pulse",true) }
[export]
def stop_timer():bool { return ue_timer_cancel("score") }
