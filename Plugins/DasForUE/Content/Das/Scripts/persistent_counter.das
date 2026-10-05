options gen2
require ue_foundation

// Explicit state survives logic reload. Each Actor owns its own counter.
[export]
def main(input:int):int {
    let count = ue_state_get_int("count", 0) + input
    ue_state_set_int("count", count)
    return count // Change to count + 100 while playing to observe hot reload.
}
