options gen2
module dsl_pawn_player_child public
require dsl_pawn_player public
[ue_class]
class ChildPawn : PlayerPawn {
    def override BeginPlay():void { Super.BeginPlay();self.Score+=10 }
    [ue_function(BlueprintPure,BlueprintEvent)]
    def override const Query(value:int=2):int { return Super.Query(value)+5 }
}
