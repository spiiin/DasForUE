options gen2
module ue_ai public
require ue_character_control public
require ue_gameplay_services public
[ue_native_object(path="/Script/AIModule.AIController")]
struct AIControllerRef { handle:UObjectHandle }
def AIControllerRef_from(value:UObjectHandle):AIControllerRef { return AIControllerRef(handle=ue_cast_nullable_object(value,"/Script/AIModule.AIController")) }
[ue_native_enum(path="/Script/DasRuntime.EDasMoveCode")]
enum MoveCode {
    Success=0
    Blocked=1
    OffPath=2
    Aborted=3
    Invalid=4
}
[ue_native_struct(path="/Script/DasRuntime.DasMoveResult")]
struct MoveResult {
    Code:MoveCode
    RequestId:int
}
def ue_ai_self_controller():AIControllerRef { return AIControllerRef_from(__ue_ai_self()) }
// false still queues an Invalid result, except invalid arguments which throw.
def ue_ai_move_to_actor(name:string; controller:AIControllerRef; target:ActorRef; handler:string=""; radius:float=50.0f; pathfinding:bool=true; partial:bool=false):bool {
    return __ue_ai_move(name,controller.handle,target.handle,ue_vector(0.0lf,0.0lf,0.0lf),true,handler,radius,pathfinding,partial)
}
def ue_ai_move_to_location(name:string; controller:AIControllerRef; location:UEVector; handler:string=""; radius:float=50.0f; pathfinding:bool=true; partial:bool=false):bool {
    return __ue_ai_move(name,controller.handle,default<UObjectHandle>,location,false,handler,radius,pathfinding,partial)
}
def ue_ai_move_cancel(name:string):bool { return __ue_ai_cancel(name) }
def ue_ai_move_active(name:string):bool { return __ue_ai_active(name) }
// Same damage adapter, including an AI controller as the instigator.
def ue_apply_damage(target:ActorRef; amount:float; source:ActorRef; instigator:AIControllerRef):float { return __ue_apply_damage(target.handle,amount,source.handle,instigator.handle) }
