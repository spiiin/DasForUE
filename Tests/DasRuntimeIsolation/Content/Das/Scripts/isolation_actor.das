options gen2
require ue_dsl
require ue_scene_components
[ue_class(module_id="isolation",Blueprintable)]
class IsolationActor : Actor {
    @ue_property=(BlueprintReadWrite)
    Score:int=10
    @ue_property=(BlueprintReadWrite)
    Begins:int=0
    @ue_property=(BlueprintReadWrite)
    Ticks:int=0
    @ue_property=(BlueprintReadWrite)
    Ready:bool=false
    @ue_component
    @RootComponent
    Pivot:SceneComponentRef
    def BeginPlay():void { self.Begins+=1; ue_latent_delay("ready",0.05,"OnReady") }
    def Tick(delta:float):void { self.Ticks+=1 }
    [ue_function(BlueprintCallable)]
    def OnReady():void { self.Ready=true }
    [ue_function(BlueprintCallable)]
    def Add(input:int):int { self.Score+=input; return self.Score }
}
