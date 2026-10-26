options gen2
require ue_gameplay_refs
require ue_enhanced_input
require ue_scene_components

[ue_class(module_id="gameplaydemo",Blueprintable)]
class GameplayActor : Actor {
    @ue_property=(BlueprintAssignable)
    OnDasSignal:GameplayScoreSignal
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        // Текущие очки игрока.
        var Score:int=0
        var Move:UEVector2D=ue_vector2d(0.0lf,0.0lf)
        var Speed:double=100.0lf
        var Ready:bool=false
        var Loaded:ObjectRef
        var LoadSucceeded:bool=false
        var AssetPath:string=""
        var Player:PlayerControllerRef
        var Controls:InputMappingContextRef
        var JumpAction:InputActionRef
        var MoveAction:InputActionRef
        var Pivot:SceneComponentRef
    }
    components = ue_components(VisibleAnywhere, BlueprintReadOnly) {
        // Обновляет очки и обрабатывает события владельца.
        var Behavior:GameplayBehaviorRef
    }

    def BeginPlay():void {
        let actor = ue_ref(type<ActorRef>,ue_self_object())
        self.Pivot = ue_scene_create(actor,"GameplayPivot")
        ue_latent_delay("ready",0.25,"OnReady")
        if (self.AssetPath!="") { ue_asset_load_async("asset",self.AssetPath,"OnLoaded") }
    }
    def Tick(delta:float):void {
        if (ue_scene_valid(self.Pivot)) {
            let current = ue_scene_relative_transform(self.Pivot)
            var position = ue_transform_translation(current)
            position.x += self.Move.x*self.Speed*double(delta)
            position.y += self.Move.y*self.Speed*double(delta)
            ue_scene_set_relative_transform(self.Pivot,ue_transform(position,ue_transform_rotation(current),ue_transform_scale(current)))
        }
    }
    [ue_function(BlueprintCallable)]
    def SetupControls():void {
        if (!ue_enhanced_has_context(self.Player,self.Controls)) {
            ue_enhanced_add_context(self.Player,self.Controls,0)
        }
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Started,"OnJump")
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Completed,"OnJump")
        ue_enhanced_bind(self.MoveAction,TriggerEvent.Triggered,"OnMove")
        ue_enhanced_bind(self.MoveAction,TriggerEvent.Completed,"OnMove")
    }
    [ue_function(BlueprintCallable)]
    def OnJump(value:bool):void { if (value) { self.OnDasSignal.Broadcast(1) } }
    [ue_function(BlueprintCallable)]
    def OnMove(value:UEVector2D):void { self.Move = value }
    [ue_function(BlueprintCallable)]
    def OnReady():void { self.Ready = true }
    [ue_function(BlueprintCallable)]
    def OnLoaded(value:ObjectRef):void { self.Loaded = value; self.LoadSucceeded = ue_object_valid(value.handle) }
    [ue_function(BlueprintCallable)]
    def StartLoad():void { ue_asset_load_async("asset",self.AssetPath,"OnLoaded") }
    [ue_function(BlueprintCallable)]
    def CancelPending():void { ue_asset_cancel_all(); ue_latent_cancel_all() }
    [ue_function(BlueprintCallable)]
    def GetSeconds():float { return self.Behavior.Seconds }
    [ue_function(BlueprintCallable)]
    def PauseScore():bool { return self.Behavior.StopScore() }
    [ue_function(BlueprintCallable)]
    def ResumeScore():void { self.Behavior.StartScore() }
}
