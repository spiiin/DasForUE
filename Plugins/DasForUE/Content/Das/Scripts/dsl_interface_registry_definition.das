options gen2
options gc
require ue_dsl
require ue_interface_values
require ue_object_types
[ue_interface(module_id="authoredinterface", type_registry="REGISTRY", BlueprintType)]
class CounterContract {
    [ue_function(BlueprintCallable)]
    def EchoState(value:InterfaceState):InterfaceState {}
    [ue_function(BlueprintCallable)]
    def NewDetails():array<InterfaceDetails> {}
    [ue_function(BlueprintCallable)]
    def CopyDetails(Values:array<InterfaceDetails>; @ue_out var Copies:array<InterfaceDetails>&):void {}
    [ue_function(BlueprintCallable)]
    def BumpDetails(var Values:array<InterfaceDetails>&):void {}
    [ue_function(BlueprintCallable)]
    def RefActor(var value:ActorRef&; @ue_out var previous:ActorRef&):void {}
    [ue_function(BlueprintCallable)]
    def RefEvents(var value:DasEventInterface&; @ue_out var previous:DasEventInterface&):void {}
    [ue_function(BlueprintCallable)]
    def RefState(var value:InterfaceState&; @ue_out var previous:InterfaceState&):void {}
    [ue_function(BlueprintCallable)]
    def RefSettings(var value:InterfaceSettings&; @ue_out var previous:InterfaceSettings&):void {}
    [ue_function(BlueprintCallable)]
    def RefVector(var value:UEVector&; @ue_out var previous:UEVector&):void {}
    [ue_function(BlueprintCallable)]
    def RefRotator(var value:UERotator&; @ue_out var previous:UERotator&):void {}
    [ue_function(BlueprintCallable)]
    def RefTransform(var value:UETransform&; @ue_out var previous:UETransform&):void {}
    [ue_function(BlueprintCallable)]
    def EchoEvents(value:DasEventInterface):DasEventInterface {}
    [ue_function(BlueprintCallable)]
    def EchoDetails(Values:array<InterfaceDetails>):array<InterfaceDetails> {}
    [ue_function(BlueprintCallable)]
    def EchoSettings(value:InterfaceSettings):InterfaceSettings {}

    [ue_function(BlueprintCallable)]
    def EchoActor(value:ActorRef):ActorRef {}
    [ue_function(BlueprintCallable)]
    def EchoObject(value:ObjectRef):ObjectRef {}
    [ue_function(BlueprintCallable)]
    def EchoVector(value:UEVector):UEVector {}
    [ue_function(BlueprintCallable)]
    def EchoRotator(value:UERotator):UERotator {}
    [ue_function(BlueprintCallable)]
    def EchoTransform(value:UETransform):UETransform {}
    [ue_function(BlueprintCallable, arg_value_enum=EDasCallMode, return_enum=EDasCallMode)]
    def EchoMode(value:UEEnumValue):UEEnumValue {}
    [ue_function(BlueprintCallable, arg_value_struct=DasCallPayload, return_struct=DasCallPayload)]
    def EchoPayload(value:UEStructValue):UEStructValue {}

    [ue_function(BlueprintCallable, Category="Counter")]
    def PushLabels(Values:array<string>; message:string):void {}

    [ue_function(BlueprintCallable, Category="Counter")]
    def AddScore(amount:int):void {}
    [ue_function(BlueprintCallable, Category="Counter")]
    def ClearScore():void {}

    [ue_function(BlueprintCallable, Category="Counter", DisplayName="Get Score", ToolTip="Read the current score")]
    def Query(Input:int):int {}
    [ue_function(BlueprintCallable)]
    def Label():string {}
    [ue_function(BlueprintCallable)]
    def Weights(Values:array<double>):array<double> {}
    [ue_function(BlueprintCallable)]
    def Stats(var Counter:int&; @ue_out var Total:int&):void {}
}
