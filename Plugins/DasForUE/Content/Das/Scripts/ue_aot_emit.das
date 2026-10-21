options gen2
options stack=1048576
require daslib/aot_cpp
require daslib/rtti
require daslib/ast_boost

// Build-time driver, executed inside Unreal so ue_foundation is registered.
[export]
def emit(program:Program?; target:Context?):string {
    var context:Context?=unsafe(reinterpret<Context?>(target))
    var result=""
    using() $(var policies:CodeOfPolicies) {
        policies.aot=false
        policies.aot_module=true
        policies.tune_frozen=true
        result=run_aot(program,context,policies)
    }
    return result
}
