options gen2
require ue_ai
var @ue_state events:int = 0
var @ue_state code:int = -1
[ue_class(module_id="aimovereload",Blueprintable)]
class AIMoveCallbacks : Actor {
    [ue_function(BlueprintCallable)]
    def Received(result:MoveResult):void {
        events = (events+1)
        ue_set_self_int("MoveCompletions",ue_get_self_int("MoveCompletions")+1)
        code = (int(result.Code))
    }
}
[export]
def main(input:int):int {
    let controller=ue_ai_self_controller()
    let goal=ue_vector(-1600.0lf,-1000.0lf,0.0lf)
    if(input==0) {
        verify(ue_ai_move_to_location("probe",controller,goal,"ue_AIMoveCallbacks_Received",10.0f),"start location move")
        var rejected=0
        try { ue_ai_move_to_location("bad",controller,goal,"ue_AIMoveCallbacks_Received",-1.0f) } recover { rejected+=1 }
        try { ue_ai_move_to_location("bad",controller,goal,"missing_handler") } recover { rejected+=1 }
        try { ue_ai_move_to_actor("bad",controller,default<ActorRef>,"ue_AIMoveCallbacks_Received") } recover { rejected+=1 }
        try { ue_ai_move_to_location("bad",default<AIControllerRef>,goal) } recover { rejected+=1 }
        verify(rejected==4 && ue_ai_move_active("probe"),"invalid arguments must preserve current request")
        return 1
    }
    if(input==2) { return ue_ai_move_active("probe") ? 1 : 0 }
    if(input==3) { return ue_ai_move_cancel("probe") ? 1 : 0 }
    if(input==4) { return code }
    if(input==5) {
        // Same position: UE reports AlreadyAtGoal synchronously, adapter defers callback.
        let own=ue_self_actor()
        return ue_ai_move_to_actor("probe",controller,own,"ue_AIMoveCallbacks_Received") ? 1 : 0
    }
    if(input==6) {
        return ue_ai_move_to_location("probe",controller,ue_vector(100000.0lf,100000.0lf,0.0lf),"ue_AIMoveCallbacks_Received",10.0f) ? 1 : 0
    }
    if(input==7) { return ue_ai_move_to_location("probe",controller,goal) ? 1 : 0 }
    return events // REVISION_MARKER
}
