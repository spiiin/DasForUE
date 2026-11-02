options gen2
require ue_vector_math
[export]
def main(input : int) : int {
    var value = UE_VECTOR_ZERO
    value.xx = UE_VECTOR2D_ONE
    return input
}
