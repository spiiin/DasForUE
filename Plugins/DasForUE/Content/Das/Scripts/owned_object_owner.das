options gen2
require owned_object_refs

[ue_class(Blueprintable)]
class OwnedActionOwner : Actor {
    // Configure an inline action in Blueprint defaults. Each owner receives its own copy.
    @ue_property=(EditAnywhere,BlueprintReadWrite,Instanced)
    ActionTemplate:OwnedActionRef
    // Ordinary strong references retain live actions; Outer alone does not.
    @ue_property=(BlueprintReadWrite)
    Actions:array<OwnedActionRef>

    [ue_function(BlueprintCallable)]
    def Create(kind:OwnedActionClass):bool {
        let owner=ue_ref(type<ActorRef>,ue_self_object())
        let action=ue_object_new(type<OwnedActionRef>,kind,owner,self.ActionTemplate)
        // Store before initialization: initialization may publish the object or invoke callbacks.
        push(self.Actions,action)
        action.Initialize(owner)
        return action.Ready()
    }
    [ue_function(BlueprintCallable)]
    def CreateDefault(kind:OwnedActionClass):bool {
        let owner=ue_ref(type<ActorRef>,ue_self_object())
        let action=ue_object_new(type<OwnedActionRef>,kind,owner)
        push(self.Actions,action)
        action.Initialize(owner)
        return action.Ready()
    }
    [ue_function(BlueprintCallable)]
    def Release():void { clear(self.Actions) }
}
