options gen2
module ue_input_value public
require ue_foundation public

enum InputValueType {
    Boolean
    Axis1D
    Axis2D
    Axis3D
}

// Values are owned copies. Getters follow UE Get<T>(), including cross-type reads.
def ue_input_value(value:bool):InputActionValue { return __ue_input_make(0,value ? 1.0lf : 0.0lf,0.0lf,0.0lf) }
def ue_input_value(value:float):InputActionValue { return __ue_input_make(1,double(value),0.0lf,0.0lf) }
def ue_input_value(value:double):InputActionValue { return __ue_input_make(1,value,0.0lf,0.0lf) }
def ue_input_value(value:UEVector2D):InputActionValue { return __ue_input_make(2,value.x,value.y,0.0lf) }
def ue_input_value(value:UEVector):InputActionValue { return __ue_input_make(3,value.x,value.y,value.z) }

def ue_input_type(value:InputActionValue):InputValueType {
    let kind=__ue_input_kind(value)
    if (kind==0) { return InputValueType.Boolean }
    if (kind==1) { return InputValueType.Axis1D }
    if (kind==2) { return InputValueType.Axis2D }
    return InputValueType.Axis3D
}
