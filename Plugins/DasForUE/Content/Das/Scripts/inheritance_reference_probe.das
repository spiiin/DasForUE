options gen2
require ue_inheritance_base
[ue_object_ref(script="ue_inheritance_base.das",type="BaseCounter")]
struct BaseCounterRef { handle:UObjectHandle }
[export]
def main(input:int):int {
    let base=ue_ref(type<BaseCounterRef>,ue_self_object())
    return base.Compute(input)
}
