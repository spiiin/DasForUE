options gen2
module ue_call_scope public
require ue_object_types public
require daslib/rtti
require strings

// Borrow the frame only for this block. The UObject itself is not owned by the scope.
def with_ue_call(target : UObjectHandle; method : string;
    body : block<(call : UCallHandle) : void>) : void {
    let call = ue_begin_call(target, method)
    var failed = false
    var failure : string
    // A VM panic bypasses ordinary defer/finally. Catch it before releasing the
    // frame, then propagate its message after cleanup, outside the recover block.
    try { invoke(body, call) } recover {
        failed = true
        failure = rtrim(this_context().last_exception)
    }
    ue_release_call(call)
    if (failed) { panic(failure) }
}
