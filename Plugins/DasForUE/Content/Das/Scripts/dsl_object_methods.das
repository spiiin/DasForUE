options gen2
require ue_dsl
var remembered = ue_null_object()
[ue_class(module_id="objects",Blueprintable)]
class ObjectBridge {
    [ue_function(BlueprintCallable)]
    def Echo(value:UObjectHandle):UObjectHandle { return value }
    [ue_function(BlueprintCallable)]
    def Remember(value:UObjectHandle):void { remembered = value }
    [ue_function(BlueprintCallable)]
    def Recall():UObjectHandle { return remembered }
    [ue_function(BlueprintCallable)]
    def IsRememberedValid():bool { return ue_object_valid(remembered) }
    [ue_function(BlueprintCallable,arg_actor_class="/Script/Engine.Actor",return_class="/Script/Engine.Actor")]
    def ActorIdentity(actor:UObjectHandle):UObjectHandle { return actor }
    [ue_function(BlueprintCallable,return_class="/Script/Engine.Actor")]
    def WrongActorResult():UObjectHandle { return ue_self_object() }
    [ue_function(BlueprintCallable)]
    def ReadAcrossGC(value:UObjectHandle):int {
        let call = ue_begin_call(ue_self_object(), "ForceTestGC")
        ue_invoke_call(call)
        ue_release_call(call)
        return ue_object_get_int(value, "health")
    }
}
