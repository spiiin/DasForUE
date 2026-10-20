# GameplayAbilities with DAS logic

`[ue_gameplay_ability]` creates a Blueprint class deriving from the native
UDasAuthoredGameplayAbility. It supports declarative effect actions and ordinary DAS ability methods,
with an optional resource cost and tag-based cooldown. Activation, commit and
completion use the native GameplayAbility lifecycle. DAS inheritance and
UE-reflected fields/methods on the ability are not supported yet.

Keep definitions alongside their AttributeSet and effects in the same module:

```das
options gen2
require ue_dsl

[ue_attribute_set]
class CombatAttributes {
    @min=0
    @max=100
    Health:float=40.0f
    @min=0
    Mana:float=20.0f
}
[ue_gameplay_effect(attributes="CombatAttributes")]
class PotionHeal { Health:float=25.0f }
[ue_gameplay_effect(attributes="CombatAttributes")]
class ManaCost { Mana:float= -5.0f }

[ue_gameplay_ability(effect="PotionHeal",cost="ManaCost",
    cooldown=0.4f,cooldown_tag="Ability.Cooldown.Restore")]
class RestoreHealth {}

[ue_gameplay_ability(effect="PotionHeal",auto_end=false)]
class HeldHeal {}
```

Effect and cost resolve by simple name or inferred definition ID within that
source's explicit effect registry. IDs default to filename.classname; explicit
`id` is optional. Unknown references, conflicting declaration annotations and
unsupported fields/options are rejected. The action applies to the ASC owner.
Its magnitudes must be constants. A cost must be an Instant effect consisting
of strictly negative Add modifiers, without SetByCaller. All referenced attribute
sets must be registered on the ASC before granting the ability.

A positive cooldown must be at least 0.01 seconds and have a valid registered
GameplayTag. Add the tag to the project's GameplayTags dictionary before import:

```ini
+GameplayTagList=(Tag="Ability.Cooldown.Restore",DevComment="Restore ability")
```

The importer checks the tag before creating any assets. Omitting cooldown means
zero seconds; setting a cooldown tag without a positive duration is refused.
Abilities which share a cooldown tag share its blocking behavior. A native
HasDuration GameplayEffect holds that tag for the chosen time; no DAS timer or
parallel cooldown registry is used.

## Import and use

`Das.GASImport gas_combat_definitions.das /Game/GAS/Combat` now also creates
GA_RestoreHealth and GA_HeldHeal. Their CDOs store typed effect classes, the
cooldown tag/duration and auto-end setting. Identical reimport preserves classes;
changed definitions require a new folder while structural migration is deferred.
These assets are saved with their effect/AttributeSet dependencies.

`require ue_gas` exports AuthoredGameplayAbilityClass and typed helpers. On an
actor with a registered AuthoredAbilitySystemComponentRef, configure the imported
class in a property and use:

```das
settings=ue_properties(EditAnywhere,BlueprintReadOnly) {
    var Restoration:AuthoredGameplayAbilityClass
}

[ue_function(BlueprintCallable)]
def RestoreWithCostAndCooldown():bool {
    let grant=ue_gas_give_ability(self.Abilities,self.Restoration)
    return ue_gas_activate_ability(grant)
}
```

Giving the same class and level is idempotent. Level is an integer in 1..10000
stored in the native spec; constant magnitudes do not scale with it. A changed
class or level under an existing definition name is refused. The ASC permits up
to 128 tracked grants. Each grant carries a checked owner reference and a local
non-reused ID; revoke and regrant produce a different ID.

| Helper | Behavior |
|---|---|
| ue_gas_give_ability(asc, kind, level=1) | Give or retrieve the same grant; invalid input is refused |
| ue_gas_activate_ability(grant) | Check native activation requirements and execute Activate, or the declarative action; false when blocked/activation fails |
| ue_gas_ability_state(grant) | Owned snapshot: Granted, Active, CanActivate, Ability, Level, CooldownRemaining, GrantId, Phase, Committed and Error |
| ue_gas_cancel_ability(grant) | Cancel an active ability |
| ue_gas_finish_ability(grant) | End an active ability normally |
| ue_gas_remove_ability(grant) | Cancel if active and revoke the grant |

An expired owner or removed grant yields inactive/ungranted state and false for
activation/cancel/finish/remove. A previously obtained state remains a snapshot.
`auto_end=true` is the default: an action can succeed and immediately end, leaving
Active=false while cooldown continues. `auto_end=false` keeps the instanced ability
active until explicit finish, cancellation or removal; retriggering is blocked.

Cost is checked using GAS and the authored minimum. Temporary current-value
bonuses cannot hide insufficient spendable base attributes. A failed resource or
cooldown check does not spend resources. Commit happens before the action; later
action rejection, cancellation or revocation does not refund committed cost or
remove cooldowns/effects. Removing and regranting an ability cannot bypass an
already active cooldown.

## Managed lifecycle events

```das
state=ue_properties(BlueprintReadWrite) {
    var LastAbilityPhase:string=""
}
[ue_event(source="Abilities",name="AbilityChanged")]
def OnAbility(change:GASAbilityChange):void {
    self.LastAbilityPhase=ue_name_text(change.Phase)
}
```

GASAbilityChange and GASAbilityState share the same native owned snapshot type.
Phases are Granted, Activated, Blocked, Rejected, Ended, Cancelled and Removed.
Queries use State. Cancelled is also a boolean field. Payload Active/Granted and
cooldown values describe the instant of the event; by callback delivery an
auto-ended ability may already be inactive. Delivery uses the existing managed
queue, never re-entering an active VM. Snapshots survive callback completion and
GC. Releasing the script owner removes subscriptions, while native gameplay state
continues until explicitly ended/revoked or the ASC owner ends play.

This is a local authority-only, non-replicated adapter with one ASC per owner.
Target data, gameplay-event activation, ability tasks, prediction/replication
and attribute-capture calculations remain
future increments.

Validation: the focused authoring commandlet built with DasRuntime/DasEditor and
passed 14 checks on creation plus 14 on reopening the same saved assets in a new
process. It checks real cost/cooldown tags and world ticks, manual and automatic
lifecycle, stale grant IDs, callback snapshots after GC and actor DSL compilation.
No full PIE or packaged run was performed. See logs/gas-ability-validation.json.


## Ordinary DAS ability methods

See `Plugins/DasForUE/Content/Das/Scripts/gas_scripted_abilities.das` for the
complete AttributeSet/effects/ability example. Require `ue_gas` and use:

```das
[ue_gameplay_ability(effect="ScriptHeal",cost="ScriptCost",
    cooldown=0.4f,cooldown_tag="Ability.Cooldown.Restore")]
class ScriptedRestore {
    Activations:int=0

    def Activate(context:GASAbilityContext) {
        self.Activations+=1
        if(!ue_gas_commit(context)) {
            ue_gas_cancel(context)
            return
        }
        if(!ue_gas_apply_ability_effect(context)) {
            ue_gas_cancel(context)
            return
        }
        ue_gas_end(context)
    }
    def Cancel(context:GASAbilityContext) {
        // Cancellation-specific cleanup.
    }
    def End(context:GASAbilityContext) {
        // Cleanup shared by normal completion and cancellation.
    }
}
```

`Activate` is required for a scripted ability. `End` and `Cancel` are optional.
These are unique ordinary instance methods returning void and taking one
by-value `GASAbilityContext`. They need no `ue_function` annotation. Ordinary
DAS helper methods and fields are allowed. Fields are private script state,
not UPROPERTY fields. One native InstancedPerActor ability owns one persistent
DAS context; separate owners/ability instances do not share these fields.

| Context field | Meaning |
|---|---|
| Ability | Checked reference to the native ability instance |
| Owner / Avatar | Typed ActorRef values from native GAS actor info |
| System | Typed AuthoredAbilitySystemComponentRef |
| GrantId / ActivationId | ASC-local grant ID and per-instance activation serial |
| Level | Native ability spec level |
| Phase | Activate, Cancel or End |
| Cancelled | True for cancellation, including its subsequent End hook |

The context is an owned snapshot. Copying or retaining it is safe; object
references remain checked. `ue_gas_commit(context)`, `ue_gas_end(context)` and
`ue_gas_cancel(context)` validate the live native ability and activation serial.
An ended/revoked instance or a context from a previous activation returns false.
Mutating a snapshot does not change native actor info. It is not an authorization
token: explicitly using the current activation's serial targets that activation.

Scripted mode never auto-commits, auto-applies the preset effect or auto-ends;
`auto_end` applies only to declarative abilities. Omitting `effect` is allowed
when Activate exists, allowing the body to perform its own gameplay work through
the owner/avatar/System. No AttributeSet is required when both preset and cost are omitted.
An active ability stays active when Activate returns
until it explicitly ends, is cancelled or is revoked. Repeated commit succeeds
without spending cost or starting cooldown twice during the same activation.
`ue_gas_apply_ability_effect(context)` applies the optional preset action to the
owner only after successful commit. Each call applies the effect again, so call
it once for a single action. Costs/cooldowns are never refunded by end/cancel.

Normal completion invokes End. Cancellation invokes Cancel and then End. Native
termination happens before those cleanup hooks: commit/end/cancel from them
return false, and a new activation is blocked until the current hook returns.
The hooks are synchronous and can execute inside Activate. ASC AbilityChanged
notifications retain their separate managed, deferred delivery semantics.

`ue_gas_activate_ability(grant)` reports accepted script activation, which can
finish synchronously without committing. Inspect state.Committed for actual
commit success and state.Error for hook failures. A failed Activate records the
error and cancels the remaining active ability. Cleanup errors are recorded too;
they do not restore ended native state. Owner/world teardown skips new script
execution. Revoking/destroying the ability releases its persistent context.

Saved ability CDOs carry the script filename and generated declaration contract.
Hook signatures/options must agree with the loaded source. Body-only edits use
the runtime's existing safe reload path; a replacement context resets DAS fields.
Contract changes require import into a new asset folder while migration is
deferred. Package the source modules along with other DAS scripts.

Focused validation for these hooks uses `Scripts/TestGASHooks.ps1`: one process
with a real ASC and native ability, explicit and external completion/cancellation,
exactly-once cost, stale activation snapshots, persistent fields and VM release.
No full PIE, reopening or packaged run is part of this check. Its current result
is recorded in `logs/gas-hooks-validation.json`.

The latest focused hooks run passed all 12 checks (exit code 0).

Tasks, targets and calculations are covered in [GASAdvanced.md](GASAdvanced.md).
