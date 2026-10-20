options gen2

// Foundation micro-benchmark baseline: the same loop without externs.
[export]
def main(input : int) : int {
    var total = 0
    for (i in range(input)) {
        total += i & 1
    }
    return total
}
