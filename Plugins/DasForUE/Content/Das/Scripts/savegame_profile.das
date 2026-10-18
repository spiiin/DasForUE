options gen2
require ue_savegame

[ue_class(Blueprintable)]
class PlayerSave : SaveGame {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Current score stored in the save slot.
        var Score:int=10
        // Display name of the player.
        var PlayerName:string="Player"
    }
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void { self.Score+=amount }
    [ue_function(BlueprintPure)]
    def GetScore():int { return self.Score }
}
