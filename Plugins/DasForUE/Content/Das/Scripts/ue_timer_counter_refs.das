options gen2
module ue_timer_counter_refs public
require ue_dsl public

[ue_object_ref(script="dsl_managed_timers.das",type="TimerCounter")]
struct TimerCounterRef { handle:UObjectHandle }
