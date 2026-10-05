options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
    let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
    let values <- [self]
    let result <- fixture_EchoActors(self, values)
    return 1
}
