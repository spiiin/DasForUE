options gen2
require ue_gas_playground_refs
require ue_character_control

[ue_class(Blueprintable)]
class GASPlaygroundPlayer : Character {
    settings=ue_properties(EditAnywhere,BlueprintReadOnly) {
        var Caster:AbilityPlaygroundRef
        var Controls:InputMappingContextRef
        var MoveAction:InputActionRef
        var LookAction:InputActionRef
        var JumpAction:InputActionRef
        var AimAction:InputActionRef
        var ConfirmAction:InputActionRef
        var CancelAction:InputActionRef
    }
    state=ue_properties(BlueprintReadWrite) {
        var ControlsBound:bool=false
        var MoveEvents:int=0
        var LookEvents:int=0
        var JumpEvents:int=0
        var AimEvents:int=0
        var ConfirmEvents:int=0
        var CancelEvents:int=0
    }
    @ue_component
    Camera:CameraComponentRef
    defaults=ue_defaults() {
        self.bUseControllerRotationYaw=true
        self.Camera.RelativeLocation=ue_vector(0.0lf,0.0lf,64.0lf)
        self.Camera.bUsePawnControlRotation=true
    }
    def BeginPlay():void {
        verify(ue_is_valid(self.Caster),"Map must assign the GAS caster")
        self.CharacterMovement.MaxWalkSpeed=450.0f
        self.CharacterMovement.JumpZVelocity=500.0f
    }
    def Tick(delta:float):void {
        if(!ue_character_controls_ready(ue_character_self())) {
            if(self.ControlsBound) { ue_input_unbind_all(); ue_enhanced_clear_contexts() }
            self.ControlsBound=false; return
        }
        if(!self.ControlsBound) { self.BindControls() }
        self.SyncAim()
    }
    [ue_function(BlueprintCallable)]
    def BindControls():void {
        ue_enhanced_add_context(ue_pawn_controller(ue_character_self()),self.Controls,0)
        ue_enhanced_bind(self.MoveAction,TriggerEvent.Triggered,self.OnMove)
        ue_enhanced_bind(self.LookAction,TriggerEvent.Triggered,self.OnLook)
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Started,self.OnJump)
        ue_enhanced_bind(self.JumpAction,TriggerEvent.Completed,self.OnJump)
        ue_enhanced_bind(self.AimAction,TriggerEvent.Started,self.OnAim)
        ue_enhanced_bind(self.AimAction,TriggerEvent.Completed,self.OnAim)
        ue_enhanced_bind(self.AimAction,TriggerEvent.Canceled,self.OnAim)
        ue_enhanced_bind(self.ConfirmAction,TriggerEvent.Started,self.OnConfirm)
        ue_enhanced_bind(self.CancelAction,TriggerEvent.Started,self.OnCancel)
        self.ControlsBound=true
    }
    def SyncAim():void {
        // The separate authored ASC actor follows the camera; its transform remains the ability's ray.
        if(ue_is_valid(self.Caster)) {
            let actor=ue_ref(type<ActorRef>,self.Caster.handle)
            actor.K2_TeleportTo(self.Camera.K2_GetComponentLocation(),self.Camera.K2_GetComponentRotation())
        }
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
        if(value) { self.Jump() } else { self.StopJumping() }
    }
    [ue_function(BlueprintCallable)]
    def OnAim(value:bool):void {
        self.AimEvents+=1
        self.SyncAim()
        if(value) { self.Caster.BeginAim() } else { self.Caster.CancelAim() }
    }
    [ue_function(BlueprintCallable)]
    def OnConfirm(value:bool):void {
        if(!value) { return }
        self.ConfirmEvents+=1
        self.SyncAim()
        self.Caster.ConfirmAim()
    }
    [ue_function(BlueprintCallable)]
    def OnCancel(value:bool):void {
        if(!value) { return }
        self.CancelEvents+=1
        self.Caster.CancelAim()
    }
}
