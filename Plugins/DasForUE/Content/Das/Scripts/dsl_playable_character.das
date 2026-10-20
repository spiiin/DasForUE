options gen2
module dsl_playable_character public
require ue_native_gameplay public
require ue_gameplay_services public
require ue_health_refs public
require ue_interaction public
require ue_effects public

[ue_widget_layout]
struct HUDView { Score:TextBlockRef; Health:TextBlockRef; Damage:ButtonRef }

// The same field operators work in ordinary functions and in authored methods.
def set_movement_speed(movement:CharacterMovementRef; value:float):float {
    movement.MaxWalkSpeed=value
    return movement.MaxWalkSpeed
}

// Assign the mapping context, six input actions and HUD class in Blueprint. Possess this Character with a local
// PlayerController (or set Auto Possess Player in the level). Tick waits for input.
[ue_class(Blueprintable)]
class PlayableCharacter : Character {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Mapping Context: WASD, mouse XY and Space.
        var Controls:InputMappingContextRef
        var MoveAction:InputActionRef
        var LookAction:InputActionRef
        var JumpAction:InputActionRef
        var InteractAction:InputActionRef
        var HurtAction:InputActionRef
        var UIAction:InputActionRef
        var HurtSound:SoundBaseRef
        var HurtVFX:NiagaraSystemRef
        var HUDClass:UserWidgetClass
    }
    state=ue_properties(BlueprintReadWrite) {
        var Score:int=0
        var HUD:UserWidgetRef
        var UIOpen:bool=false
        var Interactions:int=0
        var HealthChanges:int=0
        var HurtEffectsStarted:int=0
        var HurtEffectsFinished:int=0
        var HUDClicks:int=0
        var ControlsBound:bool=false
        var MoveEvents:int=0
        var LookEvents:int=0
        var JumpEvents:int=0
    }
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Camera:CameraComponentRef
        var Label:TextRenderComponentRef
        var Health:HealthComponentRef
    }
    defaults=ue_defaults() {
        self.HUDClass=ue_class_default("/Script/DasRuntime.DasGameplayHUD")
        self.bUseControllerRotationYaw=true
        self.Camera.RelativeLocation=ue_vector(0.0lf,0.0lf,64.0lf)
        self.Camera.bUsePawnControlRotation=true
        self.Label.RelativeLocation=ue_vector(0.0lf,0.0lf,130.0lf)
    }
    def BeginPlay():void {
        self.Label.K2_SetText(ue_text("DasForUE","PlayerLabel","Player"))
        self.CharacterMovement.MaxWalkSpeed=450.0f
        self.CharacterMovement.JumpZVelocity=500.0f
    }
    def Tick(delta:float):void {
        let pawn=ue_character_self()
        if (!ue_character_controls_ready(pawn)) {
            if (self.ControlsBound) { ue_input_unbind_all(); ue_enhanced_clear_contexts() }
            if(ue_widget_visible(self.HUD)) { ue_widget_remove(self.HUD);self.HUD=default<UserWidgetRef>;self.UIOpen=false }
            self.ControlsBound=false
            return
        }
        if (!self.ControlsBound) { self.BindControls() }
        if(!ue_widget_visible(self.HUD)) { self.SetupHUD() }
    }
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void {
        self.Score+=amount
        self.Label.K2_SetText(ue_text_invariant("Score: {self.Score}"))
        self.RefreshHUD()
    }
    [ue_function(BlueprintCallable)]
    def BindControls():void {
        let pawn=ue_character_self()
        ue_enhanced_add_context(ue_pawn_controller(pawn),self.Controls,0)
        ue_enhanced_bind(self.MoveAction,TriggerEvent.Triggered,self.OnMove)
        ue_enhanced_bind(self.LookAction,TriggerEvent.Triggered,self.OnLook)
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Started,self.OnJump)
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Completed,self.OnJump)
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Canceled,self.OnJump)
        if(ue_is_valid(self.InteractAction)) { ue_enhanced_bind(self.InteractAction,TriggerEvent.Started,self.OnInteract) }
        if(ue_is_valid(self.HurtAction)) { ue_enhanced_bind(self.HurtAction,TriggerEvent.Started,self.OnHurt) }
        if(ue_is_valid(self.UIAction)) { ue_enhanced_bind(self.UIAction,TriggerEvent.Started,self.OnToggleUI) }
        self.ControlsBound=true
    }
    [ue_function(BlueprintCallable)]
    def OnMove(value:InputActionValue):void {
        let axis=ue_input_axis2d(value)
        self.MoveEvents+=1
        ue_character_move(ue_character_self(),float(axis.y),float(axis.x))
    }
    [ue_function(BlueprintCallable)]
    def OnLook(value:InputActionValue):void {
        let axis=ue_input_axis2d(value)
        self.LookEvents+=1
        self.AddControllerYawInput(float(axis.x))
        self.AddControllerPitchInput(float(axis.y))
    }
    [ue_function(BlueprintCallable)]
    def OnJump(value:bool):void {
        self.JumpEvents+=1
        if (value) { self.Jump() } else { self.StopJumping() }
    }
    [ue_function(BlueprintCallable)]
    def SetupHUD():void {
        if(ue_widget_visible(self.HUD)) { return }
        if(!ue_class_valid(self.HUDClass)) { return }
        self.UIOpen=false
        self.HUD=ue_widget_create(self.HUDClass,ue_pawn_controller(ue_character_self()))
        verify(ue_widget_show(self.HUD),"Cannot add HUD to local player screen")
        let view=ue_widget_view(type<HUDView>,self.HUD)
        ue_widget_bind_click(view.Damage,self.HurtFromHUD)
        self.RefreshHUD()
    }
    def RefreshHUD():void {
        if(!ue_widget_visible(self.HUD)) { return }
        let view=ue_widget_view(type<HUDView>,self.HUD)
        ue_widget_set_text(view.Score,ue_text_invariant("Score: {self.Score}"))
        ue_widget_set_text(view.Health,ue_text_invariant("Health: {self.Health.Current} / {self.Health.Maximum}"))
    }
    [ue_event(source="Health",name="Changed")]
    def OnHealthChanged(current:float; maximum:float):void { self.HealthChanges+=1;self.RefreshHUD() }
    [ue_event(name="OnTakeAnyDamage")]
    def OnDamageFeedback(damaged:ActorRef; amount:float; kind:ObjectRef; instigator:ObjectRef; source:ActorRef):void {
        if(!(amount>0.0f && amount<=3.402823466e38f)) { return }
        let start=self.Camera.K2_GetComponentLocation()
        let forward=self.Camera.GetForwardVector()
        let point=ue_vector(start.x+forward.x*100.0lf,start.y+forward.y*100.0lf,start.z+forward.z*100.0lf)
        if(ue_effect_at("hurt",self.HurtSound,self.HurtVFX,point,self.OnHurtEffectFinished,0.25f,0.7f,0.2f)) { self.HurtEffectsStarted+=1 }
    }
    [ue_function(BlueprintCallable)]
    def OnHurtEffectFinished():void { self.HurtEffectsFinished+=1 }
    [ue_function(BlueprintCallable)]
    def HurtFromHUD():void { self.HUDClicks+=1;self.ReceiveDamage(25.0f) }
    [ue_function(BlueprintCallable)]
    def ReceiveDamage(amount:float):void {
        ue_apply_damage(ue_self_actor(),amount,ue_self_actor(),ue_pawn_controller(ue_character_self()))
    }
    [ue_function(BlueprintCallable)]
    def OnHurt(pressed:bool):void { if(pressed) { self.ReceiveDamage(25.0f) } }
    [ue_function(BlueprintCallable)]
    def OnToggleUI(pressed:bool):void {
        if(!pressed || !ue_widget_visible(self.HUD)) { return }
        self.UIOpen=!self.UIOpen
        ue_widget_input(self.HUD,self.UIOpen)
    }
    [ue_function(BlueprintCallable)]
    def OnInteract(pressed:bool):void {
        if(!pressed) { return }
        let start=self.Camera.K2_GetComponentLocation()
        let forward=self.Camera.GetForwardVector()
        let finish=ue_vector(start.x+forward.x*500.0lf,start.y+forward.y*500.0lf,start.z+forward.z*500.0lf)
        let hit=ue_line_trace(start,finish)
        let target=ue_hit_actor(hit)
        if(hit.blocking_hit && ue_interact(target,ue_self_actor())) { self.Interactions+=1 }
    }
    [ue_function(BlueprintCallable)]
    def TakeControl(player:PlayerControllerRef):bool { return ue_pawn_possess(player,ue_character_self()) }
    [ue_function(BlueprintCallable)]
    def SetSpeed(value:float):float {
        return set_movement_speed(self.CharacterMovement,value)
    }
}
