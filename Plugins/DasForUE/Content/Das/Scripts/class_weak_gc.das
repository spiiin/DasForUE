options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
let created = fixture_NewFixture(self)
verify(C_DasObjectFixture_5f019a81_valid(created))
let c = ue_begin_call(ue_self_object(), "ForceTestGC")
ue_invoke_call(c)
ue_release_call(c)
verify(!C_DasObjectFixture_5f019a81_valid(created))
return 1
}
