options gen2
require ue_generated
require ue_foundation
def collect() { let g = ue_begin_call(ue_self_object(), "ForceTestGC"); ue_invoke_call(g); ue_release_call(g) }
[export]
def main(input:int):int {
let c = ue_begin_call(ue_self_object(), "ReplaceFixtureObjects")
let old = C_DasObjectFixture_5f019a81_raw(fixture_NewFixture(C_DasObjectFixture_5f019a81_from(ue_self_object())))
ue_arg_array(c, "Values", 1)
ue_arg_object(c, "Values.0", old)
ue_invoke_call(c)
collect()
verify(!ue_object_valid(old))
let replacement = ue_result_object(c, "Values.0")
verify(ue_object_valid(replacement))
ue_release_call(c)
collect()
verify(!ue_object_valid(replacement))
return 1
}
