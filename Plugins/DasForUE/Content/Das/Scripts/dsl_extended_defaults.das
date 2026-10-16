options gen2
require ue_scene_components
require ue_object_types
[ue_native_object(path="/Script/Engine.StaticMeshComponent")]
struct DefaultsMeshRef { handle:UObjectHandle }
[ue_object_ref(script="dsl_extended_defaults_worker.das",type="DefaultsWorker")]
struct DefaultsWorkerRef { handle:UObjectHandle }
[ue_class(module_id="extendeddefaults",Blueprintable)]
class ExtendedDefaults {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var CDOPoints:int=1
        var CDOLabel:string="inline"
        var CDOMove:UEVector2D
        var CDOTransform:UETransform
        var CDOAsset:ObjectRef
        var CDOActorClass:ActorClass
        var CDOColor:UEStructValue=ue_struct_value("/Script/CoreUObject.LinearColor")
    }
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Pivot:SceneComponentRef
        var Mesh:DefaultsMeshRef
        var Worker:DefaultsWorkerRef
        var Sibling:DefaultsWorkerRef
    }
    defaults=ue_defaults() {
        self.CDOPoints=31
        self.CDOLabel="from defaults"
        self.CDOMove=ue_vector2d(2.0lf,4.0lf)
        self.CDOTransform=ue_transform(ue_vector(10.0lf,20.0lf,30.0lf),ue_rotator(0.0lf,45.0lf,0.0lf),ue_vector(2.0lf,2.0lf,2.0lf))
        self.CDOColor=ue_struct_default("/Script/CoreUObject.LinearColor","(R=0.25,G=0.5,B=0.75,A=1)")
        self.CDOAsset=ue_asset_default("/Engine/BasicShapes/Cube.Cube")
        self.CDOActorClass=ue_class_default("/Script/Engine.StaticMeshActor")
        self.Pivot.RelativeLocation=ue_vector(0.0lf,0.0lf,50.0lf)
        self.Pivot.RelativeRotation=ue_rotator(0.0lf,30.0lf,0.0lf)
        self.Mesh.LightingChannels=ue_default_text("(bChannel0=False,bChannel1=True,bChannel2=False)")
        self.Mesh.StaticMesh=ue_asset_default("/Engine/BasicShapes/Cube.Cube")
        self.Worker.Bonus=42
        self.Worker.Pose=ue_transform(ue_vector(1.0lf,2.0lf,3.0lf),ue_rotator(0.0lf,90.0lf,0.0lf),ue_vector(1.0lf,1.0lf,1.0lf))
        self.Worker.Visual=ue_asset_default("/Engine/BasicShapes/Cube.Cube")
        self.Worker.SpawnType=ue_class_default("/Script/Engine.StaticMeshActor")
    }
    [ue_function(BlueprintPure)]
    def VerifyDefaults():bool {
        return self.CDOPoints==31 && self.CDOLabel=="from defaults" && self.CDOMove.x==2.0lf && ue_object_valid(self.CDOAsset.handle) && ue_class_path(self.CDOActorClass)=="/Script/Engine.StaticMeshActor" && ue_struct_get_float(self.CDOColor,"R")==0.25
    }
}
