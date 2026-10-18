options gen2
require @PAWN_AI@
[export]
def main(phase:int):int {
    let origin=ue_vector(-600.0lf,0.0lf,50.0lf)
    let end=ue_vector(600.0lf,0.0lf,50.0lf)
    let extent=ue_vector(100.0lf,100.0lf,200.0lf)
    let data=C_NavigationData_60de3025_null()
    let filter=ue_class_null()
    let actor=C_Actor_adf65d5b_from(ue_self_object())
    verify(C_NavigationSystemV1_bb9972f2_valid(ue_navigation_GetNavigationSystem()),"live NavigationSystem")
    if(phase==0) {
        let point=ue_navigation_K2_ProjectPointToNavigation(origin,data,filter,extent)
        verify(point.out_returnvalue && point.out_projectedlocation.x < -500.0lf && point.out_projectedlocation.z < 50.0lf,"project onto floor")
        var randomCount=0
        var reachableCount=0
        for(index in range(0,64)) {
            let random=ue_navigation_K2_GetRandomLocationInNavigableRadius(point.out_projectedlocation,150.0,data,filter)
            let reachable=ue_navigation_K2_GetRandomReachablePointInRadius(point.out_projectedlocation,150.0,data,filter)
            if(random.out_returnvalue) {
                let dx=random.out_randomlocation.x-point.out_projectedlocation.x
                let dy=random.out_randomlocation.y-point.out_projectedlocation.y
                verify(dx*dx+dy*dy<=22501.0lf,"random navigable radius")
                verify(ue_navigation_K2_ProjectPointToNavigation(random.out_randomlocation,data,filter,extent).out_returnvalue,"random navigable point is on NavMesh")
                randomCount+=1
            }
            if(reachable.out_returnvalue) {
                let rx=reachable.out_randomlocation.x-point.out_projectedlocation.x
                let ry=reachable.out_randomlocation.y-point.out_projectedlocation.y
                verify(rx*rx+ry*ry<=22501.0lf,"random reachable radius")
                verify(ue_navigation_K2_ProjectPointToNavigation(reachable.out_randomlocation,data,filter,extent).out_returnvalue,"random reachable point is on NavMesh")
                reachableCount+=1
            }
            if(randomCount>=8 && reachableCount>=8) { break }
        }
        verify(randomCount>=8 && reachableCount>=8,"eight successful random samples per query within bounded attempts")
        let goal=ue_navigation_K2_ProjectPointToNavigation(end,data,filter,extent)
        verify(goal.out_returnvalue,"project goal")
        let blocked=ue_navigation_NavigationRaycast(point.out_projectedlocation,goal.out_projectedlocation,filter,C_Controller_78a9db5e_null())
        verify(blocked.out_returnvalue && blocked.out_hitlocation.x > -250.0lf && blocked.out_hitlocation.x < 0.0lf,"ray hits obstacle NavMesh boundary")
        let clearStart=ue_navigation_K2_ProjectPointToNavigation(ue_vector(-600.0lf,900.0lf,50.0lf),data,filter,extent)
        let clearEnd=ue_navigation_K2_ProjectPointToNavigation(ue_vector(600.0lf,900.0lf,50.0lf),data,filter,extent)
        verify(clearStart.out_returnvalue && clearEnd.out_returnvalue,"project clear ray endpoints")
        let clear=ue_navigation_NavigationRaycast(clearStart.out_projectedlocation,clearEnd.out_projectedlocation,filter,C_Controller_78a9db5e_null())
        verify(!clear.out_returnvalue,"clear NavMesh ray")
    } elif(phase==1) {
        let path=ue_navigation_FindPathToLocationSynchronously(origin,end,actor,filter)
        verify(C_NavigationPath_550114bf_valid(path) && ue_path_IsValid(path) && !ue_path_IsPartial(path),"complete synchronous path")
        verify(ue_path_GetPathLength(path)>1200.0lf && ue_path_GetPathCost(path)>0.0lf,"path detours around obstacle")
        let cost=ue_navigation_GetPathCost(origin,end,data,filter)
        let size=ue_navigation_GetPathLength(origin,end,data,filter)
        verify(cost.out_returnvalue==E_ENavigationQueryResult_c4f9ee47.v_Success && cost.out_pathcost>0.0lf,"cost Success and out double")
        verify(size.out_returnvalue==E_ENavigationQueryResult_c4f9ee47.v_Success && size.out_pathlength>1200.0lf,"length Success and out double")
        // Owner is at Start: actor-goal path traverses the same obstacle in reverse.
        let toActor=ue_navigation_FindPathToActorSynchronously(end,actor,50.0,actor,filter)
        verify(C_NavigationPath_550114bf_valid(toActor) && ue_path_IsValid(toActor) && !ue_path_IsPartial(toActor) && ue_path_GetPathLength(toActor)>1200.0lf,"complete Actor-goal path")
    } elif(phase==2) {
        let island=ue_vector(3000.0lf,0.0lf,50.0lf)
        verify(ue_navigation_K2_ProjectPointToNavigation(island,data,filter,extent).out_returnvalue,"disconnected island is navigable")
        let partial=ue_navigation_FindPathToLocationSynchronously(origin,island,actor,filter)
        verify(C_NavigationPath_550114bf_valid(partial) && ue_path_IsValid(partial) && ue_path_IsPartial(partial),"island cannot produce complete path")
        let outside=ue_navigation_K2_ProjectPointToNavigation(ue_vector(10000.0lf,10000.0lf,50.0lf),data,filter,extent)
        verify(!outside.out_returnvalue,"outside NavMesh projection fails")
    } else { return -1 }
    return 42
}
