options gen2
module dsl_playable_character public
require ue_native_gameplay public

// The same field operators work in ordinary functions and in authored methods.
def set_movement_speed(movement:CharacterMovementRef; value:float):float {
    movement.MaxWalkSpeed=value
    return movement.MaxWalkSpeed
}

// Assign the four input assets in Blueprint. Possess this Character with a local
// PlayerController (or set Auto Possess Player in the level). Tick waits for input.
[ue_class(Blueprintable)]
class PlayableCharacter : Character {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Mapping Context: WASD, mouse XY and Space.
        var Controls:InputMappingContextRef
        var MoveAction:InputActionRef
        var LookAction:InputActionRef
        var JumpAction:InputActionRef
    }
    state=ue_properties(BlueprintReadWrite) {
        var ControlsBound:bool=false
        var MoveEvents:int=0
        var LookEvents:int=0
        var JumpEvents:int=0
    }
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Camera:CameraComponentRef
    }
    defaults=ue_defaults() {
        self.bUseControllerRotationYaw=true
        self.Camera.RelativeLocation=ue_vector(0.0lf,0.0lf,64.0lf)
        self.Camera.bUsePawnControlRotation=true
    }
    def BeginPlay():void {
        self.CharacterMovement.MaxWalkSpeed=450.0f
        self.CharacterMovement.JumpZVelocity=500.0f
    }
    def Tick(delta:float):void {
        let pawn=ue_character_self()
        if (!ue_character_controls_ready(pawn)) {
            if (self.ControlsBound) { ue_input_unbind_all(); ue_enhanced_clear_contexts() }
            self.ControlsBound=false
            return
        }
        if (!self.ControlsBound) { self.BindControls() }
    }
    [ue_function(BlueprintCallable)]
    def BindControls():void {
        let pawn=ue_character_self()
        ue_enhanced_add_context(ue_pawn_controller(pawn),self.Controls,0)
        ue_enhanced_bind(self.MoveAction,"Triggered","OnMove")
        ue_enhanced_bind(self.LookAction,"Triggered","OnLook")
        ue_enhanced_bind(self.JumpAction,"Started","OnJump")
        ue_enhanced_bind(self.JumpAction,"Completed","OnJump")
        ue_enhanced_bind(self.JumpAction,"Canceled","OnJump")
        self.ControlsBound=true
    }
    [ue_function(BlueprintCallable)]
    def OnMove(value:UEVector2D):void {
        self.MoveEvents+=1
        ue_character_move(ue_character_self(),float(value.y),float(value.x))
    }
    [ue_function(BlueprintCallable)]
    def OnLook(value:UEVector2D):void {
        self.LookEvents+=1
        self.AddControllerYawInput(float(value.x))
        self.AddControllerPitchInput(float(value.y))
    }
    [ue_function(BlueprintCallable)]
    def OnJump(value:bool):void {
        self.JumpEvents+=1
        if (value) { self.Jump() } else { self.StopJumping() }
    }
    [ue_function(BlueprintCallable)]
    def TakeControl(player:PlayerControllerRef):bool { return ue_pawn_possess(player,ue_character_self()) }
    [ue_function(BlueprintCallable)]
    def SetSpeed(value:float):float {
        return set_movement_speed(self.CharacterMovement,value)
    }
}
