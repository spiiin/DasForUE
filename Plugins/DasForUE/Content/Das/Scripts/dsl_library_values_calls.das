options gen2
require ue_dsl
require ue_object_types
require dsl_library_values
[export]
def main(input:int):int {
    let context=ue_ref(type<ObjectRef>,ue_self_object())
    var recovered=false
    try { GameValues`Time(ue_ref(type<ObjectRef>,ue_null_object())) } recover { recovered=true }
    if (!recovered) { return -100 }
    // Scope restoration permits normal owner access after a rejected nested call.
    let restored=ue_self_object()
    let now=GameValues`Time(context)
    return input+int(now)
}

[export]
def cross_world(target:UObjectHandle):double {
    let now=GameValues`Time(ue_ref(type<ObjectRef>,target))
    let restored=ue_self_object()
    return now
}
