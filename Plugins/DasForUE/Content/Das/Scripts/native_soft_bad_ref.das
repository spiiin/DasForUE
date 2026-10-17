options gen2
require ue_native_gameplay
[export]
def main(input:int):int {
 let api=default<NativeHashProbeRef>
 var value=default<SoftActorClass>;var previous:SoftActorClass;api.ChangeSoftAsset(value,previous)
 return input
}
