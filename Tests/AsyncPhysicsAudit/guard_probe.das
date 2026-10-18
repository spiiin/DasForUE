options gen2
require ue_foundation
[export]
def main(input:int):int {
    if(input==0) {
        let root=ue_begin_call(ue_self_object(),"K2_GetRootComponent")
        ue_invoke_call(root)
        let body=ue_result_object(root,"returnvalue")
        ue_release_call(root)
        let denied=ue_begin_call(body,"GetBodyInstanceAsyncPhysicsTickHandle")
        ue_release_call(denied)
    } else {
        let denied=ue_begin_static_call("/Script/Engine.PhysicsThreadLibrary","AddForce","")
        ue_release_call(denied)
    }
    return 42
}
