options gen2
require ue_savegame

[ue_struct(BlueprintType)]
struct BuiltinAppearance {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Rotation:UEQuat=ue_quat_identity()
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Tint:UELinearColor=ue_linear_color(0.25f,0.5f,0.75f,1.0f)
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Packed:UEColor=ue_color(64,128,192,255)
}

[ue_delegate]
struct BuiltinRotationQuery { signature:function<(rotation:UEQuat; tint:UELinearColor; packed:UEColor):UEQuat> }

[ue_class(Blueprintable)]
class BuiltinValuesExample : SaveGame {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Rotation:UEQuat=ue_quat_identity()
        var Tint:UELinearColor=ue_linear_color(0.0f,0.5f,0.75f,1.0f)
        var Packed:UEColor=ue_color(64,128,192,255)
        var Rotations:array<UEQuat>
        var Palette:table<string;UELinearColor>
    }
    [ue_function(BlueprintCallable)]
    def Echo(rotation:UEQuat; tint:UELinearColor; packed:UEColor):UEQuat {
        self.Rotation=rotation
        self.Tint=tint
        self.Packed=packed
        return self.Rotation
    }
    [ue_function(BlueprintCallable)]
    def Mutate(var tint:UELinearColor&; @ue_out var previous:UEColor&):void {
        previous=self.Packed
        tint=ue_lerp(tint,self.Tint,0.5lf)
    }
    [ue_function(BlueprintCallable)]
    def Exercise():bool {
        self.Rotation=ue_quat(ue_rotator(10.0lf,20.0lf,30.0lf))
        self.Rotation.w=1.0lf
        self.Rotation=ue_normalized(self.Rotation,0.00000001lf)
        self.Tint.r=0.25f
        self.Packed.r=uint8(42)
        var poses:array<UEQuat>
        push(poses,self.Rotation)
        self.Rotations:=poses
        var palette:table<string;UELinearColor>
        palette["Main"]=self.Tint
        self.Palette:=palette
        var tint=self.Tint
        var previous=ue_color(0,0,0,0)
        self.Mutate(tint,previous)
        verify(tint.r==self.Tint.r && previous==self.Packed)
        let callback=ue_bind(type<BuiltinRotationQuery>,ue_self_object(),"Echo")
        let reply=callback.Execute(self.Rotation,tint,previous)
        return ue_nearly_equal(reply,self.Rotation,0.000000001lf) && self.Tint.r==0.25f && self.Packed.r==uint8(42)
    }
}
