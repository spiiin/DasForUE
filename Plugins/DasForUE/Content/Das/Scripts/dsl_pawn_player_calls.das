options gen2
require ue_dsl
[ue_object_ref(script="dsl_pawn_player.das", type="PlayerPawn")]
struct PlayerRef { handle:UObjectHandle }
[export]
def main():int {
    let player=ue_ref(type<PlayerRef>,ue_self_object())
    var total=4
    var doubled=0
    let value=player.Adjust(total,doubled)
    if (value!=player.Score || total!=4+player.Score || doubled!=total*2) { return -1 }
    return player.Query()+player.Probe()
}
