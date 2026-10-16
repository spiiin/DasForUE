options gen2
require ue_dsl
[ue_class(module_id="nativebase", Blueprintable)]
class NativeComponent : ActorComponent {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Bonus:int=3
    def BeginPlay():void { self.Bonus+=1 }
}
