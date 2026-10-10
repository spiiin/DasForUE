options gen2
options gc
require ue_dsl

[ue_class(module_id="arrayevents",Blueprintable)]
class ArrayEventActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Label:UENameValue=ue_name_value("None")
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Scores:array<int>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Weights:array<double>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Messages:array<string>

    [ue_event(source="self",name="OnDasNameSignal")]
    def OnLabel(value:UENameValue):void { self.Label = value }
    [ue_event(source="self",name="OnDasArrayIntSignal")]
    def OnScores(values:array<int>):void { self.Scores := values }
    [ue_event(source="self",name="OnDasArrayDoubleSignal")]
    def OnWeights(values:array<double>):void { self.Weights := values }
    [ue_event(source="self",name="OnDasArrayStringSignal")]
    def OnMessages(values:array<string>):void { self.Messages := values }
}
