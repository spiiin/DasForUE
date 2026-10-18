options gen2
require ue_gameplay_services
require ue_native_gameplay
require ue_collision_fixture
[export]
def main(input:int):int {
    let target=C_DasWeakCollisionFixture_0d943e41_from(ue_self_object())
    let value=ue_collision_fixture_MakeWeakSnapshot(target)
    verify(value.f_marker==17,"weak snapshot marker")
    verify(ue_collision_fixture_CollectWeakSnapshot(target,value),"weak call frame must not pin target across GC")
    // Keep an output frame alive while a second native call runs a real GC.
    let held=ue_begin_call(ue_self_object(),"MakeWeakSnapshot")
    ue_invoke_call(held)
    var held_value=value
    held_value.f_target=ue_collision_fixture::C_Object_55c3753a_from(ue_result_object(held,"returnvalue.target"))
    verify(ue_collision_fixture_CollectWeakSnapshot(target,held_value),"retained weak output frame must not pin")
    verify(!ue_object_is_a(ue_result_object(held,"returnvalue.target"),"/Script/CoreUObject.Object"),"expired weak output is null")
    ue_release_call(held)
    let hit=ue_line_trace(ue_vector(0.0lf,0.0lf,0.0lf),ue_vector(600.0lf,0.0lf,0.0lf))
    verify(hit.blocking_hit && hit.distance>200.0lf && hit.distance<300.0lf,"real collision")
    verify(hit.trace_start.x==0.0lf && hit.trace_end.x==600.0lf,"native trace endpoints")
    verify(hit.penetration_depth==0.0lf && !hit.start_penetrating,"nonpenetrating hit")
    verify(hit.element_index>=0 && hit.element_index<=255,"element byte")
    verify(ue_is_valid(ue_hit_actor(hit)) && ue_is_valid(ue_hit_component(hit)),"live checked hit references")
    let penetrating=ue_sphere_trace(ue_vector(300.0lf,0.0lf,0.0lf),ue_vector(450.0lf,0.0lf,0.0lf),10.0lf)
    verify(penetrating.start_penetrating && penetrating.penetration_depth>0.0lf,"native penetration depth")
    let bone=ue_hit_bone_name(hit)
    let my_bone=ue_hit_my_bone_name(hit)
    let material=ue_hit_physical_material(hit)
    let physics_owner=ue_hit_physics_owner(hit)
    let overlaps <- ue_sphere_overlap(ue_vector(300.0lf,0.0lf,0.0lf),75.0lf)
    verify(length(overlaps)==1 && overlaps[0].blocking_hit,"real overlap result")
    verify(ue_same_object(ue_overlap_actor(overlaps[0]),ue_hit_actor(hit)),"overlap actor identity")
    verify(ue_same_object(ue_overlap_component(overlaps[0]),ue_hit_component(hit)),"overlap component identity")
    verify(overlaps[0].my_item_index==-1,"native overlap query item")
    var invalid_radius=false
    try { let invalid <- ue_sphere_overlap(ue_vector(0.0lf,0.0lf,0.0lf),0.0lf) } recover { invalid_radius=true }
    verify(invalid_radius,"invalid overlap radius rejected")
    var bad:UEOverlapResult
    var rejected=false
    try { let invalid=ue_overlap_actor(bad) } recover { rejected=true }
    verify(rejected,"uninitialized collision scope")
    let victim=ue_hit_actor(hit)
    victim.K2_DestroyActor()
    verify(!ue_is_valid(ue_hit_actor(hit)) && !ue_is_valid(ue_hit_component(hit)),"destroyed hit references become null")
    verify(!ue_is_valid(ue_overlap_actor(overlaps[0])) && !ue_is_valid(ue_overlap_component(overlaps[0])),"destroyed overlap references become null")
    return 42
}
