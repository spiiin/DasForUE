options gen2
require ue_dsl

// Configure legacy Jump/MoveForward mappings and enable input on this Actor first.
[ue_class(module_id="input",Blueprintable)]
class InputActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    JumpCount:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    JumpHeld:bool=false
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    MoveValue:float=0.0

    // Call from Blueprint once InputComponent exists (e.g. after EnableInput).
    [ue_function(BlueprintCallable)]
    def BindControls():void {
        ue_input_bind_action("Jump","Pressed","OnJumpPressed",true,false)
        ue_input_bind_action("Jump","Released","OnJumpReleased",true,false)
        ue_input_bind_axis("MoveForward","OnMove",true,false)
    }
    [ue_function(BlueprintCallable)]
    def OnJumpPressed():void { self.JumpCount += 1; self.JumpHeld = true }
    [ue_function(BlueprintCallable)]
    def OnJumpReleased():void { self.JumpHeld = false }
    [ue_function(BlueprintCallable)]
    def OnMove(value:float):void { self.MoveValue = value }
    [ue_function(BlueprintCallable)]
    def InputReady():bool { return ue_input_is_available() }
    [ue_function(BlueprintCallable)]
    def UnbindControls():int { return ue_input_unbind_all() }
}
