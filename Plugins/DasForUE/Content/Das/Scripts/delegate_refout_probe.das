options gen2
require ue_delegate_examples
[export]
def main(input:int):int {
    let change=ue_bind(type<FValueMutation>,ue_self_object(),"MutateValues")
    var count=5
    var text="start"
    var accepted=false
    var description="old"
    let answer=change.Execute(count,text,accepted,description)
    if(count!=8 || text!="start 世界" || !accepted || description!=text || answer!=16) {
        panic("native delegate ref/out did not publish expected values")
    }
    var output=55
    var optional=FOutputValue()
    optional.ExecuteIfBound(output)
    if(output!=55) { panic("unbound optional delegate changed out argument") }
    optional.BindUFunction(ue_self_object(),"OutValue")
    optional.Execute(output)
    if(output!=10) { panic("native out-only delegate failed") }
    return answer+count
}
