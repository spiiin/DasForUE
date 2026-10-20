options gen2
require ue_foundation

// Explicit state survives logic reload. Each Actor owns its own counter.
var @ue_state count:int = 0
[export]
def main(input:int):int {
    count += input
    return count // Change to count + 100 while playing to observe hot reload.
}
