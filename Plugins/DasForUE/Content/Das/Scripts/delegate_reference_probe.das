options gen2
require ue_delegate_examples
[ue_object_ref(script="dsl_delegate_actor.das",type="DelegateExample")]
struct DelegateExampleRef { handle:UObjectHandle }
[export]
def main(input:int):int {
    let actor=ue_ref(type<DelegateExampleRef>,ue_self_object())
    actor.OnExampleEvent.RemoveUFunction(actor.handle,"FirstHandler")
    actor.OnExampleEvent.Broadcast(input)
    return actor.Score
}
