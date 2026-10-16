options gen2
require ue_dsl
[ue_object_ref(script="dsl_defaults_child.das", type="DefaultChild")]
struct DefaultChildRef { handle:UObjectHandle }
[ue_object_ref(script="dsl_defaults_actor.das", type="DefaultActor")]
struct DefaultActorRef { handle:UObjectHandle }
[export]
def main():int {
    let child=ue_ref(type<DefaultChildRef>,ue_self_object())
    let base=ue_ref(type<DefaultActorRef>,ue_self_object())
    if (child.DefaultEvent()!=14 || base.DefaultEvent()!=12) { return -301 }
    if (child.SuperProbe()!=22 || child.EventProbe()!=12) { return -302 }
    return 1
}
