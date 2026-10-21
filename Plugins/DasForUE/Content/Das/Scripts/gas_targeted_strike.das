options gen2
require ue_gas_playground_refs
require ue_api_world

[ue_gameplay_ability]
class TargetedStrike {
    Selection:int=0
    ConfirmTask:int=0
    TimeoutTask:int=0

    def Query(context:GASAbilityContext):GASTargetingQuery {
        let driver=ue_ref(type<AbilityPlaygroundRef>,context.Avatar.handle)
        let avatar=ue_api_world::C_Actor_adf65d5b_from(context.Avatar.handle)
        let start=ue_actor_K2_GetActorLocation(avatar)
        let forward=ue_actor_GetActorForwardVector(avatar)
        let finish=ue_vector(start.x+forward.x*double(driver.Range),start.y+forward.y*double(driver.Range),start.z+forward.z*double(driver.Range))
        var query=ue_gas_trace(start,finish,3,driver.Radius)
        query.MaxTargets=1
        return <-query
    }
    def Activate(context:GASAbilityContext) {
        let driver=ue_ref(type<AbilityPlaygroundRef>,context.Avatar.handle)
        self.Selection=ue_gas_targeting_begin(context,self.Query(context))
        if(self.Selection==0) { ue_gas_cancel(context); return }
        var preview <- ue_gas_targeting_preview(context,self.Selection)
        driver.PreviewTarget=length(preview.Actors)>0 ? preview.Actors[0] : default<ActorRef>
        self.ConfirmTask=ue_gas_wait_event(context,ue_tag("GameplayEvent.Das.Sample.StrikeConfirm"))
        self.TimeoutTask=ue_gas_wait_delay(context,driver.AimTimeout)
        verify(self.ConfirmTask>0 && self.TimeoutTask>0,"Cannot start strike input/timeout tasks")
    }
    def Task(context:GASAbilityContext) {
        if(context.Task.Id==self.TimeoutTask) { ue_gas_cancel(context); return }
        if(context.Task.Id!=self.ConfirmTask) { return }
        let driver=ue_ref(type<AbilityPlaygroundRef>,context.Avatar.handle)
        // Re-query current position/direction before consuming the preview.
        if(!ue_gas_targeting_update(context,self.Selection,self.Query(context))) { ue_gas_cancel(context); return }
        var preview <- ue_gas_targeting_preview(context,self.Selection)
        if(!preview.Valid || length(preview.Actors)!=1) { ue_gas_cancel(context); return }
        verify(ue_class_valid(driver.HitEffect) && ue_class_valid(driver.AuraEffect),"Assign strike effect classes in Details")
        var hitSpec=ue_gas_make_spec(context.System,driver.HitEffect)
        var auraSpec=ue_gas_make_spec(context.System,driver.AuraEffect)
        var selected <- ue_gas_targeting_confirm(context,self.Selection)
        self.Selection=0
        if(!selected.Confirmed || length(selected.Targets.Hits)!=1) {
            ue_gas_release_targets(selected.Targets)
            ue_gas_cancel(context)
            return
        }
        if(!ue_gas_commit(context)) {
            ue_gas_release_targets(selected.Targets)
            ue_gas_cancel(context)
            return
        }
        var hits <- ue_gas_apply_targets(context,hitSpec,selected.Targets)
        var auras <- ue_gas_apply_targets(context,auraSpec,selected.Targets)
        ue_gas_release_targets(selected.Targets)
        // Effects are independent native applications, not a transactional pair.
        if(length(hits)!=1 || !hits[0].Applied || length(auras)!=1 || !auras[0].Applied) {
            ue_gas_cancel(context)
            return
        }
        driver.LastTarget=preview.Actors[0]
        driver.Shots+=1
        ue_gas_end(context)
    }
    def Cancel(context:GASAbilityContext) {
        let driver=ue_ref(type<AbilityPlaygroundRef>,context.Avatar.handle)
        driver.CancelledShots+=1
    }
    def End(context:GASAbilityContext) {
        // Native GAS has already released this activation's tasks and unconfirmed selection.
        self.Selection=0
        self.ConfirmTask=0
        self.TimeoutTask=0
        let driver=ue_ref(type<AbilityPlaygroundRef>,context.Avatar.handle)
        driver.PreviewTarget=default<ActorRef>
        // Applied duration effects belong to the target and outlive this ability.
    }
}
