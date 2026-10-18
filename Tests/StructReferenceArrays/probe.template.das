options gen2
require ue_reference_arrays_fixture
[export]
def main(input:int):int {
    let owner=@ACTOR@_from(ue_self_object())
    let actor_class=__ue_class_loaded("/Script/Engine.Actor","/Script/Engine.Actor","")
    let pawn_class=__ue_class_loaded("/Script/Engine.Pawn","/Script/Engine.Actor","")
    var value:@ENVELOPE@
    push(value.f_inner.f_actors,owner)
    push(value.f_inner.f_actors,@ACTOR@_null())
    push(value.f_inner.f_actors,owner)
    push(value.f_inner.f_classes,actor_class)
    push(value.f_inner.f_classes,pawn_class)
    push(value.f_inner.f_classes,ue_class_null())
    resize(value.f_entries,2)
    push(value.f_entries[0].f_actors,owner)
    push(value.f_entries[0].f_classes,pawn_class)
    let result <- ue_reference_arrays_EchoReferences(value)
    verify(length(result.f_inner.f_actors)==3,"nested object array output size")
    verify(__ue_same_object(@ACTOR@_raw(result.f_inner.f_actors[0]),ue_self_object()),"returned object identity")
    verify(!@ACTOR@_valid(result.f_inner.f_actors[1]),"nullable object array output")
    verify(__ue_same_object(@ACTOR@_raw(result.f_inner.f_actors[2]),ue_self_object()),"duplicate object entries preserved")
    verify(length(result.f_inner.f_classes)==3,"nested class array output size")
    verify(ue_class_path(result.f_inner.f_classes[0])=="/Script/Engine.Actor" && ue_class_path(result.f_inner.f_classes[1])=="/Script/Engine.Pawn","class and subclass output")
    verify(!ue_class_valid(result.f_inner.f_classes[2]),"nullable class array output")
    verify(length(result.f_entries)==2 && length(result.f_entries[0].f_actors)==1 && length(result.f_entries[1].f_actors)==0,"array of structs with nested object arrays")
    verify(ue_class_path(result.f_entries[0].f_classes[0])=="/Script/Engine.Pawn" && length(result.f_entries[1].f_classes)==0,"nested class array and empty member")
    clear(value.f_inner.f_actors)
    clear(value.f_inner.f_classes)
    clear(value.f_entries)
    let cleared <- ue_reference_arrays_EchoReferences(value)
    verify(length(cleared.f_inner.f_actors)==0 && length(cleared.f_inner.f_classes)==0 && length(cleared.f_entries)==0,"resized arrays do not retain prior call entries")
    verify(length(result.f_inner.f_actors)==3 && length(result.f_entries)==2,"detached result is unaffected by source mutation")

    let root_call=ue_begin_call(ue_self_object(),"K2_GetRootComponent")
    ue_invoke_call(root_call)
    let root=ue_result_object(root_call,"returnvalue")
    ue_release_call(root_call)
    let wrong_object=ue_begin_static_call("/Script/DasRuntime.DasHitTransportLibrary","EchoReferences","")
    ue_arg_array(wrong_object,"value.inner.actors",1)
    var rejected=false
    try { ue_arg_object(wrong_object,"value.inner.actors.0",root) } recover { rejected=true }
    verify(rejected,"component cannot become Actor array element")
    ue_release_call(wrong_object)
    let wrong_class=ue_begin_static_call("/Script/DasRuntime.DasHitTransportLibrary","EchoReferences","")
    ue_arg_array(wrong_class,"value.inner.classes",1)
    let object_class=__ue_class_loaded("/Script/CoreUObject.Object","/Script/CoreUObject.Object","")
    rejected=false
    try { ue_arg_class(wrong_class,"value.inner.classes.0",object_class) } recover { rejected=true }
    verify(rejected,"non Actor class cannot become Actor metaclass array element")
    ue_release_call(wrong_class)
    let missing=ue_begin_static_call("/Script/DasRuntime.DasHitTransportLibrary","EchoReferences","")
    ue_arg_array(missing,"value.inner.actors",1)
    ue_arg_array(missing,"value.inner.classes",0)
    ue_arg_array(missing,"value.entries",0)
    rejected=false
    try { ue_invoke_call(missing) } recover { rejected=true }
    verify(rejected,"missing nested object element rejected")
    ue_release_call(missing)
    print("DAS_STRUCT_REFERENCE_ARRAYS_PASS object class nested null duplicates resize guards\n")
    return 42
}
