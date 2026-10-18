options gen2
require @COLLISION@
[export]
def main(phase:int):int {
    let find=ue_begin_call(ue_self_object(),"GetComponentByClass")
    ue_require_call_context(find,false)
    ue_arg_class(find,"componentclass",__ue_class_loaded("/Script/Engine.PhysicsConstraintComponent","/Script/CoreUObject.Object",""))
    ue_invoke_call(find)
    let joint=C_PhysicsConstraintComponent_522ccfed_from(ue_result_object(find,"returnvalue"))
    ue_release_call(find)
    let pair=ue_constraint_GetConstrainedComponents(joint)
    let first=pair.out_outcomponent1
    let second=pair.out_outcomponent2
    verify(C_PrimitiveComponent_5efdd477_valid(first) && C_PrimitiveComponent_5efdd477_valid(second),"two live bodies")
    verify(!__ue_same_object(C_PrimitiveComponent_5efdd477_raw(first),C_PrimitiveComponent_5efdd477_raw(second)),"distinct constrained bodies")
    let handle=ue_constraint_GetConstraint(joint)
    verify(ue_constraint_valid(handle),"live physical constraint handle")
    let none=ue_name("None")
    let locked=E_ELinearConstraintMotion_2cabf017.v_LCM_Locked
    let free=E_ELinearConstraintMotion_2cabf017.v_LCM_Free
    let limited=E_ELinearConstraintMotion_2cabf017.v_LCM_Limited
    let angular_locked=E_EAngularConstraintMotion_61d05f8e.v_ACM_Locked
    let angular_free=E_EAngularConstraintMotion_61d05f8e.v_ACM_Free
    let angular_limited=E_EAngularConstraintMotion_61d05f8e.v_ACM_Limited
    ue_joint_SetDisableCollision(handle,true)
    ue_joint_SetLinearBreakable(handle,false,1000000.0)
    ue_joint_SetAngularBreakable(handle,false,1000000.0)
    if(phase!=9 && phase!=11) {
        ue_joint_SetLinearLimits(handle,locked,locked,locked,0.0)
        ue_joint_SetAngularLimits(handle,angular_locked,0.0,angular_locked,0.0,angular_locked,0.0)
    }
    if(phase==0) {
        ue_body_AddImpulse(second,ue_vector(100.0lf,0.0lf,0.0lf),none,false)
    } elif(phase==1 || phase==8) {
        let changed=ue_joint_SetLinearLimits(handle,free,locked,locked,0.0)
        let state=ue_joint_GetLinearLimits(changed)
        verify(state.out_xmotion==free && state.out_ymotion==locked,"free linear getter/ref-out")
        ue_body_AddImpulse(second,ue_vector(100.0lf,0.0lf,0.0lf),none,false)
    } elif(phase==2 || phase==9) {
        let limit=phase==2 ? 25.0 : 10.0
        let changed=ue_joint_SetLinearLimits(handle,limited,locked,locked,limit)
        let state=ue_joint_GetLinearLimits(changed)
        verify(state.out_xmotion==limited && state.out_limit==limit,"linear limits getter/ref-out")
        if(phase==2) { ue_body_AddImpulse(second,ue_vector(100.0lf,0.0lf,0.0lf),none,false) }
    } elif(phase==3) {
        ue_body_SetPhysicsAngularVelocityInDegrees(second,ue_vector(90.0lf,0.0lf,0.0lf),false,none)
    } elif(phase==4 || phase==10) {
        let changed=ue_joint_SetAngularLimits(handle,angular_locked,0.0,angular_locked,0.0,angular_free,0.0)
        let state=ue_joint_GetAngularLimits(changed)
        verify(state.out_twistmotiontype==angular_free,"free angular getter/ref-out")
        ue_body_SetPhysicsAngularVelocityInDegrees(second,ue_vector(90.0lf,0.0lf,0.0lf),false,none)
    } elif(phase==5 || phase==11) {
        let limit=phase==5 ? 20.0 : 10.0
        let changed=ue_joint_SetAngularLimits(handle,angular_locked,0.0,angular_locked,0.0,angular_limited,limit)
        let state=ue_joint_GetAngularLimits(changed)
        verify(state.out_twistmotiontype==angular_limited && state.out_twistlimitangle==limit,"angular limits getter/ref-out")
        if(phase==5) { ue_body_SetPhysicsAngularVelocityInDegrees(second,ue_vector(90.0lf,0.0lf,0.0lf),false,none) }
    } elif(phase==6) {
        ue_constraint_BreakConstraint(joint)
        verify(ue_constraint_IsBroken(joint) && !ue_constraint_valid(handle),"break invalidates snapshot")
        var rejected=false
        try { ue_joint_SetDisableCollision(handle,false) } recover { rejected=true }
        verify(rejected,"broken handle setter rejected")
        ue_body_AddImpulse(second,ue_vector(100.0lf,0.0lf,0.0lf),none,false)
    } elif(phase==7) {
        let pending=ue_begin_static_call("/Script/Engine.ConstraintInstanceBlueprintLibrary","SetDisableCollision","")
        __ue_arg_constraint(pending,"accessor",handle)
        ue_arg_bool(pending,"bdisablecollision",false)
        ue_constraint_BreakConstraint(joint)
        ue_constraint_SetConstrainedComponents(joint,first,none,second,none)
        verify(!ue_constraint_IsBroken(joint) && !ue_constraint_valid(handle),"same bodies and broken state, new generation")
        var rejected=false
        try { ue_invoke_call(pending) } recover { rejected=true }
        verify(rejected,"pending stale setter rejected before dispatch")
        ue_release_call(pending)
        rejected=false
        try { ue_joint_SetDisableCollision(handle,false) } recover { rejected=true }
        verify(rejected,"snapshot from old generation rejected")
        let fresh=ue_constraint_GetConstraint(joint)
        verify(ue_constraint_valid(fresh),"fresh accessor after physical recreation")
        let state=ue_joint_GetDisableCollsion(fresh)
        verify(state.out_returnvalue,"stale mutations did not alter new joint")
        let motion=ue_joint_GetLinearLimits(state.out_accessor)
        verify(motion.out_xmotion==locked,"recreated locked profile")
        ue_body_AddImpulse(second,ue_vector(100.0lf,0.0lf,0.0lf),none,false)
    } else { return -1 }
    return 42
}
