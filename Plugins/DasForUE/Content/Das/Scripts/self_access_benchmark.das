options gen2
require ue_foundation

// Foundation micro-benchmark: one self int read and one write per iteration.
[export]
def main(input : int) : int {
    for (i in range(input)) {
        ue_set_self_int("health", ue_get_self_int("health") + 1)
    }
    return ue_get_self_int("health")
}
