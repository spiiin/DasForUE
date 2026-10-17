options gen2
require ue_animation
[ue_class(Blueprintable)]
class BadMontageCallback : Actor {
    @ue_property=(BlueprintReadWrite)
    Montage:AnimMontageRef
    @ue_component
    @RootComponent
    Mesh:SkeletalMeshComponentRef
    [ue_function(BlueprintCallable)]
    def Play():float { return ue_montage_play("bad",self.Mesh,self.Montage,"Wrong") }
    [ue_function(BlueprintCallable)]
    def Wrong(event:int):void {}
}
