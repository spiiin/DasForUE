options gen2
options gc
require @COLLISION@
require ue_hit_transport_fixture
require ue_gameplay_services

[export]
def main(input:int):int {
    let start=ue_vector(0.0lf,0.0lf,0.0lf)
    let finish=ue_vector(800.0lf,0.0lf,0.0lf)
    let channel=@COLLISION@::E_ETraceTypeQuery_c61871df.v_TraceTypeQuery1
    let debug=@COLLISION@::E_EDrawDebugTrace_59edfe33.v_None
    let color=ue_linear_color(1.0f,0.0f,0.0f,1.0f)
    var ignored:array<@COLLISION@::C_Actor_adf65d5b>
    let line <- @COLLISION@::ue_collision_LineTraceSingle(start,finish,channel,false,ignored,debug,true,color,color,0.0)
    let hit=ue_hit_result_view(line.out_outhit)
    verify(line.out_returnvalue && hit.blocking_hit && hit.distance>450.0lf && hit.distance<470.0lf,"generated line single hit")
    verify(hit.trace_start.x==0.0lf && hit.trace_end.x==800.0lf,"native endpoints survive opaque transport")
    verify(ue_is_valid(ue_hit_actor(hit)) && ue_is_valid(ue_hit_component(hit)),"live actor/component")
    let echoed=ue_hit_transport_fixture::ue_hit_fixture_Echo(line.out_outhit)
    let echo_view=ue_hit_result_view(echoed)
    verify(echo_view.distance==hit.distance && echo_view.item==hit.item,"native scalar roundtrip")
    verify(__ue_same_object(ue_hit_transport_fixture::C_Actor_adf65d5b_raw(ue_hit_transport_fixture::ue_hit_fixture_CachedActor(echoed)),ue_hit_actor(hit).handle),"ActorInstanceHandle native roundtrip")
    let changed=ue_hit_transport_fixture::ue_hit_fixture_ChangeItem(echoed)
    verify(ue_hit_result_view(changed).item==hit.item+7 && ue_hit_result_view(echoed).item==hit.item,"ref input/output is a detached native copy")
    let broken <- @COLLISION@::ue_collision_gameplay_BreakHitResult(echoed)
    verify(broken.out_bblockinghit && broken.out_tracestart.x==0.0lf && broken.out_traceend.x==800.0lf,"generated native HitResult consumer")
    verify(__ue_same_object(@COLLISION@::C_Actor_adf65d5b_raw(broken.out_hitactor),ue_hit_actor(hit).handle),"native consumer actor identity")

    let sphere <- @COLLISION@::ue_collision_SphereTraceSingle(start,finish,15.0,channel,false,ignored,debug,true,color,color,0.0)
    let box <- @COLLISION@::ue_collision_BoxTraceSingle(start,finish,ue_vector(10.0lf,10.0lf,10.0lf),ue_rotator(0.0lf,0.0lf,0.0lf),channel,false,ignored,debug,true,color,color,0.0)
    let capsule <- @COLLISION@::ue_collision_CapsuleTraceSingle(start,finish,10.0,20.0,channel,false,ignored,debug,true,color,color,0.0)
    verify(sphere.out_returnvalue && box.out_returnvalue && capsule.out_returnvalue,"generated sphere/box/capsule sweeps")
    verify(ue_hit_result_view(sphere.out_outhit).distance<hit.distance,"sweep radius changes impact")
    let miss <- @COLLISION@::ue_collision_LineTraceSingle(ue_vector(0.0lf,1000.0lf,0.0lf),ue_vector(800.0lf,1000.0lf,0.0lf),channel,false,ignored,debug,true,color,color,0.0)
    verify(!miss.out_returnvalue && !ue_hit_result_view(miss.out_outhit).blocking_hit,"generated miss still returns valid native HitResult")
    let penetration <- @COLLISION@::ue_collision_SphereTraceSingle(ue_vector(500.0lf,0.0lf,0.0lf),ue_vector(700.0lf,0.0lf,0.0lf),10.0,channel,false,ignored,debug,true,color,color,0.0)
    let penetrating=ue_hit_result_view(penetration.out_outhit)
    verify(penetrating.start_penetrating && penetrating.penetration_depth>0.0lf,"native sweep penetration fields")
    let multi <- @COLLISION@::ue_collision_SphereTraceMulti(start,finish,10.0,channel,false,ignored,debug,true,color,color,0.0)
    verify(multi.out_returnvalue && length(multi.out_outhits)==2,"generated multi sweep array")
    verify(!ue_hit_result_view(multi.out_outhits[0]).blocking_hit && ue_hit_result_view(multi.out_outhits[1]).blocking_hit,"touch and blocking hit preserved")
    let many <- ue_hit_transport_fixture::ue_hit_fixture_EchoMany(multi.out_outhits)
    verify(length(many)==2 && ue_hit_result_view(many[1]).distance==ue_hit_result_view(multi.out_outhits[1]).distance,"native HitResult array input/output")
    var empty:array<UEHitResult>
    let empty_echo <- ue_hit_transport_fixture::ue_hit_fixture_EchoMany(empty)
    verify(length(empty_echo)==0,"empty native HitResult arrays")
    var envelope:ue_hit_transport_fixture::@ENVELOPE@
    envelope.f_hit=echoed
    push(envelope.f_hits,changed)
    let nested <- ue_hit_transport_fixture::ue_hit_fixture_EchoEnvelope(envelope)
    verify(ue_hit_result_view(nested.f_hit).distance==hit.distance && length(nested.f_hits)==1 && ue_hit_result_view(nested.f_hits[0]).item==hit.item+7,"nested opaque HitResult and array")

    let weak=ue_hit_transport_fixture::ue_hit_fixture_MakeWeakHit()
    verify(ue_is_valid(ue_hit_physical_material(ue_hit_result_view(weak))),"live weak material")
    let held=ue_begin_static_call("/Script/DasRuntime.DasHitTransportLibrary","MakeWeakHit","")
    ue_invoke_call(held)
    let held_snapshot=__ue_result_hit_result(held,"returnvalue")
    let pending=ue_begin_static_call("/Script/DasRuntime.DasHitTransportLibrary","Echo","")
    __ue_arg_hit_result(pending,"hit",held_snapshot)
    ue_hit_transport_fixture::ue_hit_fixture_CollectWeak()
    verify(!ue_is_valid(ue_hit_physical_material(ue_hit_result_view(weak))),"snapshot must not retain weak material through GC")
    verify(!ue_is_valid(ue_hit_physical_material(ue_hit_result_view(__ue_result_hit_result(held,"returnvalue")))),"held output frame must remain weak")
    ue_release_call(held)
    ue_invoke_call(pending)
    let expired=__ue_result_hit_result(pending,"returnvalue")
    ue_release_call(pending)
    verify(!ue_is_valid(ue_hit_physical_material(ue_hit_result_view(expired))),"pending input frame must remain weak")

    var invalid:UEHitResult
    var rejected=false
    try { let bad=ue_hit_result_view(invalid) } recover { rejected=true }
    verify(rejected,"uninitialized HitResult rejected")
    let wrong=ue_begin_static_call("/Script/DasRuntime.DasHitTransportLibrary","EchoVector","")
    rejected=false
    try { __ue_arg_hit_result(wrong,"value",echoed) } recover { rejected=true }
    verify(rejected,"wrong native struct rejected")
    ue_release_call(wrong)
    let missing=ue_begin_static_call("/Script/DasRuntime.DasHitTransportLibrary","Echo","")
    rejected=false
    try { ue_invoke_call(missing) } recover { rejected=true }
    verify(rejected,"unassigned HitResult input rejected")
    rejected=false
    try { let bad=__ue_result_hit_result(missing,"returnvalue") } recover { rejected=true }
    verify(rejected,"result before invocation rejected")
    __ue_arg_hit_result(missing,"hit",echoed)
    rejected=false
    try { __ue_arg_hit_result(missing,"hit.HitObjectHandle",echoed) } recover { rejected=true }
    verify(rejected,"private ActorInstanceHandle traversal rejected")
    ue_release_call(missing)

    for(index in range(600)) { let garbage=ue_hit_result_empty() }
    ue_struct_collect()
    verify(ue_struct_value_count()<256 && ue_hit_result_view(many[1]).blocking_hit,"opaque values participate in existing root collection")
    let victim=ue_hit_actor(hit)
    victim.K2_DestroyActor()
    let after=ue_hit_result_view(echoed)
    verify(!ue_is_valid(ue_hit_actor(after)) && !ue_is_valid(ue_hit_component(after)),"native copied weak references after Destroy")
    verify(!ue_is_valid(ue_hit_actor(ue_hit_result_view(many[1]))),"array hit remains weak after Destroy")
    return 42
}
