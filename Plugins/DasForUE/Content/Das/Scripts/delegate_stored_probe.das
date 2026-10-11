options gen2
require ue_delegate_examples
var StoredDelegate=FScoreQuery()
[export]
def main(input:int):int {
    if(input==0){ StoredDelegate.BindUFunction(ue_self_object(),"AddHealth"); return 0 }
    return StoredDelegate.Execute(input)
}
