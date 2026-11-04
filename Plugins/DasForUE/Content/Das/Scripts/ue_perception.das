options gen2
module ue_perception public
require ue_character_control public

[ue_native_enum(path="/Script/DasRuntime.EDasAISense")]
enum AISense {
    Invalid=0
    Sight=1
    Hearing=2
    Other=3
    Damage=4
}

// Snapshot of a native stimulus, captured when UE emits the event.
// Editing this value never changes the perception component's native memory.
[ue_native_struct(path="/Script/DasRuntime.DasAIStimulus")]
struct AIStimulus {
    Sense:AISense
    SuccessfullySensed:bool
    Expired:bool
    Valid:bool
    Age:float
    ExpirationAge:float
    Strength:float
    StimulusLocation:UEVector
    ReceiverLocation:UEVector
    Tag:UENameValue
}

[ue_native_object(path="/Script/AIModule.AIPerceptionComponent")]
struct PerceptionComponentRef { handle:UObjectHandle }

// Configure reusable sight/hearing subobjects owned strongly by the native component.
def ue_perception_configure(component:PerceptionComponentRef; sight:float; loseSight:float;
                            angle:float; hearing:float; maxAge:float; enemies:bool=true; friendlies:bool=true; neutrals:bool=true):void {
    __ue_perception_configure(component.handle,sight,loseSight,angle,hearing,maxAge,enemies,friendlies,neutrals)
}

// Damage stimuli are delivered by the real UE perception system.
def ue_perception_configure_damage(component:PerceptionComponentRef; maxAge:float):void {
    __ue_perception_configure_damage(component.handle,maxAge)
}

def ue_perception_report_damage(damaged:ActorRef; instigator:ActorRef; amount:float;
                                 eventLocation:UEVector; hitLocation:UEVector):void {
    __ue_perception_report_damage(damaged.handle,instigator.handle,amount,eventLocation,hitLocation)
}

def ue_controller_is_friendly(controller:ControllerRef; other:ActorRef):bool {
    return __ue_controller_is_friendly(controller.handle,other.handle)
}
