options gen2
require ue_gas
require gas_advanced_abilities

var first:GASAbilityContext
var callback:GASAbilityTaskResult
var events:int=0
var endings:int=0
var cancellations:int=0

[ue_gameplay_ability]
class TaskProbe {
    Runs:int=0
    Ends:int=0
    Events:int=0
    Saved:GASAbilityTaskResult
    Context:GASAbilityContext
    EventTask:int=0
    def Activate(context:GASAbilityContext) {
        self.Runs+=1
        if(self.Runs>1) {
            verify(self.Ends==self.Runs-1,"End/Cancel kept persistent class state")
            verify(!ue_gas_cancel_task(self.Context,self.EventTask),"stale activation cannot stop next task")
        }
        self.Context=context
        verify(ue_gas_commit(context),"commit task ability")
        self.EventTask=ue_gas_wait_event(context,ue_tag("Data.Healing"),false)
        let timer=ue_gas_wait_delay(context,0.05f)
        verify(self.EventTask>0 && timer>0,"native wait tasks")
        let cancelled=ue_gas_wait_delay(context,0.02f)
        verify(cancelled>0 && ue_gas_cancel_task(context,cancelled),"manual task cancellation")
    }
    def Task(context:GASAbilityContext) {
        verify(ue_name_text(context.Phase)=="Task" && context.ActivationId==self.Context.ActivationId,"task activation context")
        if(ue_name_text(context.Task.Kind)=="GameplayEvent") {
            verify(context.Task.Id==self.EventTask && context.Task.Magnitude==7.0f && ue_is_valid(context.Task.Target),"detached native event data")
            self.Events+=1
            self.Saved=context.Task
        } else {
            verify(ue_name_text(context.Task.Kind)=="Delay" && self.Events==2,"only live tasks delivered")
            verify(self.Saved.Magnitude==7.0f && ue_name_text(self.Saved.EventTag)=="Data.Healing","event snapshot survived callback")
            verify(context.Ability.ScriptTaskCount()==1,"only recurring event remains")
            verify(ue_gas_end(context),"delay completes ability")
            verify(!context.System.ActivateAuthoredAbility(context.GrantId),"task callback cannot restart its activation")
        }
    }
    def Cancel(context:GASAbilityContext) { verify(context.Cancelled,"cancel flag") }
    def End(context:GASAbilityContext) {
        self.Ends+=1
        verify(context.Ability.ScriptTaskCount()==0,"tasks cleaned before lifecycle end")
        self.Events=0
    }
}

[export]
def main(phase:int):int {
    let source=AuthoredAbilitySystemComponentRef_from(ue_get_self_object("Abilities"))
    let other=ActorRef(handle=ue_get_self_object("Other"))
    let kind=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_ScaledHit.GE_ScaledHit_C"))
    var spec=ue_gas_make_spec(source,kind,2.0f)
    var targets=ue_gas_target(other)
    let frame=ue_begin_call(ue_self_object(),"ReadProbeHit")
    ue_invoke_call(frame)
    let hit=__ue_result_hit_result(frame,"returnvalue")
    ue_release_call(frame)
    var copied=ue_gas_hit_target(source,hit)
    verify(length(copied.Hits)==1,"opaque native HitResult copied into source-owned target data")
    ue_gas_release_targets(copied)
    verify(length(copied.Hits)==0,"explicit release of hit storage")
    // The same actor twice is deduplicated by native TargetData.
    push(targets.Actors,other)
    var results <- ue_gas_apply_targets(source,spec,targets)
    verify(length(results)==1 && results[0].Valid && results[0].Applied,"typed TargetData and calculation result")
    return 42
}
