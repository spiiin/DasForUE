options gen2
options gc
require ue_foundation
var anchor = ue_struct_value("/Script/DasRuntime.DasCallPayload")
var saved : array<UEStructValue>
def step(value:UEStructValue; index:int):UEStructValue {
    let kept = value
    let next = ue_struct_with_int(value,"Count",index)
    if (index > 0 && index % 257 == 0) {
        ue_struct_collect()
        verify(ue_struct_get_int(kept,"Count") == index-1)
    }
    return next
}
[export]
def main(input:int):int {
    if (length(saved)==0) {
        resize(saved,3)
        saved[0] = ue_struct_with_int(anchor,"Count",101)
        saved[1] = ue_struct_with_int(anchor,"Count",202)
        saved[2] = ue_struct_with_int(anchor,"Count",303)
    }
    var value = anchor
    for (i in range(input)) { value = step(value,i) }
    ue_struct_collect()
    verify(ue_struct_get_int(anchor,"Count")==0)
    verify(ue_struct_get_int(saved[0],"Count")==101)
    verify(ue_struct_get_int(saved[1],"Count")==202)
    verify(ue_struct_get_int(saved[2],"Count")==303)
    verify(ue_struct_value_count()<64)
    return ue_struct_get_int(value,"Count")
}
