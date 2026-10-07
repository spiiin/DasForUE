options gen2
options gc
require ue_dsl
[ue_class(module_id="objectfields",Blueprintable)]
class ObjectFields {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Target : UObjectHandle = ue_object_value("/Script/Engine.Actor")
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Component : UObjectHandle = ue_object_value("/Script/Engine.ActorComponent")
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Held : UObjectHandle = ue_object_value("/Script/CoreUObject.Object")
    @ue_property=(EditAnywhere,BlueprintReadOnly)
    Observed : UObjectHandle = ue_object_value("/Script/Engine.Actor")
    [ue_function(BlueprintCallable)]
    def SetTarget(value:UObjectHandle):void { self.Target = value }
    [ue_function(BlueprintCallable,return_class="/Script/Engine.Actor")]
    def GetTarget():UObjectHandle { return self.Target }
    [ue_function(BlueprintCallable)]
    def SetComponent(value:UObjectHandle):void { self.Component = value }
    [ue_function(BlueprintCallable,return_class="/Script/Engine.ActorComponent")]
    def GetComponent():UObjectHandle { return self.Component }
    [ue_function(BlueprintCallable)]
    def Hold(value:UObjectHandle):void { self.Held = value }
    [ue_function(BlueprintCallable)]
    def Recall():UObjectHandle { return self.Held }
    [ue_function(BlueprintCallable)]
    def HasTarget():bool { return ue_object_valid(self.Target) }
    [ue_function(BlueprintCallable)]
    def Clear():void { self.Target = ue_null_object(); self.Held = ue_null_object() }
}
