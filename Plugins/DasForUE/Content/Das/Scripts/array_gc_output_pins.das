options gen2
require ue_generated
require ue_foundation
def collect() { let g = ue_begin_call(ue_self_object(), "ForceTestGC"); ue_invoke_call(g); ue_release_call(g) }
[export]
def main(input:int):int {
let c = ue_begin_call(ue_self_object(), "MakeFixtureObjects")
ue_invoke_call(c)
collect()
verify(ue_result_array_size(c, "Values") == 3)
let first = ue_result_object(c, "Values.0")
verify(ue_object_valid(first) && !ue_object_valid(ue_result_object(c, "Values.1")))
ue_release_call(c)
collect()
verify(!ue_object_valid(first))
return 1
}
