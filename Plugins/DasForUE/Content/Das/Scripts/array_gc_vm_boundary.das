options gen2
require ue_generated
require ue_foundation
def collect() { let g = ue_begin_call(ue_self_object(), "ForceTestGC"); ue_invoke_call(g); ue_release_call(g) }
[export]
def main(input:int):int {
let made <- fixture_MakeFixtureObjects(C_DasObjectFixture_5f019a81_from(ue_self_object()))
verify(C_DasObjectFixture_5f019a81_valid(made[0]))
collect()
verify(!C_DasObjectFixture_5f019a81_valid(made[0]))
return 1
}
