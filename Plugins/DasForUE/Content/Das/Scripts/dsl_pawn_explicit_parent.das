options gen2
require ue_dsl
[ue_class(parent="Pawn")]
class ExplicitPawn {
    defaults=ue_defaults() { self.BaseEyeHeight=54.0f }
    [ue_function(BlueprintPure)]
    def Value():int { return 7 }
}
[ue_class(parent="/Script/Engine.Character")]
class ExplicitCharacter {
    [ue_function(BlueprintPure)]
    def Value():int { return 9 }
}
