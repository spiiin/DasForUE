# GAS health and instant effects

Initial single-player adapter for Unreal Gameplay Ability System. It uses a real
UAbilitySystemComponent, UAttributeSet and instant UGameplayEffect subclasses.
Editor build and the focused runtime scenario passed 10 checks; details are in
`logs/gas-validation.json`. Re-run with `Scripts/TestGAS.ps1`. No networking or prediction is provided.

```das
options gen2
require ue_gas

[ue_class(Blueprintable)]
class Fighter : Character {
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Abilities:HealthAbilitySystemComponentRef
    }
    def BeginPlay():void {
        ue_gas_initialize_health(self.Abilities,100.0f)
    }
    [ue_event(source="Abilities",name="AttributeChanged")]
    def OnAttribute(change:GameplayAttributeChange):void {
        // Detached copy: Attribute, OldValue, NewValue. Safe to retain.
    }
    [ue_function(BlueprintCallable)]
    def Heal(amount:float):bool {
        return ue_gas_heal(self.Abilities,amount)
    }
}
```

Read current values with `ue_gas_value(asc,HealthAttribute.Health)` or MaxHealth.
Only one ASC is allowed on the owner by this adapter. Owner and avatar are the
same actor. Repeated initialization preserves existing values. MaxHealth is
positive and Health is clamped to [0, MaxHealth]. Native values use float.

The adapter registers native magnitude tags Data.Healing and Data.Damage. These
are independent of item classification tags such as Item.Consumable.Potion.

```das
var spec=ue_gas_make_spec(self.Abilities,HealthEffect.Heal)
ue_gas_set_magnitude(spec,ue_tag("Data.Healing"),25.0f)
let result=ue_gas_apply(self.Abilities,spec)
// result.Applied may be true while result.HasActiveHandle is false: instant GE.
```

A spec is an owned snapshot recipe, reusable/copyable without borrowing native
memory. An actual GAS spec is made when applying it. It supports the two starter
effects, a positive finite level and one required SetByCaller magnitude. Authored
effect classes, durations, periods, stacks and active-effect management use the
separate [authored GAS adapter](GASAuthoring.md); attribute capture calculations
remain future work. Level is stored on the GAS spec; these starter effects do not scale
magnitude by level. Amount is finite and nonnegative; damage applies its negative
internally. Invalid input, wrong tags, missing magnitudes, uninitialized or foreign
world sources are rejected. A valid request blocked by GAS returns Applied=false.
Healing at maximum may succeed without an attribute change callback.

Owner OnTakeAnyDamage is bridged once to the instant damage effect. Use the
existing ue_apply_damage path; do not also subtract health in a DAS damage handler
or attach the old non-GAS HealthComponent to the same actor.

Native attribute changes are copied into a reflected signal; the existing managed
ue_event transport owns the captured payload. It queues callbacks while DAS is
active and drains after the active call, without VM re-entry. Releasing/reloading
the script removes its subscriptions. EndPlay detaches the adapter's native GAS
and owner damage delegates. Health itself belongs to the native ASC and survives
script VM replacement. Arbitrary authored AttributeSet/Ability classes, effect
stacking, replication and production packaging are outside this first increment.

The playable Character/pickup example heals 25 in addition to awarding score;
full-health pickups are still consumed and respawn. Re-export its Blueprint/map
with CreatePlayableCharacter.ps1 after updating the plugin and generated native
registry; an old saved Blueprint does not acquire new components automatically.

For user-defined sets and effects, see [GAS authoring](GASAuthoring.md).
