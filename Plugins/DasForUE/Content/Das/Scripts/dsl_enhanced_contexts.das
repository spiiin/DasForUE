options gen2
require ue_enhanced_input

// Assign assets and a local PlayerController in this world's Blueprint instance.
[ue_class(module_id="mappingcontexts",Blueprintable)]
class MappingContextActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Player:PlayerControllerRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Controls:InputMappingContextRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    JumpAction:InputActionRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    JumpHeld:bool=false

    // Call after Enhanced PlayerInput/InputComponent setup is complete.
    [ue_function(BlueprintCallable)]
    def Setup():void {
        if (!ue_enhanced_has_context(self.Player,self.Controls)) {
            ue_enhanced_add_context(self.Player,self.Controls,0)
        }
        ue_enhanced_bind(self.JumpAction,"Triggered","OnJump")
        ue_enhanced_bind(self.JumpAction,"Completed","OnJump")
    }
    [ue_function(BlueprintCallable)]
    def OnJump(value:bool):void { self.JumpHeld = value }
    [ue_function(BlueprintCallable)]
    def ClearOwned():int { return ue_enhanced_clear_contexts() }
    [ue_function(BlueprintCallable)]
    def ContextActive():bool { return ue_enhanced_has_context(self.Player,self.Controls) }
}
