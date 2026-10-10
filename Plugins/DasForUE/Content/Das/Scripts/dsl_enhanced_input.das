options gen2
require ue_enhanced_input

// Assign these InputAction assets in Blueprint and install their Mapping Context
// through the project's LocalPlayer subsystem. Call BindControls once possessed/ready.
[ue_class(module_id="enhancedinput",Blueprintable)]
class EnhancedInputActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    JumpAction:InputActionRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    ThrottleAction:InputActionRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    MoveAction:InputActionRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    JumpHeld:bool=false
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Throttle:float=0.0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    MoveValue:UEVector=ue_vector(0.0lf,0.0lf,0.0lf)

    [ue_function(BlueprintCallable)]
    def BindControls():void {
        ue_enhanced_bind(self.JumpAction,"Triggered","OnJump")
        ue_enhanced_bind(self.JumpAction,"Completed","OnJump")
        ue_enhanced_bind(self.ThrottleAction,"Triggered","OnThrottle")
        ue_enhanced_bind(self.MoveAction,"Triggered","OnMove")
    }
    [ue_function(BlueprintCallable)]
    def OnJump(value:bool):void { self.JumpHeld = value }
    [ue_function(BlueprintCallable)]
    def OnThrottle(value:float):void { self.Throttle = value }
    [ue_function(BlueprintCallable)]
    def OnMove(value:UEVector):void { self.MoveValue = value }
    [ue_function(BlueprintCallable)]
    def Ready():bool { return ue_enhanced_is_available() }
    [ue_function(BlueprintCallable)]
    def UnbindControls():int { return ue_input_unbind_all() }
}
