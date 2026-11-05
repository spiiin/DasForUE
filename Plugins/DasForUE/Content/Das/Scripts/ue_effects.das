options gen2
module ue_effects public
require ue_object_types public
require ue_dsl public
require ue_scene_components public
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

// Fire-and-forget, auto-destroyed by Unreal, independent of the caller's EndPlay.
def ue_effect_burst(sound:SoundBaseRef; system:NiagaraSystemRef; transform:UETransform; volume:float=1.0f; pitch:float=1.0f):bool {
    return __ue_effect_burst(sound.handle,system.handle,transform,volume,pitch)
}

// Attached, auto-destroying cast presentation. Unreal owns the returned component.
def ue_niagara_spawn_attached(system:NiagaraSystemRef; component:SceneComponentRef; socket:UENameValue; pooled:bool=false):UObjectHandle {
    let call=ue_begin_static_call("/Script/Niagara.NiagaraFunctionLibrary","SpawnSystemAttached", "")
    ue_arg_object(call,"systemtemplate",system.handle)
    ue_arg_object(call,"attachtocomponent",component.handle)
    __ue_method_arg_name(call,"attachpointname",socket)
    __ue_method_arg_vector(call,"location",ue_vector(0.0lf,0.0lf,0.0lf))
    __ue_method_arg_rotator(call,"rotation",ue_rotator(0.0lf,0.0lf,0.0lf))
    ue_arg_enum(call,"locationtype","/Script/Engine.EAttachLocation",pooled ? "KeepRelativeOffset" : "SnapToTarget")
    ue_arg_bool(call,"bautodestroy",!pooled)
    ue_arg_bool(call,"bautoactivate",true)
    ue_arg_enum(call,"poolingmethod","/Script/Niagara.ENCPoolMethod",pooled ? "ManualRelease" : "None")
    ue_arg_bool(call,"bprecullcheck",!pooled)
    ue_invoke_call(call)
    let result=ue_result_object(call,"returnvalue")
    ue_release_call(call)
    return result
}

def ue_sound_play_2d(sound:SoundBaseRef; volume:float=1.0f; pitch:float=1.0f):void {
    let call=ue_begin_static_call("/Script/Engine.GameplayStatics","PlaySound2D","worldcontextobject")
    ue_arg_object(call,"sound",sound.handle)
    ue_arg_float(call,"volumemultiplier",volume)
    ue_arg_float(call,"pitchmultiplier",pitch)
    ue_arg_float(call,"starttime",0.0f)
    ue_arg_object(call,"concurrencysettings",default<UObjectHandle>)
    ue_arg_object(call,"owningactor",default<UObjectHandle>)
    ue_arg_bool(call,"bisuisound",true)
    ue_invoke_call(call)
    ue_release_call(call)
}

// Component helpers remain generic over checked native references, without a catalog dependency.
def ue_niagara_component_start(component:auto; system:NiagaraSystemRef):void {
    let receiver=ue_cast_object(component.handle,"/Script/Niagara.NiagaraComponent")
    let asset=ue_begin_call(receiver,"SetAsset")
    ue_arg_object(asset,"inasset",system.handle)
    ue_arg_bool(asset,"bresetexistingoverrideparameters",true)
    ue_invoke_call(asset)
    ue_release_call(asset)
    let activate=ue_begin_call(receiver,"Activate")
    ue_arg_bool(activate,"breset",true)
    ue_invoke_call(activate)
    ue_release_call(activate)
}

def ue_audio_component_play(component:auto; sound:SoundBaseRef; start:float=0.0f):void {
    let receiver=ue_cast_object(component.handle,"/Script/Engine.AudioComponent")
    let asset=ue_begin_call(receiver,"SetSound")
    ue_arg_object(asset,"newsound",sound.handle)
    ue_invoke_call(asset)
    ue_release_call(asset)
    let play=ue_begin_call(receiver,"Play")
    ue_arg_float(play,"starttime",start)
    ue_invoke_call(play)
    ue_release_call(play)
}

// MetaSound triggers use the native audio parameter interface.
def ue_audio_trigger(component : auto; name : string) : void {
    __ue_audio_trigger(component.handle, ue_name_value(name))
}

// Auto-destroying audio; the caller can retain the component to send a release trigger.
def ue_sound_spawn_attached(sound : SoundBaseRef; component : SceneComponentRef) : UObjectHandle {
    let call = ue_begin_static_call("/Script/Engine.GameplayStatics", "SpawnSoundAttached", "")
    ue_arg(call, "Sound", sound)
    ue_arg(call, "AttachToComponent", component)
    ue_arg(call, "AttachPointName", ue_name_value("None"))
    ue_arg(call, "Location", ue_vector(0.0lf, 0.0lf, 0.0lf))
    ue_arg(call, "Rotation", ue_rotator(0.0lf, 0.0lf, 0.0lf))
    ue_arg_enum(call, "LocationType", "/Script/Engine.EAttachLocation", "KeepRelativeOffset")
    ue_arg(call, "bStopWhenAttachedToDestroyed", true)
    ue_arg(call, "VolumeMultiplier", 1.0f)
    ue_arg(call, "PitchMultiplier", 1.0f)
    ue_arg(call, "StartTime", 0.0f)
    ue_arg_object(call, "AttenuationSettings", default<UObjectHandle>)
    ue_arg_object(call, "ConcurrencySettings", default<UObjectHandle>)
    ue_arg(call, "bAutoDestroy", true)
    ue_invoke_call(call)
    let result = ue_result_object(call, "ReturnValue")
    ue_release_call(call)
    return result
}

def ue_sound_spawn_2d(sound : SoundBaseRef) : UObjectHandle {
    let call = ue_begin_static_call("/Script/Engine.GameplayStatics", "SpawnSound2D", "WorldContextObject")
    ue_arg(call, "Sound", sound)
    ue_arg(call, "VolumeMultiplier", 1.0f)
    ue_arg(call, "PitchMultiplier", 1.0f)
    ue_arg(call, "StartTime", 0.0f)
    ue_arg_object(call, "ConcurrencySettings", default<UObjectHandle>)
    ue_arg(call, "bPersistAcrossLevelTransition", false)
    ue_arg(call, "bAutoDestroy", true)
    ue_invoke_call(call)
    let result = ue_result_object(call, "ReturnValue")
    ue_release_call(call)
    return result
}

// Niagara inherits this engine operation; no generated Niagara API module is required.
def ue_niagara_release_to_pool(component : auto) : void {
    let call = ue_begin_call(component.handle, "ReleaseToPool")
    ue_invoke_call(call)
    ue_release_call(call)
}
