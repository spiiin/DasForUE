options gen2
require ue_effects
var @ue_state events:int = 0
[export]
def done():void { events = (events+1) }
[export]
def main(input:int):int {
    if(input==0) {
        let sound=ue_ref(type<SoundBaseRef>, ue_get_self_object("PickupSound"))
        let system=ue_ref(type<NiagaraSystemRef>, ue_get_self_object("PickupVFX"))
        let point=ue_vector(0.0lf,0.0lf,180.0lf)
        verify(ue_effect_at("probe",sound,system,point,"done"),"start effect")
        var rejected=0
        try { ue_effect_at("bad",sound,system,point,"done",-1.0f) } recover { rejected+=1 }
        try { ue_effect_at("bad",sound,system,point,"done",1.0f,0.0f) } recover { rejected+=1 }
        try { ue_effect_at("bad",sound,system,point,"missing") } recover { rejected+=1 }
        verify(rejected==3 && ue_effect_active("probe"),"invalid cue inputs must preserve existing effects")
        return 1
    }
    if(input==2) { return ue_effect_active("probe") ? 1 : 0 }
    if(input==3) { return ue_effect_stop("probe") ? 1 : 0 }
    if(input==4) { return ue_sound_play("sound",ue_ref(type<SoundBaseRef>, ue_get_self_object("PickupSound")),ue_vector(0.0lf,0.0lf,0.0lf),"done") ? 1 : 0 }
    if(input==5) { return ue_niagara_spawn("niagara",ue_ref(type<NiagaraSystemRef>, ue_get_self_object("PickupVFX")),ue_vector(0.0lf,0.0lf,0.0lf),"done") ? 1 : 0 }
    if(input==6) { return ue_effect_stop_all() }
    return events // REVISION_MARKER
}
