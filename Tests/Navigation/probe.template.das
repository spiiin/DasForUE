options gen2
require @PAWN_AI@
require ue_navigation
[export]
def main(phase:int):int {
    let origin=ue_vector(-600.0lf,0.0lf,50.0lf)
    let end=ue_vector(600.0lf,0.0lf,50.0lf)
    let extent=ue_vector(100.0lf,100.0lf,200.0lf)
    let data=@PAWN_AI@::C_NavigationData_60de3025_null()
    let filter=ue_class_null()
    let actor=@PAWN_AI@::C_Actor_adf65d5b_from(ue_self_object())
    verify(@PAWN_AI@::C_NavigationSystemV1_bb9972f2_valid(@PAWN_AI@::ue_navigation_GetNavigationSystem()),"live NavigationSystem")
    if(phase==0) {
        let point=@PAWN_AI@::ue_navigation_K2_ProjectPointToNavigation(origin,data,filter,extent)
        verify(point.out_returnvalue && point.out_projectedlocation.x < -500.0lf && point.out_projectedlocation.z < 50.0lf,"project onto floor")
        var randomCount=0
        var reachableCount=0
        for(index in range(0,64)) {
            let random=@PAWN_AI@::ue_navigation_K2_GetRandomLocationInNavigableRadius(point.out_projectedlocation,150.0,data,filter)
            let reachable=@PAWN_AI@::ue_navigation_K2_GetRandomReachablePointInRadius(point.out_projectedlocation,150.0,data,filter)
            if(random.out_returnvalue) {
                let dx=random.out_randomlocation.x-point.out_projectedlocation.x
                let dy=random.out_randomlocation.y-point.out_projectedlocation.y
                verify(dx*dx+dy*dy<=22501.0lf,"random navigable radius")
                verify(@PAWN_AI@::ue_navigation_K2_ProjectPointToNavigation(random.out_randomlocation,data,filter,extent).out_returnvalue,"random navigable point is on NavMesh")
                randomCount+=1
            }
            if(reachable.out_returnvalue) {
                let rx=reachable.out_randomlocation.x-point.out_projectedlocation.x
                let ry=reachable.out_randomlocation.y-point.out_projectedlocation.y
                verify(rx*rx+ry*ry<=22501.0lf,"random reachable radius")
                verify(@PAWN_AI@::ue_navigation_K2_ProjectPointToNavigation(reachable.out_randomlocation,data,filter,extent).out_returnvalue,"random reachable point is on NavMesh")
                reachableCount+=1
            }
            if(randomCount>=8 && reachableCount>=8) { break }
        }
        verify(randomCount>=8 && reachableCount>=8,"eight successful random samples per query within bounded attempts")
        let goal=@PAWN_AI@::ue_navigation_K2_ProjectPointToNavigation(end,data,filter,extent)
        verify(goal.out_returnvalue,"project goal")
        let blocked=@PAWN_AI@::ue_navigation_NavigationRaycast(point.out_projectedlocation,goal.out_projectedlocation,filter,@PAWN_AI@::C_Controller_78a9db5e_null())
        verify(blocked.out_returnvalue && blocked.out_hitlocation.x > -250.0lf && blocked.out_hitlocation.x < 0.0lf,"ray hits obstacle NavMesh boundary")
        let clearStart=@PAWN_AI@::ue_navigation_K2_ProjectPointToNavigation(ue_vector(-600.0lf,900.0lf,50.0lf),data,filter,extent)
        let clearEnd=@PAWN_AI@::ue_navigation_K2_ProjectPointToNavigation(ue_vector(600.0lf,900.0lf,50.0lf),data,filter,extent)
        verify(clearStart.out_returnvalue && clearEnd.out_returnvalue,"project clear ray endpoints")
        let clear=@PAWN_AI@::ue_navigation_NavigationRaycast(clearStart.out_projectedlocation,clearEnd.out_projectedlocation,filter,@PAWN_AI@::C_Controller_78a9db5e_null())
        verify(!clear.out_returnvalue,"clear NavMesh ray")
    } elif(phase==1) {
        let path=@PAWN_AI@::ue_navigation_FindPathToLocationSynchronously(origin,end,actor,filter)
        verify(@PAWN_AI@::C_NavigationPath_550114bf_valid(path) && @PAWN_AI@::ue_path_IsValid(path) && !@PAWN_AI@::ue_path_IsPartial(path),"complete synchronous path")
        verify(@PAWN_AI@::ue_path_GetPathLength(path)>1200.0lf && @PAWN_AI@::ue_path_GetPathCost(path)>0.0lf,"path detours around obstacle")
        let cost=@PAWN_AI@::ue_navigation_GetPathCost(origin,end,data,filter)
        let size=@PAWN_AI@::ue_navigation_GetPathLength(origin,end,data,filter)
        verify(cost.out_returnvalue==@PAWN_AI@::E_ENavigationQueryResult_c4f9ee47.v_Success && cost.out_pathcost>0.0lf,"cost Success and out double")
        verify(size.out_returnvalue==@PAWN_AI@::E_ENavigationQueryResult_c4f9ee47.v_Success && size.out_pathlength>1200.0lf,"length Success and out double")
        let compact=ue_navigation_path(origin,end,ue_ref(type<ActorRef>,ue_self_object()))
        verify(ue_is_valid(compact) && compact.IsValid() && !compact.IsPartial() && compact.GetPathLength()>1200.0lf,"compact typed full navigation path")
        let distance=ue_navigation_distance(origin,end)
        verify(distance.Status==NavigationQueryStatus.Success && distance.Length==size.out_pathlength,"compact query preserves native corridor length and status")
        // Owner is at Start: actor-goal path traverses the same obstacle in reverse.
        let toActor=@PAWN_AI@::ue_navigation_FindPathToActorSynchronously(end,actor,50.0,actor,filter)
        verify(@PAWN_AI@::C_NavigationPath_550114bf_valid(toActor) && @PAWN_AI@::ue_path_IsValid(toActor) && !@PAWN_AI@::ue_path_IsPartial(toActor) && @PAWN_AI@::ue_path_GetPathLength(toActor)>1200.0lf,"complete Actor-goal path")
    } elif(phase==2) {
        let island=ue_vector(3000.0lf,0.0lf,50.0lf)
        verify(@PAWN_AI@::ue_navigation_K2_ProjectPointToNavigation(island,data,filter,extent).out_returnvalue,"disconnected island is navigable")
        let partial=@PAWN_AI@::ue_navigation_FindPathToLocationSynchronously(origin,island,actor,filter)
        verify(@PAWN_AI@::C_NavigationPath_550114bf_valid(partial) && @PAWN_AI@::ue_path_IsValid(partial) && @PAWN_AI@::ue_path_IsPartial(partial),"island cannot produce complete path")
        let compactPartial=ue_navigation_path(origin,island)
        verify(ue_is_valid(compactPartial) && compactPartial.IsValid() && compactPartial.IsPartial(),"compact query preserves partial path")
        let outside=@PAWN_AI@::ue_navigation_K2_ProjectPointToNavigation(ue_vector(10000.0lf,10000.0lf,50.0lf),data,filter,extent)
        verify(!outside.out_returnvalue,"outside NavMesh projection fails")
    } else { return -1 }
    return 42
}
