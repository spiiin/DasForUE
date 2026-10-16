options gen2
module dsl_native_defaults public
require ue_native_self_refs public
require ue_native_engine_values public
[ue_class(module_id="nativedefaults",Blueprintable)]
class NativeDefaults : Actor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=7
    defaults=ue_defaults() {
        self.InitialLifeSpan=10.0f*3.0f
        self.bHidden=(1==2)
        self.RootComponent.RelativeScale3D=ue_vector(1.0lf,4.0lf,5.0lf)
    }
    [ue_function(BlueprintCallable)]
    def Read(input:int):int { if(self.InitialLifeSpan<0.0f) { return -1 };return self.Score+input }
    // Native fields are live reflected values, including inside script-only helpers.
    def ChangeNativeHelper(input:float):float {
        self.CustomTimeDilation=input
        self.CustomTimeDilation+=0.5f
        self.CustomTimeDilation*=2.0f
        self.CustomTimeDilation-=1.0f
        self.CustomTimeDilation/=2.0f
        return self.CustomTimeDilation
    }
    [ue_function(BlueprintCallable)]
    def ChangeNative(input:float):float { return self.ChangeNativeHelper(input) }
    [ue_function(BlueprintPure)]
    def const NativeLife():float { return self.InitialLifeSpan }
    [ue_function(BlueprintPure)]
    def const NativeRootValid():bool { return ue_scene_valid(self.RootComponent) }
    [ue_function(BlueprintPure)]
    def const NativeRoot():SceneComponentRef { return self.RootComponent }
    [ue_function(BlueprintPure)]
    def const NativeOwner():ActorRef { return self.Owner }
    [ue_function(BlueprintPure)]
    def const NativeRootScale():UEVector {
        return ue_transform_scale(ue_scene_relative_transform(self.RootComponent))
    }
    [ue_function(BlueprintPure)]
    def const NativeCollision():SpawnCollision { return self.SpawnCollisionHandlingMethod }
    [ue_function(BlueprintCallable)]
    def SetNativeCollision(value:SpawnCollision):SpawnCollision {
        self.SpawnCollisionHandlingMethod=value
        return self.SpawnCollisionHandlingMethod
    }
    [ue_function(BlueprintCallable)]
    def RejectForgedNativeCollision():bool {
        let before=self.SpawnCollisionHandlingMethod
        var invalid:SpawnCollision
        unsafe { invalid=reinterpret<SpawnCollision>(99) }
        var rejected=false
        try { self.SpawnCollisionHandlingMethod=invalid } recover { rejected=true }
        return rejected && self.SpawnCollisionHandlingMethod==before
    }
    [ue_function(BlueprintPure)]
    def const NativeScaleX():double { return self.RootComponent.RelativeScale3D.x }
    [ue_function(BlueprintPure)]
    def const NativeRelativeLocation():UEVector { return self.RootComponent.RelativeLocation }
    [ue_function(BlueprintPure)]
    def const NativeRelativeRotation():UERotator { return self.RootComponent.RelativeRotation }
    [ue_function(BlueprintPure)]
    def const NativeRelativeScale():UEVector { return self.RootComponent.RelativeScale3D }
    [ue_function(BlueprintPure)]
    def const NativeTransform():UETransform { return self.ActorTransform }
    [ue_function(BlueprintCallable)]
    def SetNativeTransform(value:UETransform):UETransform {
        self.ActorTransform=value
        return self.ActorTransform
    }
    [ue_function(BlueprintCallable)]
    def SetNativeInstigator(value:NativePawnRef):NativePawnRef {
        self.Instigator=value
        return self.Instigator
    }
    // Deliberately forged wrapper: the checked converter must reject it before writing UE.
    [ue_function(BlueprintCallable)]
    def ForgedInstigator():void {
        self.Instigator=NativePawnRef(handle=ue_self_object())
    }
    [ue_function(BlueprintCallable)]
    def AppendNativeTag(value:UENameValue):int {
        push(self.Tags,value)
        return int(length(self.Tags))
    }
    [ue_function(BlueprintCallable)]
    def SetNativeTag(index:int; value:UENameValue):void { self.Tags[index]=value }
    [ue_function(BlueprintCallable)]
    def EraseNativeTag(index:int):void { erase(self.Tags,index) }
    [ue_function(BlueprintCallable)]
    def ClearNativeTags():void { clear(self.Tags) }
    [ue_function(BlueprintCallable)]
    def PopNativeTag():void { pop(self.Tags) }
    [ue_function(BlueprintCallable)]
    def SetNativeTags(values:array<UENameValue>):void { self.Tags := values }
    [ue_function(BlueprintPure)]
    def const NativeTagAt(index:int):UENameValue { return self.Tags[index] }
    [ue_function(BlueprintPure)]
    def const NativeTagsSnapshot():array<UENameValue> {
        var values <- clone(self.Tags)
        return <- values
    }
    [ue_function(BlueprintCallable)]
    def DetachedNativeTags():int {
        var values <- clone(self.Tags)
        push(values,ue_name_value("Detached"))
        return int(length(values))
    }
    def NativeTagValueWithSideEffect():UENameValue {
        push(self.Tags,ue_name_value("SideEffect"))
        return ue_name_value("Updated")
    }
    [ue_function(BlueprintCallable)]
    def SetNativeTagWithSideEffect():void { self.Tags[0]=self.NativeTagValueWithSideEffect() }
}
