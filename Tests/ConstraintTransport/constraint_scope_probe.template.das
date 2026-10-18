options gen2
require @COLLISION@
require ue_constraint_fixture
[export]
def main(input:int):int {
    if(input==0) {
        let c=ue_constraint_fixture_CreateTestConstraint()
        let component=@COLLISION@::@COMPONENT@_from(ue_constraint_fixture::@COMPONENT@_raw(c))
        let handle=@COLLISION@::ue_constraint_GetConstraint(component)
        unsafe { return int(*reinterpret<uint64?>(addr(handle))) }
    }
    var foreign:UEConstraintHandle
    unsafe { *reinterpret<uint64?>(addr(foreign))=uint64(input) }
    verify(!ue_constraint_valid(foreign),"capability from previous context is invalid")
    var rejected=false
    try { @COLLISION@::ue_joint_SetDisableCollision(foreign,true) } recover { rejected=true }
    verify(rejected,"foreign context rejected before native dispatch")
    print("DAS_CONSTRAINT_FOREIGN_CONTEXT_PASS\n")
    return 42
}
