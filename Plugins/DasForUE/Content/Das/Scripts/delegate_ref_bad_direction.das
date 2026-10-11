options gen2
require ue_delegate_examples
[ue_delegate]
struct FReferenceValue { def signature(var Value:int&):void {} }
[export]
def main(input:int):int {
    let wrong=ue_bind(type<FReferenceValue>,ue_self_object(),"OutValue")
    return 0
}
