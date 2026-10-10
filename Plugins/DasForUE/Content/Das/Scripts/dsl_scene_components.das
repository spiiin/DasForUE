options gen2
require ue_scene_components

[ue_class(module_id="scene",Blueprintable)]
class SceneActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Pivot:SceneComponentRef

    def BeginPlay():void {
        let actor = ue_ref(type<ActorRef>,ue_self_object())
        self.Pivot = ue_scene_create(actor,"ScriptPivot")
        ue_scene_set_relative_transform(self.Pivot,ue_transform_identity())
    }
    [ue_function(BlueprintCallable)]
    def MoveRelative(value:UETransform):void { ue_scene_set_relative_transform(self.Pivot,value,true) }
    [ue_function(BlueprintCallable)]
    def MoveWorld(value:UETransform):void { ue_scene_set_world_transform(self.Pivot,value,true) }
    [ue_function(BlueprintCallable)]
    def AttachTo(parent:SceneComponentRef):bool { return ue_scene_attach(self.Pivot,parent,"","KeepWorld",false) }
    [ue_function(BlueprintCallable)]
    def Detach():void { ue_scene_detach(self.Pivot,"KeepWorld") }
    [ue_function(BlueprintCallable)]
    def RemovePivot():bool { return ue_scene_destroy(self.Pivot) }
}
