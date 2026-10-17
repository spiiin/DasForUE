options gen2
module dsl_native_array_self public
require ue_native_engine_values public
require ue_native_self_refs public
[ue_native_enum(path="/Script/Engine.EComponentMobility")]
enum NativeMobility {
    Static=0
    Stationary=1
    Movable=2
}
[ue_class(module_id="nativearrays",Blueprintable)]
class NativeArraySelf : Actor {
    [ue_function(BlueprintCallable)]
    def ObjectHashes():bool {
        let actor=ue_ref(type<ActorRef>,ue_self_object())
        let key=ue_key(actor)
        let nil=default<ActorKey>
        if (!key_exists(self.DasNativeArrayActorSet,key) || self.DasNativeArrayObjectKeys[key]!=321) { return false }
        clear(self.DasNativeArrayActorSet)
        insert(self.DasNativeArrayActorSet,key)
        insert(self.DasNativeArrayActorSet,ue_key(type<ActorKey>,ue_self_object()))
        insert(self.DasNativeArrayActorSet,nil)
        let removed=erase(self.DasNativeArrayActorSet,nil)
        let missing=erase(self.DasNativeArrayActorSet,nil)
        var copy=clone(self.DasNativeArrayActorSet)
        insert(copy,nil)
        if (!removed || missing || length(self.DasNativeArrayActorSet)!=1 || length(copy)!=2 || !ue_key_valid(key) || ue_key_valid(nil)) { return false }
        self.DasNativeArrayActorSet := copy
        self.DasNativeArrayObjectKeys[key]=7
        self.DasNativeArrayObjectKeys[ue_key(type<ActorKey>,ue_self_object())]+=1
        self.DasNativeArrayObjectKeys[nil]=42
        self.DasNativeArrayObjectVectors[key]=ue_vector(1.0lf,2.0lf,3.0lf)
        self.DasNativeArrayObjectVectors[key].x+=4.0lf
        self.DasNativeArrayObjectEnums[key]=SpawnCollision.AlwaysSpawn
        let pawn=self.DasNativeArrayPawns[0]
        let pawnKey=ue_key(pawn)
        insert(self.DasNativeArrayPawnSet,pawnKey)
        self.DasNativeArrayPawnKeys[pawnKey]=pawn
        var invalid:table<NativePawnKey>
        unsafe { insert(invalid,reinterpret<NativePawnKey>(key)) }
        var rejected=false
        try { self.DasNativeArrayPawnSet := invalid } recover { rejected=true }
        var wrong=false
        try { ue_key(type<NativePawnKey>,actor.handle) } recover { wrong=true }
        return rejected && wrong && length(self.DasNativeArrayPawnSet)==1 && self.DasNativeArrayObjectKeys[key]==8 && self.DasNativeArrayObjectKeys[nil]==42 && self.DasNativeArrayObjectVectors[key].x==5.0lf && ue_object_valid(ue_key_ref(key).handle)
    }
    [ue_function(BlueprintCallable)]
    def ExpiredObjectHashes():bool {
        var rejectedSet=false
        var rejectedMap=false
        try { let values=self.DasNativeArrayPawnSet; length(values) } recover { rejectedSet=true }
        try { let values=self.DasNativeArrayPawnKeys; length(values) } recover { rejectedMap=true }
        var emptySet:table<NativePawnKey>
        var emptyMap:table<NativePawnKey;NativePawnRef>
        self.DasNativeArrayPawnSet := emptySet
        self.DasNativeArrayPawnKeys := emptyMap
        return rejectedSet && rejectedMap && length(self.DasNativeArrayPawnSet)==0 && length(self.DasNativeArrayPawnKeys)==0
    }

    [ue_function(BlueprintCallable)]
    def EnumSets():bool {
        if (!key_exists(self.DasNativeArrayEnumSet,SpawnCollision.AlwaysSpawn)) { return false }
        clear(self.DasNativeArrayEnumSet)
        insert(self.DasNativeArrayEnumSet,SpawnCollision.AlwaysSpawn)
        insert(self.DasNativeArrayEnumSet,SpawnCollision.AlwaysSpawn)
        insert(self.DasNativeArrayEnumSet,SpawnCollision.DontSpawnIfColliding)
        let removed=erase(self.DasNativeArrayEnumSet,SpawnCollision.AlwaysSpawn)
        let missing=erase(self.DasNativeArrayEnumSet,SpawnCollision.AlwaysSpawn)
        var copy=clone(self.DasNativeArrayEnumSet)
        insert(copy,SpawnCollision.AdjustIfPossibleButAlwaysSpawn)
        if (!removed || missing || length(self.DasNativeArrayEnumSet)!=1 || length(copy)!=2) { return false }
        self.DasNativeArrayEnumSet := copy
        clear(self.DasNativeArrayByteSet)
        insert(self.DasNativeArrayByteSet,NativeMobility.Movable)
        insert(self.DasNativeArrayByteSet,NativeMobility.Stationary)
        erase(self.DasNativeArrayByteSet,NativeMobility.Movable)
        var invalid:table<SpawnCollision>
        unsafe { insert(invalid,reinterpret<SpawnCollision>(255)) }
        var rejected=false
        try { self.DasNativeArrayEnumSet := invalid } recover { rejected=true }
        return rejected && length(self.DasNativeArrayEnumSet)==2 && key_exists(self.DasNativeArrayEnumSet,SpawnCollision.DontSpawnIfColliding) && length(self.DasNativeArrayByteSet)==1 && key_exists(self.DasNativeArrayByteSet,NativeMobility.Stationary)
    }

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
    [ue_function(BlueprintCallable)]
    def Objects():bool {
        clear(self.DasNativeArrayObjects)
        push(self.DasNativeArrayObjects,ue_ref(type<ActorRef>,ue_self_object()))
        push(self.DasNativeArrayObjects,ue_ref(type<ActorRef>,ue_null_object()))
        let pawn=self.DasNativeArrayPawns[0]
        push(self.DasNativeArrayPawns,NativePawnRef_from(ue_null_object()))
        var detached <- clone(self.DasNativeArrayPawns)
        detached[0]=NativePawnRef_from(ue_null_object())
        if(!ue_object_valid(NativePawnRef_raw(self.DasNativeArrayPawns[0]))) { return false }
        self.DasNativeArrayPawns := detached
        self.DasNativeArrayPawns[0]=pawn
        var rejected=false
        try { self.DasNativeArrayPawns[0]=NativePawnRef(handle=ue_self_object()) } recover { rejected=true }
        return rejected && length(self.DasNativeArrayPawns)==2 && ue_object_valid(NativePawnRef_raw(self.DasNativeArrayPawns[0])) && !ue_object_valid(NativePawnRef_raw(self.DasNativeArrayPawns[1]))
    }
    [ue_function(BlueprintCallable)]
    def Maps():bool {
        clear(self.DasNativeArrayIntMap)
        self.DasNativeArrayIntMap[1]=10
        self.DasNativeArrayIntMap[1]+=5
        insert(self.DasNativeArrayIntMap,2,20)
        if(!erase(self.DasNativeArrayIntMap,2)) { return false }
        var copy <- clone(self.DasNativeArrayIntMap)
        copy[1]=99
        if(self.DasNativeArrayIntMap[1]!=15) { return false }
        self.DasNativeArrayIntMap := copy
        clear(self.DasNativeArrayDoubleMap)
        self.DasNativeArrayDoubleMap["speed"]=2.5lf
        self.DasNativeArrayDoubleMap["speed"]*=2.0lf
        clear(self.DasNativeArrayNameMap)
        self.DasNativeArrayNameMap[7]=ue_name_value("Native")
        return length(self.DasNativeArrayIntMap)==1 && self.DasNativeArrayIntMap[1]==99 && self.DasNativeArrayDoubleMap["speed"]==5.0lf
    }
    [ue_function(BlueprintCallable)]
    def MapValues():bool {
        clear(self.DasNativeArrayVectorMap)
        self.DasNativeArrayVectorMap["origin"]=ue_vector(1.0lf,2.0lf,3.0lf)
        self.DasNativeArrayVectorMap["origin"].x+=9.0lf
        clear(self.DasNativeArrayEnumMap)
        self.DasNativeArrayEnumMap[1]=SpawnCollision.AlwaysSpawn
        var invalid:SpawnCollision
        unsafe { invalid=reinterpret<SpawnCollision>(99) }
        var enumRejected=false
        try { self.DasNativeArrayEnumMap[2]=invalid } recover { enumRejected=true }
        clear(self.DasNativeArrayByteMap)
        self.DasNativeArrayByteMap["mode"]=NativeMobility.Movable
        clear(self.DasNativeArrayPointMap)
        self.DasNativeArrayPointMap[1]=ue_struct_with_int(ue_struct_value("/Script/CoreUObject.IntPoint"),"X",12)
        var mixed <- clone(self.DasNativeArrayPointMap)
        mixed[2]=ue_struct_value("/Script/CoreUObject.Vector")
        var structRejected=false
        try { self.DasNativeArrayPointMap := mixed } recover { structRejected=true }
        clear(self.DasNativeArrayPawnMap)
        self.DasNativeArrayPawnMap[1]=self.DasNativeArrayPawns[0]
        self.DasNativeArrayPawnMap[2]=NativePawnRef_from(ue_null_object())
        var objectRejected=false
        try { self.DasNativeArrayPawnMap[1]=NativePawnRef(handle=ue_self_object()) } recover { objectRejected=true }
        return enumRejected && structRejected && objectRejected && length(self.DasNativeArrayEnumMap)==1 && self.DasNativeArrayEnumMap[1]==SpawnCollision.AlwaysSpawn && length(self.DasNativeArrayPointMap)==1 && ue_struct_get_int(self.DasNativeArrayPointMap[1],"X")==12 && self.DasNativeArrayVectorMap["origin"].x==10.0lf && ue_object_valid(NativePawnRef_raw(self.DasNativeArrayPawnMap[1]))
    }
    [ue_function(BlueprintCallable)]
    def Sets():bool {
        clear(self.DasNativeArrayIntSet)
        insert(self.DasNativeArrayIntSet,10)
        insert(self.DasNativeArrayIntSet,10)
        insert(self.DasNativeArrayIntSet,20)
        if(length(self.DasNativeArrayIntSet)!=2 || !erase(self.DasNativeArrayIntSet,20)) { return false }
        var copy <- clone(self.DasNativeArrayIntSet)
        insert(copy,30)
        if(key_exists(self.DasNativeArrayIntSet,30)) { return false }
        self.DasNativeArrayIntSet := copy
        clear(self.DasNativeArrayStringSet)
        insert(self.DasNativeArrayStringSet,"alpha")
        insert(self.DasNativeArrayStringSet,"alpha")
        insert(self.DasNativeArrayStringSet,"beta")
        erase(self.DasNativeArrayStringSet,"beta")
        return length(self.DasNativeArrayIntSet)==2 && key_exists(self.DasNativeArrayIntSet,10) && key_exists(self.DasNativeArrayIntSet,30) && length(self.DasNativeArrayStringSet)==1 && key_exists(self.DasNativeArrayStringSet,"alpha")
    }
    [ue_function(BlueprintCallable)]
    def NameKeys():bool {
        if(self.DasNativeArrayNameKeys[ue_name_value("pRESET_3")]!=321) { return false }
        clear(self.DasNativeArrayNameKeys)
        let unit=ue_name_value("Unit")
        self.DasNativeArrayNameKeys[unit]=1
        self.DasNativeArrayNameKeys[ue_name_value("uNIT")]+=1
        self.DasNativeArrayNameKeys[ue_name_value("Unit_0")]=10
        self.DasNativeArrayNameKeys[ue_name_value("UNIT_1")]=20
        self.DasNativeArrayNameKeys[ue_name_value("Unit_01")]=30
        self.DasNativeArrayNameKeys[ue_name_value("None")]=40
        self.DasNativeArrayNameKeys[ue_name_value("")]=41
        self.DasNativeArrayNameKeys[ue_name_value("東京_3")]=50
        if(length(self.DasNativeArrayNameKeys)!=6 || self.DasNativeArrayNameKeys[unit]!=2) { return false }
        var copy <- clone(self.DasNativeArrayNameKeys)
        copy[ue_name_value("UNIT")]=9
        if(self.DasNativeArrayNameKeys[unit]!=2) { return false }
        self.DasNativeArrayNameKeys := copy
        if(!erase(self.DasNativeArrayNameKeys,ue_name_value("uNit_0"))) { return false }
        clear(self.DasNativeArrayNameSet)
        insert(self.DasNativeArrayNameSet,unit)
        insert(self.DasNativeArrayNameSet,ue_name_value("UNIT"))
        insert(self.DasNativeArrayNameSet,ue_name_value("Unit_0"))
        insert(self.DasNativeArrayNameSet,ue_name_value("Unit_1"))
        insert(self.DasNativeArrayNameSet,ue_name_value("Unit_01"))
        insert(self.DasNativeArrayNameSet,ue_name_value("None"))
        insert(self.DasNativeArrayNameSet,ue_name_value(""))
        insert(self.DasNativeArrayNameSet,ue_name_value("東京_3"))
        if(length(self.DasNativeArrayNameSet)!=6 || !erase(self.DasNativeArrayNameSet,ue_name_value("UNIT_0"))) { return false }
        var names <- clone(self.DasNativeArrayNameSet)
        insert(names,ue_name_value("Extra"))
        if(key_exists(self.DasNativeArrayNameSet,ue_name_value("Extra"))) { return false }
        self.DasNativeArrayNameSet := names
        return length(self.DasNativeArrayNameKeys)==5 && self.DasNativeArrayNameKeys[ue_name_value("unit")]==9 && self.DasNativeArrayNameKeys[ue_name_value("UNIT_1")]==20 && self.DasNativeArrayNameKeys[ue_name_value("Unit_01")]==30 && self.DasNativeArrayNameKeys[ue_name_value("none")]==41 && length(self.DasNativeArrayNameSet)==6 && key_exists(self.DasNativeArrayNameSet,ue_name_value("unit")) && key_exists(self.DasNativeArrayNameSet,ue_name_value("EXTRA"))
    }
    [ue_function(BlueprintCallable)]
    def NameValues():bool {
        let key=ue_name_value("Value_1")
        clear(self.DasNativeArrayNameNames)
        self.DasNativeArrayNameNames[key]=ue_name_value("Result_2")
        clear(self.DasNativeArrayNameVectors)
        self.DasNativeArrayNameVectors[key]=ue_vector(1.0lf,2.0lf,3.0lf)
        self.DasNativeArrayNameVectors[ue_name_value("value_1")].x+=9.0lf
        clear(self.DasNativeArrayNamePoints)
        self.DasNativeArrayNamePoints[key]=ue_struct_with_int(ue_struct_value("/Script/CoreUObject.IntPoint"),"X",12)
        var bad <- clone(self.DasNativeArrayNamePoints)
        bad[ue_name_value("Bad")]=ue_struct_value("/Script/CoreUObject.Vector")
        var structRejected=false
        try { self.DasNativeArrayNamePoints := bad } recover { structRejected=true }
        clear(self.DasNativeArrayNameEnums)
        self.DasNativeArrayNameEnums[key]=SpawnCollision.AlwaysSpawn
        var invalid:SpawnCollision
        unsafe { invalid=reinterpret<SpawnCollision>(99) }
        var enumRejected=false
        try { self.DasNativeArrayNameEnums[key]=invalid } recover { enumRejected=true }
        clear(self.DasNativeArrayNameBytes)
        self.DasNativeArrayNameBytes[key]=NativeMobility.Movable
        clear(self.DasNativeArrayNameObjects)
        self.DasNativeArrayNameObjects[key]=self.DasNativeArrayPawns[0]
        self.DasNativeArrayNameObjects[ue_name_value("Null")]=NativePawnRef_from(ue_null_object())
        var objectRejected=false
        try { self.DasNativeArrayNameObjects[key]=NativePawnRef(handle=ue_self_object()) } recover { objectRejected=true }
        return structRejected && enumRejected && objectRejected && ue_names_equal(self.DasNativeArrayNameNames[ue_name_value("VALUE_1")],ue_name_value("result_2")) && self.DasNativeArrayNameVectors[key].x==10.0lf && length(self.DasNativeArrayNamePoints)==1 && ue_struct_get_int(self.DasNativeArrayNamePoints[key],"X")==12 && self.DasNativeArrayNameEnums[key]==SpawnCollision.AlwaysSpawn && self.DasNativeArrayNameBytes[key]==NativeMobility.Movable && ue_object_valid(NativePawnRef_raw(self.DasNativeArrayNameObjects[key]))
    }
}
