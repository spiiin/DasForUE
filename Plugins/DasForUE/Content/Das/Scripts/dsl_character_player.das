options gen2
module dsl_character_player public
require ue_dsl public
// Native lifecycle and properties use the real Unreal Character host.
[ue_class(Blueprintable)]
class PlayerCharacter : Character {
    defaults=ue_defaults() {
        self.BaseEyeHeight=55.0f
        self.bUseControllerRotationYaw=true
        self.CharacterMovement.MaxWalkSpeed=450.0f
    }
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=7
    @ue_property=(BlueprintReadWrite)
    Ticks:int=0
    @ue_property=(BlueprintReadWrite)
    Ends:int=0
    def Initialize():void { self.Score+=1 }
    def ConstructionScript():void { self.Score+=2 }
    def BeginPlay():void { self.Score+=3 }
    def Tick(delta:float):void { self.Ticks+=1 }
    def EndPlay(reason:int):void { self.Ends+=1 }
    [ue_function(BlueprintPure,BlueprintEvent)]
    def const Query(value:int=2):int { return self.Score+value }
    [ue_function(BlueprintCallable)]
    def Probe():int { return self.Query() }
    [ue_function(BlueprintCallable)]
    def NativeHeight(value:float):float { self.BaseEyeHeight=value;return self.BaseEyeHeight }
    [ue_function(BlueprintCallable)]
    def Adjust(var total:int&; @ue_out var doubled:int&):int {
        total+=self.Score
        doubled=total*2
        return self.Score
    }
}
