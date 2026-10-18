options gen2
options gc
require ue_foundation
var empty_object:UObjectHandle
var empty_class:UEClassHandle
[export]
def on_timer() { }
[export]
def on_loaded(value:UObjectHandle) { }
[export]
def main(input:int):int {
    if(input==0) { var x=0; for(i in range(0,7)) { x+=i }; return x*2 }
    if(input==1) { return ue_add(input,41) }
    if(input==2) { return ue_object_valid(ue_self_object()) ? 42 : -1 }
    if(input==3) { return ue_object_valid(ue_null_object()) ? -1 : 42 }
    if(input==4) { let v=ue_vector(2.0lf,0.0lf,0.0lf); return int(v.x)+40 }
    if(input==5) { ue_set_self_int("Counter",1); return 42 }
    if(input==6) {
        let call=ue_begin_static_call("/Script/Engine.KismetSystemLibrary","GetGameTimeInSeconds","worldcontextobject")
        ue_release_call(call); return 42
    }
    if(input==7) { ue_timer_start("blocked",1.0,"on_timer",false); return 42 }
    if(input==8) { ue_struct_collect(); return 42 }
    if(input==9) { __ue_asset_load_async("blocked","/Game/Missing.Missing","on_loaded","/Script/CoreUObject.Object"); return 42 }
    if(input==10) { let w=__ue_widget_create(empty_class,empty_object); return ue_object_valid(w) ? -1 : 42 }
    if(input==11) { return __ue_scene_destroy(empty_object) ? -1 : 42 }
    if(input==12) { let n=ue_name_value("RoleGuardName"); return ue_names_equal(n,n) ? 42 : -1 }
    return -1
}
