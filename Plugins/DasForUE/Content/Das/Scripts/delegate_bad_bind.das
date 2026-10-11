options gen2
require ue_delegate_examples
[export]
def main(input:int):int {
    var query=FScoreQuery()
    query.BindUFunction(ue_self_object(),"SetHealthValue")
    return 0
}
