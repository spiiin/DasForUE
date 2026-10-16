options gen2
module dsl_construction_component public
require ue_dsl public
[ue_class(module_id="construction",Blueprintable)]
class StandaloneConstruction : ActorComponent {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=7
    @ue_property=(BlueprintReadWrite)
    ConstructionValue:int=0
    @ue_property=(BlueprintReadWrite)
    ConstructionCalls:int=0
    @ue_property=(BlueprintReadWrite)
    InitializeCalls:int=0
    @ue_property=(BlueprintReadWrite)
    BeginSaw:int=0
    [ue_function(BlueprintCallable)]
    def Pulse():void {}
    def Initialize():void { self.InitializeCalls+=1 }
    def BeginPlay():void { self.BeginSaw=self.ConstructionValue }
    def ConstructionScript():void {
        self.ConstructionValue=self.Score*3
        self.ConstructionCalls+=1
        if(self.Score<0) { ue_timer_next_tick("forbidden","Pulse") }
    }
}
