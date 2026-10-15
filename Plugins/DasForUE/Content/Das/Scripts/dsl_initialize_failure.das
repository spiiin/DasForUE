options gen2
require ue_dsl
[ue_class(module_id="initfailure",Blueprintable)]
class FailingInitializer {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Attempts:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Begins:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Ticks:int=0
    def Initialize():void { self.Attempts+=1; panic("initializer failure fixture") }
    def BeginPlay():void { self.Begins+=1 }
    def Tick(delta:float):void { self.Ticks+=1 }
}
