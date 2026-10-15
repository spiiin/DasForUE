options gen2
require ue_initialize_actor_base
[ue_class(module_id="initinherited",Blueprintable)]
class InheritedInitializer : InitActorBase {
    def override BeginPlay():void { Super.BeginPlay(); self.BeginSawTrace=self.InitTrace }
}
