options gen2
options gc
require ue_native_gameplay
require ue_library_generated
require ue_projectile_components
// This view deliberately has no generated method wrappers.
[ue_native_interface(path="/Script/DasRuntime.DasProbeInterface")]
struct LocalProbeRef { handle:UObjectHandle }

[ue_native_object(path="/Script/Engine.Actor")]
struct LocalActorRef { handle:UObjectHandle }

def actor_view_valid(actor:ActorRef):int { return ue_is_valid(actor) ? 42 : 0 }

def class_handle_valid(kind:UEClassHandle):int { return ue_class_valid(kind) ? 42 : 0 }

struct PlainArgRecord { Value:int }

def next_arg_value(var reads:int&):int { reads+=1; return 20 }

def next_field_name(var reads:int&):string { reads+=1; return "Value" }

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
    if (mode == 12) {
        let call=ue_begin_static_call("/Script/DasRuntime.DasFunctionLibraryFixture", "Add", "")
        var reads=0
        ue_arg(call, "A", next_arg_value(reads))
        ue_arg(call, "B", 22)
        ue_invoke_call(call)
        let result=ue_result_int(call,"ReturnValue")
        ue_release_call(call)
        return reads==1 ? result : -1
    }
    if (mode == 13) {
        var hit=CollisionHit()
        hit.BlockingHit=true
        hit.Item=9
        hit.ElementIndex=uint8(233)
        hit.ImpactPoint=ue_vector(1.25lf,2.5lf,3.75lf)
        hit.BoneName=ue_name_value("macro_bone")
        hit.Actor=ue_ref(type<ActorRef>,ue_self_object())
        let call=ue_begin_call(ue_self_object(),"EchoHit")
        var nameReads=0
        ue_arg(call, next_field_name(nameReads), hit)
        ue_invoke_call(call)
        let item=ue_result_int(call,"ReturnValue.Item")
        let element=ue_result_byte(call,"ReturnValue.ElementIndex")
        let bone=ue_name_text(__ue_method_result_name(call,"ReturnValue.BoneName"))
        let point=__ue_method_result_vector(call,"ReturnValue.ImpactPoint")
        let actor=ue_result_object(call,"ReturnValue.Actor")
        let blocking=ue_result_bool(call,"ReturnValue.BlockingHit")
        ue_release_call(call)
        return (blocking && item==10 && hit.Item==9 && element==233 && bone=="macro_bone" &&
            point.x==1.25lf && point.y==2.5lf && point.z==3.75lf && nameReads==1 &&
            __ue_same_object(actor,ue_self_object())) ? 42 : -1
    }
    if (mode == 14) {
        let call=ue_begin_interface_call(ue_self_object(),"/Script/DasRuntime.DasProbeInterface","Weights")
        var values:array<double>;push(values,1.0lf);push(values,2.0lf)
        ue_arg(call,"Values",values)
        ue_invoke_call(call)
        let result=ue_result_double(call,"ReturnValue.0")+ue_result_double(call,"ReturnValue.1")
        ue_release_call(call)
        return int(result)
    }
    if (mode == 15) {
        let call=ue_begin_call(ue_self_object(),"EchoHit")
        var rejected=false
        try { ue_arg(call,"Value",17) } recover { rejected=true }
        ue_release_call(call)
        return rejected ? 1 : 0
    }
    if (mode == 16) {
        let call=ue_begin_static_call("/Script/DasRuntime.DasHitTransportLibrary","EchoVector","")
        ue_arg(call,"Value",ue_vector(1.25lf,2.5lf,3.75lf))
        ue_invoke_call(call)
        let point=__ue_method_result_vector(call,"ReturnValue")
        ue_release_call(call)
        return point.x==1.25lf && point.y==2.5lf && point.z==3.75lf ? 42 : -1
    }
    if (mode == 17) {
        let call=ue_begin_static_call("/Script/DasRuntime.DasFunctionLibraryFixture","EchoAnyClass","")
        let kind=ue_class_loaded(type<ActorClass>,"/Script/Engine.Actor")
        ue_arg(call,"Value",kind)
        ue_invoke_call(call)
        let result=ue_result_class(call,"ReturnValue")
        ue_release_call(call)
        return ue_class_path(result)=="/Script/Engine.Actor" ? 42 : -1
    }
    let payload = DasCallPayload(Count=17,Metrics=DasCallMetrics(Ratio=1.0f))
    if (mode == 4) { return ue_struct_get(payload, Count) }
    verify(ue_static(KismetMathLibrary.Abs, -2.5lf) == 2.5lf)
    return int(ue_struct_get(ue_struct_get(payload, Metrics), Ratio))
}
