options gen2
options gc
module dsl_library_values public
require ue_dsl
require ue_object_types
require ue_library_generated
require ue_library_values_types
[ue_library(Category="Examples", type_registry="/Game/DasLibraryValuesTypes/Registry.Registry")]
class GameValues {
    [ue_function(BlueprintPure)]
    def static EchoVector(value:UEVector):UEVector { return value }
    [ue_function(BlueprintPure)]
    def static EchoTransform(value:UETransform):UETransform { return value }
    [ue_function(BlueprintPure)]
    def static EchoActor(value:ActorRef):ActorRef { return value }
    [ue_function(BlueprintPure)]
    def static EchoClass(value:ActorClass):ActorClass { return value }
    [ue_function(BlueprintPure)]
    def static EchoArray(value:array<int>):array<int> { return <- clone(value) }
    [ue_function(BlueprintCallable)]
    def static Append(var values:array<int>&; @ue_out var result:UEVector&):int {
        push(values,7)
        result=ue_vector(1.0lf,2.0lf,3.0lf)
        return length(values)
    }
    [ue_function(BlueprintPure)]
    def static EchoState(value:LibraryState):LibraryState { return value }
    [ue_function(BlueprintPure)]
    def static UpdateSettings(value:LibrarySettings):LibrarySettings { var result=value;result.Count+=2;return result }
    [ue_function(BlueprintPure)]
    def static EchoMap(value:table<string;int>):table<string;int> { return <- clone(value) }
    [ue_function(BlueprintPure)]
    def static EchoSet(value:table<int;void>):table<int;void> { return <- clone(value) }
    [ue_function(BlueprintCallable, WorldContext="context")]
    def static Spawn(context:ObjectRef; kind:ActorClass):ActorRef {
        return ue_actor_spawn(type<ActorRef>,kind)
    }
    // Uses the explicit context even when called from a different DAS owner.
    [ue_function(BlueprintPure, WorldContext="context")]
    def static Time(context:ObjectRef):double { return ue_lib_Time()+1.0lf }
}
