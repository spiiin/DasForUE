options gen2
require ue_dsl
require ue_struct_map_actor_refs

// Import the existing StructMapActor sample and its type assets first.
[ue_class(module_id="authoredevents",Blueprintable)]
class AuthoredEventActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Target:StructMapActorRef

    // The same physical native host is insufficient: the logical class ID must match.
    [ue_event(source="self",name="OnDasObjectSignal")]
    def OnTarget(value:StructMapActorRef):void { self.Target = value }
}
