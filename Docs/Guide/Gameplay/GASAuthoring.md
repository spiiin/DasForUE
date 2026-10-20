# Authoring AttributeSets and instant GameplayEffects

The first authoring increment uses declaration annotations, an editor importer and
a managed native ASC adapter. Definitions become Blueprint classes with real
FGameplayAttributeData properties and UGameplayEffect modifiers. They are not DAS
object memory exposed as Unreal properties.

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
useful for damage. Only Instant duration is accepted in this increment.

The attributes option resolves a simple class name or its inferred definition ID
within the explicit registry of declarations in that source file. It does not
accept an arbitrary UObject path. Initial values must satisfy their limits;
linked maxima must exist, form an acyclic graph and remain compatible with minima.
Unknown names/annotations, conflicting declaration annotations, unsupported field
types and methods are rejected. New public APIs do not need a C++ class list entry.

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
reuses existing classes. Changed schemas, incompatible existing assets and
unloadable packages are refused; import a changed definition into a new folder.
Live structural migrations and arbitrary manual edits to imported assets are not
part of this first implementation. Saving a multi-asset batch is not a filesystem
transaction; a disk failure can require retrying the import.

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

Use `[ue_event(source="Abilities",name="AttributeChanged")]` on a method taking
GASAttributeChange. The owned snapshot contains Set and Attribute (UENameValue),
OldValue and NewValue. Delivery uses the existing queue and never re-enters the
active VM. Owner release/reload removes managed subscriptions; EndPlay also
removes the native GAS delegate bindings.

One ASC is allowed per owner in this adapter; use it instead of the fixed-health
ASC on that actor. Registration is idempotent for the same set class, capped at
16 sets; the source ASC retains at most 32 effect definitions for recipe lifetime.
Current/base values are clamped by native hooks, including dependent attributes
when a linked maximum decreases. Network replication, durations/stacking,
GameplayAbility authoring and attribute capture calculations remain future work.

Validation: Scripts/TestGASAuthored.ps1 builds and runs a focused commandlet
with real native attributes/effects and typed DAS callbacks, then reopens the saved
assets in a second process. Both runs passed 9 checks; the original health scenario
also passed all 10 checks. See logs/gas-authored-validation.json and
logs/gas-validation.json. No full PIE or packaged run was performed. The initial
post-refactor check is preserved in logs/gas-after-refactor-validation.json.
