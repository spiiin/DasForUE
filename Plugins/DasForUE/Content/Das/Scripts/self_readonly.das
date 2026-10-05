options gen2
require ue_foundation
[export]
def main(input : int) : int {
    ue_set_self_int("readOnly", input)
    return input
}
