options gen2
require ue_dsl
[ue_class(Blueprintable)]
class StarterLogic : ActorComponent {
    state=ue_properties(BlueprintReadWrite) {
        var Starts:int=0
        var Count:int=0
    }
    def BeginPlay():void { self.Starts+=1 }
}
