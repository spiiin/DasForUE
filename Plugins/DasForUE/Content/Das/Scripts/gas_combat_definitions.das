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

// Temporary MaxHealth bonus; removal restores the aggregate, not the base value.
[ue_gameplay_effect(attributes="CombatAttributes",duration=0.5f)]
class Fortitude {
    MaxHealth:float=20.0f
}

[ue_gameplay_effect(attributes="CombatAttributes",duration="Infinite",
    stacking="AggregateByTarget",stack_limit=3)]
class ManaAura {
    Mana:float=3.0f
}

// Periodic effects execute into the base attribute; their gains survive removal.
[ue_gameplay_effect(attributes="CombatAttributes",duration=0.5f,
    period=0.1f,execute_on_application=false)]
class ManaRegen {
    Mana:float=2.0f
}

[ue_gameplay_effect(attributes="CombatAttributes",duration=0.3f,
    stacking="AggregateBySource",stack_limit=3,stack_duration="NeverRefresh",
    stack_expiration="RemoveSingleStackAndRefreshDuration")]
class LayeredFortitude {
    MaxHealth:float=5.0f
}

[ue_gameplay_effect(attributes="CombatAttributes")]
class ManaCost { Mana:float= -5.0f }

[ue_gameplay_ability(effect="PotionHeal",cost="ManaCost",
    cooldown=0.4f,cooldown_tag="Ability.Cooldown.Restore")]
class RestoreHealth {}

// Demonstrates an ability which remains active until finished or cancelled.
[ue_gameplay_ability(effect="PotionHeal",auto_end=false)]
class HeldHeal {}
