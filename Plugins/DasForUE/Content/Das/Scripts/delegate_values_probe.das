options gen2
require ue_delegate_examples
require ue_object_types
[export]
def main(input:int):int {
    let target=ue_self_object()
    var query=FScoreQuery()
    verify(!query.IsBound())
    query.BindUFunction(target,"AddHealth")
    let copy=query
    query.Unbind()
    verify(!query.IsBound() && copy.IsBound())
    verify(copy.Execute(2)==12)
    var pair=FObjectValueDelegate()
    pair.ExecuteIfBound(target,100.0)
    pair.BindUFunction(target,"DelegatePair")
    pair.Execute(target,3.0)
    var event=FScoreEvent()
    event.Broadcast(100)
    event.AddUFunction(target,"DelegateIncrement")
    event.AddUFunction(target,"DelegateIncrement")
    event.AddUFunction(target,"DelegateDouble")
    verify(event.IsBound())
    event.Broadcast(4)
    event.RemoveUFunction(target,"DelegateDouble")
    event.Broadcast(1)
    event.Clear()
    verify(!event.IsBound())
    event.Broadcast(100)
    let constructed=ue_bind(type<FScoreQuery>,target,ue_name_value("AddHealth"))
    verify(constructed.Execute(0)==28)
    let typed=ue_bind(type<FScoreQuery>,ue_ref(type<ObjectRef>,target),"AddHealth")
    verify(typed.Execute(0)==28)
    return ue_object_get_int(target,"health")
}
