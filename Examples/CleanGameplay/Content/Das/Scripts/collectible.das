options gen2
require ue_native_gameplay
require ue_pickup_components
require ue_scene_components

[ue_multicast]
struct CollectedSignal { signature:function<(points:int):void> }

[ue_class(module_id="cleangameplay",Blueprintable)]
class Collectible : Actor {
    settings=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Points:int=10
        var RespawnSeconds:float=0.35f
        var MeshAsset:SoftStaticMesh=ue_soft(type<SoftStaticMesh>,"/Engine/BasicShapes/Cube.Cube")
    }
    state=ue_properties(BlueprintReadWrite) {
        var Score:int=0
        var Events:int=0
        var Restores:int=0
        var Ticks:int=0
        var Begins:int=0
        var Loads:int=0
        var Available:bool=false
        var Angle:double=0.0lf
    }
    @ue_property=(BlueprintAssignable)
    OnCollected:CollectedSignal
    @ue_component
    @RootComponent
    Pivot:SceneComponentRef
    @ue_component
    @Attach="Pivot"
    Visual:StaticMeshComponentRef

    def BeginPlay():void {
        self.Begins+=1
        self.Visual.SetCollisionEnabled(ECollisionEnabled.NoCollision)
        ue_soft_load_async("mesh",self.MeshAsset,"OnMeshLoaded")
    }
    def Tick(delta:float):void {
        self.Ticks+=1
        self.Angle+=double(delta)*45.0lf
        ue_scene_set_relative_transform(self.Pivot,ue_transform(ue_vector(0.0lf,0.0lf,0.0lf),ue_rotator(0.0lf,self.Angle,0.0lf),ue_vector(1.0lf,1.0lf,1.0lf)))
    }
    [ue_function(BlueprintCallable)]
    def OnMeshLoaded(mesh:StaticMeshRef):void {
        if (ue_is_valid(mesh) && self.Visual.SetStaticMesh(mesh)) { self.Loads+=1; self.Available=true }
    }
    [ue_function(BlueprintCallable)]
    def Collect():void {
        if (!self.Available) { return }
        self.Available=false
        self.Visual.SetVisibility(false,true)
        self.OnCollected.Broadcast(self.Points)
        ue_timer_start("respawn",self.RespawnSeconds,"Restore",false)
    }
    [ue_event(source="self",name="OnCollected")]
    def OnScoreChanged(points:int):void { self.Score+=points; self.Events+=1 }
    [ue_function(BlueprintCallable)]
    def Restore():void { self.Restores+=1; self.Available=true; self.Visual.SetVisibility(true,true) }
}
