options gen2
require ue_native_gameplay
require ue_collision_profiles
require ue_pickup_refs
require ue_interaction
require ue_effects

[ue_class(Blueprintable),ue_implements(interface="/Game/DasExamples/Shared/Interactable.Interactable_C")]
class PlayablePickup : Actor {
    settings=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var MeshAsset:SoftStaticMesh=ue_soft(type<SoftStaticMesh>,"/Engine/BasicShapes/Sphere.Sphere")
        var PickupSound:SoundBaseRef
        var PickupVFX:NiagaraSystemRef
        var Collector:PlayableCharacterRef
        var Points:int=10
        var RespawnSeconds:float=3.0f
    }
    state=ue_properties(BlueprintReadWrite) {
        var Available:bool=false
        var MeshLoading:bool=false
        var MeshReady:bool=false
        var MeshWasLoaded:bool=false
        var MeshCallbacks:int=0
        var LoadFailures:int=0
        var EffectsStarted:int=0
        var EffectsFinished:int=0
        var Pickups:int=0
        var Restores:int=0
        var OverlapEvents:int=0
    }
    @ue_component
    @RootComponent
    Trigger:SphereComponentRef
    @ue_component
    @Attach="Trigger"
    Visual:StaticMeshComponentRef

    defaults=ue_defaults() {
        self.Visual.RelativeScale3D=ue_vector(0.5lf,0.5lf,0.5lf)
    }
    def BeginPlay():void {
        verify(self.Points>0 && self.RespawnSeconds>0.0f,"Pickup points and respawn delay must be positive")
        self.Visual.SetCollisionEnabled(ECollisionEnabled.NoCollision)
        self.Trigger.SetCollisionProfileName(ue_collision_profile_name(CollisionProfile.OverlapOnlyPawn),true)
        self.Trigger.SetCollisionResponseToChannel(ECollisionChannel.ECC_Visibility,ECollisionResponse.ECR_Block)
        self.Trigger.SetSphereRadius(75.0f,true)
        self.Trigger.SetGenerateOverlapEvents(true)
        self.LoadMesh()
    }
    // Restarting cancels the previous named load and any active respawn timer.
    [ue_function(BlueprintCallable)]
    def LoadMesh():void {
        ue_asset_cancel("mesh")
        ue_timer_cancel("respawn")
        self.Available=false
        self.MeshReady=false
        self.MeshLoading=true
        self.Trigger.SetCollisionEnabled(ECollisionEnabled.NoCollision)
        self.Visual.SetVisibility(false,true)
        self.Visual.SetStaticMesh(default<StaticMeshRef>)
        self.MeshWasLoaded=ue_soft_is_loaded(self.MeshAsset)
        ue_soft_load_async("mesh",self.MeshAsset,"OnMeshLoaded")
    }
    [ue_function(BlueprintCallable)]
    def OnMeshLoaded(mesh:StaticMeshRef):void {
        self.MeshCallbacks+=1
        self.MeshLoading=false
        if(!ue_is_valid(mesh) || !self.Visual.SetStaticMesh(mesh)) {
            self.LoadFailures+=1
            return
        }
        self.MeshReady=true
        self.Available=true
        self.Visual.SetVisibility(true,true)
        self.Trigger.SetCollisionEnabled(ECollisionEnabled.QueryOnly)
    }
    [ue_event(name="OnActorBeginOverlap")]
    def OnOverlap(overlapped:ActorRef; other:ActorRef):void {
        self.OverlapEvents+=1
        self.Interact(other)
    }
    [ue_function(BlueprintCallable)]
    def Interact(interactor:ActorRef):void {
        if(!self.Available || !ue_is_valid(interactor) || !ue_is_valid(self.Collector)) { return }
        if(!ue_same_object(interactor,self.Collector)) { return }
        // Set the guard before changing collision, which can issue nested events.
        self.Available=false
        self.Trigger.SetCollisionEnabled(ECollisionEnabled.NoCollision)
        self.Visual.SetVisibility(false,true)
        self.Collector.AddScore(self.Points)
        self.Pickups+=1
        if(ue_effect_at("pickup",self.PickupSound,self.PickupVFX,self.Trigger.K2_GetComponentLocation(),"OnPickupEffectFinished",0.35f,1.4f,0.4f)) { self.EffectsStarted+=1 }
        ue_timer_start("respawn",self.RespawnSeconds,"Restore",false)
    }
    [ue_function(BlueprintCallable)]
    def OnPickupEffectFinished():void { self.EffectsFinished+=1 }
    [ue_function(BlueprintCallable)]
    def Restore():void {
        if(self.Available || !self.MeshReady) { return }
        ue_timer_cancel("respawn")
        self.Restores+=1
        self.Available=true
        self.Visual.SetVisibility(true,true)
        self.Trigger.SetCollisionEnabled(ECollisionEnabled.QueryOnly)
    }
}
