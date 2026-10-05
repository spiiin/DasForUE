options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
    let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
    let values <- [1, 2, 3]
    let changed <- fixture_TransformInts(self, values)
    verify(length(changed) == 3 && changed[0] == 11 && changed[2] == 13 && values[0] == 1)
    let reused <- fixture_TransformInts(self, changed)
    verify(reused[0] == 21)
    var empty : array<int>
    let empty_result <- fixture_TransformInts(self, empty)
    verify(length(empty_result) == 0)
    let text <- ["hello", "Привет"]
    let mutated <- fixture_MutateStrings(self, text)
    verify(mutated.out_returnvalue == 3 && length(mutated.out_values) == 3)
    verify(mutated.out_copy[2] == "世界" && mutated.out_values[1] == "Привет")
    verify(length(text) == 2)
    let names <- [ue_name(""), ue_name("Array_17")]
    let name_result <- fixture_EchoNames(self, names)
    verify(name_result[0].value == "None" && name_result[1].value == "Array_17")
    let floats <- [1.5, -2.25]
    let float_result <- fixture_EchoFloats(self, floats)
    verify(float_result[1] == -2.25)
    let doubles <- [1.23456789012345lf, -2.0lf]
    let double_result <- fixture_EchoDoubles(self, doubles)
    verify(double_result[0] == 1.23456789012345lf)
    let bools <- [true, false, true]
    let bool_result <- fixture_EchoBools(self, bools)
    verify(bool_result[0] && !bool_result[1] && bool_result[2])
    return 1
}
