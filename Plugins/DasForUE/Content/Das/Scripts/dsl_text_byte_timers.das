options gen2
require ue_callback_value_signals
[ue_class(Blueprintable,type_folder="/Game/DasCallbackObjects/Types")]
class TextByteTimerActor : Actor {
    properties=ue_properties(BlueprintReadWrite) {
        var NativeObjectEvents:int=0
        var ObjectNext:int=0
        var ReferenceNext:int=0
        var ObjectRuns:int=0
        var ReferenceRuns:int=0
        var ReferenceEvents:int=0
        var ObjectEvents:int=0
        var CancelledObjectRuns:int=0
        var Errors:int=0
        var Unexpected:int=0
        var ByteCalls:int=0
        var ByteSum:int=0
        var ByteRuns:int=0
        var TextRuns:int=0
        var BytesRuns:int=0
        var TextsRuns:int=0
        var BytesNext:int=0
        var TextsNext:int=0
        var PausedCalls:int=0
        var InvariantCalls:int=0
        var InvalidRejected:bool=false
        var FloatRuns:int=0
        var BoolRuns:int=0
        var NameRuns:int=0
        var ArrayNext:int=0
        var StateRuns:int=0
        var RecordRuns:int=0
        var ValueNext:int=0
        var ValueEvents:int=0
        var EventCopyOK:bool=false
        var Ready:bool=false
        var LastText:FText=ue_text_invariant("unset")
        var Replacement:FText=ue_text_invariant("unset")
    }
    @ue_property=(BlueprintAssignable)
    StateChanged:CallbackStateSignal
    @ue_property=(BlueprintAssignable)
    RecordChanged:CallbackRecordSignal
    @ue_property=(BlueprintAssignable)
    NativeStateChanged:CallbackNativeStateSignal
    @ue_property=(BlueprintAssignable)
    NativeRecordChanged:CallbackNativeRecordSignal
    @ue_property=(BlueprintAssignable)
    ReferenceChanged:CallbackReferenceSignal
    @ue_property=(BlueprintAssignable)
    ObjectChanged:CallbackObjectSignal
    [ue_function(BlueprintCallable)]
    def ArmObjects(value:CallbackObjectRef):void {
        // This dynamically spawned fixture tests object captures; the placed
        // fixture separately owns the pause/resume scenario.
        ue_timer_cancel("paused")
        ue_timer_start("object-repeat",0.01f,"ReceiveObject",true,value)
        ue_timer_next_tick("object-next","ReceiveObjectNext",value)
        ue_timer_start("object-cancel",1000.0f,"CancelledObject",false,value)
        ue_timer_cancel("object-cancel")
        var record=CallbackReferenceRecord()
        record.Target=value
        push(record.Targets,value)
        record.Lookup["saved"]=value
        ue_timer_start("reference-repeat",0.01f,"ReceiveReferences",true,record)
        ue_timer_next_tick("reference-next","ReceiveReferencesNext",record)
        self.ReferenceChanged.Broadcast(record)
        self.ObjectChanged.Broadcast(value)
        clear(record.Targets); clear(record.Lookup)
    }
    [ue_function(BlueprintCallable)]
    def CancelledObject(value:CallbackObjectRef):void { self.CancelledObjectRuns+=1 }
    [ue_function(BlueprintCallable)]
    def ReceiveObject(value:CallbackObjectRef):void {
        if(!ue_object_valid(value.handle)) { self.Errors+=1 }
        self.ObjectRuns+=1
        if(self.ObjectRuns>=3) { ue_timer_cancel("object-repeat") }
    }
    [ue_function(BlueprintCallable)]
    def ReceiveObjectNext(value:CallbackObjectRef):void {
        if(!ue_object_valid(value.handle)) { self.Errors+=1 };self.ObjectNext+=1
    }
    [ue_function(BlueprintCallable)]
    def ReceiveReferencesNext(var value:CallbackReferenceRecord):void {
        self.CheckReferences(value);clear(value.Targets);clear(value.Lookup);self.ReferenceNext+=1
    }
    def CheckReferences(var value:CallbackReferenceRecord):void {
        if(!ue_object_valid(value.Target.handle) || length(value.Targets)!=1 || length(value.Lookup)!=1) { self.Errors+=1 }
        if(length(value.Targets)==1 && !ue_object_valid(value.Targets[0].handle)) { self.Errors+=1 }
        if(!ue_object_valid(value.Lookup["saved"].handle)) { self.Errors+=1 }
    }
    [ue_function(BlueprintCallable)]
    def ReceiveReferences(var value:CallbackReferenceRecord):void {
        self.CheckReferences(value); clear(value.Targets); clear(value.Lookup)
        self.ReferenceRuns+=1
        if(self.ReferenceRuns>=3) { ue_timer_cancel("reference-repeat") }
    }
    [ue_event(name="ReferenceChanged")]
    def OnReferences(var value:CallbackReferenceRecord):void {
        self.CheckReferences(value); clear(value.Targets); clear(value.Lookup)
        self.ReferenceEvents+=1
    }
    [ue_event(name="OnDasCallbackActor")]
    def OnNativeActor(value:CallbackActorRef):void {
        if(!ue_object_valid(value.handle)) { self.Errors+=1 };self.NativeObjectEvents+=1
    }
    [ue_event(name="ObjectChanged")]
    def OnObject(value:CallbackObjectRef):void {
        if(!ue_object_valid(value.handle)) { self.Errors+=1 };self.ObjectEvents+=1
    }
    def BeginPlay():void {
        var state=CallbackState.Active
        var record = CallbackRecord()
        record.Score=17; record.Mode=CallbackState.Active
        record.Title=ue_text("CallbackValue","Record","Saved")
        push(record.Values,5); push(record.Values,9)
        ue_timer_start("state",0.002f,"RepeatState",true,state)
        ue_timer_start("record",0.002f,"RepeatRecord",true,record)
        ue_timer_next_tick("state-next","NextState",state)
        ue_timer_next_tick("record-next","NextRecord",record)
        ue_timer_next_tick("native-state","NextNativeState",CallbackNativeState.Active)
        ue_timer_next_tick("native-record","NextNativeRecord",CallbackNativeRecord(X=1.25lf,Y=2.0lf,Z=3.0lf,W=4.0lf))
        state=CallbackState.Idle; record.Score=99; clear(record.Values)
        record.Title=ue_text_invariant("Changed")
        ue_timer_next_tick("emit-values","EmitValues")
        let original=ue_text("TimerCapture","Source","Captured text")
        var text=original
        ue_timer_next_tick("zero","ObserveByte",uint8(0))
        ue_timer_next_tick("max","ObserveByte",uint8(255))
        ue_timer_next_tick("text","ObserveText",text)
        ue_timer_next_tick("invariant","ObserveInvariant",ue_text_invariant("Invariant"))
        ue_timer_start("byte-repeat",0.002f,"RepeatByte",true,uint8(255))
        ue_timer_start("text-repeat",0.002f,"RepeatText",true,text)
        var bytes:array<uint8> = [uint8(0),uint8(255)]
        var texts:array<FText> = [text,ue_text_invariant("Invariant")]
        ue_timer_start("bytes-repeat",0.002f,"RepeatBytes",true,bytes)
        ue_timer_start("texts-repeat",0.002f,"RepeatTexts",true,texts)
        ue_timer_next_tick("bytes-next","NextBytes",bytes)
        ue_timer_next_tick("texts-next","NextTexts",texts)
        var floats:array<float> = [1.25f,-2.5f]
        var flags:array<bool> = [true,false]
        var names:array<UENameValue> = [ue_name_value("First"),ue_name_value("Second")]
        ue_timer_start("float-array",0.002f,"RepeatFloats",true,floats)
        ue_timer_start("bool-array",0.002f,"RepeatBools",true,flags)
        ue_timer_start("name-array",0.002f,"RepeatNames",true,names)
        ue_timer_next_tick("float-next","NextFloats",floats)
        ue_timer_next_tick("bool-next","NextBools",flags)
        ue_timer_next_tick("name-next","NextNames",names)
        clear(floats); clear(flags); clear(names)
        // Original locals and callback-owned arrays cannot change saved arguments.
        bytes[0]=uint8(99); push(bytes,uint8(17))
        texts[0]=ue_text_invariant("Changed"); clear(texts)
        text=ue_text_invariant("Reassigned")
        ue_timer_next_tick("replace","MustNotRun",text)
        ue_timer_next_tick("replace","ObserveReplacement",ue_text("TimerCapture","Replacement","New"))
        try { ue_timer_next_tick("replace","ObserveByte",text) } recover { self.InvalidRejected=true }
        ue_timer_next_tick("cancel","MustNotRun",text)
        ue_timer_cancel("cancel")
        ue_timer_start("paused",0.002f,"ObservePaused",false,uint8(7))
        ue_timer_pause("paused")
        ue_timer_next_tick("resume","Resume")
    }
    def UpdateReady():void {
        self.Ready=self.StateRuns==3 && self.RecordRuns==3 && self.ValueNext==4 && self.ValueEvents>=4 && self.EventCopyOK && self.FloatRuns==3 && self.BoolRuns==3 && self.NameRuns==3 && self.ArrayNext==3 && self.ByteCalls==2 && self.ByteRuns==3 && self.TextRuns==3 && self.BytesRuns==3 && self.TextsRuns==3 && self.BytesNext==1 && self.TextsNext==1 && self.PausedCalls==1 && self.InvariantCalls==1 && ue_text_key(self.LastText)=="Source" && ue_text_key(self.Replacement)=="Replacement"
    }
    [ue_function(BlueprintCallable)]
    def ObserveByte(value:uint8):void { self.ByteCalls+=1; self.ByteSum+=int(value); self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def ObserveText(value:FText):void { self.LastText=value; if(ue_text_key(value)!="Source") { self.Errors+=1 }; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def ObserveInvariant(value:FText):void { if(!ue_text_is_invariant(value) || ue_text_to_string(value)!="Invariant") { self.Errors+=1 }; self.InvariantCalls+=1; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def RepeatByte(value:uint8):void { if(value!=uint8(255)) { self.Errors+=1 }; self.ByteRuns+=1; if(self.ByteRuns==3) { ue_timer_cancel("byte-repeat") }; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def RepeatText(value:FText):void { if(ue_text_namespace(value)!="TimerCapture" || ue_text_key(value)!="Source") { self.Errors+=1 }; self.TextRuns+=1; if(self.TextRuns==3) { ue_timer_cancel("text-repeat") }; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def RepeatBytes(var values:array<uint8>):void { self.CheckBytes(values); clear(values); self.BytesRuns+=1; if(self.BytesRuns==3) { ue_timer_cancel("bytes-repeat") }; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def RepeatTexts(var values:array<FText>):void { self.CheckTexts(values); clear(values); self.TextsRuns+=1; if(self.TextsRuns==3) { ue_timer_cancel("texts-repeat") }; self.UpdateReady() }
    def CheckBytes(values:array<uint8>):void { if(length(values)!=2 || values[0]!=uint8(0) || values[1]!=uint8(255)) { self.Errors+=1 } }
    def CheckTexts(values:array<FText>):void { if(length(values)!=2 || ue_text_key(values[0])!="Source" || ue_text_namespace(values[0])!="TimerCapture" || !ue_text_is_invariant(values[1])) { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def NextBytes(values:array<uint8>):void { self.CheckBytes(values); self.BytesNext+=1; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextTexts(values:array<FText>):void { self.CheckTexts(values); self.TextsNext+=1; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def ObserveReplacement(value:FText):void { self.Replacement=value; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def MustNotRun(value:FText):void { self.Unexpected+=1 }
    [ue_function(BlueprintCallable)]
    def Resume():void { ue_timer_resume("paused") }
    [ue_function(BlueprintCallable)]
    def ObservePaused(value:uint8):void { if(value!=uint8(7)) { self.Errors+=1 }; self.PausedCalls+=1; self.UpdateReady() }
    def CheckFloats(values:array<float>):void { if(length(values)!=2 || values[0]!=1.25f || values[1]!=-2.5f) { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatFloats(var values:array<float>):void { self.CheckFloats(values); clear(values); self.FloatRuns+=1; if(self.FloatRuns==3) { ue_timer_cancel("float-array") }; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextFloats(values:array<float>):void { self.CheckFloats(values); self.ArrayNext+=1; self.UpdateReady() }
    def CheckBools(values:array<bool>):void { if(length(values)!=2 || !values[0] || values[1]) { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatBools(var values:array<bool>):void { self.CheckBools(values); clear(values); self.BoolRuns+=1; if(self.BoolRuns==3) { ue_timer_cancel("bool-array") }; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextBools(values:array<bool>):void { self.CheckBools(values); self.ArrayNext+=1; self.UpdateReady() }
    def CheckNames(values:array<UENameValue>):void { if(length(values)!=2 || ue_name_text(values[0])!="First" || ue_name_text(values[1])!="Second") { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatNames(var values:array<UENameValue>):void { self.CheckNames(values); clear(values); self.NameRuns+=1; if(self.NameRuns==3) { ue_timer_cancel("name-array") }; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextNames(values:array<UENameValue>):void { self.CheckNames(values); self.ArrayNext+=1; self.UpdateReady() }
    def CheckRecord(value:CallbackRecord):void {
        if(value.Score!=17 || value.Mode!=CallbackState.Active || ue_text_namespace(value.Title)!="CallbackValue" || ue_text_key(value.Title)!="Record" || length(value.Values)!=2 || value.Values[0]!=5 || value.Values[1]!=9) { self.Errors+=1 }
    }
    [ue_function(BlueprintCallable)]
    def RepeatState(value:CallbackState):void { if(value!=CallbackState.Active) { self.Errors+=1 }; self.StateRuns+=1; if(self.StateRuns==3) { ue_timer_cancel("state") }; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def RepeatRecord(var value:CallbackRecord):void { self.CheckRecord(value); clear(value.Values); self.RecordRuns+=1; if(self.RecordRuns==3) { ue_timer_cancel("record") }; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextState(value:CallbackState):void { if(value!=CallbackState.Active) { self.Errors+=1 }; self.ValueNext+=1; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextRecord(value:CallbackRecord):void { self.CheckRecord(value); self.ValueNext+=1; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextNativeState(value:CallbackNativeState):void { if(value!=CallbackNativeState.Active) { self.Errors+=1 }; self.ValueNext+=1; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextNativeRecord(value:CallbackNativeRecord):void { if(value.X!=1.25lf) { self.Errors+=1 }; self.ValueNext+=1; self.UpdateReady() }
    [ue_event(name="StateChanged")]
    def OnState(value:CallbackState):void { if(value!=CallbackState.Active) { self.Errors+=1 }; self.ValueEvents+=1; self.UpdateReady() }
    [ue_event(name="RecordChanged")]
    def OnRecord(var value:CallbackRecord):void { self.CheckRecord(value); clear(value.Values); self.ValueEvents+=1; self.UpdateReady() }
    [ue_event(name="OnDasCallbackRecordRef")]
    def OnRecordRef(value:CallbackNativeRecord):void { if(value.X!=1.25lf) { self.Errors+=1 }; self.ValueEvents+=1; self.UpdateReady() }
    [ue_event(name="OnDasCallbackEnumRef")]
    def OnStateRef(value:CallbackNativeState):void { if(value!=CallbackNativeState.Active) { self.Errors+=1 }; self.ValueEvents+=1; self.UpdateReady() }
    [ue_event(name="NativeStateChanged")]
    def OnNativeState(value:CallbackNativeState):void { if(value!=CallbackNativeState.Active) { self.Errors+=1 }; self.ValueEvents+=1; self.UpdateReady() }
    [ue_event(name="NativeRecordChanged")]
    def OnNativeRecord(value:CallbackNativeRecord):void { if(value.X!=1.25lf) { self.Errors+=1 }; self.ValueEvents+=1; self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def EmitValues():void {
        var record = CallbackRecord()
        record.Score=17; record.Mode=CallbackState.Active
        record.Title=ue_text("CallbackValue","Record","Saved")
        push(record.Values,5); push(record.Values,9)
        self.StateChanged.Broadcast(CallbackState.Active)
        self.RecordChanged.Broadcast(record)
        self.NativeStateChanged.Broadcast(CallbackNativeState.Active)
        self.NativeRecordChanged.Broadcast(CallbackNativeRecord(X=1.25lf,Y=2.0lf,Z=3.0lf,W=4.0lf))
        self.EventCopyOK=length(record.Values)==2 && record.Score==17
        clear(record.Values); record.Score=99
        self.UpdateReady()
    }
    [ue_function(BlueprintCallable)]
    def MustStateNotRun(value:CallbackState):void { self.Unexpected+=1 }
    [ue_function(BlueprintCallable)]
    def MustRecordNotRun(value:CallbackRecord):void { self.Unexpected+=1 }
    [ue_function(BlueprintCallable)]
    def ArmPending():void {
        ue_timer_start("pending-state",1000.0f,"MustStateNotRun",false,CallbackState.Active)
        var pending = CallbackRecord()
        ue_timer_start("pending-record",1000.0f,"MustRecordNotRun",false,pending)
        ue_timer_start("pending-text",1000.0f,"MustNotRun",false,ue_text_invariant("Pending"))
        var values:array<FText> = [ue_text_invariant("Pending array")]
        ue_timer_start("pending-array",1000.0f,"RepeatTexts",true,values)
        ue_timer_next_tick("pending-byte","ObserveByte",uint8(255))
    }
}
