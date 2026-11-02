options gen2
options gc
require ue_native_gameplay
require ue_library_generated
// This view deliberately has no generated method wrappers.
[ue_native_interface(path="/Script/DasRuntime.DasProbeInterface")]
struct LocalProbeRef { handle:UObjectHandle }

[ue_native_object(path="/Script/Engine.Actor")]
struct LocalActorRef { handle:UObjectHandle }

def actor_view_valid(actor:ActorRef):int { return ue_is_valid(actor) ? 42 : 0 }

def class_handle_valid(kind:UEClassHandle):int { return ue_class_valid(kind) ? 42 : 0 }

[export]
def main(mode:int):int {
    if (mode == 0) { return ue_static(DasFunctionLibraryFixture.Add, 20,22) }
    if (mode == 1) { verify(ue_static(GameplayStatics.GetTimeSeconds) >= 0.0lf);return 1 }
    let target = ue_interface(type<DasProbeInterface>, ue_self_object())
    if (mode == 2) { return ue_call(type<DasProbeInterface>, Query, target,5) }
    if (mode == 3) {
        var counter=10;var total=0
        ue_call(type<DasProbeInterface>, Stats, target,counter,total)
        return counter+total
    }
    if (mode == 6) {
        var values:array<double>;push(values,1.0lf);push(values,2.0lf)
        var result <- ue_call(type<DasProbeInterface>, Weights, target,values)
        return int(result[0]+result[1])
    }
    if (mode == 7) {
        let local = ue_ref(type<LocalProbeRef>, ue_self_object())
        return local.Query(5)
    }
    if (mode == 8) {
        let local = ue_ref(type<LocalProbeRef>, ue_self_object())
        var counter=10; var total=0
        ue_call(type<LocalProbeRef>, Stats, local, counter, total)
        return counter+total
    }
    if (mode == 9) {
        let local = ue_ref(type<LocalProbeRef>, ue_self_object())
        var values:array<double>;push(values,1.0lf);push(values,2.0lf)
        var result <- local.Weights(values)
        return int(result[0]+result[1])
    }
    if (mode == 10) {
        let localActor = ue_ref(type<LocalActorRef>, ue_self_object())
        return actor_view_valid(localActor)
    }
    if (mode == 11) {
        let kind = ue_class_loaded(type<ActorClass>, "/Script/Engine.Actor")
        return class_handle_valid(kind)
    }
    let payload = DasCallPayload(Count=17,Metrics=DasCallMetrics(Ratio=1.0f))
    if (mode == 4) { return ue_struct_get(payload, Count) }
    verify(ue_static(KismetMathLibrary.Abs, -2.5lf) == 2.5lf)
    return int(ue_struct_get(ue_struct_get(payload, Metrics), Ratio))
}
