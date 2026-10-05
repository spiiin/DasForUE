options gen2
require ue_generated
require ue_foundation
var saved : array<C_DasObjectFixture_5f019a81>
[export]
def main(input:int):int {
    let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
    if (input == 0) { saved <- fixture_MakeFixtureObjects(self); return int(length(saved)) }
    if (input == 1) { if (C_DasObjectFixture_5f019a81_valid(saved[0])) { return 1 }; return 0 }
    let result <- fixture_EchoFixtureObjects(self, saved)
    return 1
}
