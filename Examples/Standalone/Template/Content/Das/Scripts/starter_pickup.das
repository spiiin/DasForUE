options gen2
require ue_native_gameplay
require starter_refs
[ue_class(Blueprintable)]
class StarterPickup : Actor {
    settings=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Collector:StarterCharacterRef
        var MeshAsset:StaticMeshRef
        var Points:int=10
        var RespawnSeconds:float=1.0f
    }
    state=ue_properties(BlueprintReadWrite) {
        var Available:bool=true
        var Pickups:int=0
        var Restores:int=0
    }
    @ue_component
    @RootComponent
    Trigger:SphereComponentRef
    @ue_component
    @Attach="Trigger"
    Visual:StaticMeshComponentRef
    defaults=ue_defaults() {
        self.MeshAsset=ue_asset_default("/Engine/BasicShapes/Cube.Cube")
        self.Visual.RelativeScale3D=ue_vector(0.5lf,0.5lf,0.5lf)
    }
    def BeginPlay():void {
        verify(self.Visual.SetStaticMesh(self.MeshAsset),"Assign pickup mesh")
        self.Visual.SetCollisionEnabled(ECollisionEnabled.NoCollision)
        self.Trigger.SetCollisionProfileName(ue_name_value("OverlapOnlyPawn"),true)
        self.Trigger.SetSphereRadius(75.0f,true)
        self.Trigger.SetGenerateOverlapEvents(true)
        self.Trigger.SetCollisionEnabled(ECollisionEnabled.QueryOnly)
    }
    [ue_event(name="OnActorBeginOverlap")]
    def Overlap(actor:ActorRef; other:ActorRef):void {
        if(!self.Available || !ue_same_object(other,self.Collector)) { return }
        self.Available=false
        self.Trigger.SetCollisionEnabled(ECollisionEnabled.NoCollision)
        self.Visual.SetVisibility(false,true)
        self.Collector.AddScore(self.Points)
        self.Pickups+=1
        ue_timer_start("respawn",self.RespawnSeconds,"Restore",false)
    }
    [ue_function(BlueprintCallable)]
    def Restore():void {
        self.Restores+=1
        self.Available=true
        self.Visual.SetVisibility(true,true)
        self.Trigger.SetCollisionEnabled(ECollisionEnabled.QueryOnly)
    }
}
