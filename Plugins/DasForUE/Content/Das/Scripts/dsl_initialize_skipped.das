options gen2
require ue_initialize_actor_base
[ue_class(module_id="initskipped",Blueprintable)]
class SkippedInitializer : InitActorBase {
    def override Initialize():void { self.InitTrace=9; self.InitCount+=1 }
    def override BeginPlay():void { Super.BeginPlay(); self.BeginSawTrace=self.InitTrace }
}
