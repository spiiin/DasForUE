options gen2
require ue_scene_components
require ue_object_types
[ue_native_object(path="/Script/Engine.StaticMeshComponent")]
struct MergeMeshRef { handle:UObjectHandle }
[ue_object_ref(script="dsl_defaults_merge_worker.das",type="MergeWorker")]
struct MergeWorkerRef { handle:UObjectHandle }
[ue_class(module_id="defaultsmerge",Blueprintable)]
class MergeActor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Auto:int=20
        var Manual:int=20
        var Position:UEVector
        var Asset:ObjectRef
        var SpawnType:ActorClass
    }
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Pivot:SceneComponentRef
        var Mesh:MergeMeshRef
        var Worker:MergeWorkerRef
        var OtherWorker:MergeWorkerRef
    }
    defaults=ue_defaults() {
        self.Position=ue_vector(4.0lf,5.0lf,6.0lf)
        self.Asset=ue_asset_default("/Engine/BasicShapes/Sphere.Sphere")
        self.SpawnType=ue_class_default("/Script/Engine.Pawn")
        self.Pivot.RelativeLocation=ue_vector(4.0lf,5.0lf,20.0lf)
        self.Mesh.StaticMesh=ue_asset_default("/Engine/BasicShapes/Sphere.Sphere")
        self.Worker.Bonus=52
        self.OtherWorker.Bonus=52
    }
    [ue_function(BlueprintCallable)]
    def Read(input:int):int { return self.Auto+self.Manual+input }
}
