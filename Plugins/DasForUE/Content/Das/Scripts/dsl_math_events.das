options gen2
require ue_probe_actor
require ue_dsl

[ue_class(module_id="mathevents",Blueprintable,parent="/Script/DasRuntime.DasProbeActor")]
class MathEventActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Position:UEVector=ue_vector(0.0lf,0.0lf,0.0lf)
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Orientation:UERotator=ue_rotator(0.0lf,0.0lf,0.0lf)
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Pose:UETransform

    [ue_event(source="self",name="OnDasVectorRefSignal")]
    def OnPosition(value:UEVector):void { self.Position = value }
    [ue_event(source="self",name="OnDasRotatorSignal")]
    def OnOrientation(value:UERotator):void { self.Orientation = value }
    [ue_event(source="self",name="OnDasRotatorRefSignal")]
    def OnOrientationRef(value:UERotator):void { self.Orientation = value }
    [ue_event(source="self",name="OnDasTransformSignal")]
    def OnPose(value:UETransform):void { self.Pose = value }
    [ue_event(source="self",name="OnDasTransformRefSignal")]
    def OnPoseRef(value:UETransform):void { self.Pose = value }
}
