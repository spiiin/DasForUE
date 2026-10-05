options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
let created = fixture_NewFixture(self)
let c = ue_begin_call(ue_self_object(), "ForceTestGC")
ue_invoke_call(c)
ue_release_call(c)
C_DasObjectFixture_5f019a81_raw(created)
return 1
}
