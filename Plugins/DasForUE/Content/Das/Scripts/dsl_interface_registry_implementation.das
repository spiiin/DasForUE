options gen2
options gc
require ue_dsl
require ue_interface_values
require ue_object_types
[ue_class(module_id="authoredinterface", type_registry="REGISTRY", Blueprintable),
 ue_implements(interface=CounterContract),
 ue_implements(interface="/Script/DasRuntime.DasEventInterface")]
class CounterActor {
    [ue_function(BlueprintCallable)]
    def FailRefVector(var value:UEVector&; @ue_out var previous:UEVector&):void { value = ue_vector(5.0lf,6.0lf,7.0lf); previous = value; panic("expected_ref_failure") }
    [ue_function(BlueprintCallable, arg_value_enum=EDasCallMode, arg_previous_enum=EDasCallMode)]
    def WrongRefEnum(var value:UEEnumValue&; @ue_out var previous:UEEnumValue&):void { value = ue_enum_value("EDasCallMode","Active"); previous = ue_enum_value("EDasLegacyMode","DasLegacyActive") }
    [ue_function(BlueprintCallable, arg_value_struct=DasCallPayload, arg_previous_struct=DasCallPayload)]
    def WrongRefStruct(var value:UEStructValue&; @ue_out var previous:UEStructValue&):void { previous = ue_struct_value("DasCallMetrics") }

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    HeldEvents : DasEventInterface
    [ue_function(BlueprintCallable)]
    def SetEvents(value:DasEventInterface):void { self.HeldEvents = value }
    [ue_function(BlueprintCallable)]
    def GetEvents():DasEventInterface { return self.HeldEvents }
    [ue_function(BlueprintCallable)]
    def ClearEvents():void { self.HeldEvents = default<DasEventInterface> }

    [ue_function(BlueprintCallable)]
    def EchoState(value:InterfaceState):InterfaceState { return value }
    [ue_function(BlueprintCallable)]
    def NewDetails():array<InterfaceDetails> {
        var result : array<InterfaceDetails>
        var item = InterfaceDetails()
        item.Amount = 77
        item.Text = "New 世界"
        push_clone(result,item)
        return <- result
    }
    [ue_function(BlueprintCallable)]
    def CopyDetails(Values:array<InterfaceDetails>; @ue_out var Copies:array<InterfaceDetails>&):void { Copies <- clone(Values) }
    [ue_function(BlueprintCallable)]
    def BumpDetails(var Values:array<InterfaceDetails>&):void { if (length(Values)>0) { Values[0].Amount += 3 } }
    [ue_function(BlueprintCallable)]
    def RefActor(var value:ActorRef&; @ue_out var previous:ActorRef&):void {previous = value; value = default<ActorRef>}
    [ue_function(BlueprintCallable)]
    def RefEvents(var value:DasEventInterface&; @ue_out var previous:DasEventInterface&):void {previous = value; value = default<DasEventInterface>}
    [ue_function(BlueprintCallable)]
    def RefState(var value:InterfaceState&; @ue_out var previous:InterfaceState&):void {previous = value; value = InterfaceState.Idle}
    [ue_function(BlueprintCallable)]
    def RefSettings(var value:InterfaceSettings&; @ue_out var previous:InterfaceSettings&):void {previous <- clone(value); value.Count += 3; value.Details.Amount += 1}
    [ue_function(BlueprintCallable)]
    def RefVector(var value:UEVector&; @ue_out var previous:UEVector&):void {previous = value; value = ue_vector(5.0lf,6.0lf,7.0lf)}
    [ue_function(BlueprintCallable)]
    def RefRotator(var value:UERotator&; @ue_out var previous:UERotator&):void {previous = value; value = ue_rotator(10.0lf,20.0lf,30.0lf)}
    [ue_function(BlueprintCallable)]
    def RefTransform(var value:UETransform&; @ue_out var previous:UETransform&):void {previous = value; value = ue_transform_identity()}
    [ue_function(BlueprintCallable)]
    def EchoEvents(value:DasEventInterface):DasEventInterface { return value }
    [ue_function(BlueprintCallable)]
    def EchoDetails(Values:array<InterfaceDetails>):array<InterfaceDetails> { return <- clone(Values) }
    [ue_function(BlueprintCallable)]
    def EchoSettings(value:InterfaceSettings):InterfaceSettings { return <- clone(value) }

    [ue_function(BlueprintCallable)]
    def EchoActor(value:ActorRef):ActorRef { return value }
    [ue_function(BlueprintCallable)]
    def EchoObject(value:ObjectRef):ObjectRef { return value }
    [ue_function(BlueprintCallable)]
    def EchoVector(value:UEVector):UEVector { return value }
    [ue_function(BlueprintCallable)]
    def EchoRotator(value:UERotator):UERotator { return value }
    [ue_function(BlueprintCallable)]
    def EchoTransform(value:UETransform):UETransform { return value }
    [ue_function(BlueprintCallable, arg_value_enum=EDasCallMode, return_enum=EDasCallMode)]
    def EchoMode(value:UEEnumValue):UEEnumValue { return value }
    [ue_function(BlueprintCallable, arg_value_struct=DasCallPayload, return_struct=DasCallPayload)]
    def EchoPayload(value:UEStructValue):UEStructValue { return value }

    [ue_function(BlueprintCallable)]
    def SupportsContract():bool {
        return ue_implements_interface(ue_self_object(), ue_type(CounterContract))
    }
    [ue_function(BlueprintCallable)]
    def SupportsNative():bool {
        return ue_implements_interface(ue_self_object(), "DasEventInterface")
    }

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    LastMessage : string = ""
    [ue_function(BlueprintCallable)]
    def PushLabels(Values:array<string>; message:string):void {
        self.Score += int(length(Values))
        self.LastMessage = message
    }

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 0
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void { self.Score += amount }
    [ue_function(BlueprintCallable)]
    def ClearScore():void { self.Score = 0 }
    [ue_function(BlueprintCallable)]
    def Notify(Amount:int):void { self.Score += Amount }
    [ue_function(BlueprintCallable)]
    def ResetCounter():void { self.Score = 0 }

    [ue_function(BlueprintPure)]
    def const Query(Input:int):int { return Input + 7 }
    [ue_function(BlueprintCallable)]
    def Label():string { return "Authored 世界" }
    [ue_function(BlueprintCallable)]
    def Weights(Values:array<double>):array<double> { var result <- clone(Values); push(result, 1.23456789012345lf); return <- result }
    [ue_function(BlueprintCallable)]
    def Stats(var Counter:int&; @ue_out var Total:int&):void { Counter += 3; Total = Counter * 2 }
}
