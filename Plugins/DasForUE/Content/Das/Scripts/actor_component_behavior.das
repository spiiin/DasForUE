options gen2
require ue_foundation
def owner():UObjectHandle { return ue_component_owner(ue_self_object()) }
[export]
def bind_events() { ue_subscribe_object_int(owner(),"OnDasSignal","on_owner_signal") }
[export]
def on_owner_signal(amount:int) { ue_set_self_int("Counter",ue_get_self_int("Counter")+amount) }
[export]
def begin_play() { ue_state_set_int("begins",ue_state_get_int("begins",0)+1); ue_set_self_int("BeginCalls",ue_get_self_int("BeginCalls")+1) }
[export]
def tick(delta:float) { ue_set_self_int("TickCalls",ue_get_self_int("TickCalls")+1); ue_set_self_float("Elapsed",ue_get_self_float("Elapsed")+delta); ue_object_set_int(owner(),"Score",ue_object_get_int(owner(),"Score")+1) }
[export]
def end_play(reason:int) { ue_set_self_int("EndCalls",ue_get_self_int("EndCalls")+1) }
[export]
def main(input:int):int { return ue_state_get_int("begins",0) }
