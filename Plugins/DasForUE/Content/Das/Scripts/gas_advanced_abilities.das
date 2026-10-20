options gen2
module gas_advanced_abilities public
require ue_gas
require math

[ue_attribute_set]
class BattleStats {
    @min=0
    Health:float=100.0f
    @min=0
    Power:float=12.0f
    @min=0
    Armor:float=3.0f
}

// A real GameplayModMagnitudeCalculation; values come from native GAS capture.
[ue_gas_calculation(attributes="BattleStats")]
class ScaledDamage {
    @source="Source"
    @snapshot=true
    Power:float=0.0f
    @source="Target"
    @snapshot=false
    Armor:float=0.0f

    def static Calculate(context:GASCalculationContext):float {
        return -max(0.0f,ue_gas_captured(context,"Power")-ue_gas_captured(context,"Armor"))*context.Level
    }
}

// Execution produces one output modifier; several execution definitions can
// be composed as separate effects. No native execution parameters escape.
[ue_gas_calculation(kind="Execution",attributes="BattleStats",output="Health")]
class ArmoredDamage {
    @source="Source"
    Power:float=0.0f
    @source="Target"
    @snapshot=false
    Armor:float=0.0f

    def static Calculate(context:GASCalculationContext):float {
        return -max(0.0f,ue_gas_captured(context,"Power")-2.0f*ue_gas_captured(context,"Armor"))
    }
}

[ue_gameplay_effect(attributes="BattleStats")]
class ScaledHit {
    @calculation="ScaledDamage"
    Health:float=0.0f
}
[ue_gameplay_effect(attributes="BattleStats",execution="ArmoredDamage")]
class ExecutedHit {}

// Grant this ability, activate it and send Data.Healing gameplay events to its ASC.
// A delay bounds the listening window; End/Cancel automatically release tasks.
[ue_gameplay_ability]
class EventWindow {
    Events:int=0
    Windows:int=0
    LastTask:GASAbilityTaskResult
    LastContext:GASAbilityContext
    Watch:int=0
    Timer:int=0

    def Activate(context:GASAbilityContext) {
        self.Windows+=1
        self.Events=0
        self.LastContext=context
        if(!ue_gas_commit(context)) {ue_gas_cancel(context);return}
        self.Watch=ue_gas_wait_event(context,ue_tag("Data.Healing"),false)
        self.Timer=ue_gas_wait_delay(context,0.1f)
        verify(self.Watch>0 && self.Timer>0,"Cannot start GAS tasks")
    }
    def Task(context:GASAbilityContext) {
        self.LastTask=context.Task
        if(ue_name_text(context.Task.Kind)=="GameplayEvent") {
            self.Events+=1
        } else {
            // The saved event result is independent of the callback frame.
            verify(self.Events>=0,"Persistent task state")
            ue_gas_end(context)
        }
    }
    def End(context:GASAbilityContext) {
        self.LastContext=context
    }
}
