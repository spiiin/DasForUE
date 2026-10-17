options gen2
require ue_animation
[ue_class(module_id="montageexample",Blueprintable)]
class MontageActor : Actor {
    settings=ue_properties(EditAnywhere,BlueprintReadWrite) { var Montage:AnimMontageRef }
    state=ue_properties(BlueprintReadWrite) {
        var RepeatOnComplete:bool=false
        var RearmStarted:int=0
        var Completed:int=0
        var Interrupted:int=0
        var BlendOut:int=0
        var NotifyBegin:int=0
        var NotifyEnd:int=0
        var LastNotify:UENameValue
    }
    @ue_component
    @RootComponent
    Mesh:SkeletalMeshComponentRef
    [ue_function(BlueprintCallable)]
    def Play():float { return ue_montage_play("action",self.Mesh,self.Montage,"OnAnimation") }
    [ue_function(BlueprintCallable)]
    def Stop():bool { return ue_montage_stop("action",0.05f) }
    [ue_function(BlueprintCallable)]
    def Cancel():bool { return ue_montage_cancel("action") }
    [ue_function(BlueprintCallable)]
    def Playing():bool { return ue_montage_playing("action") }
    [ue_function(BlueprintCallable)]
    def OnAnimation(event:MontageEvent):void {
        if(event.Kind==MontageEventKind.Completed) {
            self.Completed+=1
            // Clear the request first: the next completion must not loop forever.
            if(self.RepeatOnComplete) {
                self.RepeatOnComplete=false
                if(self.Play()>0.0f) { self.RearmStarted+=1 }
            }
        }
        if(event.Kind==MontageEventKind.Interrupted) { self.Interrupted+=1 }
        if(event.Kind==MontageEventKind.BlendOut) { self.BlendOut+=1 }
        if(event.Kind==MontageEventKind.NotifyBegin) { self.NotifyBegin+=1;self.LastNotify=event.NotifyName }
        if(event.Kind==MontageEventKind.NotifyEnd) { self.NotifyEnd+=1;self.LastNotify=event.NotifyName }
    }
}
