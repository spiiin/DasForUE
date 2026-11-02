options gen2
require ue_dsl
require ue_object_types

[ue_class(Blueprintable)]
class OwnedAction : ScriptObject {
    // Configuration copied from the inline template.
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Cost:int=10
    @ue_property=(BlueprintReadWrite)
    Owner:ActorRef
    @ue_property=(BlueprintReadWrite)
    Initializations:int=0

    [ue_function(BlueprintCallable)]
    def Initialize(owner:ActorRef):void {
        self.Owner=owner
        self.Initializations+=1
    }
    [ue_function(BlueprintPure)]
    def Ready():bool { return self.Initializations==1 && ue_is_valid(self.Owner) }
}
