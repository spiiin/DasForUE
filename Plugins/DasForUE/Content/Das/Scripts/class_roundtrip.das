options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
let base = C_DasObjectFixture_5f019a81_as_C_Object_55c3753a(self)
let derived = C_DasObjectFixture_5f019a81_from(C_Object_55c3753a_raw(base))
verify(C_DasObjectFixture_5f019a81_valid(derived))
fixture_SetFixtureTarget(self, C_DasObjectFixture_5f019a81_null())
verify(!C_Object_55c3753a_valid(fixture_GetTargetObject(self)))
let result = fixture_ExchangeObject(self, derived, base)
verify(C_Object_55c3753a_valid(result.out_inout) && C_Object_55c3753a_valid(result.out_output) && C_Object_55c3753a_valid(result.out_returnvalue))
verify(C_DasObjectFixture_5f019a81_valid(C_DasObjectFixture_5f019a81_from(C_Object_55c3753a_raw(result.out_returnvalue))))
return 1
}
