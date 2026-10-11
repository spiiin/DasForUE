options gen2
require ue_delegate_examples
[ue_class(module_id="delegateexample",Blueprintable)]
class DelegateExample {
    properties=ue_properties(BlueprintAssignable,Category="Events") {
        // Broadcasts each change to Blueprint and DAS handlers.
        var OnExampleEvent:FScoreEvent
    }

    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=0

    [ue_function(BlueprintCallable)]
    def FirstHandler(counter:int):void { self.Score+=counter }
    [ue_function(BlueprintCallable)]
    def SecondHandler(counter:int):void { self.Score+=counter*2 }
    [ue_function(BlueprintCallable)]
    def BindHandlers():void {
        self.OnExampleEvent.AddUFunction(ue_self_object(),"FirstHandler")
        self.OnExampleEvent.AddUFunction(ue_self_object(),"SecondHandler")
    }
    [ue_function(BlueprintCallable)]
    def Emit(counter:int):void { self.OnExampleEvent.Broadcast(counter) }
    [ue_function(BlueprintCallable)]
    def UnbindSecond():void {
        self.OnExampleEvent.RemoveUFunction(ue_self_object(),"SecondHandler")
    }
}
