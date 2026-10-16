options gen2
module dsl_defaults_child public
require dsl_defaults_actor public
[ue_class]
class DefaultChild : DefaultActor {
    [ue_function(BlueprintPure,BlueprintEvent)]
    def override const DefaultEvent(value:int=6):int { return Super.DefaultEvent()+value }
    [ue_function(BlueprintCallable)]
    def SuperProbe():int { return self.DefaultEvent()+Super.DefaultEvent() }
}
