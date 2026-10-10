options gen2
options gc
require ue_foundation
require ue_generated
var remembered : DasEventInterface
[export]
def main(mode:int):int {
    if (mode==0) {
        remembered = ue_interface(type<DasEventInterface>, ue_get_self_object("Target"))
        ue_set_self_object("Events",remembered.handle)
        return 1
    }
    if (mode==1) { return ue_implements_interface(ue_get_self_object("Events"),"/Script/DasRuntime.DasEventInterface") ? 1 : 0 }
    if (mode==2) { ue_set_self_object("Events",ue_null_object()); return 1 }
    if (mode==3) { return ue_implements_interface(remembered.handle,"/Script/DasRuntime.DasEventInterface") ? 1 : 0 }
    if (mode==4) { ue_set_self_object("Events",ue_get_self_object("Target")); return 1 }
    return 0
}
