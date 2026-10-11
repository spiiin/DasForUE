options gen2
require ue_dsl
[ue_class(module_id="dynamiclife",parent="ActorComponent",Blueprintable)]
class DynamicLife {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Begins:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Ticks:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Ends:int=0
    def BeginPlay() { self.Begins += 1 }
    def Tick(delta:float) { self.Ticks += 1 }
    def EndPlay(reason:int) { self.Ends += 1 }
}
