options gen2
require ue_native_gameplay
[export]
def main():bool {
 let api=NativeHashProbeRef()
 var value:table<$UEObjectKey<NativePawnRef>;string>
 api.ActorLabels:=value
 return true
}
