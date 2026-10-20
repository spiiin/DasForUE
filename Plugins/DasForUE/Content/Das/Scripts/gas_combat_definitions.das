options gen2
module gas_combat_definitions public
require ue_dsl

[ue_attribute_set]
class CombatAttributes {
    @min=0
    @max="MaxHealth"
    Health:float=100.0f
    @min=1
    MaxHealth:float=100.0f
    @min=0
    @max=100
    Mana:float=20.0f
}

[ue_gameplay_effect(attributes="CombatAttributes")]
class PotionHeal {
    Health:float=25.0f
}
[ue_gameplay_effect(attributes="CombatAttributes")]
class Hit {
    Health:float=-60.0f
}
[ue_gameplay_effect(attributes="CombatAttributes")]
class CallerHeal {
    @set_by_caller="Data.Healing"
    Health:float=0.0f
}
[ue_gameplay_effect(attributes="CombatAttributes")]
class ManaBoost {
    @operation="Multiply"
    Mana:float=2.0f
}
[ue_gameplay_effect(attributes="CombatAttributes")]
class SetMana {
    @operation="Override"
    Mana:float=7.0f
}
