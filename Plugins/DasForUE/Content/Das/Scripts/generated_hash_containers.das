options gen2
require ue_generated
require ue_foundation
def lookup(values : H_Map_int_string_aa6f8c57; key : int) : string {
    for (entry in values.entries) { if (entry.key == key) { return entry.value } }
    return ""
}
def has_int(values : H_Set_int_bf4eb51c; wanted : int) : bool {
    for (value in values.values) { if (value == wanted) { return true } }
    return false
}
def has_string(values : H_Set_string_071b6f13; wanted : string) : bool {
    for (value in values.values) { if (value == wanted) { return true } }
    return false
}
[export]
def main(input:int):int {
    let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
    var map : H_Map_int_string_aa6f8c57
    map.entries <- [H_Map_int_string_aa6f8c57_Entry(key=1, value="hello"), H_Map_int_string_aa6f8c57_Entry(key=2, value="Привет")]
    let transformed <- fixture_TransformMap(self, map)
    verify(length(transformed.entries) == 2 && lookup(transformed, 2) == "Привет 世界")
    verify(lookup(map, 1) == "hello")
    let reused <- fixture_TransformMap(self, transformed)
    verify(lookup(reused, 1) == "hello 世界 世界")
    let mutated <- fixture_MutateMap(self, map)
    verify(mutated.out_returnvalue == 2 && lookup(mutated.out_values, 99) == "added")
    verify(lookup(mutated.out_copy, 1) == "" && lookup(map, 1) == "hello")
    var empty_map : H_Map_int_string_aa6f8c57
    let empty_map_result <- fixture_TransformMap(self, empty_map)
    verify(length(empty_map_result.entries) == 0)
    let made_map <- fixture_MakeMap(self)
    verify(lookup(made_map, 7) == "out")
    var names : H_Map_name_enum_30635d8d
    names.entries <- [H_Map_name_enum_30635d8d_Entry(key=ue_name(""), value=E_EDasCallMode_aac7a973.v_Active)]
    let echoed_names <- fixture_EchoNameModeMap(self, names)
    verify(echoed_names.entries[0].key.value == "None" && echoed_names.entries[0].value == E_EDasCallMode_aac7a973.v_Active)
    var doubles : H_Map_string_double_21761321
    doubles.entries <- [H_Map_string_double_21761321_Entry(key="Unicode 世界", value=1.23456789012345lf)]
    let echoed_doubles <- fixture_EchoDoubleMap(self, doubles)
    verify(echoed_doubles.entries[0].key == "Unicode 世界" && echoed_doubles.entries[0].value == 1.23456789012345lf)
    var floats : H_Map_int_float_d002fe13
    floats.entries <- [H_Map_int_float_d002fe13_Entry(key=9, value=1.5)]
    let echoed_floats <- fixture_EchoFloatMap(self, floats)
    verify(echoed_floats.entries[0].value == 1.5)
    var bools : H_Map_int_bool_8c6d2863
    bools.entries <- [H_Map_int_bool_8c6d2863_Entry(key=9, value=true)]
    let echoed_bools <- fixture_EchoBoolMap(self, bools)
    verify(echoed_bools.entries[0].value)
    var set : H_Set_int_bf4eb51c
    set.values <- [1,2,3]
    let changed_set <- fixture_TransformSet(self, set)
    verify(length(changed_set.values) == 3 && has_int(changed_set,99) && !has_int(changed_set,1))
    verify(has_int(set,1))
    let reused_set <- fixture_TransformSet(self, changed_set)
    verify(length(reused_set.values) == 3 && has_int(reused_set,99))
    var empty_set : H_Set_int_bf4eb51c
    let empty_set_result <- fixture_TransformSet(self, empty_set)
    verify(length(empty_set_result.values) == 1 && has_int(empty_set_result,99))
    let made_set <- fixture_MakeSet(self)
    verify(length(made_set.values) == 1 && has_int(made_set,7))
    var strings : H_Set_string_071b6f13
    strings.values <- ["remove", "Привет"]
    let changed_strings <- fixture_MutateSet(self, strings)
    verify(changed_strings.out_returnvalue == 2 && has_string(changed_strings.out_values,"世界"))
    verify(has_string(changed_strings.out_copy,"Привет") && has_string(strings,"remove"))
    var name_set : H_Set_name_171ad8fa
    name_set.values <- [ue_name("None"), ue_name("Name_17")]
    let echoed_name_set <- fixture_EchoNameSet(self, name_set)
    verify(length(echoed_name_set.values) == 2)
    var modes : H_Set_enum_cc9ec014
    modes.values <- [E_EDasCallMode_aac7a973.v_Active, E_EDasCallMode_aac7a973.v_Paused]
    let echoed_modes <- fixture_EchoModeSet(self, modes)
    verify(length(echoed_modes.values) == 2)
    return 1
}
