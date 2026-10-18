options gen2
require ue_foundation
var seconds:double=0.0lf
[export]
def physics_tick(dt:float; time:float) {
    let body=ue_physics_body(0)
    ue_physics_add_force(body,ue_physics_input(0),0.0lf,0.0lf,false)
    seconds+=double(dt)
    ue_physics_output(0,seconds)
    ue_physics_output(1,ue_physics_input(0))
}
