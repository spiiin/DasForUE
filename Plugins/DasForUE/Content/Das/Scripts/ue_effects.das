options gen2
module ue_effects public
require ue_object_types public
require ue_dsl public
require ue_soft_references public
[ue_native_object(path="/Script/Engine.SoundBase")]
struct SoundBaseRef { handle:UObjectHandle }
def SoundBaseRef_from(value:UObjectHandle):SoundBaseRef { return SoundBaseRef(handle=ue_cast_nullable_object(value,"/Script/Engine.SoundBase")) }
[ue_native_object(path="/Script/Niagara.NiagaraSystem")]
struct NiagaraSystemRef { handle:UObjectHandle }
def NiagaraSystemRef_from(value:UObjectHandle):NiagaraSystemRef { return NiagaraSystemRef(handle=ue_cast_nullable_object(value,"/Script/Niagara.NiagaraSystem")) }
typedef SoftSound = $UESoftObject<SoundBaseRef>
typedef SoftNiagaraSystem = $UESoftObject<NiagaraSystemRef>
def ue_effect_play(name:string; sound:SoundBaseRef; system:NiagaraSystemRef; transform:UETransform; handler:string=""; volume:float=1.0f; pitch:float=1.0f):bool {
    return __ue_effect_play(name,sound.handle,system.handle,transform,handler,volume,pitch)
}
def ue_effect_at(name:string; sound:SoundBaseRef; system:NiagaraSystemRef; location:UEVector; handler:string=""; volume:float=1.0f; pitch:float=1.0f; scale:float=1.0f):bool {
    let rotation=ue_rotator(0.0lf,0.0lf,0.0lf)
    let size=ue_vector(double(scale),double(scale),double(scale))
    let transform=ue_transform(location,rotation,size)
    return ue_effect_play(name,sound,system,transform,handler,volume,pitch)
}
def ue_sound_play(name:string; sound:SoundBaseRef; location:UEVector; handler:string=""; volume:float=1.0f; pitch:float=1.0f):bool { return ue_effect_at(name,sound,default<NiagaraSystemRef>,location,handler,volume,pitch) }
def ue_niagara_spawn(name:string; system:NiagaraSystemRef; location:UEVector; handler:string=""):bool { return ue_effect_at(name,default<SoundBaseRef>,system,location,handler) }
def ue_effect_stop(name:string):bool { return __ue_effect_stop(name) }
def ue_effect_stop_all():int { return __ue_effect_stop_all() }
def ue_effect_active(name:string):bool { return __ue_effect_active(name) }
