options gen2
require ue_interface_reentry_contract
require ue_interface_reentry_calls
[ue_class(type_folder="/Game/DasInterfaceReentry/Types",Blueprintable),ue_implements(interface=ReentryContract)]
class InterfaceReentryActor : Actor {
    [ue_function(BlueprintCallable)]
    def Label():string { return "Reentrant label" }
    @ue_property=(BlueprintReadWrite)
    Target:ObjectRef
    [ue_function(BlueprintCallable)]
    def Bounce(Input:int):int {
        if(Input==0) { return 1 }
        return interface_bounce(self.Target.handle,Input>0 ? Input-1 : Input)+1
    }
    [ue_function(BlueprintCallable)]
    def Fault(@ue_out var number:uint8&; var title:FText&):void {
        number=uint8(201)
        title=ue_text_invariant("Must not escape failed frame")
        panic("interface failure sentinel")
    }
}
