options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
    let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
    var floats : H_Set_float_112e6353
    floats.values <- [1.5, -2.25]
    let echoed_floats <- fixture_EchoFloatSet(self, floats)
    verify(length(echoed_floats.values) == 2)
    var doubles : H_Set_double_7bda134e
    doubles.values <- [1.23456789012345lf]
    let echoed_doubles <- fixture_EchoDoubleSet(self, doubles)
    verify(echoed_doubles.values[0] == 1.23456789012345lf)
    var map : H_Map_float_bool_d958a815
    map.entries <- [H_Map_float_bool_d958a815_Entry(key=-0.0,value=true), H_Map_float_bool_d958a815_Entry(key=1.5,value=false)]
    let echoed_map <- fixture_EchoFloatKeyMap(self, map)
    verify(length(echoed_map.entries) == 2)
    return 1
}
