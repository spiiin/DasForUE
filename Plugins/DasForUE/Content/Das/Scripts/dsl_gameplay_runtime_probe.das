options gen2
require ue_probe_actor
options gc
require ue_scene_components

[ue_class(module_id="gameplayprobe",Blueprintable,parent="/Script/DasRuntime.DasProbeActor")]
class GameplayRuntimeProbe {
    @ue_property=(BlueprintReadWrite)
    Pulses:int=0
    @ue_property=(BlueprintReadWrite)
    SnapshotSum:int=0
    @ue_property=(BlueprintReadWrite)
    NextSum:int=0
    @ue_property=(BlueprintReadWrite)
    Delays:int=0
    @ue_property=(BlueprintReadWrite)
    Loads:int=0
    @ue_property=(BlueprintReadWrite)
    CancelledCalls:int=0
    @ue_property=(BlueprintReadWrite)
    SceneOK:bool=false
    @ue_property=(BlueprintReadWrite)
    LoadOK:bool=false
    @ue_property=(BlueprintReadWrite)
    Floats:float=0.0
    @ue_property=(BlueprintReadWrite)
    Voids:int=0
    @ue_property=(BlueprintReadWrite)
    Text:string=""
    @ue_property=(BlueprintReadWrite)
    ArraySum:int=0
    @ue_property=(BlueprintReadWrite)
    Position:UEVector2D=ue_vector2d(0.0lf,0.0lf)

    def BeginPlay():void {
        ue_timer_start("repeat",0.03,"Pulse",true)
        var values:array<int>
        push(values,3)
        push(values,4)
        ue_timer_start("snapshot",0.02,"Snapshot",false,values)
        push(values,99)
        ue_timer_next_tick("next","Next",ue_vector2d(6.0lf,7.0lf))
        ue_timer_next_tick("cancelled","Cancelled")
        ue_timer_cancel("cancelled")
        ue_latent_delay("ready",0.02,"Ready")
        ue_latent_delay("cancelled",0.02,"Cancelled")
        ue_latent_cancel("cancelled")
        ue_asset_load_async("material","/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial","Loaded")

        let actor=ue_ref(type<ActorRef>,ue_self_object())
        let root=ue_scene_create(actor,"GameplayRoot")
        let child=ue_scene_create(actor,"GameplayChild")
        let attached=ue_scene_attach(child,root)
        var cycleRejected=false
        try { ue_scene_attach(root,child) } recover { cycleRejected=true }
        ue_scene_set_relative_transform(child,ue_transform(ue_vector(1.0lf,2.0lf,3.0lf),ue_rotator(0.0lf,0.0lf,0.0lf),ue_vector(1.0lf,1.0lf,1.0lf)))
        let position=ue_transform_translation(ue_scene_relative_transform(child))
        ue_scene_detach(child,"KeepWorld")
        let destroyedChild=ue_scene_destroy(child)
        let destroyedRoot=ue_scene_destroy(root)
        var destroyedRejected=false
        try { ue_scene_world_transform(child) } recover { destroyedRejected=true }
        self.SceneOK=cycleRejected && destroyedRejected && attached && position.x==1.0lf && position.y==2.0lf && position.z==3.0lf && destroyedChild && destroyedRoot && !ue_scene_valid(child)
    }
    [ue_function(BlueprintCallable)]
    def Pulse():void { self.Pulses += 1 }
    [ue_function(BlueprintCallable)]
    def Snapshot(values:array<int>):void {
        for (value in values) { self.SnapshotSum += value }
    }
    [ue_function(BlueprintCallable)]
    def Next(value:UEVector2D):void { self.NextSum=int(value.x+value.y) }
    [ue_function(BlueprintCallable)]
    def Ready():void { self.Delays += 1 }
    [ue_function(BlueprintCallable)]
    def Cancelled():void { self.CancelledCalls += 1 }
    [ue_function(BlueprintCallable)]
    def Loaded(value:ObjectRef):void {
        self.Loads += 1
        self.LoadOK=ue_object_valid(value.handle)
    }
    [ue_function(BlueprintCallable)]
    def Stop():void { ue_timer_cancel("repeat") }
    [ue_function(BlueprintCallable)]
    def ArmPending():void {
        ue_timer_next_tick("destroyed","Cancelled")
        ue_latent_delay("destroyed",0.1,"Cancelled")
    }
    [ue_event(source="self",name="OnDasFloatSignal")]
    def OnFloat(value:float):void { self.Floats += value }
    [ue_event(source="self",name="OnDasVoidSignal")]
    def OnVoid():void { self.Voids += 1 }
    [ue_event(source="self",name="OnDasStringSignal")]
    def OnText(value:string):void { self.Text=value }
    [ue_event(source="self",name="OnDasArrayIntSignal")]
    def OnArray(values:array<int>):void {
        for (value in values) { self.ArraySum += value }
    }
    [ue_event(source="self",name="OnDasVector2DRefSignal")]
    def OnPosition(value:UEVector2D):void { self.Position=value }
}
