options gen2
require ue_foundation

[export]
def main(input:int):int {
    __ue_self_map_set_int("InteropCounts",0,__ue_self_map_get_int("InteropCounts",0)+1)
    if (input==15) {
        var key=3
        __ue_self_map_set_int("InteropCounts",key,41)
        var text="世界 Key"
        __ue_self_map_set_double("InteropStrings",text,2.5lf)
        var byte_key=uint8(7)
        __ue_self_map_set_int("InteropBytes",byte_key,11)
        verify(__ue_self_map_get_int("InteropCounts",key)==41)
        verify(__ue_self_map_get_double("InteropStrings",text)==2.5lf)
        verify(__ue_self_map_get_int("InteropBytes",byte_key)==11)
        return 1
    }
    if (input==16) { __ue_self_map_set_double("InteropStrings",3,7.0lf); return 0 }
    if (input==17) { __ue_self_map_set_int("InteropLocked",0,9); return 0 }

    if (input==0) {
        ue_set_self_int("health",ue_get_self_int("health")+2)
        return ue_get_self_int("health")
    }
    if (input==1) { return ue_get_self_int("missingAotField") }
    if (input==2) { ue_set_self_int("readOnly",9); return 0 }
    if (input==3) { return ue_get_self_int("speed") }
    if (input==4) {
        ue_set_self_string("DSLText","native AOT")
        ue_set_self_bool("bScalar",true)
        ue_set_self_double("DSLPrecise",2.5lf)
        ue_set_self_float("speed",3.5f)
        if (ue_get_self_string("DSLText") != "native AOT" || !ue_get_self_bool("bScalar")) { return -1 }
        return int(ue_get_self_double("DSLPrecise") + double(ue_get_self_float("speed")))
    }
    if (input==5) {
        // Object externs in compiled code: self, null, cast, same object, is-a.
        let s=ue_self_object()
        let f=ue_cast_nullable_object(s,"/Script/DasRuntime.DasObjectFixture")
        var r=0
        if (ue_object_valid(s)) { r+=1 }
        if (!ue_object_valid(ue_null_object())) { r+=2 }
        if (ue_object_valid(f) && __ue_same_object(s,f)) { r+=4 }
        if (ue_object_is_a(s,"/Script/CoreUObject.Object")) { r+=8 }
        return r
    }
    if (input==7) {
        // Math externs and operators in compiled code: constructors, + - * += != and transform / rotation helpers.
        let a=ue_vector(1.0lf,2.0lf,2.0lf)
        let b=a+a-ue_vector(1.0lf,2.0lf,2.0lf)
        let c=b*2.0lf
        let p=ue_transform_position(ue_transform_identity(),c)
        let r=ue_rotate_vector(ue_rotator(0.0lf,90.0lf,0.0lf),p)
        var s=r
        s+=ue_vector(0.0lf,0.0lf,0.0lf)
        if (s!=r) { return -1 }
        return int(ue_length(s)+0.5lf)
    }
    if (input==8) {
        // A non-finite value: the extern's own error, the same on both paths.
        // An overflow to infinity (not 0 / 0: MSVC refuses a constant division by zero in the generated code).
        let bad=ue_vector(1.0e308lf,0.0lf,0.0lf)*double(input*input)
        return int(ue_length(bad+ue_vector(1.0lf,0.0lf,0.0lf)))
    }
    if (input==9) {
        // Names and texts as tokens: constructors, comparison and conversion are compiled.
        let n=ue_name_value("AotTag")
        var r=0
        if (ue_names_equal(n,ue_name_value("AotTag"))) { r+=1 }
        if (!ue_names_equal(n,ue_name_value("Other"))) { r+=2 }
        if (ue_text_to_string(ue_text_invariant("hello"))=="hello") { r+=4 }
        return r
    }
    if (input==10) {
        // A reflected UFUNCTION call through a call frame (UCallHandle) is compiled.
        let call=ue_begin_static_call("/Script/Engine.KismetMathLibrary","Abs_Int","")
        ue_arg_int(call,"a",-5)
        ue_invoke_call(call)
        let r=ue_result_int(call,"returnvalue")
        ue_release_call(call)
        return r
    }
    if (input==11) {
        // A missing UFUNCTION: the extern's own error, the same on both paths.
        let call=ue_begin_static_call("/Script/Engine.KismetMathLibrary","NoSuchFunction","")
        ue_release_call(call)
        return 0
    }
    if (input==12) {
        // Fields of values: generated code reads the C++ members (v.X for the script's v.x).
        let v=ue_vector(1.0lf,2.0lf,3.0lf)
        let r=ue_rotator(10.0lf,20.0lf,30.0lf)
        var hit : UETraceHit
        return int(v.x*100.0lf+v.y*10.0lf+v.z)+int(r.yaw)*1000+int(hit.distance)+hit.face_index
    }
    if (input==13) {
        // An extern that takes a block (the scope of an authored library function) runs the compiled block.
        var r=1
        __ue_library_scope(ue_self_object(),false) $() {
            r+=40
        }
        return r
    }
    if (input==14) {
        // An error inside the block: the same on both paths.
        __ue_library_scope(ue_self_object(),false) $() {
            let call=ue_begin_static_call("/Script/Engine.KismetMathLibrary","NoSuchFunction","")
            ue_release_call(call)
        }
        return 0
    }
    if (input==6) {
        // A null handle where an object is required: the extern's own error, the same on both paths.
        return ue_object_valid(ue_cast_object(ue_null_object(),"/Script/CoreUObject.Object")) ? 1 : 0
    }
    return ue_get_self_int("health")
}
