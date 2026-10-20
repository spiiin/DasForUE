options gen2
require ue_foundation

// Foundation micro-benchmark: one self int read per iteration.
[export]
def main(input : int) : int {
    var total = 0
    for (i in range(input)) {
        total += ue_get_self_int("health")
    }
    return total
}
