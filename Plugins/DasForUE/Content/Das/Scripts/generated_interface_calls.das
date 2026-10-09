options gen2
require ue_foundation
require ue_generated
[export]
def main(mode:int):int {
    if (mode == 9) {
        let empty = DasProbeInterface_null()
        verify(!DasProbeInterface_valid(empty))
        return 1
    }
    if (mode == 10) { return ue_probe_Query(DasProbeInterface_null(), 5) }
    if (mode == 11) { return ue_probe_Query(DasProbeInterface(handle=ue_self_object()), 5) }
    if (mode == 12) {
        let call = ue_begin_interface_call(ue_self_object(), "/Script/DasRuntime.DasProbeInterface", "Query")
        ue_expect_interface_signature(call, "stale-signature")
        return 0
    }
    let receiver = ue_interface(type<DasProbeInterface>, ue_self_object())
    verify(DasProbeInterface_valid(receiver))
    if (mode == 0) { return receiver.Query(5) }
    if (mode == 1) { var counter = 10; var total = -1; receiver.Stats(counter, total); return counter + total }
    if (mode == 2) { verify(receiver.Label() == "Blueprint 世界"); return 1 }
    if (mode == 3) {
        var values : array<double>
        push(values, 2.5lf)
        push(values, 1.25lf)
        var result <- receiver.Weights(values)
        verify(length(result) == 2 && values[0] == 2.5lf)
        result[0] = 99.0lf
        var again <- receiver.Weights(values)
        return int((again[0] + again[1]) * 100.0lf)
    }
    if (mode == 4) {
        var values : H_Map_int_string_aa6f8c57
        push(values.entries, H_Map_int_string_aa6f8c57_Entry(key=7, value="Generated 世界"))
        var result <- DasCollectionInterface_from(ue_self_object()).EchoMap(values)
        verify(length(result.entries) == 1 && result.entries[0].key == 7 && result.entries[0].value == "Generated 世界")
        result.entries[0].value = "Changed"
        verify(values.entries[0].value == "Generated 世界")
        return 1
    }
    if (mode == 5) {
        var values : H_Set_int_bf4eb51c
        push(values.values, 31)
        var result <- DasCollectionInterface_from(ue_self_object()).EchoSet(values)
        verify(length(result.values) == 1 && result.values[0] == 31)
        return 31
    }
    if (mode == 6) { DasEventInterface_from(ue_self_object()).Notify(17); return 1 }
    if (mode == 7) { DasEventInterface_from(ue_self_object()).ResetCounter(); return 1 }
    return 0
}
