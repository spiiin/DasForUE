options gen2
options gc
require ue_dsl
require ue_method_values
require ue_object_types
require METHOD_REFS_MODULE
def exercise_values(api:ValueTargetRef):int {
    let state=api.EchoState(MethodState.Active)
    var input=MethodSettings()
    input.Point.Count=11
    input.Point.Note="hello 世界"
    input.Total=20
    push_clone(input.Items,MethodPoint())
    input.Weights["a"]=2.5lf
    insert(input.Tags,"red")
    var result=api.EchoSettings(input)
    var old=MethodSettings()
    api.ChangeSettings(input,old)
    if (!(result.Point.Position.x==1.0lf && result.Items[0].Position.z==3.0lf && result.Orientation.yaw==20.0lf && ue_transform_scale(result.Pose).x==1.0lf && input.Point.Position.x==5.0lf && input.Orientation.yaw==25.0lf && ue_transform_translation(input.Pose).z==10.0lf && ue_transform_scale(input.Pose).x==2.0lf && old.Point.Position.x==1.0lf && old.Orientation.yaw==20.0lf && ue_transform_scale(old.Pose).x==1.0lf)) { return -19 }
    if (!(result.Point.Mode==MethodState.Active && result.Items[0].Mode==MethodState.Active && input.Point.Mode==MethodState.Idle && input.Items[0].Mode==MethodState.Idle && old.Point.Mode==MethodState.Active && old.Items[0].Mode==MethodState.Active)) { return -30 }
    var updated=state
    var previous=MethodState.Idle
    api.ChangeState(updated,previous)
    try { api.FailSettings(input) } recover {}
    var ints:array<int>
    push(ints,1)
    push(ints,2)
    let copied=api.CopyInts(ints)
    var oldInts:array<int>
    push(oldInts,1000)
    api.ChangeInts(ints,oldInts)
    try { api.FailInts(ints) } recover {}
    var empty:array<int>
    let emptyResult=api.CopyInts(empty)
    var weights:table<string; double>
    weights["a"]=2.5lf
    var oldWeights:table<string; double>
    weights["unused"]=0.0lf
    erase(weights,"unused")
    api.ChangeMap(weights,oldWeights)
    var copiedMap=api.CopyMap(weights)
    var tags:table<string>
    insert(tags,"red")
    var oldTags:table<string>
    api.ChangeSet(tags,oldTags)
    let copiedSet=api.CopySet(tags)
    var points:array<MethodPoint>
    push_clone(points,MethodPoint())
    let copiedPoints=api.CopyPoints(points)
    var oldPoints:array<MethodPoint>
    api.ChangePoints(points,oldPoints)
    if (!(length(copied)==2 && copied[0]==1 && length(ints)==3 && ints[2]==9 && length(oldInts)==2 && length(emptyResult)==0)) { return -1 }
    if (!(weights["a"]==3.5lf && weights["b"]==4.5lf && oldWeights["a"]==2.5lf && length(oldWeights)==1 && copiedMap["b"]==4.5lf)) { return -2 }
    if (!(key_exists(tags,"blue") && !key_exists(tags,"red") && key_exists(oldTags,"red") && key_exists(copiedSet,"blue"))) { return -3 }
    if (!(copiedPoints[0].Count==2 && length(points)==2 && points[0].Count==4 && oldPoints[0].Count==2)) { return -4 }
    if (!(length(result.Items)==1 && result.Items[0].Count==2 && result.Weights["a"]==2.5lf && !key_exists(result.Tags,"blue"))) { return -5 }
    if (!(input.Items[0].Count==12 && input.Weights["a"]==3.5lf && key_exists(input.Tags,"blue") && old.Items[0].Count==2 && old.Weights["a"]==2.5lf)) { return -6 }
    if (!(result.Point.Count==11 && result.Point.Note=="hello 世界" && result.Total==20 && input.Point.Count==16 && input.Point.Note=="hello 世界!" && input.Total==27 && old.Point.Count==11 && old.Point.Note=="hello 世界" && old.Total==20 && updated==MethodState.Idle && previous==MethodState.Active)) { return -7 }
    var actor=ue_ref(type<ActorRef>,ue_self_object())
    let returnedActor=api.EchoActor(actor)
    var previousActor:ActorRef
    api.RefActor(actor,previousActor)
    if (!ue_object_is_a(returnedActor.handle,"Actor") || ue_object_is_a(actor.handle,"Actor") || !ue_object_is_a(previousActor.handle,"Actor")) { return -8 }
    if (ue_object_is_a(api.NativeActor.handle,"Actor") || DasEventInterface_valid(api.NativeEvents) || ue_object_is_a(api.LockedNativeActor.handle,"Actor")) { return -26 }
    api.NativeActor=returnedActor
    api.NativeEvents=DasEventInterface_from(api.handle)
    if (!ue_object_is_a(api.NativeActor.handle,"Actor") || !DasEventInterface_valid(api.NativeEvents)) { return -27 }
    var wrongFieldGet=false
    try { __ue_authored_get_object(api.handle,"methodvalues.ValueTarget","NativeActor","/Script/CoreUObject.Object") } recover { wrongFieldGet=true }
    var wrongFieldSet=false
    try { __ue_authored_set_object(api.handle,"methodvalues.ValueTarget","NativeActor","/Script/CoreUObject.Object",default<UObjectHandle>) } recover { wrongFieldSet=true }
    if (!wrongFieldGet || !wrongFieldSet || !ue_object_is_a(api.NativeActor.handle,"Actor")) { return -28 }
    api.NativeActor=default<ActorRef>
    api.NativeEvents=default<DasEventInterface>
    if (ue_object_is_a(api.NativeActor.handle,"Actor") || DasEventInterface_valid(api.NativeEvents)) { return -29 }
    let nullActor=api.EchoActor(default<ActorRef>)
    if (ue_object_is_a(nullActor.handle,"Actor")) { return -9 }
    var events=DasEventInterface_from(api.handle)
    let returnedEvents=api.EchoEvents(events)
    var previousEvents:DasEventInterface
    api.RefEvents(events,previousEvents)
    if (!DasEventInterface_valid(returnedEvents) || DasEventInterface_valid(events) || !DasEventInterface_valid(previousEvents)) { return -10 }
    var vector=api.EchoVector(ue_vector(1.0lf,2.0lf,3.0lf))
    var previousVector=ue_vector(0.0lf,0.0lf,0.0lf)
    api.RefVector(vector,previousVector)
    try { api.FailVector(vector) } recover {}
    var rotation=api.EchoRotator(ue_rotator(1.0lf,2.0lf,3.0lf))
    var previousRotation=ue_rotator(0.0lf,0.0lf,0.0lf)
    api.RefRotator(rotation,previousRotation)
    var transform=api.EchoTransform(ue_transform(ue_vector(8.0lf,9.0lf,10.0lf),ue_rotator(0.0lf,0.0lf,0.0lf),ue_vector(2.0lf,2.0lf,2.0lf)))
    var previousTransform=ue_transform_identity()
    api.RefTransform(transform,previousTransform)
    if (!(vector.x==5.0lf && vector.y==6.0lf && vector.z==7.0lf && previousVector.x==1.0lf && previousVector.z==3.0lf)) { return -11 }
    if (!(rotation.pitch==10.0lf && rotation.yaw==20.0lf && rotation.roll==30.0lf && previousRotation.yaw==2.0lf)) { return -12 }
    if (!(ue_transform_translation(previousTransform).x==8.0lf && ue_transform_scale(previousTransform).x==2.0lf && ue_transform_translation(transform).x==0.0lf && ue_transform_scale(transform).x==1.0lf)) { return -13 }
    if (api.ChangeNestedMathFields()!=1 || api.ReadNestedMathFields()!=1) { return -20 }
    var rejectedNested=false
    try { api.FailNestedMath() } recover { rejectedNested=true }
    if (!rejectedNested || api.ReadNestedMathFields()!=1) { return -21 }
    if (api.SetSelfEnum(MethodState.Idle)!=1 || api.Settings.Point.Mode!=MethodState.Idle || api.SetSelfEnum(MethodState.Active)!=1 || api.Settings.Point.Mode!=MethodState.Active) { return -33 }
    if (api.SetSelfReferences(returnedActor,DasEventInterface_from(api.handle))!=1 || !ue_object_is_a(api.Settings.Point.Owner.handle,"Actor") || !DasEventInterface_valid(api.Settings.Point.Events)) { return -34 }
    if (api.SetSelfReferences(default<ActorRef>,default<DasEventInterface>)!=1 || ue_object_is_a(api.Settings.Point.Owner.handle,"Actor") || DasEventInterface_valid(api.Settings.Point.Events)) { return -35 }
    var replacement=MethodPoint()
    replacement.Note="whole member"
    if (api.ReplaceSelfValues(replacement,input,ue_vector(5.0lf,8.0lf,3.0lf),ue_rotator(10.0lf,40.0lf,30.0lf),ue_transform_identity())!=1) { return -36 }
    var envelope=api.Envelope
    if (!(api.Settings.Point.Note=="whole member" && api.Settings.Point.Position.y==8.0lf && envelope.Sentinel==91 && envelope.Payload.Items[0].Count==input.Items[0].Count && envelope.Payload.Weights["a"]==input.Weights["a"] && key_exists(envelope.Payload.Tags,"blue") && envelope.Payload.Point.Mode==input.Point.Mode)) { return -37 }
    envelope.Payload.Items[0].Count=999
    if (api.Envelope.Payload.Items[0].Count==999) { return -38 }
    if (api.ReplaceSelfContainers(points,weights,tags)!=1) { return -39 }
    var containerCopy=api.Settings
    if (!(containerCopy.Items[0].Count==points[0].Count && containerCopy.Weights["b"]==weights["b"] && key_exists(containerCopy.Tags,"blue"))) { return -40 }
    containerCopy.Items[0].Count=999
    containerCopy.Weights["b"]=999.0lf
    erase(containerCopy.Tags,"blue")
    var containerReadback=api.Settings
    if (containerReadback.Items[0].Count==999 || containerReadback.Weights["b"]==999.0lf || !key_exists(containerReadback.Tags,"blue") || points[0].Count==999) { return -41 }
    var emptyPoints:array<MethodPoint>
    var emptyWeights:table<string;double>
    var emptyTags:table<string>
    if (api.ReplaceSelfContainers(emptyPoints,emptyWeights,emptyTags)!=1) { return -42 }
    if (api.ReplaceLocalSamples()!=1) { return -43 }
    if (api.MutateSelfContainers()!=1) { return -48 }
    if (api.MultiIndexContainers()!=1) { return -54 }
    clear(api.Groups)
    if (api.MathArrays()!=1) { return -55 }
    var vectors=api.CopyVectors(api.DirectVectors)
    var oldVectors:array<UEVector>
    api.RefVectors(vectors,oldVectors)
    var rotators=api.CopyRotators(api.DirectRotators)
    var oldRotators:array<UERotator>
    api.RefRotators(rotators,oldRotators)
    var transforms=api.CopyTransforms(api.DirectTransforms)
    var oldTransforms:array<UETransform>
    api.RefTransforms(transforms,oldTransforms)
    if (vectors[0].x!=8.0lf || oldVectors[0].x!=5.0lf || vectors[1].z!=9.0lf || api.DirectVectors[0].x!=5.0lf || rotators[0].yaw!=35.0lf || oldRotators[0].yaw!=25.0lf || ue_transform_translation(transforms[0]).z!=10.0lf || ue_transform_translation(oldTransforms[0]).z!=6.0lf) { return -56 }
    api.DirectVectors := vectors
    api.DirectRotators[0].yaw += 2.0lf
    api.DirectTransforms[0]=transforms[0]
    api.Groups[0].Path[0].z=12.0lf
    if (api.DirectVectors[0].x!=8.0lf || api.DirectRotators[0].yaw!=27.0lf || ue_transform_translation(api.DirectTransforms[0]).z!=10.0lf || api.Groups[0].Path[0].z!=12.0lf) { return -57 }
    clear(api.DirectVectors)
    clear(api.DirectRotators)
    clear(api.DirectTransforms)
    clear(api.Groups)
    if (api.EnumArrays()!=1) { return -58 }
    var states=api.CopyStates(api.DirectStates)
    var oldStates:array<MethodState>
    api.RefStates(states,oldStates)
    if (states[0]!=MethodState.Idle || length(states)!=3 || oldStates[0]!=MethodState.Active || length(oldStates)!=2 || api.DirectStates[0]!=MethodState.Active) { return -59 }
    api.DirectStates := states
    api.DirectStates[1]=MethodState.Idle
    api.Groups[0].States[0]=MethodState.Active
    var rejected=false
    try { api.FailStates(oldStates) } recover { rejected=true }
    if (!rejected || oldStates[0]!=MethodState.Active || api.DirectStates[1]!=MethodState.Idle || api.Groups[0].States[0]!=MethodState.Active) { return -60 }
    erase(api.DirectStates,0l)
    clear(api.DirectStates)
    clear(api.Groups)
    if (api.DirectContainers()!=1) { return -51 }
    api.DirectPoints[0].Count += 4
    push(api.DirectSamples,9.0lf)
    insert_clone(api.DirectWeights,"foreign",8.0lf)
    let directRemoved=erase(api.DirectTags,"direct")
    var directCopy=api.DirectSamples
    directCopy[0]=100.0lf
    if (!directRemoved || api.DirectPoints[0].Count!=35 || api.DirectSamples[0]!=5.0lf || api.DirectSamples[1]!=9.0lf || api.DirectWeights["foreign"]!=8.0lf) { return -52 }
    api.DirectSamples := directCopy
    if (api.DirectSamples[0]!=100.0lf) { return -53 }
    clear(api.DirectSamples)
    clear(api.DirectPoints)
    clear(api.DirectWeights)
    clear(api.DirectTags)
    var beforeIndexed=api.Settings
    if (api.IndexedContainers()!=1) { return -49 }
    api.Settings=beforeIndexed
    push(api.Settings.Samples,2.0lf)
    api.Settings.Samples[0] += 3.0lf
    api.Settings.Weights["foreign"]=7.5lf
    let foreignRemoved=erase(api.Settings.Weights,"foreign")
    insert(api.Settings.Tags,"foreign")
    push_clone(api.Settings.Items,MethodPoint())
    api.Settings.Items[0].Count=29
    api.Settings.Items[0].Position.x += 2.0lf
    var foreignRejected=false
    try { api.Settings.Samples[99]=4.0lf } recover { foreignRejected=true }
    var foreignPackRejected=false
    try { api.Settings.Items[0].Position.x *= 1.0e308lf } recover { foreignPackRejected=true }
    if (!foreignRemoved || !foreignRejected || !foreignPackRejected || api.Settings.Samples[0]!=5.0lf || api.Settings.Items[0].Count!=29 || api.Settings.Items[0].Position.x!=3.0lf || !key_exists(api.Settings.Tags,"foreign")) { return -50 }
    clear(api.Settings.Tags)
    pop(api.Settings.Samples)
    api.Settings=beforeIndexed
    if (ue_object_valid(api.Partner.handle) || ue_object_valid(api.LockedPartner.handle)) { return -44 }
    api.Partner=api
    if (!ue_object_valid(api.Partner.handle) || api.Partner.EchoState(MethodState.Active)!=MethodState.Active) { return -45 }
    api.SetSelfPeer(default<ValueTargetRef>)
    if (ue_object_valid(api.Partner.handle)) { return -46 }
    api.SetSelfPeer(api)
    var forgedPeer=api
    forgedPeer.handle=ue_self_object()
    var wrongPeer=false
    try { api.Partner=forgedPeer } recover { wrongPeer=true }
    if (!wrongPeer || !ue_object_valid(api.Partner.handle)) { return -47 }
    api.Partner=default<ValueTargetRef>
    var propertyValue=api.Settings
    var propertyPrevious=api.Settings
    propertyValue.Point.Position.z=19.0lf
    propertyValue.Point.Mode=MethodState.Idle
    propertyValue.Point.Note="property 世界"
    push_clone(propertyValue.Items,MethodPoint())
    propertyValue.Weights["property"]=3.5lf
    insert(propertyValue.Tags,"property")
    api.Settings=propertyValue
    var propertyReadback=api.Settings
    if (!(propertyReadback.Point.Position.z==19.0lf && propertyReadback.Point.Note=="property 世界" && length(propertyReadback.Items)==1 && propertyReadback.Items[0].Position.x==1.0lf && propertyReadback.Weights["property"]==3.5lf && key_exists(propertyReadback.Tags,"property") && propertyPrevious.Point.Position.z==3.0lf && length(propertyPrevious.Items)==0 && length(propertyPrevious.Weights)==0)) { return -22 }
    if (propertyReadback.Point.Mode!=MethodState.Idle || propertyPrevious.Point.Mode!=MethodState.Active || propertyReadback.Items[0].Mode!=MethodState.Active) { return -31 }
    propertyReadback.Point.Count=100
    if (api.Settings.Point.Count!=2 || api.LockedSettings.Point.Position.x!=1.0lf || api.State!=MethodState.Active || api.LockedState!=MethodState.Idle) { return -23 }
    api.State=MethodState.Idle
    if (api.State!=MethodState.Idle) { return -24 }
    api.Settings=propertyPrevious
    var rejectedStructIdentity=false
    try { __ue_require_struct_type(ue_struct_value("/Script/CoreUObject.Vector"),"/Script/CoreUObject.Rotator") } recover { rejectedStructIdentity=true }
    var rejectedEnumIdentity=false
    try { __ue_checked_enum_number(__ue_authored_get_enum(api.handle,"methodvalues.ValueTarget","State"),"/Script/Engine.EAutoReceiveInput") } recover { rejectedEnumIdentity=true }
    if (!rejectedStructIdentity || !rejectedEnumIdentity) { return -25 }
    if (api.ReadMathFields()!=1) { return -16 }
    let changedPosition=api.ChangeMathFields()
    if (!(changedPosition.x==5.0lf && changedPosition.y==8.0lf && api.Position.x==5.0lf && api.Orientation.yaw==25.0lf && ue_transform_scale(api.Pose).x==2.0lf)) { return -17 }
    api.Position=ue_vector(10.0lf,11.0lf,12.0lf)
    api.Orientation=ue_rotator(15.0lf,16.0lf,17.0lf)
    api.Pose=ue_transform_identity()
    if (!(api.Position.z==12.0lf && api.Orientation.yaw==16.0lf && ue_transform_scale(api.Pose).x==1.0lf)) { return -18 }
    var authored=api.EchoAuthored(api)
    var previousAuthored:ValueTargetRef
    api.RefAuthored(authored,previousAuthored)
    let nullAuthored=api.EchoAuthored(default<ValueTargetRef>)
    if (ue_object_is_a(authored.handle,"Actor") || !ue_object_is_a(previousAuthored.handle,"Actor") || ue_object_is_a(nullAuthored.handle,"Actor")) { return -14 }
    try { api.EchoAuthored(ValueTargetRef(handle=ue_self_object())) } recover { return 1 }
    return -15
}
[ue_class(module_id="methodvalues", type_registry="REGISTRY", Blueprintable)]
class ValueDriver {
    [ue_function(BlueprintCallable)]
    def RejectNativeAuthoredField(target:UObjectHandle):int {
        let api=ue_ref(type<ValueTargetRef>,target)
        var rejected=false
        try { let invalid=api.Partner; ue_object_valid(invalid.handle) } recover { rejected=true }
        api.Partner=default<ValueTargetRef>
        return (rejected && !ue_object_valid(api.Partner.handle)) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def ExpiredNativeFields(target:UObjectHandle):int {
        let api=ue_ref(type<ValueTargetRef>,target)
        let expired=api.NativeActor
        let expiredNested=api.Settings.Point.Owner
        if (api.SetSelfReferences(default<ActorRef>,default<DasEventInterface>)!=1) { return 0 }
        api.NativeActor=default<ActorRef>
        return (!ue_object_is_a(expired.handle,"Actor") && !ue_object_is_a(expiredNested.handle,"Actor") && !ue_object_is_a(api.Settings.Point.Owner.handle,"Actor") && !ue_object_is_a(api.NativeActor.handle,"Actor")) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def Exercise(target:UObjectHandle):int { return exercise_values(ue_ref(type<ValueTargetRef>,target)) }
}
