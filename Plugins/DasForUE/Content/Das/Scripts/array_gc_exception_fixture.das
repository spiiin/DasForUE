options gen2
require ue_generated
require ue_foundation
def collect() { let g = ue_begin_call(ue_self_object(), "ForceTestGC"); ue_invoke_call(g); ue_release_call(g) }
[export]
def main(input:int):int {
let c = ue_begin_call(ue_self_object(), "MakeFixtureObjects")
ue_invoke_call(c)
panic("array pin cleanup fixture")
return 1
}
