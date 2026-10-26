options gen2
require ue_probe_actor
options gc
require ue_enhanced_input

[ue_class(module_id="vector2d",Blueprintable,parent="/Script/DasRuntime.DasProbeActor")]
class Vector2DActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Position:UEVector2D=ue_vector2d(0.0lf,0.0lf)
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Points:array<UEVector2D>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    NamedPoints:table<string;UEVector2D>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    MoveAction:InputActionRef

    [ue_function(BlueprintCallable)]
    def Echo(value:UEVector2D):UEVector2D { return value }
    [ue_function(BlueprintCallable)]
    def Adjust(var value:UEVector2D&; @ue_out var previous:UEVector2D&):void {
        previous = value
        value.x += 1.0lf
    }
    [ue_function(BlueprintCallable)]
    def Apply(value:UEVector2D):void { self.Position = value }
    [ue_function(BlueprintCallable)]
    def Schedule():void {
        ue_timer_next_tick("position","Apply",ue_vector2d(3.0lf,4.0lf))
    }
    [ue_function(BlueprintCallable)]
    def BindMove():void {
        ue_enhanced_bind(self.MoveAction,"Triggered","OnMove")
    }
    [ue_function(BlueprintCallable)]
    def OnMove(value:UEVector2D):void { self.Position = value }
    [ue_event(source="self",name="OnDasVector2DRefSignal")]
    def OnPosition(value:UEVector2D):void { self.Position = value }
}
