options gen2
module ue_enhanced_input public
require ue_dsl public
require ue_input_value public

[ue_native_object(path="/Script/EnhancedInput.InputAction")]
struct InputActionRef { handle:UObjectHandle }

def InputActionRef_from(value:UObjectHandle):InputActionRef {
    return InputActionRef(handle=ue_cast_nullable_object(value,"/Script/EnhancedInput.InputAction"))
}
def InputActionRef_raw(value:InputActionRef):UObjectHandle {
    return ue_cast_nullable_object(value.handle,"/Script/EnhancedInput.InputAction")
}
// Enhanced Input trigger events (ETriggerEvent). Prefer these over trigger strings.
enum TriggerEvent {
    Triggered
    Started
    Ongoing
    Canceled
    Completed
}
def ue_trigger_event_name(trigger:TriggerEvent):string {
    return "{trigger}"
}
def ue_enhanced_bind(action:InputActionRef; trigger:string; handler:string):void {
    __ue_enhanced_bind(InputActionRef_raw(action),trigger,handler)
}
def ue_enhanced_bind(action:InputActionRef; trigger:TriggerEvent; handler:string):void {
    __ue_enhanced_bind(InputActionRef_raw(action),ue_trigger_event_name(trigger),handler)
}
def ue_enhanced_unbind(action:InputActionRef; trigger:string):bool {
    return __ue_enhanced_unbind(InputActionRef_raw(action),trigger)
}
def ue_enhanced_unbind(action:InputActionRef; trigger:TriggerEvent):bool {
    return __ue_enhanced_unbind(InputActionRef_raw(action),ue_trigger_event_name(trigger))
}

[ue_native_object(path="/Script/Engine.PlayerController")]
struct PlayerControllerRef { handle:UObjectHandle }
def PlayerControllerRef_from(value:UObjectHandle):PlayerControllerRef {
    return PlayerControllerRef(handle=ue_cast_nullable_object(value,"/Script/Engine.PlayerController"))
}
def PlayerControllerRef_raw(value:PlayerControllerRef):UObjectHandle {
    return ue_cast_nullable_object(value.handle,"/Script/Engine.PlayerController")
}
[ue_native_object(path="/Script/EnhancedInput.InputMappingContext")]
struct InputMappingContextRef { handle:UObjectHandle }
def InputMappingContextRef_from(value:UObjectHandle):InputMappingContextRef {
    return InputMappingContextRef(handle=ue_cast_nullable_object(value,"/Script/EnhancedInput.InputMappingContext"))
}
def InputMappingContextRef_raw(value:InputMappingContextRef):UObjectHandle {
    return ue_cast_nullable_object(value.handle,"/Script/EnhancedInput.InputMappingContext")
}
def ue_enhanced_add_context(player:PlayerControllerRef; context:InputMappingContextRef; priority:int):void {
    __ue_enhanced_add_context(PlayerControllerRef_raw(player),InputMappingContextRef_raw(context),priority)
}
def ue_enhanced_remove_context(player:PlayerControllerRef; context:InputMappingContextRef):bool {
    return __ue_enhanced_remove_context(PlayerControllerRef_raw(player),InputMappingContextRef_raw(context))
}
def ue_enhanced_has_context(player:PlayerControllerRef; context:InputMappingContextRef):bool {
    return __ue_enhanced_has_context(PlayerControllerRef_raw(player),InputMappingContextRef_raw(context))
}
