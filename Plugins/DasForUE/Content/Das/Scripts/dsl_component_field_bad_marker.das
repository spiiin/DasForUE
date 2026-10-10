options gen2
require ue_dsl
require ue_component_types
[ue_class(module_id="actorcomponent", Blueprintable)]
class ComponentOwner {
    @ue_component
    @ue_component
    @ue_property=(VisibleAnywhere, BlueprintReadOnly)
    @Category="Components"
    @ToolTip="Подсчёт очков"
    Behavior : ScoreComponent
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 0

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Begins : int = 0

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Ticks : int = 0

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Ends : int = 0

    [ue_function(BlueprintCallable)]
    def GetScore():int { return self.Score }

    def BeginPlay() { self.Begins += 1; self.Score += 100 }
    def Tick(delta:float) { self.Ticks += 1 }
    def EndPlay(reason:int) { self.Ends += 1 }
}
