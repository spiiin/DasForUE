options gen2
require ue_probe_actor
require ue_dsl

// Emit the inherited OnDas*Signal events using Blueprint-callable EmitDas*Signal.
[ue_class(module_id="events", Blueprintable,parent="/Script/DasRuntime.DasProbeActor")]
class ScalarEventActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Total:float=0.0
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Enabled:bool=false
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Label:string=""
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Pulses:int=0

    [ue_event(source="self",name="OnDasFloatSignal")]
    def OnAmount(value:float):void { self.Total += value }
    [ue_event(source="self",name="OnDasBoolSignal")]
    def OnEnabled(value:bool):void { self.Enabled = value }
    [ue_event(source="self",name="OnDasStringSignal")]
    def OnLabel(value:string):void { self.Label = value }
    [ue_event(source="self",name="OnDasVoidSignal")]
    def OnPulse():void { self.Pulses += 1 }
}
