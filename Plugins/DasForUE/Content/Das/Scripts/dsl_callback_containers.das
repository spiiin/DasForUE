options gen2
require ue_callback_container_signals
[ue_class(Blueprintable,type_folder="/Game/DasCallbackObjects/Types")]
class ContainerCallbackActor : Actor {
    properties=ue_properties(BlueprintReadWrite) {
        var PreservedRuns:int=0
        var RejectedReplacements:int=0
        var ShapesNext:int=0
        var ShapesEvents:int=0
        var MixedRuns:int=0
        var MixedNext:int=0
        var MixedEvents:int=0
        var MixedUnexpected:int=0
        var NativeDamageCalls:int=0
        var RefMixedRuns:int=0
        var RefMixedNext:int=0
        var Errors:int=0
        var Next:int=0
        var Events:int=0
        var Ready:bool=false
        var RefArrays:int=0
        var RefMaps:int=0
        var RefNext:int=0
        var RefEvents:int=0
        var NativeEvents:int=0
        var StatesRuns:int=0
        var NativeStatesRuns:int=0
        var RecordsRuns:int=0
        var MathRuns:int=0
        var RecordMapRuns:int=0
        var TextsRuns:int=0
        var StatesSetRuns:int=0
        var IntsRuns:int=0
    }
    @ue_property=(BlueprintAssignable)
    StatesChanged:CallbackStatesSignal
    @ue_property=(BlueprintAssignable)
    RecordsChanged:CallbackRecordsSignal
    @ue_property=(BlueprintAssignable)
    MathChanged:CallbackMathSignal
    @ue_property=(BlueprintAssignable)
    RecordMapChanged:CallbackRecordMapSignal
    @ue_property=(BlueprintAssignable)
    TextsChanged:CallbackTextsSignal
    @ue_property=(BlueprintAssignable)
    StatesSetChanged:CallbackStatesSetSignal
    @ue_property=(BlueprintAssignable)
    IntsChanged:CallbackIntsSignal
    @ue_property=(BlueprintAssignable)
    ObjectsChanged:CallbackObjectsSignal
    @ue_property=(BlueprintAssignable)
    ObjectMapChanged:CallbackObjectMapSignal
    @ue_property=(BlueprintAssignable)
    NativeStatesChanged:CallbackNativeStatesSignal
    @ue_property=(BlueprintAssignable)
    MixedChanged:CallbackMixedSignal
    @ue_property=(BlueprintAssignable)
    ShapesChanged:CallbackShapesSignal
    def BeginPlay():void {
        var shapeTexts:table<int;FText>;shapeTexts[7]=ue_text("MultiCallback","Shape","Saved")
        var shapeStates:table<CallbackState> ={CallbackState.Active,CallbackState.Idle}
        ue_timer_next_tick("shapes","NextShapes",ue_vector(1.25lf,2.0lf,3.0lf),ue_name_value("Mixed"),shapeTexts,shapeStates)
        clear(shapeTexts);clear(shapeStates)

        var mixedRecord=CallbackRecord();mixedRecord.Score=17;push(mixedRecord.Values,5)
        var mixedValues:array<int> = [3,7]
        let mixedText=ue_text("MultiCallback","Saved","Snapshot")
        ue_timer_start("mixed-repeat",0.002f,"RepeatMixed",true,17,mixedText,uint8(255),CallbackState.Active,mixedRecord,mixedValues)
        ue_timer_next_tick("mixed-next","NextMixed",17,mixedText,uint8(255),CallbackState.Active,mixedRecord,mixedValues)
        ue_timer_start("mixed-cancel",1000.0f,"CancelledMixed",false,17,mixedText)
        ue_timer_cancel("mixed-cancel")
        ue_timer_start("mixed-preserved",0.002f,"PreservedMixed",false,17,mixedText)
        try { ue_timer_start("mixed-preserved",0.0f,"CancelledMixed",false,17,mixedText) } recover { self.RejectedReplacements+=1 }

        mixedRecord.Score=99;clear(mixedRecord.Values);clear(mixedValues)

        var nativeStates:array<CallbackNativeState> = [CallbackNativeState.Active,CallbackNativeState.Paused]
        ue_timer_start("NativeStates",0.002f,"RepeatNativeStates",true,nativeStates)
        ue_timer_next_tick("NativeStates-next","NextNativeStates",nativeStates)
        clear(nativeStates)

        var states:array<CallbackState> =[CallbackState.Active,CallbackState.Idle]
        var record=CallbackRecord();record.Score=17;record.Mode=CallbackState.Active
        record.Title=ue_text("ContainerCallback","Title","Saved");push(record.Values,5)
        var records:array<CallbackRecord>;push_clone(records,record)
        var math:array<CallbackNativeRecord>;push(math,CallbackNativeRecord(X=1.25lf,Y=2.0lf,Z=3.0lf,W=4.0lf))
        var recordMap:table<string;CallbackRecord>;recordMap["saved"]:=record
        var texts:table<int;FText>;texts[7]=ue_text("ContainerCallback","Text","Saved")
        var statesSet:table<CallbackState> ={CallbackState.Active,CallbackState.Idle}
        var ints:table<int> ={5,9}
        ue_timer_start("States",0.002f,"RepeatStates",true,states)
        ue_timer_next_tick("States-next","NextStates",states)
        clear(states)
        ue_timer_start("Records",0.002f,"RepeatRecords",true,records)
        ue_timer_next_tick("Records-next","NextRecords",records)
        clear(records)
        ue_timer_start("Math",0.002f,"RepeatMath",true,math)
        ue_timer_next_tick("Math-next","NextMath",math)
        clear(math)
        ue_timer_start("RecordMap",0.002f,"RepeatRecordMap",true,recordMap)
        ue_timer_next_tick("RecordMap-next","NextRecordMap",recordMap)
        clear(recordMap)
        ue_timer_start("Texts",0.002f,"RepeatTexts",true,texts)
        ue_timer_next_tick("Texts-next","NextTexts",texts)
        clear(texts)
        ue_timer_start("StatesSet",0.002f,"RepeatStatesSet",true,statesSet)
        ue_timer_next_tick("StatesSet-next","NextStatesSet",statesSet)
        clear(statesSet)
        ue_timer_start("Ints",0.002f,"RepeatInts",true,ints)
        ue_timer_next_tick("Ints-next","NextInts",ints)
        clear(ints)
    }
    def UpdateReady():void { self.Ready=self.PreservedRuns==1 && self.RejectedReplacements==1 && self.ShapesNext==1 && self.ShapesEvents==1 && self.MixedRuns==3 && self.MixedNext==1 && self.MixedEvents==1 && self.MixedUnexpected==0 && self.Next==8 && self.Events==8 && self.NativeStatesRuns==3 && self.StatesRuns==3 && self.RecordsRuns==3 && self.MathRuns==3 && self.RecordMapRuns==3 && self.TextsRuns==3 && self.StatesSetRuns==3 && self.IntsRuns==3 }
    def CheckNativeStates(var value:array<CallbackNativeState>):void { if(length(value)!=2 || value[0]!=CallbackNativeState.Active || value[1]!=CallbackNativeState.Paused) { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatNativeStates(var value:array<CallbackNativeState>):void { self.CheckNativeStates(value);clear(value);self.NativeStatesRuns+=1;if(self.NativeStatesRuns==3) { ue_timer_cancel("NativeStates") };self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextNativeStates(var value:array<CallbackNativeState>):void { self.CheckNativeStates(value);self.NativeStatesChanged.Broadcast(value);clear(value);self.Next+=1;self.UpdateReady() }
    [ue_event(name="NativeStatesChanged")]
    def OnNativeStates(var value:array<CallbackNativeState>):void { self.CheckNativeStates(value);clear(value);self.Events+=1;self.UpdateReady() }
    def CheckStates(var value:array<CallbackState>):void { if(length(value)!=2 || value[0]!=CallbackState.Active || value[1]!=CallbackState.Idle) { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatStates(var value:array<CallbackState>):void { self.CheckStates(value);clear(value);self.StatesRuns+=1;if(self.StatesRuns==3) { ue_timer_cancel("States") };self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextStates(var value:array<CallbackState>):void { self.CheckStates(value);self.StatesChanged.Broadcast(value);clear(value);self.Next+=1;self.UpdateReady() }
    [ue_event(name="StatesChanged")]
    def OnStates(var value:array<CallbackState>):void { self.CheckStates(value);clear(value);self.Events+=1;self.UpdateReady() }
    def CheckRecords(var value:array<CallbackRecord>):void { if(length(value)!=1 || value[0].Score!=17 || value[0].Mode!=CallbackState.Active || ue_text_key(value[0].Title)!="Title" || length(value[0].Values)!=1 || value[0].Values[0]!=5) { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatRecords(var value:array<CallbackRecord>):void { self.CheckRecords(value);clear(value);self.RecordsRuns+=1;if(self.RecordsRuns==3) { ue_timer_cancel("Records") };self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextRecords(var value:array<CallbackRecord>):void { self.CheckRecords(value);self.RecordsChanged.Broadcast(value);clear(value);self.Next+=1;self.UpdateReady() }
    [ue_event(name="RecordsChanged")]
    def OnRecords(var value:array<CallbackRecord>):void { self.CheckRecords(value);clear(value);self.Events+=1;self.UpdateReady() }
    def CheckMath(var value:array<CallbackNativeRecord>):void { if(length(value)!=1 || value[0].X!=1.25lf) { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatMath(var value:array<CallbackNativeRecord>):void { self.CheckMath(value);clear(value);self.MathRuns+=1;if(self.MathRuns==3) { ue_timer_cancel("Math") };self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextMath(var value:array<CallbackNativeRecord>):void { self.CheckMath(value);self.MathChanged.Broadcast(value);clear(value);self.Next+=1;self.UpdateReady() }
    [ue_event(name="MathChanged")]
    def OnMath(var value:array<CallbackNativeRecord>):void { self.CheckMath(value);clear(value);self.Events+=1;self.UpdateReady() }
    def CheckRecordMap(var value:table<string;CallbackRecord>):void { if(length(value)!=1 || value["saved"].Score!=17 || length(value["saved"].Values)!=1 || ue_text_key(value["saved"].Title)!="Title") { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatRecordMap(var value:table<string;CallbackRecord>):void { self.CheckRecordMap(value);clear(value);self.RecordMapRuns+=1;if(self.RecordMapRuns==3) { ue_timer_cancel("RecordMap") };self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextRecordMap(var value:table<string;CallbackRecord>):void { self.CheckRecordMap(value);self.RecordMapChanged.Broadcast(value);clear(value);self.Next+=1;self.UpdateReady() }
    [ue_event(name="RecordMapChanged")]
    def OnRecordMap(var value:table<string;CallbackRecord>):void { self.CheckRecordMap(value);clear(value);self.Events+=1;self.UpdateReady() }
    def CheckTexts(var value:table<int;FText>):void { if(length(value)!=1 || ue_text_namespace(value[7])!="ContainerCallback" || ue_text_key(value[7])!="Text") { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatTexts(var value:table<int;FText>):void { self.CheckTexts(value);clear(value);self.TextsRuns+=1;if(self.TextsRuns==3) { ue_timer_cancel("Texts") };self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextTexts(var value:table<int;FText>):void { self.CheckTexts(value);self.TextsChanged.Broadcast(value);clear(value);self.Next+=1;self.UpdateReady() }
    [ue_event(name="TextsChanged")]
    def OnTexts(var value:table<int;FText>):void { self.CheckTexts(value);clear(value);self.Events+=1;self.UpdateReady() }
    def CheckStatesSet(var value:table<CallbackState>):void { if(length(value)!=2 || !key_exists(value,CallbackState.Active) || !key_exists(value,CallbackState.Idle)) { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatStatesSet(var value:table<CallbackState>):void { self.CheckStatesSet(value);clear(value);self.StatesSetRuns+=1;if(self.StatesSetRuns==3) { ue_timer_cancel("StatesSet") };self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextStatesSet(var value:table<CallbackState>):void { self.CheckStatesSet(value);self.StatesSetChanged.Broadcast(value);clear(value);self.Next+=1;self.UpdateReady() }
    [ue_event(name="StatesSetChanged")]
    def OnStatesSet(var value:table<CallbackState>):void { self.CheckStatesSet(value);clear(value);self.Events+=1;self.UpdateReady() }
    def CheckInts(var value:table<int>):void { if(length(value)!=2 || !key_exists(value,5) || !key_exists(value,9)) { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatInts(var value:table<int>):void { self.CheckInts(value);clear(value);self.IntsRuns+=1;if(self.IntsRuns==3) { ue_timer_cancel("Ints") };self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def NextInts(var value:table<int>):void { self.CheckInts(value);self.IntsChanged.Broadcast(value);clear(value);self.Next+=1;self.UpdateReady() }
    [ue_event(name="IntsChanged")]
    def OnInts(var value:table<int>):void { self.CheckInts(value);clear(value);self.Events+=1;self.UpdateReady() }
    [ue_function(BlueprintCallable)]
    def ArmReferences(target:CallbackObjectRef):void {
        var mixedWeights:array<int> = [3,7]
        let mixedText=ue_text("MultiCallback","Reference","Saved")
        ue_timer_start("ref-mixed",0.01f,"RepeatReferenceMixed",true,target,mixedText,mixedWeights)
        ue_timer_next_tick("ref-mixed-next","NextReferenceMixed",target,mixedText,mixedWeights)
        clear(mixedWeights)

        var objects:array<CallbackObjectRef> =[target]
        var lookup:table<string;CallbackObjectRef>;lookup["saved"]=target
        ue_timer_start("ref-array",0.01f,"RepeatObjects",true,objects)
        ue_timer_next_tick("ref-array-next","NextObjects",objects)
        ue_timer_start("ref-map",0.01f,"RepeatObjectMap",true,lookup)
        ue_timer_next_tick("ref-map-next","NextObjectMap",lookup)
        self.ObjectsChanged.Broadcast(objects);self.ObjectMapChanged.Broadcast(lookup)
        clear(objects);clear(lookup)
    }
    def CheckObjects(value:array<CallbackObjectRef>):void { if(length(value)!=1 || !ue_object_valid(value[0].handle)) { self.Errors+=1 } }
    def CheckObjectMap(var value:table<string;CallbackObjectRef>):void { if(length(value)!=1 || !ue_object_valid(value["saved"].handle)) { self.Errors+=1 } }
    [ue_function(BlueprintCallable)]
    def RepeatObjects(var value:array<CallbackObjectRef>):void { self.CheckObjects(value);clear(value);self.RefArrays+=1;if(self.RefArrays==3) { ue_timer_cancel("ref-array") } }
    [ue_function(BlueprintCallable)]
    def NextObjects(var value:array<CallbackObjectRef>):void { self.CheckObjects(value);clear(value);self.RefNext+=1 }
    [ue_function(BlueprintCallable)]
    def RepeatObjectMap(var value:table<string;CallbackObjectRef>):void { self.CheckObjectMap(value);clear(value);self.RefMaps+=1;if(self.RefMaps==3) { ue_timer_cancel("ref-map") } }
    [ue_function(BlueprintCallable)]
    def NextObjectMap(var value:table<string;CallbackObjectRef>):void { self.CheckObjectMap(value);clear(value);self.RefNext+=1 }
    [ue_event(name="ObjectsChanged")]
    def OnObjects(var value:array<CallbackObjectRef>):void { self.CheckObjects(value);clear(value);self.RefEvents+=1 }
    [ue_event(name="ObjectMapChanged")]
    def OnObjectMap(var value:table<string;CallbackObjectRef>):void { self.CheckObjectMap(value);clear(value);self.RefEvents+=1 }
    [ue_event(name="OnDasCallbackActors")]
    def OnNativeActors(var value:array<CallbackActorRef>):void { if(length(value)!=1 || !ue_object_valid(value[0].handle)) { self.Errors+=1 };clear(value);self.NativeEvents+=1 }

    def CheckMixed(number:int;text:FText;flag:uint8;mode:CallbackState;var record:CallbackRecord;var values:array<int>):void {
        if(number!=17 || flag!=uint8(255) || mode!=CallbackState.Active || ue_text_key(text)!="Saved" || ue_text_namespace(text)!="MultiCallback" || record.Score!=17 || length(record.Values)!=1 || record.Values[0]!=5 || length(values)!=2 || values[0]!=3 || values[1]!=7) { self.Errors+=1 }
    }
    [ue_function(BlueprintCallable)]
    def RepeatMixed(number:int;text:FText;flag:uint8;mode:CallbackState;var record:CallbackRecord;var values:array<int>):void {
        self.CheckMixed(number,text,flag,mode,record,values);clear(record.Values);clear(values)
        self.MixedRuns+=1;if(self.MixedRuns==3) { ue_timer_cancel("mixed-repeat") };self.UpdateReady()
    }
    [ue_function(BlueprintCallable)]
    def NextMixed(number:int;text:FText;flag:uint8;mode:CallbackState;var record:CallbackRecord;var values:array<int>):void {
        self.CheckMixed(number,text,flag,mode,record,values);self.MixedChanged.Broadcast(number,text,flag,mode,record,values)
        clear(record.Values);clear(values);self.MixedNext+=1;self.UpdateReady()
    }
    [ue_event(name="MixedChanged")]
    def OnMixed(number:int;text:FText;flag:uint8;mode:CallbackState;var record:CallbackRecord;var values:array<int>):void {
        self.CheckMixed(number,text,flag,mode,record,values);clear(record.Values);clear(values);self.MixedEvents+=1;self.UpdateReady()
    }
    [ue_function(BlueprintCallable)]
    def PreservedMixed(number:int;text:FText):void {
        if(number!=17 || ue_text_key(text)!="Saved") { self.Errors+=1 }
        self.PreservedRuns+=1;self.UpdateReady()
    }
    [ue_function(BlueprintCallable)]
    def CancelledMixed(number:int;text:FText):void { self.MixedUnexpected+=1 }
    [ue_function(BlueprintCallable)]
    def RepeatReferenceMixed(target:CallbackObjectRef;text:FText;var weights:array<int>):void {
        if(!ue_object_valid(target.handle) || ue_text_key(text)!="Reference" || length(weights)!=2 || weights[0]!=3 || weights[1]!=7) { self.Errors+=1 }
        clear(weights);self.RefMixedRuns+=1;if(self.RefMixedRuns==3) { ue_timer_cancel("ref-mixed") }
    }
    [ue_function(BlueprintCallable)]
    def NextReferenceMixed(target:CallbackObjectRef;text:FText;var weights:array<int>):void {
        if(!ue_object_valid(target.handle) || ue_text_key(text)!="Reference" || length(weights)!=2 || weights[0]!=3 || weights[1]!=7) { self.Errors+=1 }
        clear(weights);self.RefMixedNext+=1
    }
    [ue_event(name="OnTakeAnyDamage")]
    def OnDamage(damaged:CallbackActorRef;amount:float;kind:CallbackObjectRef;instigator:CallbackObjectRef;source:CallbackActorRef):void {
        if(!ue_object_valid(damaged.handle) || amount!=12.5f || !ue_object_valid(kind.handle) || ue_object_valid(instigator.handle) || !ue_object_valid(source.handle)) { self.Errors+=1 }
        self.NativeDamageCalls+=1
    }
    def CheckShapes(position:UEVector;tag:UENameValue;var texts:table<int;FText>;var states:table<CallbackState>):void {
        if(position.x!=1.25lf || ue_name_text(tag)!="Mixed" || length(texts)!=1 || ue_text_key(texts[7])!="Shape" || length(states)!=2 || !key_exists(states,CallbackState.Active)) { self.Errors+=1 }
    }
    [ue_function(BlueprintCallable)]
    def NextShapes(position:UEVector;tag:UENameValue;var texts:table<int;FText>;var states:table<CallbackState>):void {
        self.CheckShapes(position,tag,texts,states);self.ShapesChanged.Broadcast(position,tag,texts,states)
        clear(texts);clear(states);self.ShapesNext+=1;self.UpdateReady()
    }
    [ue_event(name="ShapesChanged")]
    def OnShapes(position:UEVector;tag:UENameValue;var texts:table<int;FText>;var states:table<CallbackState>):void {
        self.CheckShapes(position,tag,texts,states);clear(texts);clear(states);self.ShapesEvents+=1;self.UpdateReady()
    }

}
