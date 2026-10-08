options gen2
options gc
require ue_dsl
require ue_object_types
var remembered : ActorRef
[ue_class(module_id="typedobjects", Blueprintable)]
class TargetHolder {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : ActorRef
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Held : ObjectRef
    [ue_function(BlueprintCallable)]
    def SetTarget(target:ActorRef):void { self.Target = target }
    [ue_function(BlueprintCallable)]
    def GetTarget():ActorRef { return self.Target }
    [ue_function(BlueprintCallable)]
    def EchoActor(target:ActorRef):ActorRef { return target }
    [ue_function(BlueprintCallable)]
    def EchoObject(value:ObjectRef):ObjectRef { return value }
    [ue_function(BlueprintCallable)]
    def Clear():void { self.Target = default<ActorRef> }
    [ue_function(BlueprintCallable)]
    def SetHeld(value:ObjectRef):void { self.Held = value }
    [ue_function(BlueprintCallable)]
    def ClearHeld():void { self.Held = default<ObjectRef> }
    [ue_function(BlueprintCallable)]
    def Remember(target:ActorRef):void { remembered = target }
    [ue_function(BlueprintCallable)]
    def Recall():ActorRef { return remembered }
}
