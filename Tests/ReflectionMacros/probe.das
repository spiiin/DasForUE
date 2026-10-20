options gen2
options gc
require ue_native_gameplay
require ue_library_generated
[export]
def main(mode:int):int {
    if (mode == 0) { return ue_static(DasFunctionLibraryFixture.Add, 20,22) }
    if (mode == 1) { verify(ue_static(GameplayStatics.GetTimeSeconds) >= 0.0lf);return 1 }
    let target = ue_interface(type<DasProbeInterface>, ue_self_object())
    if (mode == 2) { return ue_call(type<DasProbeInterface>, Query, target,5) }
    if (mode == 3) {
        var counter=10;var total=0
        ue_call(type<DasProbeInterface>, Stats, target,counter,total)
        return counter+total
    }
    if (mode == 6) {
        var values:array<double>;push(values,1.0lf);push(values,2.0lf)
        var result <- ue_call(type<DasProbeInterface>, Weights, target,values)
        return int(result[0]+result[1])
    }
    let payload = DasCallPayload(Count=17,Metrics=DasCallMetrics(Ratio=1.0f))
    if (mode == 4) { return ue_struct_get(payload, Count) }
    verify(ue_static(KismetMathLibrary.Abs, -2.5lf) == 2.5lf)
    return int(ue_struct_get(ue_struct_get(payload, Metrics), Ratio))
}
