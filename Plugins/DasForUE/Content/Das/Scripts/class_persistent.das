options gen2
require ue_generated
require ue_foundation
var saved = C_DasObjectFixture_5f019a81_null()
[export]
def main(input:int):int {
    if (input == 0) {
        saved = fixture_NewFixture(C_DasObjectFixture_5f019a81_from(ue_self_object()))
        if (C_DasObjectFixture_5f019a81_valid(saved)) { return 1 }; return 0
    }
    if (input == 1) { if (C_DasObjectFixture_5f019a81_valid(saved)) { return 1 }; return 0 }
    return fixture_AddHealth(saved, 1)
}
