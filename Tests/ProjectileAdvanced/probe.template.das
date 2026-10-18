options gen2
require @COLLISION@
[export]
def main(input:int):int {
    var params:@PARAMS@
    params.f_startlocation=ue_vector(0.0lf,0.0lf,0.0lf)
    params.f_launchvelocity=ue_vector(1000.0lf,0.0lf,0.0lf)
    params.f_btracewithcollision=true
    params.f_projectileradius=5.0
    params.f_maxsimtime=1.0
    params.f_btracewithchannel=true
    params.f_tracechannel=@CHANNEL@.v_ECC_Visibility
    params.f_simfrequency=30.0
    params.f_overridegravityz=-0.00001
    params.f_drawdebugtype=@DEBUG@.v_None
    params.f_drawdebugtime=0.0
    params.f_btracecomplex=false
    let hit <- ue_collision_gameplay_Blueprint_PredictProjectilePath_Advanced(params)
    verify(hit.out_returnvalue,"advanced prediction: collision")
    let view=ue_hit_result_view(hit.out_predictresult.f_hitresult)
    verify(view.blocking_hit && view.location.x>450.0lf && view.location.x<470.0lf,"advanced nested native hit")
    verify(length(hit.out_predictresult.f_pathdata)>2,"advanced path point array")
    verify(hit.out_predictresult.f_pathdata[0].f_location.x==0.0lf,"path begins at requested start")
    let target=@ACTOR@_from(__ue_trace_actor(view))
    verify(@ACTOR@_valid(target),"advanced hit actor identity")
    // An ignored target is deliberately stored inside the input struct.
    push(params.f_actorstoignore,target)
    let ignored <- ue_collision_gameplay_Blueprint_PredictProjectilePath_Advanced(params)
    verify(!ignored.out_returnvalue,"nested Actor array excludes collider")
    verify(!ue_hit_result_view(ignored.out_predictresult.f_hitresult).blocking_hit,"ignored native result is a miss")
    verify(length(ignored.out_predictresult.f_pathdata)>length(hit.out_predictresult.f_pathdata),"ignored path continues beyond collider")
    // Resizing the source array must not retain entries from a previous call.
    clear(params.f_actorstoignore)
    let restored <- ue_collision_gameplay_Blueprint_PredictProjectilePath_Advanced(params)
    verify(restored.out_returnvalue,"cleared Actor array restores collision")
    verify(__ue_same_object(__ue_trace_actor(ue_hit_result_view(restored.out_predictresult.f_hitresult)),@ACTOR@_raw(target)),"roundtrip preserves target identity")
    push(params.f_actorstoignore,@ACTOR@_null())
    let with_null <- ue_collision_gameplay_Blueprint_PredictProjectilePath_Advanced(params)
    verify(with_null.out_returnvalue,"nullable element in nested Actor array")
    push(params.f_actorstoignore,target)
    push(params.f_actorstoignore,target)
    let duplicates <- ue_collision_gameplay_Blueprint_PredictProjectilePath_Advanced(params)
    verify(!duplicates.out_returnvalue,"null and duplicate ignored actors")
    clear(params.f_actorstoignore)
    params.f_startlocation=ue_vector(0.0lf,1000.0lf,0.0lf)
    let miss <- ue_collision_gameplay_Blueprint_PredictProjectilePath_Advanced(params)
    verify(!miss.out_returnvalue && length(miss.out_predictresult.f_pathdata)>2,"advanced spatial miss")
    print("DAS_PROJECTILE_ADVANCED_PASS hit ignore clear null duplicates miss\n")
    return 42
}
