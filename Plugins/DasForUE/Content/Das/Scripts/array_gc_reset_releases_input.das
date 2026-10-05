options gen2
require ue_generated
require ue_foundation
def collect() { let g = ue_begin_call(ue_self_object(), "ForceTestGC"); ue_invoke_call(g); ue_release_call(g) }
[export]
def main(input:int):int {
let c = ue_begin_call(ue_self_object(), "EchoFixtureObjects")
let fresh = C_DasObjectFixture_5f019a81_raw(fixture_NewFixture(C_DasObjectFixture_5f019a81_from(ue_self_object())))
ue_arg_array(c, "Values", 1)
ue_arg_object(c, "Values.0", fresh)
collect()
verify(ue_object_valid(fresh))
ue_arg_array(c, "Values", 0)
collect()
verify(!ue_object_valid(fresh))
ue_invoke_call(c)
verify(ue_result_array_size(c, "ReturnValue") == 0)
return 1
}
