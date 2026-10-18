options gen2
require ue_foundation
var elapsed:double=0.0lf
var remembered:UEPhysicsBodyToken
var seen:bool=false
[export]
def main(input:int):int { return input }
[export]
def physics_tick(dt:float; time:float) {
    let mode=int(ue_physics_input(15))
    ue_physics_output(0,elapsed)
    if(mode==0) { return }
    let body=ue_physics_body(int(ue_physics_input(10)))
    if(mode==7) {
        if(!seen) { remembered=body; seen=true; return }
        ue_physics_add_force(remembered,1000.0lf,0.0lf,0.0lf,false)
        return
    }
    if(mode==3) { ue_physics_add_force(body,1.0lf/ue_physics_input(14),0.0lf,0.0lf,false); return }
    if(mode==4) { ue_physics_add_force(body,1.0e308lf,0.0lf,0.0lf,true); return }
    if(mode==5) { for(i in range(0,65)) { ue_physics_add_force(body,1.0lf,0.0lf,0.0lf,false) }; return }
    if(mode==6) { ue_physics_add_force(default<UEPhysicsBodyToken>,1.0lf,0.0lf,0.0lf,false); return }
    if(mode==9) { try { ue_physics_add_force(default<UEPhysicsBodyToken>,1.0lf,0.0lf,0.0lf,false) } recover { } }
    if(mode==10) {
        var copy:array<UEPhysicsBodyToken>
        push(copy,body)
        ue_physics_add_force(copy[0],ue_physics_input(0),ue_physics_input(1),ue_physics_input(2),false)
    } else {
        ue_physics_add_force(body,ue_physics_input(0),ue_physics_input(1),ue_physics_input(2),mode==2)
    }
    if(mode==8) { ue_physics_output(16,1.0lf) }
    elapsed+=double(dt)
    ue_physics_output(0,elapsed)
}
