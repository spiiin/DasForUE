options gen2
options stack=1048576
require daslib/aot_cpp
require daslib/rtti
require daslib/ast_boost

// A function that can reach a collect point (needCallerStackFrame, transitively) stays interpreted, as [no_aot]
// keeps it. Struct snapshot collection finds live tokens by walking interpreter frames only, so a compiled frame
// holding a snapshot would lose it; daScript's own proof covers its heap, not these tokens, and fails the build
// where this keeps the function interpreted.
def keep_collect_carriers_interpreted(program:Program?) {
    program.getThisModule |> for_each_module_function($(var fn:FunctionPtr) {
        if (!fn.flags.noAot && fn.moreFlags2.needCallerStackFrame) {
            fn.flags.noAot=true
            print("AOT: '{fn.name}' stays interpreted: it can reach a struct snapshot collection\n")
        }
    })
}

// Build-time driver, executed inside Unreal so ue_foundation is registered.
[export]
def emit(program:Program?; target:Context?):string {
    var context:Context?=unsafe(reinterpret<Context?>(target))
    var result=""
    keep_collect_carriers_interpreted(program)
    using() $(var policies:CodeOfPolicies) {
        policies.aot=false
        policies.aot_module=true
        policies.tune_frozen=true
        result=run_aot(program,context,policies)
    }
    return result
}
