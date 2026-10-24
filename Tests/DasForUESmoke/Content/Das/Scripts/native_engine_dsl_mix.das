options gen2
require ue_native_gameplay
require ue_engine

// ue_engine next to the DSL (ue_native_gameplay requires ue_dsl): `Actor` is the DSL base class, AActor the native
// type, ActorRef the gameplay reference; each name has one meaning, so none needs qualifying. Run with an actor
// as self; main returns 0 or the number of the first failed check.
[ue_class(module_id="nativeenginedslmix")]
class NativeEngineDslMix : Actor {
    [ue_function(BlueprintCallable)]
    def SameActor(target : ActorRef) : bool {
        var native : AActor = as_AActor(target.handle)
        return native.valid() && native == as_AActor(ue_self_object())
    }
}

[export]
def main(input:int):int {
    var a : AActor = as_AActor(ue_self_object())
    let reference = ue_ref(type<ActorRef>, ue_self_object())
    if (!a.valid() || !ue_is_valid(reference)) { return 1 }
    // The gameplay call (routed to the bind) and the bind agree.
    if (reference.K2_GetActorLocation() != a.get_actor_location()) { return 2 }
    return 0
}
