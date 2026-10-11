options gen2
require ue_dsl
require ue_library_generated
[ue_class(module_id="libraries",Blueprintable)]
class LibraryProbe {
    [ue_function(BlueprintCallable)]
    def Check(input:bool):bool {
        return ue_lib_Add(20,22)==42 && ue_lib_Abs(-3.0lf)==3.0lf && ue_lib_Time()>=0.0lf && ue_lib_Context()
    }
    [ue_function(BlueprintCallable)]
    def WorldId(input:int):int { return ue_lib_WorldId() }
    [ue_function(BlueprintCallable)]
    def OverrideWorld(input:bool):bool {
        let call=ue_begin_static_call("/Script/Engine.GameplayStatics","GetTimeSeconds","worldcontextobject")
        ue_arg_object(call,"worldcontextobject",ue_self_object())
        return false
    }
    [ue_function(BlueprintCallable)]
    def BadWorldContract(input:bool):bool {
        let call=ue_begin_static_call("/Script/Engine.GameplayStatics","GetTimeSeconds","")
        return false
    }
    [ue_function(BlueprintCallable)]
    def NotLibrary(input:bool):bool {
        let call=ue_begin_static_call("/Script/Engine.Actor","K2_GetActorLocation","")
        return false
    }
}
