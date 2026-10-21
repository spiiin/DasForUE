options gen2
require ue_foundation

[export]
def main(input:int):int {
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
    return ue_get_self_int("health")
}
