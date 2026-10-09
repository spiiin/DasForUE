options gen2
options gc
require ue_dsl
require ue_object_types
[ue_interface(module_id="authoredinterface", BlueprintType)]
class CounterContract {
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
