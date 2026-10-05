options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
    let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
    let fresh = fixture_NewFixture(self)
    let g = ue_begin_call(ue_self_object(), "ForceTestGC")
    ue_invoke_call(g)
    ue_release_call(g)
    let values <- [fresh]
    let result <- fixture_EchoFixtureObjects(self, values)
    return 1
}
