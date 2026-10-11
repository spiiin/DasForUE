options gen2
require ue_delegate_examples
[export]
def main(input:int):int {
    let owner=ue_self_object()
    let create=ue_begin_call(owner,"NewFixture")
    ue_invoke_call(create)
    let target=ue_result_object(create,"ReturnValue")
    ue_release_call(create)
    var query=FScoreQuery()
    query.BindUFunction(target,"AddHealth")
    verify(query.IsBound())
    let gc=ue_begin_call(owner,"ForceTestGC")
    ue_invoke_call(gc)
    ue_release_call(gc)
    verify(!query.IsBound())
    return 1
}
