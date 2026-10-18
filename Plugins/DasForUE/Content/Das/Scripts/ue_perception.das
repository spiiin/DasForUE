options gen2
module ue_perception public
require ue_character_control public

[ue_native_enum(path="/Script/DasRuntime.EDasAISense")]
enum AISense {
    Invalid=0
    Sight=1
    Hearing=2
    Other=3
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
