options gen2
require ue_dsl

[ue_class(module_id="latent",Blueprintable)]
class LatentActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Completed:int=0

    def BeginPlay():void {
        ue_latent_delay("intro",0.5,"OnIntroFinished")
    }
    [ue_function(BlueprintCallable)]
    def OnIntroFinished():void { self.Completed += 1 }
    [ue_function(BlueprintCallable)]
    def Restart():void { ue_latent_delay("intro",0.5,"OnIntroFinished") }
    [ue_function(BlueprintCallable)]
    def Cancel():bool { return ue_latent_cancel("intro") }
    [ue_function(BlueprintCallable)]
    def Pending():bool { return ue_latent_is_pending("intro") }
    [ue_function(BlueprintCallable)]
    def CancelAll():int { return ue_latent_cancel_all() }
}
