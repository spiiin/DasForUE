options gen2
require @COLLISION@
[export]
def main(phase:int):int {
    let call=ue_begin_call(ue_self_object(),"K2_GetRootComponent")
    ue_require_call_context(call,false)
    ue_invoke_call(call)
    let body=C_PrimitiveComponent_5efdd477_from(ue_result_object(call,"returnvalue"))
    ue_release_call(call)
    let find=ue_begin_call(ue_self_object(),"GetComponentByClass")
    ue_require_call_context(find,false)
    ue_arg_class(find,"componentclass",__ue_class_loaded("/Script/Engine.PhysicsHandleComponent","/Script/CoreUObject.Object",""))
    ue_invoke_call(find)
    let handle=C_PhysicsHandleComponent_c490d7a8_from(ue_result_object(find,"returnvalue"))
    ue_release_call(find)
    let zero=ue_vector(0.0lf,0.0lf,0.0lf)
    let none=ue_name("None")
    if(phase==0) {
        ue_body_SetEnableGravity(body,false)
        ue_body_SetSimulatePhysics(body,true)
        ue_body_SetMassOverrideInKg(body,none,10.0,true)
        ue_body_SetLinearDamping(body,0.0)
        ue_body_SetAngularDamping(body,0.0)
        verify(ue_body_GetMass(body)>9.9 && ue_body_GetMass(body)<10.1,"mass override")
        ue_body_SetPhysicsLinearVelocity(body,ue_vector(100.0lf,0.0lf,0.0lf),false,none)
        ue_body_SetPhysicsAngularVelocityInDegrees(body,ue_vector(0.0lf,0.0lf,90.0lf),false,none)
    } elif(phase==1) {
        ue_body_SetPhysicsLinearVelocity(body,zero,false,none)
        ue_body_SetPhysicsAngularVelocityInDegrees(body,zero,false,none)
        ue_body_AddImpulse(body,ue_vector(1000.0lf,0.0lf,0.0lf),none,false)
    } elif(phase==2 || phase==3) {
        if(phase==2) { ue_body_SetPhysicsLinearVelocity(body,zero,false,none) }
        ue_body_AddForce(body,ue_vector(1000.0lf,0.0lf,0.0lf),none,false)
    } elif(phase==4) {
        ue_body_SetLinearDamping(body,4.0)
        ue_body_SetAngularDamping(body,4.0)
        verify(ue_body_GetLinearDamping(body)==4.0 && ue_body_GetAngularDamping(body)==4.0,"damping getters")
        ue_body_SetPhysicsLinearVelocity(body,ue_vector(100.0lf,0.0lf,0.0lf),false,none)
        ue_body_SetPhysicsAngularVelocityInDegrees(body,ue_vector(0.0lf,0.0lf,90.0lf),false,none)
    } elif(phase==5) {
        ue_body_SetLinearDamping(body,0.0)
        ue_body_SetAngularDamping(body,0.0)
        ue_body_SetPhysicsLinearVelocity(body,zero,false,none)
        ue_body_SetPhysicsAngularVelocityInDegrees(body,zero,false,none)
        ue_body_SetEnableGravity(body,true)
    } elif(phase==6) {
        ue_body_SetEnableGravity(body,false)
        ue_body_SetPhysicsLinearVelocity(body,zero,false,none)
        ue_body_SetPhysicsAngularVelocityInDegrees(body,zero,false,none)
        ue_body_SetLinearDamping(body,1.0)
        ue_handle_SetLinearStiffness(handle,5000.0)
        ue_handle_SetLinearDamping(handle,500.0)
        ue_handle_SetAngularStiffness(handle,5000.0)
        ue_handle_SetAngularDamping(handle,500.0)
        ue_handle_SetInterpolationSpeed(handle,50.0)
        let position=ue_body_GetCenterOfMass(body,none)
        ue_handle_GrabComponentAtLocationWithRotation(handle,body,none,position,ue_rotator(0.0lf,0.0lf,0.0lf))
        verify(__ue_same_object(C_PrimitiveComponent_5efdd477_raw(ue_handle_GetGrabbedComponent(handle)),C_PrimitiveComponent_5efdd477_raw(body)),"grabbed component identity")
        ue_handle_SetTargetLocation(handle,ue_vector(position.x+100.0lf,position.y,position.z+50.0lf))
    } elif(phase==7) {
        let target=ue_handle_GetTargetLocationAndRotation(handle)
        ue_handle_SetTargetLocationAndRotation(handle,ue_vector(target.out_targetlocation.x,target.out_targetlocation.y+100.0lf,target.out_targetlocation.z),ue_rotator(0.0lf,90.0lf,0.0lf))
    } elif(phase==8) {
        ue_handle_ReleaseComponent(handle)
        verify(!C_PrimitiveComponent_5efdd477_valid(ue_handle_GetGrabbedComponent(handle)),"release is nullable")
        ue_handle_ReleaseComponent(handle)
        ue_body_SetLinearDamping(body,0.0)
        ue_body_SetPhysicsLinearVelocity(body,ue_vector(60.0lf,0.0lf,0.0lf),false,none)
    } elif(phase==9) {
        var rejected=false
        try { ue_handle_ReleaseComponent(C_PhysicsHandleComponent_c490d7a8_null()) } recover { rejected=true }
        verify(rejected,"null physics handle receiver rejected")
        verify(ue_body_GetPhysicsLinearVelocity(body,none).x>50.0lf,"generated velocity read after stepping")
    } else { return -1 }
    return 42
}
