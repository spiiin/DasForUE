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

## Targeted strike with impact and a timed aura

[gas_targeted_strike.das](../../../Plugins/DasForUE/Content/Das/Scripts/gas_targeted_strike.das) combines activation-owned targeting, a native gameplay-event task, commit, target effect application and cue assets. It reads editable effect classes through the typed AbilityPlaygroundRef, rather than hard-coding effect class paths. The declaration-only reference lives in ue_gas_playground_refs.das.

The `gas-ability-playground` build-agent suite prepares two placeable Blueprints under `/Game/DasExamples/GASCues`: BP_AbilityPlayground (caster) and BP_AbilityTarget (visible sphere target). It also imports the existing cue definitions, GA_TargetedStrike and GE_PlaygroundTimedAura. The target receives PlaygroundStats; the caster's Attributes/HitEffect/AuraEffect/StrikeAbility class properties are assigned automatically. Existing demo Niagara copies are reused. These are generated demo assets, separate from user maps/projects.

Place the caster facing a target within its Range (600 by default). Connect your input Blueprint to these methods:

1. BeginAim activates the ability, performs a Visibility sphere sweep (Radius 5) and exposes PreviewTarget. No damage, cue or commit happens yet.
2. ConfirmAim sends the registered GameplayEvent.Das.Sample.StrikeConfirm event. The ability repeats the current actor-position/forward-vector query, confirms exactly one target, commits and applies PlaygroundHit and PlaygroundTimedAura to that target. Hit damage is one point. The real native TargetData hit positions the sound/Niagara burst at ImpactPoint.
3. The ability ends and releases its wait tasks. The target's two-second native duration effect remains alive independently, producing OnActive/WhileActive for the fountain. Native expiry invokes Removed, which destroys the owned aura component.
4. CancelAim cancels an outstanding aim without effects. A miss at confirmation cancels too. AimTimeout (five seconds by default) bounds an unanswered aim.

The caster actor's transform is the ray origin/direction; a player input graph can update that transform from its camera before BeginAim/ConfirmAim. PreviewTarget is an initial preview, not a continuously updated reticle. LastTarget, Shots and CancelledShots expose the result to UI/Blueprints. The visible target blocks Visibility and requires the authored ASC/compatible AttributeSet. This is local authoritative gameplay; networking/prediction remain deferred.

Hit and aura are separate native effect applications, not a transactional pair. Both class references are checked/specs prepared before applying. Application failure cancels the ability but does not roll back an already-applied hit. Committed effects belong to the target after the ability ends; cancelling a later activation does not undo them. The sample uses the same bounded named impact and idempotent aura helpers as [GASCues.md](GASCues.md). General effect stacking/cue aggregation policy is native GAS, not a new transaction/stacking layer in this example.

The single focused offscreen runtime scenario checks actual trace preview, harmless cancel, confirmation/commit and separate source/target attributes, real sound/Niagara components at the native hit point, aura retention after ability end and GC, native timed expiry/Removed cleanup, re-querying aim before confirmation, and EndPlay cleanup. It builds first through the shared agent; full PIE, packaged/cook validation and perceptual audio/visual QA are outside this step. The fixed sound/Niagara soft paths still require explicit cook inclusion as documented for the cue playground.

Verified by build-agent job `gasstrike-20261020b`: Editor Development build and `DAS_GAS_ABILITY_PLAYGROUND_RESULT=PASS checks=9` in one offscreen runtime scenario (8.75 seconds of commandlet logic). The scenario uses real native physics hits, GameplayAbility tasks, effects and sound/Niagara components; full PIE/packaged/network and perceptual audio/visual checks were not run. Cancellation is checked through the DAS Cancel hook counter, inactive/uncommitted state and released tasks: the State query does not retain the Cancelled bit from terminal notification snapshots.


## Playable map and real PIE

The `gas-ability-pie` build-agent suite generates and saves
`/Game/DasExamples/GASCues/GASAbilityMap` in the smoke project. Open that map
in the editor and use Play. The map selects its own GameMode; project defaults
are not changed. Generated assets stay in the dedicated example folder.

`dsl_gas_playground_player.das` is the first-person DAS Character. Its typed
`Caster` reference points to the map's `BP_AbilityPlayground` actor. Each tick,
and immediately before input actions, that actor follows the camera transform,
so the existing ability traces from the player's viewpoint. The Character's
capsule ignores Visibility so it does not block its own aim. Two target spheres
have independent authored AttributeSets. A small native HUD draws a reticle,
controls and the last selected target's health; input and ability behavior live
in DAS.

| Control | Action |
|---|---|
| WASD / mouse / Space | Move / look / jump |
| Hold right mouse or Q | Begin targeting |
| Left mouse or E while aiming | Confirm the current trace |
| C or release aim before confirmation | Cancel |
| Escape | Leave PIE |

Aim preview is captured on activation; confirmation repeats the current camera
trace. A hit subtracts one health point, plays the native impact sound/Niagara,
and applies `PlaygroundMapAura`, a five-second effect. Native expiry calls
Removed and destroys its aura component. The older commandlet keeps its separate
two-second `PlaygroundTimedAura`. This remains local authoritative gameplay;
networking and prediction are deferred.

The focused PIE suite opts into a real RHI/audio instead of NullRHI. It drives
mapped keyboard/mouse events through PlayerController, checks movement, jumping,
camera rotation, aim/cancel/confirm, native sound/Niagara components, duration
expiry and EndPIE context release. It saves `logs/gas-ability-pie.png`. No
packaged run is included. This automated check proves input routing and native
component lifecycle; it does not establish perceptual audio quality.


Input action/mapping references are stored on the placed Character instance,
not in its Blueprint CDO. This keeps generated class defaults consistent with
the DSL and allows preparing the same map again. The preparation normalizes
only input fields of the dedicated generated demo Blueprint from earlier runs.
The screenshot is requested after Niagara has advanced for several frames.
Gameplay waits use the native PIE world clock; a separate 240-second wall limit
bounds startup/render-readback stalls.

Verified by build-agent job `gasmap-20261020e`: Editor Development build and
`DAS_GAS_ABILITY_PIE_RESULT=PASS checks=10`; Unreal Automation reports one
success, zero failed tests and zero tests with warnings
(16.86 seconds). The PIE check covers mapped input,
camera aim, harmless cancellation, confirmed target damage, sound/Niagara
components, native timed expiry/Removed, viewport capture and complete session
release after EndPIE. Results/source hashes are recorded in
`logs/gas-ability-pie-validation.json`. Packaged/network checks were not run.


## Packaged map

The `gas-ability-packaged` agent suite regenerates the same map and packages it
in Win64 Development. Cue Niagara assets and cue-notify Blueprints must be
cooked even when they are found by script soft paths or GAS discovery. The
runner includes their demo folder and engine sounds explicitly. The example's
GameMode registers its cue search path in both editor and cooked games; it does
not persist changes to project configuration.

The archived game is under `Build/Packaged/Windows`. For ordinary interactive
play, launch its DasForUESmoke executable with map argument
`/Game/DasExamples/GASCues/GASAbilityMap`. Controls are the same as PIE. The
`-DasGASAbilityPackaged` switch instead starts the automated mapped-input probe
and exits after its result. This validates the dynamic DAS runtime in a cooked
Development game; it does not establish Shipping, AOT or networking support.

Verified by build-agent job `gaspackage-20261020e` on 2026-10-20:
Editor/game builds, cook/stage/archive and actual Win64 Development launch
passed. The runtime reports `DAS_GAS_ABILITY_PACKAGED_RESULT=PASS checks=12`;
the complete focused suite took 182 seconds after engine build caches were warm.
The six DAS modules were read from the archive's `RemappedPlugins` tree.
Checks cover cooked input/GAS/mesh assets, possession/HUD, mapped move/jump/look,
aim preview/cancel/confirm, target damage, a playing audio component on a live
audio device, Niagara systems, native aura expiry/Removed, viewport capture and
release of DAS contexts. The captured image shows impact/aura particles and
target health 99. Audio playback state was checked; listening quality was not.
Proof and hashes are in `logs/gas-ability-packaged-validation.json`, runtime log
in `logs/gas-ability-packaged.log`, image in `logs/gas-ability-packaged.png`.

This run caught a cooked-build portability bug: UE FNames can expose `ID`
where Editor-generated DAS declarations use `Id`. Native struct field lookup
now compares FName identity and rejects ambiguous declarations; generated
native schema/call signature checks ignore identifier casing while preserving
field/type/flag checks. Export also reloads existing input assets before saving
and fully replaces only the dedicated generated example map.

## Shipping acceptance and AOT limitation

Build-agent job `gasship-20261020b` passed the same map in actual Win64 Shipping,
with 12 checks, including a pure native AOT check that requires `bUsedAot`.
The Shipping executable is under `Build/PackagedShipping/Windows`; logs and
hashes are `logs/gas-ability-shipping.txt` and
`logs/gas-ability-shipping-validation.json`. The probe is explicitly enabled
only in this acceptance build; engine Shipping optimization and logging remain
unchanged. It checks bundled DAS/cooked assets, mapped controls, targeting,
cancel/commit/damage, sound/Niagara, native expiry/Removed and context cleanup.

The map's DAS gameplay and cue hooks still execute in the VM. The separate
pure `aot_smoke` function executes linked native code without fallback.
UE externs remain `noAot` until an AOT bridge preserves their execution guards;
this result must not be described as full AOT of the GAS map. Networking was
not tested. See [Shipping test setup](../../Dev/Testing.md#gas-map-in-shipping-and-aot-scope).
