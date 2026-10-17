options gen2
require ue_soft_upcast_types
require ue_native_gameplay
[export]
def main(input:int):int {
    let api=default<NativeHashProbeRef>
    var curve=default<SoftCurve>
    var previous:SoftObject
    api.ChangeSoftAsset(curve,previous)
    return input
}
