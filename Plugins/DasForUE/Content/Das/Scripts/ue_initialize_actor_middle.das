options gen2
module ue_initialize_actor_middle public
require ue_initialize_actor_base public
[ue_class(module_id="initactors",Blueprintable)]
class InitActorMiddle : InitActorBase {
    def override Initialize():void { Super.Initialize(); Super.Initialize(); self.InitTrace=self.InitTrace*10+2; self.InitCount+=1 }
}
