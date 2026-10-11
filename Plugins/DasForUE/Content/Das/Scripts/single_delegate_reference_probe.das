options gen2
require ue_delegate_examples
[ue_object_ref(script="dsl_single_delegate_actor.das",type="SingleDelegateActor")]
struct SingleDelegateRef { handle:UObjectHandle }
[export]
def main(input:int):int {
    let actor=ue_ref(type<SingleDelegateRef>,ue_self_object())
    actor.Callback=actor.Copy
    let result=actor.Callback.Execute(input)
    actor.Callback=FScoreQuery()
    return result
}

[export]
def method_probe(input:int):int {
    let actor=ue_ref(type<SingleDelegateRef>,ue_self_object())
    let callback=actor.GetCallback()
    let copy=actor.EchoCallback(callback)
    actor.StoreCallback(copy)
    return actor.ExecuteCallback(actor.Callback,input)
}
