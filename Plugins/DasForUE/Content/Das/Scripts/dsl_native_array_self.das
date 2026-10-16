options gen2
module dsl_native_array_self public
require ue_native_engine_values public
[ue_native_enum(path="/Script/Engine.EComponentMobility")]
enum NativeMobility {
    Static=0
    Stationary=1
    Movable=2
}
[ue_class(module_id="nativearrays",Blueprintable)]
class NativeArraySelf : Actor {
    [ue_function(BlueprintCallable)]
    def Enums():bool {
        clear(self.DasNativeArrayEnums)
        push(self.DasNativeArrayEnums,SpawnCollision.AlwaysSpawn)
        push(self.DasNativeArrayEnums,SpawnCollision.DontSpawnIfColliding)
        self.DasNativeArrayEnums[0]=SpawnCollision.AdjustIfPossibleButAlwaysSpawn
        var detached <- clone(self.DasNativeArrayEnums)
        clear(detached)
        var invalid:SpawnCollision
        unsafe { invalid=reinterpret<SpawnCollision>(99) }
        var rejected=false
        try { self.DasNativeArrayEnums[0]=invalid } recover { rejected=true }
        pop(self.DasNativeArrayEnums)
        push(self.DasNativeArrayEnums,SpawnCollision.AlwaysSpawn)
        erase(self.DasNativeArrayEnums,1)
        return rejected && length(self.DasNativeArrayEnums)==1 && self.DasNativeArrayEnums[0]==SpawnCollision.AdjustIfPossibleButAlwaysSpawn
    }
    [ue_function(BlueprintCallable)]
    def Bytes():bool {
        clear(self.DasNativeArrayBytes)
        push(self.DasNativeArrayBytes,NativeMobility.Movable)
        self.DasNativeArrayBytes[0]=NativeMobility.Stationary
        return self.DasNativeArrayBytes[0]==NativeMobility.Stationary
    }
    [ue_function(BlueprintCallable)]
    def Vectors():bool {
        clear(self.DasNativeArrayVectors)
        push(self.DasNativeArrayVectors,ue_vector(1.0lf,2.0lf,3.0lf))
        self.DasNativeArrayVectors[0].x+=9.0lf
        var detached <- clone(self.DasNativeArrayVectors)
        detached[0].y=77.0lf
        if(self.DasNativeArrayVectors[0].y!=2.0lf) { return false }
        self.DasNativeArrayVectors := detached
        var rejected=false
        try { self.DasNativeArrayVectors[999]=ue_vector(0.0lf,0.0lf,0.0lf) } recover { rejected=true }
        return rejected && self.DasNativeArrayVectors[0].x==10.0lf && self.DasNativeArrayVectors[0].y==77.0lf
    }
    [ue_function(BlueprintCallable)]
    def Rotators():bool {
        clear(self.DasNativeArrayRotators)
        push(self.DasNativeArrayRotators,ue_rotator(1.0lf,2.0lf,3.0lf))
        self.DasNativeArrayRotators[0].yaw+=8.0lf
        return self.DasNativeArrayRotators[0].yaw==10.0lf
    }
    [ue_function(BlueprintCallable)]
    def Transforms():bool {
        clear(self.DasNativeArrayTransforms)
        push(self.DasNativeArrayTransforms,ue_transform_identity())
        return ue_transform_scale(self.DasNativeArrayTransforms[0]).x==1.0lf
    }
    [ue_function(BlueprintCallable)]
    def Points():bool {
        clear(self.DasNativeArrayPoints)
        var point=ue_struct_value("/Script/CoreUObject.IntPoint")
        point=ue_struct_with_int(point,"X",12)
        push(self.DasNativeArrayPoints,point)
        var rejected=false
        try { self.DasNativeArrayPoints[0]=ue_struct_value("/Script/CoreUObject.Vector") } recover { rejected=true }
        var mixed <- clone(self.DasNativeArrayPoints)
        push(mixed,ue_struct_value("/Script/CoreUObject.Vector"))
        var wholeRejected=false
        try { self.DasNativeArrayPoints := mixed } recover { wholeRejected=true }
        return rejected && wholeRejected && length(self.DasNativeArrayPoints)==1 && ue_struct_get_int(self.DasNativeArrayPoints[0],"X")==12
    }
}
