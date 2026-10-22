# Authoring AttributeSets and GameplayEffects

The first authoring increment uses declaration annotations, an editor importer and
a managed native ASC adapter. Definitions become Blueprint classes with real
FGameplayAttributeData properties and UGameplayEffect modifiers. They are not DAS
object memory exposed as Unreal properties.

Declarative GameplayAbility actions, costs and cooldowns are described in
[GameplayAbilities](GASAbilities.md).

Keep definitions in one local Scripts module. No inheritance or user methods are
supported in these data-only declarations yet. Values use float, as GAS does.

```das
options gen2
require ue_dsl

[ue_attribute_set]
class CombatAttributes {
    @min=0
    @max="MaxHealth"
    Health:float=100.0f
    @min=1
    MaxHealth:float=100.0f
    @min=0
    Mana:float=20.0f
}

[ue_gameplay_effect(attributes="CombatAttributes")]
class PotionHeal {
    Health:float=25.0f
}

[ue_gameplay_effect(attributes="CombatAttributes")]
class VariableHeal {
    @set_by_caller="Data.Healing"
    Health:float=0.0f
}
```

An effect field names the attribute it modifies. Its default is the constant
magnitude. `@operation="Add"` is the default; Multiply and Override are also
supported. A SetByCaller modifier takes its magnitude from the spec instead of
using the field default. Signed finite magnitudes are allowed; negative Add is
useful for damage. Omitting duration preserves Instant behavior. A positive numeric
`duration` creates a HasDuration effect in seconds; `duration="Infinite"` stays active
until removed. Active non-periodic modifiers affect the current aggregate and
revert when removed. Periodic modifiers execute into the base value, so their
completed changes survive removal.

The attributes option resolves a simple class name or its inferred definition ID
within the explicit registry of declarations in that source file. It does not
accept an arbitrary UObject path. Initial values must satisfy their limits;
linked maxima must exist, form an acyclic graph and remain compatible with minima.
Unknown names/annotations, conflicting declaration annotations, unsupported field
types and methods are rejected. New public APIs do not need a C++ class list entry.

## Duration, periods and stacks

```das
[ue_gameplay_effect(attributes="CombatAttributes",duration=5.0f)]
class Fortitude { MaxHealth:float=20.0f }

[ue_gameplay_effect(attributes="CombatAttributes",duration="Infinite",
    stacking="AggregateByTarget",stack_limit=3)]
class ManaAura { Mana:float=3.0f }

[ue_gameplay_effect(attributes="CombatAttributes",duration=5.0f,
    period=1.0f,execute_on_application=false)]
class ManaRegen { Mana:float=2.0f }
```

Period is zero by default. A nonzero period must be at least 0.01 seconds and
requires HasDuration or Infinite. `execute_on_application` defaults to true and
can only be set explicitly for periodic effects. Numeric duration must be finite
and positive; use the Infinite policy instead of a negative number.

Stacking is None by default. AggregateBySource combines applications from the same
source ASC; AggregateByTarget combines them on the target regardless of source.
Both require an explicit `stack_limit` in 1..128. `deny_overflow=true` by default:
an application above that limit reports Applied=false without clearing the
existing stack. `deny_overflow=false` uses GAS's capped application behavior.

Optional policies use the Unreal enum names:

| Option | Values | Default |
|---|---|---|
| stack_duration | RefreshOnSuccessfulApplication, NeverRefresh, ExtendDuration | RefreshOnSuccessfulApplication |
| stack_period | ResetOnSuccessfulApplication, NeverReset | ResetOnSuccessfulApplication |
| stack_expiration | ClearEntireStack, RemoveSingleStackAndRefreshDuration, RefreshDuration | ClearEntireStack |

Duration and expiration policies require a timed stacking effect; period reset
requires a periodic stacking effect. Options which cannot affect the chosen policy
are rejected instead of silently ignored. GAS controls its timers, aggregators,
stack scaling and expiration; the bridge does not implement a parallel scheduler.

## Import in the editor

After building and regenerating the native registry, use:

```text
Das.GASImport gas_combat_definitions.das /Game/GAS/Combat
```

This creates AS_CombatAttributes and GE_PotionHeal (plus the other effect classes
in the source) as normal Blueprint assets. Rules and effect modifiers are stored
as typed native data in their CDOs; JSON is only the compiler/import transport.
SetByCaller tags must already exist in the project's GameplayTags dictionary.

The importer preflights the batch before creating assets. Identical reimport
reuses existing classes.

### Changing definitions

Reimporting into the same folder updates the assets in place: the Blueprints and
their classes stay the same, so references from other Blueprints, maps and
abilities keep working.

- Effects, abilities, calculations (MMC, Execution) and cues: any data change
  (magnitudes, operations, durations, stacking, effect/cost/cue links, cooldowns,
  script entries) rewrites the asset's CDO.
- AttributeSets: new attributes are added as variables; initial values and limits
  change in place.
- Refused (import into a new folder): removing or renaming an attribute, changing
  a cue tag, changing a calculation kind (MMC/Execution), changing a stable id.
- A failure restores every asset updated by the import.

Saving the `.das` file updates the folder it was imported into (auto-reimport,
`Das.DSL.AutoReimport`): every loaded GAS asset remembers its script and folder.
During Play the update waits until Play stops. Updated assets stay dirty until
saved; `Das.GASImport` saves them. Assets imported before this version remember
their folder after one `Das.GASImport` into the same folder.

Incompatible existing assets and unloadable packages are still refused. Saving a
multi-asset batch is not a filesystem transaction; a disk failure can require
retrying the import.

## Use from gameplay

`require ue_gas` exposes AuthoredAbilitySystemComponentRef, AttributeSetClass and
AuthoredGameplayEffectClass. Add the native component using ue_components and
configure typed class fields from the imported Blueprint assets in Details.

```das
components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
    var Abilities:AuthoredAbilitySystemComponentRef
}
settings=ue_properties(EditAnywhere,BlueprintReadOnly) {
    var Attributes:AttributeSetClass
    var HealEffect:AuthoredGameplayEffectClass
}
```

Inside BeginPlay:

```das
ue_gas_add_attributes(self.Abilities,self.Attributes)
let current=ue_gas_value(self.Abilities,"CombatAttributes","Health")
let spec=ue_gas_make_spec(self.Abilities,self.HealEffect)
let result=ue_gas_apply(self.Abilities,spec)
```

For a caller magnitude:

```das
var spec=ue_gas_make_spec(self.Abilities,self.HealEffect)
ue_gas_set_magnitude(spec,ue_tag("Data.Healing"),25.0f)
let result=ue_gas_apply(self.Abilities,spec)
```

Only required caller tags are accepted and every required magnitude must be
provided. Missing attributes or magnitudes, non-finite values, foreign worlds and
dead sources are refused before applying the effect. An instant effect can have
Applied=true and HasActiveHandle=false.

For an active effect:

```das
let result=ue_gas_apply(self.Abilities,ue_gas_make_spec(self.Abilities,self.AuraEffect))
let state=ue_gas_effect_state(result)
if(state.Active && state.Stacks>1) {
    ue_gas_remove_effect(result,1) // remove one stack
}
ue_gas_remove_effect(result) // remove all remaining stacks
```

State is an owned snapshot with Active, Infinite, Stacks, Duration, Remaining and
Period. Infinite effects report Remaining=-1. Removed/expired effects return an
inactive state; repeated removal returns false. HasActiveHandle records the apply
result, so query Active for current liveness. The result carries a checked target
reference and a local int32 ID; the target retains the real native GAS handle.
IDs are never reused on that ASC, and the handle table is bounded to 1024 tracked
active effects. EndPlay invalidates it. Script release removes subscriptions but
does not cancel gameplay effects. Existing fixed-health APIs remain unchanged.

Use `[ue_event(source="Abilities",name="AttributeChanged")]` on a method taking
GASAttributeChange. The owned snapshot contains Set and Attribute (UENameValue),
OldValue and NewValue. Delivery uses the existing queue and never re-enters the
active VM. Owner release/reload removes managed subscriptions; EndPlay also
removes the native GAS delegate bindings.

One ASC is allowed per owner in this adapter; use it instead of the fixed-health
ASC on that actor. Registration is idempotent for the same set class, capped at
16 sets; the source ASC retains at most 32 effect definitions for recipe lifetime.
Current/base values are clamped by native hooks, including dependent attributes
when a linked maximum decreases. Network replication, arbitrary DAS ability bodies/tasks and attribute capture calculations remain future work.

Validation: Scripts/TestGASAuthored.ps1 builds and runs a focused commandlet
with real native attributes/effects and typed DAS callbacks, then reopens the saved
assets in a second process. The duration/stacking extension is covered by the same focused scenario, including
real world ticks, overflow, partial removal, expiration and detached snapshots.
The extended scenario passed 11 checks on creation and 11 on reopening saved assets.
The two plugin modules (DasRuntime and DasEditor) built successfully; the full
engine target was not rebuilt because another chat was using shared engine DLLs.
The original health scenario is tested separately. See logs/gas-authored-validation.json and
logs/gas-validation.json. No full PIE or packaged run was performed. The initial
post-refactor check is preserved in logs/gas-after-refactor-validation.json.
