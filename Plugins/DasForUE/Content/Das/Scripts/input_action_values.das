options gen2
require ue_input_value

[export]
def main(input:int):int {
    let jump:InputActionValue=ue_input_value(true)
    let throttle=ue_input_value(0.5)
    let move=ue_input_value(ue_vector2d(1.0lf,0.0lf))
    let look=ue_input_value(ue_vector(0.0lf,1.0lf,0.0lf))
    let direction=ue_input_axis2d(move)
    if (ue_input_bool(jump) && ue_input_axis1d(throttle)>0.0lf &&
        direction.x>0.0lf && ue_input_type(look)==InputValueType.Axis3D) {
        return 42
    }
    return 0
}
