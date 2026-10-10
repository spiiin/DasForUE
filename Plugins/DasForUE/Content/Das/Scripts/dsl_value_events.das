options gen2
require ue_dsl
require ue_object_types

[ue_class(module_id="valueevents",Blueprintable)]
class ValueEventActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Total:double=0.0lf
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Position:UEVector=ue_vector(0.0lf,0.0lf,0.0lf)
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Target:ObjectRef

    [ue_event(source="self",name="OnDasDoubleSignal")]
    def OnAmount(value:double):void { self.Total += value }
    [ue_event(source="self",name="OnDasVectorSignal")]
    def OnPosition(value:UEVector):void { self.Position = value }
    [ue_event(source="self",name="OnDasObjectSignal")]
    def OnTarget(value:ObjectRef):void { self.Target = value }
}
