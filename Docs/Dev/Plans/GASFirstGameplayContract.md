# GAS: first gameplay contract

Status: initial native implementation verified: editor build and one focused
commandlet scenario passed 10 checks (see `logs/gas-validation.json`). The original proposed API below remains a design sketch. Implemented
surface and deviations are recorded here; see [GAS guide](../../Guide/Gameplay/GAS.md).

The next increment adds [authored data-only AttributeSets and instant GameplayEffects](../../Guide/Gameplay/GASAuthoring.md). Editor build passed; the focused scenario passed 9 checks on creation and 9 after reopening saved assets in another process. The original health scenario still passes all 10 checks.

A further authored-DSL increment adds native duration, period and stacking policies,
plus checked active-effect queries/removal; see the authoring guide for its current
surface. The initial starter scope below is retained as historical context.

[Declarative GameplayAbility authoring](../../Guide/Gameplay/GASAbilities.md) now
adds standard GAS cost/cooldown, grant/activate/cancel/end/revoke and typed managed
lifecycle snapshots. Arbitrary DAS activation bodies and ability tasks are deferred.

## Initial implementation decisions

- `ue_gas.das` exposes HealthAbilitySystemComponentRef, HealthAttribute,
  HealthEffect, GameplayAttributeChange, HealthEffectSpec and HealthEffectResult.
- Initialize with `ue_gas_initialize_health(asc, maximum)`; ownership/avatar are
  inferred from the checked component. No redundant owner argument is required.
- Use `[ue_event(source="Abilities",name="AttributeChanged")]` with one typed
  GameplayAttributeChange argument. The native ASC adapter bridges its GAS
  delegates to this reflected signal. Existing subscription ownership, snapshots
  and queueing are reused; named `ue_gas_watch/unwatch` are not added.
- The starter effects use HealthEffect.Heal / Damage, not new class constants.
  HealthEffectSpec is an owned reflected recipe for these two effects and
  materializes an actual GameplayEffectSpec at apply time. It does not promise
  captured source attributes or generic effect classes yet. No opaque native
  handle storage or extra VM registry is introduced.
- Native HealthEffectResult uses Valid / Applied / HasActiveHandle; the public
  apply helper rejects invalid requests. Event fields use Attribute / OldValue /
  NewValue, following existing native struct naming.
- Existing Character/pickup scripts now consume this adapter; the old authored
  HealthComponent remains available as an independent non-GAS example.

The remaining sections preserve the original design direction, not a list of
already available APIs.

Related: [roadmap](Roadmap.md), [wrapper improvement plan](ImprovementPlan.md),
[current gameplay services](../../Guide/Gameplay/GameplayServices.md).

## Goal and scope

Extend the existing PlayableCharacter / PlayablePickup / HUD example: damage
reduces Health; a potion applies a real instant GameplayEffect that restores it;
the existing HUD receives managed attribute-change callbacks. Keep score, input,
mesh loading and pickup respawn behavior.

First increment is single-player, game-thread GAS with one ASC per Character,
Health and MaxHealth, instant healing/damage effects, and attribute subscriptions.
No networking, prediction, GameplayAbility authoring, duration effects, cooldowns,
stacking, GameplayCues or arbitrary authored AttributeSet classes in this increment.
Those are subsequent capabilities, not properties of this initial adapter.

## Native backend

- Enable GameplayAbilities and declare the required GameplayAbilities,
  GameplayTasks and GameplayTags module dependencies when implementing.
- Use a real UAbilitySystemComponent and reflected UDasHealthAttributeSet with
  FGameplayAttributeData Health / MaxHealth. The native set is a starter adapter,
  not an assertion that arbitrary DAS classes can inherit UAttributeSet today.
- Initialize actor info with the Character as both owner and avatar. The managed
  initialization helper checks component ownership and registers the attribute
  set once. Repeated initialization must not reset health or duplicate the set.
  A different owner/avatar model requires a later explicit initialization API.
- Enforce finite positive MaxHealth and 0 <= Health <= MaxHealth in native hooks.
  Cover current-value changes and base/executed-effect changes; do not rely solely
  on PreAttributeChange. Lowering MaxHealth also clamps current Health.
- Provide native starter Heal / Damage effect classes: instant additive modifiers
  with registered SetByCaller magnitude tags Data.Healing / Data.Damage. A missing,
  negative or non-finite magnitude is rejected before applying either effect.
- Use typed GameplayEffectClass references and owned effect-spec handles. Do not
  reconstruct GAS state with JSON or store attribute values in DAS fields.
- Reuse the refactored type registry, checked object references, native-call guards,
  callback ownership and extern manifest. Do not introduce another kind table or
  parallel marshalling dispatcher while those systems are being consolidated.

## Proposed public surface

Names are provisional; implementation must check them against the current wrapper.

| API/value | Contract |
| --- | --- |
| AbilitySystemComponentRef | Checked native component reference, usable in ue_components. |
| HealthAttributes.Health / MaxHealth | Typed GameplayAttribute descriptors for the native starter set. Resolved through the registry, not user-provided property paths. |
| ue_gas_initialize_health(asc, owner, maximum) | Initialize once; require ASC belongs to owner; owner = avatar; initial Health = maximum. |
| ue_gas_value(asc, attribute) | Read current numeric value; reject missing sets or incompatible descriptors. |
| ue_gas_watch(key, asc, attribute, callback) | Bind a method reference with one GameplayAttributeChange argument; same key replaces the old subscription in this script instance. |
| ue_gas_unwatch(key) | Idempotent cancellation, including already queued delivery. |
| HealthEffects.Heal / Damage | Typed GameplayEffectClass constants for native starter effects. |
| ue_gas_make_spec(sourceASC, effectClass, level) | Owned spec with source context; finite positive level; retain required native resources until release/application. |
| ue_gas_set_magnitude(spec, tag, amount) | Set a registered SetByCaller magnitude; validate the starter effect's required tag and amount before application. |
| ue_gas_apply(targetASC, spec) | Apply a valid spec; return a typed result with `applied`. This is separate from an optional active-effect handle. |

Invalid references, incompatible types/worlds, missing required magnitudes and
uninitialized actor info raise a DAS runtime error through the existing call guard.
`applied=false` represents a valid request that GAS refused. A capped heal can
return `applied=true` with no attribute notification if the value did not change.
For instant effects use WasSuccessfullyApplied(), not IsValid() of the returned
FActiveGameplayEffectHandle. Active-effect removal is outside the first increment.

## Callback and lifetime contract

GameplayAttributeChange is an owned snapshot containing `attribute`, `old_value`
and `new_value`. Retaining a copy after callback return is safe. Do not expose
FOnAttributeChangeData.GEModData, aggregator pointers, borrowed structs or raw UObjects.

GetGameplayAttributeValueChangeDelegate is a native C++ multicast delegate, not a
dynamic reflected delegate. Add a managed subscription adapter; do not treat this
as an existing generic ue_event binding. Native capture copies the values and
queues DAS delivery through the existing callback pump. It must never re-enter an
active DAS VM. Reads after applying an effect see the new value synchronously;
HUD callbacks run later and describe the captured change, not a fresh read.

Destroying the script instance, EndPlay, accepted reload or explicit cancellation
unbinds delegate handles and invalidates pending deliveries. Weak references and
a subscription generation prevent stale or duplicate delivery. Callback errors
use existing managed callback reporting. Bind before gameplay changes; initialize
and refresh the HUD explicitly because binding does not synthesize an initial event.

## Proposed DAS usage

This excerpt replaces the health-related part of PlayableCharacter. Unchanged
input, HUD setup, feedback and interaction code is omitted. All GAS names are
proposed; ordinary annotations, components and method references use existing idioms.

```das
options gen2
require ue_dsl
require ue_gas
require ue_gameplay_tags

[ue_class(Blueprintable)]
class PlayableCharacter : Character {
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Abilities:AbilitySystemComponentRef
    }

    def BeginPlay():void {
        ue_gas_initialize_health(self.Abilities,ue_self_actor(),100.0f)
        ue_gas_watch("health",self.Abilities,HealthAttributes.Health,self.OnHealthChanged)
        ue_gas_watch("max-health",self.Abilities,HealthAttributes.MaxHealth,self.OnHealthChanged)
        self.RefreshHUD()
    }

    def OnHealthChanged(change:GameplayAttributeChange):void {
        // change.old_value / change.new_value remain safe to copy and retain.
        self.RefreshHUD()
    }

    [ue_function(BlueprintCallable)]
    def Heal(amount:float):bool {
        let spec=ue_gas_make_spec(self.Abilities,HealthEffects.Heal,1.0f)
        ue_gas_set_magnitude(spec,ue_tag("Data.Healing"),amount)
        let result=ue_gas_apply(self.Abilities,spec)
        return result.applied
    }

    def RefreshHUD():void {
        // Use these values in the existing HUD update after checking HUD validity.
        let current=ue_gas_value(self.Abilities,HealthAttributes.Health)
        let maximum=ue_gas_value(self.Abilities,HealthAttributes.MaxHealth)
    }
}
```

The private callback method needs no BlueprintCallable exposure: resolve it using
the existing method-reference machinery, not a reflected public trampoline. If
that machinery still requires an export for this payload, implement a hidden
callback export rather than changing the user example to BlueprintCallable.

Add `Healing:float=25.0f` to the existing pickup settings and call
`self.Collector.Heal(self.Healing)` inside its guarded successful collection path.
Preserve score and pickup behavior; a potion collected at full health is still
consumed and respawns. Register Data.Healing / Data.Damage in the demo tag config;
Item.Consumable.Potion remains the item classification, not a magnitude key.

Replace the old HealthComponent as the source of health state. Keep the existing
ReceiveDamage -> ue_apply_damage path for feedback, but route OnTakeAnyDamage to
one managed native Damage-effect bridge. The removed component must not also
subtract damage. Reuse this bridge for HUD damage and the H input action.

## Implementation order and acceptance

1. After the wrapper refactor stabilizes, add module dependencies, starter native
   AttributeSet/effects and checked ASC initialization. Regenerate discovered native
   references through the current generator; no manually maintained class list.
2. Add owned attribute/spec/result adapters and the managed subscription bridge;
   register them with existing extern/type/callback mechanisms.
3. Migrate the existing Character/pickup/HUD and tag config. Keep arbitrary authored
   AttributeSet/effect DSL design for the next increment, after the real backend works.
4. Build once and run one short focused game-world check: initialize 100; apply
   existing damage to reach 40; collect heal 25 to reach 65; apply heal 100 to cap
   at 100. Confirm applied instant effects succeed without an active handle, HUD
   notifications arrive after the call, a retained event snapshot stays unchanged,
   and cancellation suppresses a queued notification. Include a bounded invalid
   magnitude refusal in the same check. No full PIE/package matrix at this stage.

This contract itself does not change runnable scripts, native runtime or generated
bindings. Its example must not be copied into the plugin's script discovery paths
until the proposed APIs exist.

## Local Unreal source basis

Verified against the installed engine sources under
`<UnrealEngine>/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/`:

- AbilitySystemComponent.h: InitAbilityActorInfo, AddAttributeSetSubobject,
  GetGameplayAttributeValueChangeDelegate.
- AttributeSet.h: PreAttributeChange, PreAttributeBaseChange,
  PostGameplayEffectExecute and attribute clamping responsibilities.
- GameplayEffectTypes.h: FOnAttributeChangeData contains OldValue, NewValue and a
  borrowed GEModData pointer; FOnGameplayAttributeValueChange is a native multicast.
- ActiveGameplayEffectHandle.h: instant effects may report WasSuccessfullyApplied
  while IsValid is false.
