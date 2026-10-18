options gen2
options gc
require @COLLISION@
require ue_constraint_fixture

[export]
def main(input:int):int {
    let component=ue_constraint_fixture_CreateTestConstraint()
    let joint=@COLLISION@::@COMPONENT@_from(ue_constraint_fixture::@COMPONENT@_raw(component))
    let handle=@COLLISION@::ue_constraint_GetConstraint(joint)
    verify(ue_constraint_valid(handle),"live component constraint capability")
    let set_result=@COLLISION@::ue_joint_SetDisableCollision(handle,true)
    verify(ue_constraint_valid(set_result),"library ref/out returns checked capability")
    let read=@COLLISION@::ue_joint_GetDisableCollsion(set_result)
    verify(read.out_returnvalue,"native constraint library mutation/query")
    @COLLISION@::ue_joint_SetDisableCollision(read.out_accessor,false)
    let pending=ue_begin_static_call("/Script/Engine.ConstraintInstanceBlueprintLibrary","SetDisableCollision","")
    __ue_arg_constraint(pending,"accessor",handle)
    ue_arg_bool(pending,"bdisablecollision",true)
    let held=ue_begin_call(ue_constraint_fixture::@COMPONENT@_raw(component),"GetConstraint")
    ue_invoke_call(held)
    ue_constraint_fixture_RebuildConstraint(component)
    verify(!ue_constraint_valid(handle),"native physical generation invalidates existing capability")
    var rejected=false
    try { ue_invoke_call(pending) } recover { rejected=true }
    verify(rejected,"pending call revalidates generation before side effects")
    ue_release_call(pending)
    rejected=false
    try { let bad=__ue_result_constraint(held,"returnvalue") } recover { rejected=true }
    verify(rejected,"getter output generation captured at invocation, not late result read")
    ue_release_call(held)
    let fresh=@COLLISION@::ue_constraint_GetConstraint(joint)
    ue_struct_collect()
    verify(ue_constraint_valid(fresh),"fresh capability after recreation")
    let unchanged=@COLLISION@::ue_joint_GetDisableCollsion(fresh)
    verify(!unchanged.out_returnvalue,"stale pending mutation had no side effects")
    rejected=false
    try { @COLLISION@::ue_joint_SetDisableCollision(handle,true) } recover { rejected=true }
    verify(rejected,"old capability cannot silently retarget new physical constraint")
    var empty:UEConstraintHandle
    verify(!ue_constraint_valid(empty),"uninitialized handle invalid")
    rejected=false
    try { @COLLISION@::ue_joint_SetDisableCollision(empty,true) } recover { rejected=true }
    verify(rejected,"uninitialized accessor rejected")
    let wrong=ue_begin_call(ue_constraint_fixture::@COMPONENT@_raw(component),"SetDisableCollision")
    rejected=false
    try { __ue_arg_constraint(wrong,"bdisablecollision",fresh) } recover { rejected=true }
    verify(rejected,"wrong native argument type rejected")
    ue_release_call(wrong)
    let unassigned=ue_begin_static_call("/Script/Engine.ConstraintInstanceBlueprintLibrary","SetDisableCollision","")
    ue_arg_bool(unassigned,"bdisablecollision",true)
    rejected=false
    try { ue_invoke_call(unassigned) } recover { rejected=true }
    verify(rejected,"unassigned accessor cannot become a native default accessor")
    ue_release_call(unassigned)
    let direction=ue_begin_call(ue_constraint_fixture::@COMPONENT@_raw(component),"GetConstraint")
    rejected=false
    try { __ue_arg_constraint(direction,"returnvalue",fresh) } recover { rejected=true }
    verify(rejected,"return value is not an input capability")
    rejected=false
    try { let early=__ue_result_constraint(direction,"returnvalue") } recover { rejected=true }
    verify(rejected,"unexecuted result rejected")
    ue_release_call(direction)
    let alive_frame=ue_begin_call(ue_constraint_fixture::@COMPONENT@_raw(component),"GetConstraint")
    ue_invoke_call(alive_frame)
    let alive_snapshot=__ue_result_constraint(alive_frame,"returnvalue")
    ue_constraint_fixture_DestroyOwner(component)
    ue_constraint_fixture_CollectWeak()
    verify(ue_constraint_fixture_LastOwnerCollected(),"capability and held native output must not pin component owner through GC")
    verify(!ue_constraint_valid(fresh) && !ue_constraint_valid(alive_snapshot),"Destroy invalidates handles")
    rejected=false
    try { let bad=__ue_result_constraint(alive_frame,"returnvalue") } recover { rejected=true }
    verify(rejected,"destroyed owner in held getter frame rejected")
    ue_release_call(alive_frame)
    print("DAS_CONSTRAINT_TRANSPORT_PASS generation pending getter provenance weak GC destroy guards\n")
    return 42
}
