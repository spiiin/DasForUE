options gen2
module ue_native_hash_probe public
require ue_character_control public

// ABI validation receiver; this is not an authored gameplay base class.
[ue_native_object(path="/Script/DasRuntime.DasNativeHashProbeComponent")]
struct NativeHashProbeRef { handle:UObjectHandle }
def NativeHashProbeRef_from(value:UObjectHandle):NativeHashProbeRef {
 return NativeHashProbeRef(handle=ue_cast_nullable_object(value,"/Script/DasRuntime.DasNativeHashProbeComponent"))
}
