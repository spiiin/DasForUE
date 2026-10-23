options gen2
require ue_dsl
require ue_library_generated
require ue_object_types
require ue_scene_components
[ue_class(module_id="libraries",Blueprintable)]
class LibraryProbe {
    [ue_function(BlueprintCallable)]
    def Check(input:bool):bool {
        return ue_lib_Add(20,22)==42 && ue_lib_Abs(-3.0lf)==3.0lf && ue_lib_Time()>=0.0lf && ue_lib_Context()
    }
    [ue_function(BlueprintCallable)]
    def Classes(input:bool):bool {
        let klass=ue_class_loaded(type<ActorRef>,"/Script/Engine.Actor")
        let echoed=ue_lib_EchoActorClass(klass)
        let any=ue_lib_EchoAnyClass(klass)
        let output=ue_lib_ClassOutputs(klass)
        var values:array<UEClassHandle>
        push(values,klass)
        push(values,ue_class_null())
        let returned=ue_lib_EchoClassArray(values)
        let receiver=ue_ref(type<ue_library_generated::C_Actor_adf65d5b>,ue_self_object())
        let base=ue_class_loaded(type<ObjectRef>,"/Script/Engine.ActorComponent")
        let components=ue_lib_GetComponentsByClass(receiver,base)
        let found=ue_lib_GetActorOfClass(klass)
        return ue_class_path(echoed)==ue_class_path(klass) && ue_class_path(any)==ue_class_path(klass) && ue_class_path(output)==ue_class_path(klass) && length(returned)==2 && ue_class_valid(returned[0]) && !ue_class_valid(returned[1]) && !ue_class_valid(ue_lib_EchoActorClass(ue_class_null())) && ue_object_valid(found.handle) && length(components)>=1 && ue_object_valid(components[0].handle)
    }
    [ue_function(BlueprintCallable)]
    def WrongMetaClass(input:bool):bool {
        let klass=ue_class_loaded(type<SceneComponentRef>,"/Script/Engine.SceneComponent")
        let output=ue_lib_EchoActorClass(klass)
        return ue_class_valid(output)
    }
    [ue_function(BlueprintCallable)]
    def AbstractClass(input:bool):bool {
        let klass=ue_class_loaded(type<ObjectRef>,"/Script/CoreUObject.Object")
        return ue_class_valid(ue_lib_EchoAnyClass(klass))
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
