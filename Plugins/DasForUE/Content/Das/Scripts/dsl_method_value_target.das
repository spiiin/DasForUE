options gen2
options gc
require ue_dsl
require ue_method_values
require ue_object_types
require METHOD_REFS_MODULE
def multi_counted_index(var counter:int&):int { counter += 1; return 0 }
[ue_class(module_id="methodvalues", type_registry="REGISTRY", Blueprintable),
 ue_implements(interface="/Script/DasRuntime.DasEventInterface")]
class ValueTarget {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Position:UEVector=ue_vector(1.0lf,2.0lf,3.0lf)
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    NativeActor:ActorRef
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Partner:ValueTargetRef
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedPartner:ValueTargetRef
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    NativeEvents:DasEventInterface
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedNativeActor:ActorRef
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Settings:MethodSettings=MethodSettings()
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Envelope:MethodEnvelope=MethodEnvelope()
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    State:MethodState=MethodState.Active
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedState:MethodState=MethodState.Idle
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedSettings:MethodSettings=MethodSettings()
    [ue_function(BlueprintCallable)]
    def ChangeNestedMathFields():int {
        self.Settings.Point.Position.x += 4.0lf
        self.Settings.Point.Position.y = 8.0lf
        self.Settings.Orientation.yaw *= 2.0lf
        return (self.Settings.Point.Position.x==5.0lf && self.Settings.Point.Position.y==8.0lf && self.Settings.Orientation.yaw==40.0lf && self.Settings.Total==3 && self.Settings.Point.Count==2 && ue_transform_scale(self.Settings.Pose).x==1.0lf) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def ReadNestedMathFields():int {
        return (self.Settings.Point.Position.x==5.0lf && self.Settings.Point.Position.y==8.0lf && self.Settings.Orientation.yaw==40.0lf && self.Settings.Total==3 && self.Settings.Point.Count==2 && ue_transform_scale(self.Settings.Pose).x==1.0lf && self.LockedSettings.Point.Position.x==1.0lf) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def SetSelfEnum(value:MethodState):int {
        self.Settings.Point.Mode=value
        return (self.Settings.Point.Mode==value && self.Settings.Point.Count==2 && self.Settings.Point.Position.x==5.0lf) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def SetSelfReferences(owner:ActorRef; events:DasEventInterface):int {
        self.Settings.Point.Owner=owner
        self.Settings.Point.Events=events
        return (ue_object_is_a(self.Settings.Point.Owner.handle,"Actor")==ue_object_is_a(owner.handle,"Actor") && DasEventInterface_valid(self.Settings.Point.Events)==DasEventInterface_valid(events) && self.Settings.Point.Count==2 && self.Settings.Point.Position.x==5.0lf) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def ReplaceSelfValues(point:MethodPoint; settings:MethodSettings; vector:UEVector; rotation:UERotator; transform:UETransform):int {
        self.Settings.Point=point
        self.Settings.Point.Position=vector
        self.Settings.Orientation=rotation
        self.Settings.Pose=transform
        self.Envelope.Payload := settings
        return (self.Settings.Point.Count==point.Count && self.Settings.Point.Mode==point.Mode && self.Settings.Point.Position.x==vector.x && self.Settings.Orientation.yaw==rotation.yaw && ue_transform_scale(self.Settings.Pose).x==ue_transform_scale(transform).x && self.Settings.Total==3 && self.Envelope.Sentinel==91 && length(self.Envelope.Payload.Items)==length(settings.Items)) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def ReplaceSelfContainers(items:array<MethodPoint>; weights:table<string;double>; tags:table<string>):int {
        self.Settings.Items := items
        self.Settings.Weights := weights
        self.Settings.Tags := tags
        return (length(self.Settings.Items)==length(items) && length(self.Settings.Weights)==length(weights) && length(self.Settings.Tags)==length(tags) && self.Settings.Point.Count==2 && self.Settings.Total==3) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def ReplaceLocalSamples():int {
        var values:array<double>
        push(values,1.5lf)
        push(values,2.5lf)
        self.Settings.Samples := values
        values[0]=99.0lf
        var huge:array<double>
        for (index in range(0,4097)) { push(huge,0.0lf) }
        var rejected=false
        try { self.Settings.Samples := huge } recover { rejected=true }
        return (rejected && length(self.Settings.Samples)==2 && self.Settings.Samples[0]==1.5lf && self.Settings.Samples[1]==2.5lf && self.Settings.Total==3) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def SetSelfPeer(value:ValueTargetRef):void { self.Partner=value }
    [ue_function(BlueprintCallable)]
    def MutateSelfContainers():int {
        insert(self.Settings.Tags,"operation")
        insert(self.Settings.Weights,"operation",8.5lf)
        let removed=erase(self.Settings.Tags,"operation")
        let missing=erase(self.Settings.Tags,"operation")
        let removedMap=erase(self.Settings.Weights,"operation")
        erase(self.Settings.Samples,0)
        if (!removed || missing || !removedMap || length(self.Settings.Samples)!=1 || self.Settings.Samples[0]!=2.5lf) { return 0 }
        clear(self.Settings.Samples)
        clear(self.Settings.Tags)
        clear(self.Settings.Weights)
        return (length(self.Settings.Samples)==0 && length(self.Settings.Tags)==0 && length(self.Settings.Weights)==0 && self.Settings.Total==3) ? 1 : 0
    }
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectSamples:array<double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectPoints:array<MethodPoint>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectWeights:table<string;double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectTags:table<string>
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedDirect:array<double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Groups:array<MethodGroup>
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedGroups:array<MethodGroup>
    [ue_function(BlueprintCallable)]
    def MultiIndexContainers():int {
        push_clone(self.Groups,MethodGroup())
        push(self.Groups[0].Scores,2.0lf)
        push(self.Groups[0l].Scores,4.0lf)
        self.Groups[0].Scores[1l] += 3.0lf
        self.Groups[0l].Weights["multi"]=8.0lf
        insert(self.Groups[0].Tags,"multi")
        let removed=erase(self.Groups[0l].Tags,"multi")
        push_clone(self.Groups[0].Points,MethodPoint())
        self.Groups[0l].Points[0].Count=42
        self.Groups[0].Points[0l].Mode=MethodState.Idle
        self.Groups[0].Points[0].Position.x += 4.0lf
        var badOuter=false
        try { self.Groups[99].Scores[0]=9.0lf } recover { badOuter=true }
        var badInner=false
        try { self.Groups[0].Scores[99]=9.0lf } recover { badInner=true }
        var badPack=false
        try { self.Groups[0].Points[0].Position.x *= 1.0e308lf } recover { badPack=true }
        if (!removed || !badOuter || !badInner || !badPack || self.Groups[0].Scores[0]!=2.0lf || self.Groups[0].Scores[1]!=7.0lf || self.Groups[0].Weights["multi"]!=8.0lf || self.Groups[0].Points[0].Count!=42 || self.Groups[0].Points[0].Mode!=MethodState.Idle || self.Groups[0].Points[0].Position.x!=5.0lf || self.Groups[0].Sentinel!=65) { return 0 }
        var count=0
        self.Groups[multi_counted_index(count)].Scores[multi_counted_index(count)] += 1.0lf
        if (count!=2 || self.Groups[0].Scores[0]!=3.0lf) { return 0 }
        self.Groups[0].Scores[0]=2.0lf
        var replacement:array<double>
        push(replacement,11.0lf)
        self.Groups[0].Scores := replacement
        pop(self.Groups[0].Scores)
        clear(self.Groups[0].Points)
        clear(self.Groups[0].Weights)
        return (length(self.Groups[0].Scores)==0 && length(self.Groups[0].Points)==0 && length(self.Groups[0].Weights)==0 && self.Groups[0].Sentinel==65) ? 1 : 0
    }
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectVectors:array<UEVector>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectRotators:array<UERotator>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectTransforms:array<UETransform>
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedVectors:array<UEVector>
    [ue_function(BlueprintCallable)]
    def CopyVectors(values:array<UEVector>):array<UEVector> { return <- clone(values) }
    [ue_function(BlueprintCallable)]
    def CopyRotators(values:array<UERotator>):array<UERotator> { return <- clone(values) }
    [ue_function(BlueprintCallable)]
    def CopyTransforms(values:array<UETransform>):array<UETransform> { return <- clone(values) }
    [ue_function(BlueprintCallable)]
    def RefVectors(var values:array<UEVector>&; @ue_out var previous:array<UEVector>&):void {
        previous <- clone(values)
        values[0].x += 3.0lf
        push(values,ue_vector(7.0lf,8.0lf,9.0lf))
    }
    [ue_function(BlueprintCallable)]
    def RefRotators(var values:array<UERotator>&; @ue_out var previous:array<UERotator>&):void {
        previous <- clone(values)
        values[0].yaw += 10.0lf
    }
    [ue_function(BlueprintCallable)]
    def RefTransforms(var values:array<UETransform>&; @ue_out var previous:array<UETransform>&):void {
        previous <- clone(values)
        values[0]=ue_transform(ue_vector(8.0lf,9.0lf,10.0lf),ue_rotator(0.0lf,0.0lf,0.0lf),ue_vector(2.0lf,2.0lf,2.0lf))
    }
    [ue_function(BlueprintCallable)]
    def MathArrays():int {
        push(self.DirectVectors,ue_vector(1.0lf,2.0lf,3.0lf))
        push(self.DirectRotators,ue_rotator(10.0lf,20.0lf,30.0lf))
        push(self.DirectTransforms,ue_transform_identity())
        self.DirectVectors[0].x += 4.0lf
        self.DirectRotators[0l].yaw += 5.0lf
        self.DirectTransforms[0]=ue_transform(ue_vector(4.0lf,5.0lf,6.0lf),ue_rotator(0.0lf,0.0lf,0.0lf),ue_vector(2.0lf,2.0lf,2.0lf))
        push_clone(self.Groups,MethodGroup())
        push(self.Groups[0].Path,ue_vector(1.0lf,2.0lf,3.0lf))
        self.Groups[0].Path[0l].y += 5.0lf
        var rejected=false
        try { self.DirectVectors[0].x *= 1.0e308lf } recover { rejected=true }
        return (rejected && self.DirectVectors[0].x==5.0lf && self.DirectRotators[0].yaw==25.0lf && ue_transform_scale(self.DirectTransforms[0]).x==2.0lf && self.Groups[0].Path[0].y==7.0lf) ? 1 : 0
    }
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectStates:array<MethodState>
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedStates:array<MethodState>
    [ue_function(BlueprintCallable)]
    def CopyStates(values:array<MethodState>):array<MethodState> { return <- clone(values) }
    [ue_function(BlueprintCallable)]
    def RefStates(var values:array<MethodState>&; @ue_out var previous:array<MethodState>&):void {
        previous <- clone(values)
        values[0]=MethodState.Idle
        push(values,MethodState.Active)
    }
    [ue_function(BlueprintCallable)]
    def FailStates(var values:array<MethodState>&):void {
        values[0]=MethodState.Idle
        panic("enum array failure")
    }
    [ue_function(BlueprintCallable)]
    def EnumArrays():int {
        push(self.DirectStates,MethodState.Active)
        push(self.DirectStates,MethodState.Idle)
        self.DirectStates[1l]=MethodState.Active
        push_clone(self.Groups,MethodGroup())
        push(self.Groups[0].States,MethodState.Active)
        self.Groups[0l].States[0]=MethodState.Idle
        var invalid:MethodState
        unsafe { invalid = reinterpret<MethodState>(99) }
        var rejected=false
        try { self.DirectStates[0]=invalid } recover { rejected=true }
        return (rejected && self.DirectStates[0]==MethodState.Active && self.DirectStates[1]==MethodState.Active && self.Groups[0].States[0]==MethodState.Idle) ? 1 : 0
    }
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectActors:array<ActorRef>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectEventRefs:array<DasEventInterface>
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedActors:array<ActorRef>
    [ue_function(BlueprintCallable)]
    def CopyActors(values:array<ActorRef>):array<ActorRef> { return <- clone(values) }
    [ue_function(BlueprintCallable)]
    def CopyEventRefs(values:array<DasEventInterface>):array<DasEventInterface> { return <- clone(values) }
    [ue_function(BlueprintCallable)]
    def RefActors(var values:array<ActorRef>&; @ue_out var previous:array<ActorRef>&):void {
        previous <- clone(values)
        values[0]=default<ActorRef>
        push(values,default<ActorRef>)
    }
    [ue_function(BlueprintCallable)]
    def RefEventRefs(var values:array<DasEventInterface>&; @ue_out var previous:array<DasEventInterface>&):void {
        previous <- clone(values)
        values[0]=default<DasEventInterface>
    }
    [ue_function(BlueprintCallable)]
    def FailActors(var values:array<ActorRef>&):void {
        values[0]=default<ActorRef>
        panic("reference array failure")
    }
    [ue_function(BlueprintCallable)]
    def ReferenceArrays(actor:ActorRef; events:DasEventInterface):int {
        push(self.DirectActors,actor)
        push(self.DirectActors,default<ActorRef>)
        self.DirectActors[1l]=actor
        push(self.DirectEventRefs,events)
        push_clone(self.Groups,MethodGroup())
        push(self.Groups[0].Actors,actor)
        push(self.Groups[0].EventRefs,events)
        self.Groups[0].Actors[0l]=default<ActorRef>
        return (ue_object_valid(self.DirectActors[0].handle) && ue_object_valid(self.DirectActors[1].handle) && ue_object_valid(self.DirectEventRefs[0].handle) && !ue_object_valid(self.Groups[0].Actors[0].handle) && ue_object_valid(self.Groups[0].EventRefs[0].handle)) ? 1 : 0
    }
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    DirectPartners:array<ValueTargetRef>
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedPartners:array<ValueTargetRef>
    [ue_function(BlueprintCallable)]
    def CopyPartners(values:array<ValueTargetRef>):array<ValueTargetRef> { return <- clone(values) }
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    AlternativePartners:array<AlternativeTargetRef>
    [ue_function(BlueprintCallable)]
    def RefPartners(var values:array<ValueTargetRef>&; @ue_out var previous:array<ValueTargetRef>&; fail:bool):void {
        previous <- clone(values)
        values[0]=default<ValueTargetRef>
        if (fail) { panic("authored reference array failure") }
        push(values,default<ValueTargetRef>)
    }
    [ue_function(BlueprintCallable)]
    def AuthoredReferenceArrays(value:ValueTargetRef):int {
        push(self.DirectPartners,value)
        push(self.DirectPartners,default<ValueTargetRef>)
        self.DirectPartners[1l]=value
        return (ue_object_valid(self.DirectPartners[0].handle) && ue_object_valid(self.DirectPartners[1].handle)) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def DirectContainers():int {
        if (length(self.DirectSamples)!=0 || length(self.DirectPoints)!=0 || length(self.DirectWeights)!=0 || length(self.DirectTags)!=0) { return 0 }
        push(self.DirectSamples,2.0lf)
        self.DirectSamples[0l] += 3.0lf
        push_clone(self.DirectPoints,MethodPoint())
        self.DirectPoints[0].Count=31
        self.DirectPoints[0].Mode=MethodState.Idle
        self.DirectWeights["direct"]=7.0lf
        insert(self.DirectTags,"direct")
        var invalid=false
        try { self.DirectSamples[99]=1.0lf } recover { invalid=true }
        return (invalid && self.DirectSamples[0]==5.0lf && self.DirectPoints[0].Count==31 && self.DirectWeights["direct"]==7.0lf && key_exists(self.DirectTags,"direct") && length(self.LockedDirect)==0) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def IndexedContainers():int {
        push(self.Settings.Samples,2.0lf)
        push(self.Settings.Samples,4.0lf,0)
        self.Settings.Samples[0]=5.0lf
        self.Settings.Samples[1l] += 3.0lf
        self.Settings.Weights["indexed"]=7.0lf
        self.Settings.Weights["indexed"] *= 2.0lf
        push_clone(self.Settings.Items,MethodPoint())
        self.Settings.Items[0].Count=17
        self.Settings.Items[0].Mode=MethodState.Idle
        self.Settings.Items[0].Position.x += 4.0lf
        var rejected=false
        try { self.Settings.Samples[99]=9.0lf } recover { rejected=true }
        if (!rejected || length(self.Settings.Samples)!=2 || self.Settings.Samples[0]!=5.0lf || self.Settings.Samples[1]!=5.0lf || self.Settings.Weights["indexed"]!=14.0lf || self.Settings.Items[0].Count!=17 || self.Settings.Items[0].Mode!=MethodState.Idle || self.Settings.Items[0].Position.x!=5.0lf) { return 0 }
        var packingRejected=false
        try { self.Settings.Items[0].Position.x *= 1.0e308lf } recover { packingRejected=true }
        if (!packingRejected || self.Settings.Items[0].Position.x!=5.0lf || self.Settings.Weights["indexed"]!=14.0lf) { return 0 }
        insert_clone(self.Settings.Weights,"clone",3.0lf)
        push(self.Settings.Samples,6.0lf,1l)
        erase(self.Settings.Samples,1l,1l)
        pop(self.Settings.Samples)
        return (length(self.Settings.Samples)==1 && self.Settings.Samples[0]==5.0lf && self.Settings.Total==3) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def FailNestedMath():void {
        self.Settings.Point.Position.x *= 1.0e308lf
    }
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Orientation:UERotator=ue_rotator(10.0lf,20.0lf,30.0lf)
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Pose:UETransform=ue_transform_identity()
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedPosition:UEVector
    [ue_function(BlueprintCallable)]
    def ReadMathFields():int {
        return (self.Position.x==1.0lf && self.Position.y==2.0lf && self.Orientation.yaw==20.0lf && ue_transform_scale(self.Pose).x==1.0lf && self.LockedPosition.x==0.0lf) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def ChangeMathFields():UEVector {
        self.Position.x += 4.0lf
        self.Position.y = 8.0lf
        self.Orientation.yaw += 5.0lf
        self.Pose=ue_transform(ue_vector(8.0lf,9.0lf,10.0lf),ue_rotator(0.0lf,0.0lf,0.0lf),ue_vector(2.0lf,2.0lf,2.0lf))
        return self.Position
    }

    [ue_function(BlueprintCallable)]
    def Notify(Amount:int):void {}
    [ue_function(BlueprintCallable)]
    def ResetCounter():void {}
    [ue_function(BlueprintCallable)]
    def EchoState(value:MethodState):MethodState { return value }
    [ue_function(BlueprintCallable)]
    def EchoSettings(value:MethodSettings):MethodSettings { return <- clone(value) }
    [ue_function(BlueprintCallable)]
    def ChangeState(var value:MethodState&; @ue_out var previous:MethodState&):void {
        previous=value
        value=MethodState.Idle
    }
    [ue_function(BlueprintCallable)]
    def ChangeSettings(var value:MethodSettings&; @ue_out var previous:MethodSettings&):void {
        previous <- clone(value)
        value.Point.Count += 5
        value.Point.Mode=MethodState.Idle
        if (length(value.Items)>0) { value.Items[0].Mode=MethodState.Idle }
        value.Point.Note += "!"
        value.Total += 7
        value.Point.Position.x += 4.0lf
        value.Orientation.yaw += 5.0lf
        value.Pose=ue_transform(ue_vector(8.0lf,9.0lf,10.0lf),ue_rotator(0.0lf,0.0lf,0.0lf),ue_vector(2.0lf,2.0lf,2.0lf))
        if (length(value.Items)>0) { value.Items[0].Count += 10 }
        value.Weights["a"] += 1.0lf
        insert(value.Tags,"blue")
    }
    [ue_function(BlueprintCallable)]
    def FailSettings(var value:MethodSettings&):void { value.Total=999; panic("natural method failure") }
    [ue_function(BlueprintCallable)]
    def CopyInts(values:array<int>):array<int> { return <- clone(values) }
    [ue_function(BlueprintCallable)]
    def ChangeInts(var values:array<int>&; @ue_out var previous:array<int>&):void {
        previous <- clone(values)
        push(values,9)
    }
    [ue_function(BlueprintCallable)]
    def ChangeMap(var values:table<string; double>&; @ue_out var previous:table<string; double>&):void {
        previous <- clone(values)
        values["a"] += 1.0lf
        values["b"] = 4.5lf
    }
    [ue_function(BlueprintCallable)]
    def CopyMap(values:table<string; double>):table<string; double> { return <- clone(values) }
    [ue_function(BlueprintCallable)]
    def ChangeSet(var values:table<string>&; @ue_out var previous:table<string>&):void {
        previous <- clone(values)
        erase(values,"red")
        insert(values,"blue")
    }
    [ue_function(BlueprintCallable)]
    def CopySet(values:table<string>):table<string> { return <- clone(values) }
    [ue_function(BlueprintCallable)]
    def CopyPoints(values:array<MethodPoint>):array<MethodPoint> { return <- clone(values) }
    [ue_function(BlueprintCallable)]
    def ChangePoints(var values:array<MethodPoint>&; @ue_out var previous:array<MethodPoint>&):void {
        previous <- clone(values)
        if (length(values)>0) { values[0].Count += 2 }
        push_clone(values,MethodPoint())
    }
    [ue_function(BlueprintCallable)]
    def FailInts(var values:array<int>&):void { push(values,999); panic("container method failure") }
    [ue_function(BlueprintCallable)]
    def EchoActor(value:ActorRef):ActorRef { return value }
    [ue_function(BlueprintCallable)]
    def RefActor(var value:ActorRef&; @ue_out var previous:ActorRef&):void {
        previous=value
        value=default<ActorRef>
    }
    [ue_function(BlueprintCallable)]
    def EchoEvents(value:DasEventInterface):DasEventInterface { return value }
    [ue_function(BlueprintCallable)]
    def RefEvents(var value:DasEventInterface&; @ue_out var previous:DasEventInterface&):void {
        previous=value
        value=default<DasEventInterface>
    }
    [ue_function(BlueprintCallable)]
    def EchoVector(value:UEVector):UEVector { return value }
    [ue_function(BlueprintCallable)]
    def EchoRotator(value:UERotator):UERotator { return value }
    [ue_function(BlueprintCallable)]
    def EchoTransform(value:UETransform):UETransform { return value }
    [ue_function(BlueprintCallable)]
    def RefVector(var value:UEVector&; @ue_out var previous:UEVector&):void {
        previous=value
        value=ue_vector(5.0lf,6.0lf,7.0lf)
    }
    [ue_function(BlueprintCallable)]
    def RefRotator(var value:UERotator&; @ue_out var previous:UERotator&):void {
        previous=value
        value=ue_rotator(10.0lf,20.0lf,30.0lf)
    }
    [ue_function(BlueprintCallable)]
    def RefTransform(var value:UETransform&; @ue_out var previous:UETransform&):void {
        previous=value
        value=ue_transform_identity()
    }
    [ue_function(BlueprintCallable)]
    def FailVector(var value:UEVector&):void { value=ue_vector(99.0lf,0.0lf,0.0lf); panic("math method failure") }
    [ue_function(BlueprintCallable)]
    def EchoAuthored(value:ValueTargetRef):ValueTargetRef { return value }
    [ue_function(BlueprintCallable)]
    def RefAuthored(var value:ValueTargetRef&; @ue_out var previous:ValueTargetRef&):void {
        previous=value
        value=default<ValueTargetRef>
    }
}
