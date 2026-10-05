options gen2
require ue_foundation
var calls = 0
[export]
def main(input : int) : int {
    calls++
    return ue_add(input, calls + 1)
}
