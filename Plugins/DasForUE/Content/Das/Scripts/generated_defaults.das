options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
    let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
    verify(fixture_PolicyDefault(self) == 7)
    verify(fixture_PolicyDefault(self, 23) == 23)
    verify(fixture_DefaultScalars(self) == "defaults")
    verify(fixture_DefaultScalars(self, 9) == "Hello \{world\}")
    verify(fixture_DefaultScalars(self, 8, false, 2.0, 3.0lf, "override") == "override")
    verify(fixture_DefaultScalars(self) == "defaults")
    verify(fixture_DefaultWeight(self) == 1.234568lf) // exact CPP_Default_Value from UHT
    verify(fixture_DefaultWeight(self, 1.23456789012345lf) == 1.23456789012345lf)
    return 1
}
