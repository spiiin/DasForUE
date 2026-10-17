options gen2
module ue_animation public
require ue_character_control public
require ue_soft_references public

[ue_native_object(path="/Script/Engine.AnimMontage")]
struct AnimMontageRef { handle:UObjectHandle }
def AnimMontageRef_from(value:UObjectHandle):AnimMontageRef { return AnimMontageRef(handle=ue_cast_nullable_object(value,"/Script/Engine.AnimMontage")) }
typedef SoftAnimMontage = $UESoftObject<AnimMontageRef>

[ue_native_enum(path="/Script/DasRuntime.EDasMontageEventKind")]
enum MontageEventKind {
    Completed=0
    Interrupted=1
    BlendOut=2
    NotifyBegin=3
    NotifyEnd=4
}
[ue_native_struct(path="/Script/DasRuntime.DasMontageEvent")]
struct MontageEvent {
    Kind:MontageEventKind
    NotifyName:UENameValue
}

def ue_montage_play(name:string; mesh:SkeletalMeshComponentRef; montage:AnimMontageRef; handler:string; rate:float=1.0f; position:float=0.0f; section:UENameValue=default<UENameValue>):float {
    return __ue_montage_play(name,mesh.handle,montage.handle,handler,rate,position,section)
}
def ue_montage_stop(name:string; blend_out:float=0.1f):bool { return __ue_montage_stop(name,blend_out) }
// Cancel revokes callbacks and stops only this owned playback instance, silently.
def ue_montage_cancel(name:string):bool { return __ue_montage_cancel(name) }
def ue_montage_playing(name:string):bool { return __ue_montage_playing(name) }
