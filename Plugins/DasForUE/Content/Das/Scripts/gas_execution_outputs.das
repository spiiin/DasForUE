options gen2
module gas_execution_outputs public
require ue_gas

[ue_attribute_set]
class ExecutionStats {
    Health:float=100.0f
    Threat:float=0.0f
    Power:float=10.0f
    Armor:float=4.0f
}

// Values read here include native execution-scoped adjustments; stored attributes stay unchanged.
[ue_gas_calculation(kind="Execution",attributes="ExecutionStats")]
class DamageAndThreat {
    @source="Source"
    @scoped_add=5.0f
    Power:float=0.0f
    @source="Target"
    @snapshot=false
    @scoped_multiply=2.0f
    Armor:float=0.0f
    @set_by_caller="Data.Healing"
    Bonus:float=0.0f
    @output=true
    @attribute="Health"
    Damage:float=0.0f
    @output=true
    @attribute="Threat"
    Aggro:float=0.0f

    def static Calculate(context:GASCalculationContext):GASExecutionResult {
        let power=ue_gas_captured(context,"Power")
        let armor=ue_gas_captured(context,"Armor")
        let bonus=ue_gas_caller(context,"Bonus")
        var result:GASExecutionResult
        ue_gas_output(result,"Damage",-(power-armor+bonus))
        ue_gas_output(result,"Aggro",power)
        return <-result
    }
}
[ue_gameplay_effect(attributes="ExecutionStats",execution="DamageAndThreat")]
class Strike {}

// The existing scalar shorthand uses the same native output validation/emission path.
[ue_gas_calculation(kind="Execution",attributes="ExecutionStats",output="Health")]
class ScalarDamage {
    def static Calculate(context:GASCalculationContext):float { return -1.0f }
}
[ue_gameplay_effect(attributes="ExecutionStats",execution="ScalarDamage")]
class ScalarStrike {}

// Named required SetByCaller aliases also work in magnitude calculations.
[ue_gas_calculation(attributes="ExecutionStats")]
class BonusMagnitude {
    @set_by_caller="Data.Healing"
    Bonus:float=0.0f
    def static Calculate(context:GASCalculationContext):float { return ue_gas_caller(context,"Bonus") }
}
[ue_gameplay_effect(attributes="ExecutionStats")]
class BonusEffect {
    @calculation="BonusMagnitude"
    Health:float=0.0f
}
