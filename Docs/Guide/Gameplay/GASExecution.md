# Authored GAS execution outputs

`require ue_gas` exposes `GASExecutionResult`, `ue_gas_output` and named required `ue_gas_caller` inputs. A complete example is [gas_execution_outputs.das](../../../Plugins/DasForUE/Content/Das/Scripts/gas_execution_outputs.das).

```das
[ue_gas_calculation(kind="Execution",attributes="CombatStats")]
class DamageAndThreat {
    @source="Source"
    @scoped_add=5.0f
    Power:float=0.0f

    @source="Target"
    @snapshot=false
    Armor:float=0.0f

    @set_by_caller="Data.DamageBonus"
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
        var result:GASExecutionResult
        ue_gas_output(result,"Damage",-(power-armor+ue_gas_caller(context,"Bonus")))
        ue_gas_output(result,"Aggro",power)
        return <-result
    }
}
[ue_gameplay_effect(attributes="CombatStats",execution="DamageAndThreat")]
class Strike {}
```

CombatStats in the fragment is an explicit `[ue_attribute_set]` registry entry declaring Health/Threat/Power/Armor. Data.DamageBonus must be a registered GameplayTag. Calculation fields describe schema, not mutable attribute storage; every declaration field is float with zero default.

## Output declarations

`@output=true` marks an output alias. `@attribute` selects an attribute (defaults to the field name), `@attributes` selects another explicitly registered AttributeSet (defaults to the calculation's attributes option). `@operation="Add"`, `"Multiply"` or `"Override"` selects a native GameplayModOp. Multiply uses native MultiplyCompound. Outputs always modify the target ASC; targeting another actor is handled by TargetData, not by an output alias.

`Calculate(context):GASExecutionResult` supplies a map from declared output aliases to finite floats. `ue_gas_output` rejects duplicate insertion and non-finite values. The native adapter validates the entire map before adding any execution modifier, rejects unknown aliases and emits modifiers in declaration order. Omitted outputs are allowed; an empty result emits nothing. Up to 16 output declarations, 16 captures and 16 required caller aliases are supported.

For one output, the existing scalar shorthand remains valid:

```das
[ue_gas_calculation(kind="Execution",attributes="CombatStats",output="Health")]
class FlatDamage {
    def static Calculate(context:GASCalculationContext):float { return -1.0f }
}
```

It is normalized into the same declared output list and uses the same native validation/emission. Scalar execution output fields are not allowed; structured execution cannot combine output fields with a class-level output option or a class-level non-default operation. Set operations on each output field instead. Magnitude calculations still return float.

## Required named SetByCaller

A field annotated only with `@set_by_caller="Registered.Tag"` declares a required input alias. Read it with `ue_gas_caller(context,"Alias")`. These aliases share the calculation's Values dictionary with captures, so all field names are unique (including case-insensitive FName comparison). SetByCaller retains the original tag-keyed values for existing readers.

Supply values on the effect spec with `ue_gas_set_magnitude(spec,ue_tag("Registered.Tag"),amount)`. The importer collects required tags from both native magnitude and execution calculations into the effect's RequiredMagnitudes. The authored apply path validates presence, finiteness and rejects undeclared tags before creating/applying the native spec. Zero is a supplied value. Declarative ability effect/cost references cannot automatically supply these values and are rejected; scripted ability logic can build a spec and provide them explicitly.

If an external native caller bypasses authored apply and constructs an incomplete native spec, the calculation emits no outputs (or zero for MMC) and logs a missing-caller diagnostic. This is not rollback for other parts of a native effect.

## Native execution-scoped capture modifiers

A capture field may have one finite numeric `@scoped_add`, `@scoped_multiply` or `@scoped_override`. These options are allowed only on execution captures, not on outputs, callers or magnitude calculations. The importer creates real FGameplayEffectExecutionScopedModifierInfo entries in the effect's native Execution.CalculationModifiers. They are folded into capture evaluation by FGameplayEffectCustomExecutionParameters, not emulated in DAS.

The source/target attributes and their base/current values are unchanged. The temporary modifier is tied to the native capture definition, so two aliases of the same native attribute/source/snapshot see the same scoped aggregate. Snapshot/live capture rules still follow native GAS. This step supports literal scoped magnitudes; tag requirements, SetByCaller magnitudes on the scoped modifier itself and transient aggregators are not exposed yet.

## Validation and remaining limits

All output attributes resolve through the existing explicit registry and are checked for presence on the target before authored application. Native contract hashes include outputs/callers/scoped metadata and output AttributeSet hashes. Changed captures, outputs and callers update the calculation in place; changing its kind (MMC/Execution) needs a fresh import folder ([Changing definitions](GASAuthoring.md#changing-definitions)).

Script errors or an invalid execution result omit this execution's output; they do not roll back direct modifiers or other executions in the same GameplayEffect. Native attribute clamps and callbacks retain normal GAS semantics. Successful montage playback, target-actor UI, gameplay cues, replication and prediction remain separate work.

The focused `gas-execution` suite performs one short commandlet run after the build: two outputs, required/unknown inputs, zero input, native scoped modifiers without persistent mutation/accumulation, scalar shorthand and MMC caller aliases. Run builds/tests only through the build agent queue; see [Testing](../../Dev/Testing.md).

Verified on this increment: agent job `gasexec-run-17f7c2130e`, exit 0, Editor Development build and `DAS_GAS_EXECUTION_RESULT=PASS checks=8` in one commandlet process. No full PIE or packaged run.
