options gen2
require ue_dsl
require ue_object_types
[ue_class(module_id="badextendeddefaults",Blueprintable)]
class BadDefaults {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Value:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    ClassType:ActorClass
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    ActorAsset:ActorRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Asset:ObjectRef
    defaults=ue_defaults() {
        self.Value=10+20
    }
    [ue_function(BlueprintCallable)]
    def Ping():int { return self.Value }
}
