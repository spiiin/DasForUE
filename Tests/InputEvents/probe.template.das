options gen2
options gc = true
require _MODULE_
require ue_input_events

var savedKeys:array<UEKeyEvent>
var savedPointer:UEPointerEvent
var savedBase:UEInputEvent
var savedRawKey:UEStructValue
var checks:int=0
def expect_event(good:bool):void { if(!good){panic("input event getter mismatch at {checks}")};checks+=1 }

[export]
def accept_events(base:UEStructValue;key:UEStructValue;analog:UEStructValue;character:UEStructValue;pointer:UEStructValue;navigation:UEStructValue):int {
    checks=0
    let b=ue_input_event(base)
    let k=ue_key_event(key)
    let a=ue_analog_input_event(analog)
    let c=ue_character_event(character)
    let p=ue_pointer_event(pointer)
    let n=ue_navigation_event(navigation)
    expect_event(ue_input_key_InputEvent_IsAltDown(b))
    expect_event(ue_input_key_InputEvent_IsCommandDown(b))
    expect_event(ue_input_key_InputEvent_IsControlDown(b))
    expect_event(ue_input_key_InputEvent_IsLeftAltDown(b))
    expect_event(!ue_input_key_InputEvent_IsLeftCommandDown(b))
    expect_event(!ue_input_key_InputEvent_IsLeftControlDown(b))
    expect_event(ue_input_key_InputEvent_IsLeftShiftDown(b))
    expect_event(ue_input_key_InputEvent_IsRepeat(b))
    expect_event(!ue_input_key_InputEvent_IsRightAltDown(b))
    expect_event(ue_input_key_InputEvent_IsRightCommandDown(b))
    expect_event(ue_input_key_InputEvent_IsRightControlDown(b))
    expect_event(!ue_input_key_InputEvent_IsRightShiftDown(b))
    expect_event(ue_input_key_InputEvent_IsShiftDown(b))
    expect_event(ue_input_key_GetAnalogValue(a)==0.75)
    expect_event(ue_input_key_GetKey(k).f_keyname.value=="SpaceBar")
    expect_event(ue_input_key_GetUserIndex(k)==7)
    let delta=ue_input_key_PointerEvent_GetCursorDelta(p)
    expect_event(delta.x==2.0lf && delta.y==1.0lf)
    let effect=ue_input_key_PointerEvent_GetEffectingButton(p)
    expect_event(effect.f_keyname.value=="LeftMouseButton")
    let gesture=ue_input_key_PointerEvent_GetGestureDelta(p)
    expect_event(gesture.x==0.0lf && gesture.y==0.5lf)
    expect_event(int(ue_input_key_PointerEvent_GetGestureType(p))==0)
    let last=ue_input_key_PointerEvent_GetLastScreenSpacePosition(p)
    expect_event(last.x==10.0lf && last.y==19.0lf)
    expect_event(ue_input_key_PointerEvent_GetPointerIndex(p)==3)
    let screen=ue_input_key_PointerEvent_GetScreenSpacePosition(p)
    expect_event(screen.x==12.0lf && screen.y==20.0lf)
    expect_event(ue_input_key_PointerEvent_GetTouchpadIndex(p)==0)
    expect_event(ue_input_key_PointerEvent_GetUserIndex(p)==7)
    expect_event(ue_input_key_PointerEvent_GetWheelDelta(p)==0.5)
    expect_event(ue_input_key_PointerEvent_IsMouseButtonDown(p,effect))
    expect_event(!ue_input_key_PointerEvent_IsTouchEvent(p))
    expect_event(ue_input_key_InputEvent_IsShiftDown(ue_ui_GetInputEventFromKeyEvent(k)))
    expect_event(ue_input_key_GetKey(ue_ui_GetKeyEventFromAnalogInputEvent(a)).f_keyname.value=="Gamepad_LeftX")
    expect_event(ue_input_key_InputEvent_IsShiftDown(ue_ui_GetInputEventFromCharacterEvent(c)))
    expect_event(ue_input_key_InputEvent_IsShiftDown(ue_ui_GetInputEventFromPointerEvent(p)))
    expect_event(ue_input_key_InputEvent_IsShiftDown(ue_ui_GetInputEventFromNavigationEvent(n)))
    push(savedKeys,k)
    savedKeys <- clone(savedKeys)
    savedPointer := p
    savedBase := ue_ui_GetInputEventFromPointerEvent(p)
    savedRawKey := key
    ue_struct_collect()
    return checks
}
[export]
def read_saved(input:int):int {
    ue_struct_collect()
    if(length(savedKeys)!=1){return -1}
    if(ue_input_key_GetKey(savedKeys[0]).f_keyname.value!="SpaceBar"){return -2}
    if(ue_input_key_PointerEvent_GetPointerIndex(savedPointer)!=3){return -3}
    if(!ue_input_key_InputEvent_IsShiftDown(savedBase)){return -4}
    return 1
}
[export]
def bad_default(input:int):int { var k:UEKeyEvent;let key=ue_input_key_GetKey(k);return 0 }
[export]
def bad_kind(input:int):int { let p=ue_pointer_event(savedRawKey);return ue_input_key_PointerEvent_GetUserIndex(p) }
[export]
def bad_navigation(input:int):int { let call=ue_begin_static_call("/Script/Engine.KismetInputLibrary","Key_GetNavigationDirectionFromAnalog","");return 0 }
[export]
def bad_unassigned(input:int):int { let call=ue_begin_static_call("/Script/Engine.KismetInputLibrary","GetKey","");ue_invoke_call(call);return 0 }
