options gen2
options gc
module dsl_library_values public
require ue_dsl
require ue_object_types
require ue_library_generated
[ue_library(Category="Examples")]
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
    // Uses the explicit context even when called from a different DAS owner.
    [ue_function(BlueprintPure, WorldContext="missing")]
    def static Time(context:ObjectRef):double { return ue_lib_Time()+1.0lf }
}
