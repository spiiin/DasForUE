options gen2
require ue_dsl
require ue_object_types

[ue_class(module_id="interfaceevents",Blueprintable)]
class InterfaceEventActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Target:DasEventInterface

    // Native event parameter is UObject*. Non-null values must implement the interface.
    [ue_event(source="self",name="OnDasObjectSignal")]
    def OnTarget(value:DasEventInterface):void { self.Target = value }
}
