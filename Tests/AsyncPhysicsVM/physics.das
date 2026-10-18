options gen2
require ue_foundation
var ticks:int=0
[export]
def main(input:int):int { return ticks+input }
[export]
def physics_tick(dt:float; time:float) {
    ticks+=1
    ue_physics_output(0,ue_physics_input(0)*2.0lf)
    ue_physics_output(1,double(ticks))
    ue_physics_output(2,double(dt))
    ue_physics_output(3,double(time))
    let mode=int(ue_physics_input(15))
    if(mode==1) { ue_physics_output(16,1.0lf) }
    if(mode==2) { ue_physics_output(5,1.0lf/ue_physics_input(14)) }
    if(mode==4) { try { ue_physics_output(-1,1.0lf) } recover { ue_physics_output(0,999.0lf) } }
    if(mode==3) { ue_physics_output(5,double(ue_add(1,2))) }
}
