options gen2
require rogue_patrol_point

[export]
def main(input : int) : int {
    let reference = default<RoguePatrolPointRef>
    return ue_is_valid(reference) ? -1 : 42
}
