options gen2
module ue_input_events public
require ue_foundation public

// Only trusted native snapshots can be unwrapped; no script event constructors.
def ue_input_event(value:UEStructValue):UEInputEvent { return __ue_unpack_inputevent(value) }
def ue_key_event(value:UEStructValue):UEKeyEvent { return __ue_unpack_keyevent(value) }
def ue_analog_input_event(value:UEStructValue):UEAnalogInputEvent { return __ue_unpack_analoginputevent(value) }
def ue_character_event(value:UEStructValue):UECharacterEvent { return __ue_unpack_characterevent(value) }
def ue_pointer_event(value:UEStructValue):UEPointerEvent { return __ue_unpack_pointerevent(value) }
def ue_navigation_event(value:UEStructValue):UENavigationEvent { return __ue_unpack_navigationevent(value) }
