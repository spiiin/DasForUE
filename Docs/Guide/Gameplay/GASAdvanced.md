# GAS tasks, targets and calculations

`gas_advanced_abilities.das` is the combined example (require ue_gas and math). These adapters extend the
local authority-only GAS integration; they do not add replication or prediction.

## Ability tasks

An ordinary optional `Task(context:GASAbilityContext)` method receives native
task callbacks. `context.Phase` is Task and `context.Task` is an owned result:
Id, Kind, Phase, EventTag, Magnitude, Instigator and Target. Its activation serial
is checked before delivery. A saved result survives callback return.

```das
[ue_gameplay_ability]
class ListeningAbility {
    def Activate(context:GASAbilityContext) {
        if(!ue_gas_commit(context)) {ue_gas_cancel(context);return}
        ue_gas_wait_event(context,ue_tag("Data.Healing"),false)
        ue_gas_wait_delay(context,1.0f)
    }
    def Task(context:GASAbilityContext) {
        if(ue_name_text(context.Task.Kind)=="Delay") {ue_gas_end(context)}
    }
}
```

| Helper | Native implementation / result |
|---|---|
| ue_gas_wait_delay(context, seconds) | UAbilityTask_WaitDelay; Delay / Finished |
| ue_gas_wait_event(context, tag, once=true, exact=true) | UAbilityTask_WaitGameplayEvent; GameplayEvent / Received |
| ue_gas_play_montage(context, montage, rate=1, section="") | UAbilityTask_PlayMontageAndWait; Montage / BlendedIn, BlendOut, Completed, Interrupted or Cancelled |
| ue_gas_cancel_task(context, taskId) | Unbind and end one owned task without a script callback |

A positive ID means the task was accepted; zero rejects invalid, inactive, stale
or over-capacity requests. Delay is bounded to 0.01..3600 seconds. An ability may
own at most 64 tracked tasks. IDs are not reused by the same native instance.
The ability needs a Task method to create tasks. Montage requires a live native
AnimInstance and a valid montage/section; montage gameplay callbacks use native
GAS rules. A task may finish synchronously while it is being created, so callback
code should use context.Task.Id rather than assume the caller already stored it.

End, cancel and revoke stop outstanding tasks and remove their listeners before
the End hook. They do not emit additional Task callbacks; Cancel/End remain the
cleanup hooks. A callback from a previous activation is discarded. Repeating
event tasks stop only on explicit task cancellation or ability completion.

`asc.SendAuthoredGameplayEvent(tagName, magnitude, instigator, target)` validates
the world and registered tag and forwards FGameplayEventData into the native ASC.
Its bool reports a valid submitted event, not a guarantee that a listener exists.

## TargetData

`GASTargetData` contains checked Actors and source-owned native hit IDs. It does not
retain native target-data pointers. Choose actors directly or reuse hit results
from the existing trace/sweep APIs:

```das
var targets=ue_gas_target(otherActor)
// Alternatively: var targets=ue_gas_hit_target(context.System,hit)
var spec=ue_gas_make_spec(context.System,damageEffect)
var results <- ue_gas_apply_targets(context,spec,targets)
```

Outside an ability, `ue_gas_apply_targets(asc,spec,targets)` applies a normal
authored effect. The context overload requires a current committed activation
and the same source ASC. Both return an array of per-target GASEffectResult.
Instant results now also identify their Target ASC.

Native hits are copied from the existing opaque UEHitResult adapter. The source ASC
holds at most 1024 immutable hit copies under non-reused IDs. Call
`ue_gas_release_targets(targets)` after use; release invalidates every copy of those
hit IDs. ASC EndPlay releases remaining entries. Actor-only targets need no release.

Native FGameplayAbilityTargetData_ActorArray / SingleTargetHit values are built at
application. At most 64 input entries are accepted. Actors are deduplicated;
for multiple hits on one actor the last hit supplies that target's effect context.
The hit is copied into the native GameplayEffectContext. Expired actors, invalid
positions, foreign worlds and actors without the authored ASC are refused before
application. Native attribute/capture compatibility is checked per target. This
is not an atomic multi-target transaction: inspect every result. Empty input or
invalid target membership returns an empty array.

Local trace/sweep/overlap selection and explicit confirmation/cancellation are
available through [GAS targeting](GASTargeting.md). Native target-actor UI and
network TargetData transport remain future work.

## Captures and DAS calculations

Calculations are separate classes with an ordinary static float method. Float
fields declare captures, not object state. `@source` chooses Source or Target,
`@snapshot` defaults to true, `@attribute` overrides the attribute name, and
`@attributes` optionally selects another set from the same source registry.

```das
[ue_gas_calculation(attributes="BattleStats")]
class DamageMagnitude {
    @source="Source"
    Power:float=0.0f
    @source="Target"
    @snapshot=false
    Armor:float=0.0f
    def static Calculate(context:GASCalculationContext):float {
        return -max(0.0f,ue_gas_captured(context,"Power")-ue_gas_captured(context,"Armor"))*context.Level
    }
}
[ue_gameplay_effect(attributes="BattleStats")]
class Damage {
    @calculation="DamageMagnitude"
    Health:float=0.0f
}
```

The importer creates GC_DamageMagnitude, a real Blueprint subclass of the native
UGameplayModMagnitudeCalculation adapter, and assigns it to the modifier.
Calculations may have up to 16 named captures. Short names resolve through the
same source's explicit AttributeSet/calculation registry. Unknown captures,
wrong calculation kinds, duplicate aliases and conflicting literal/SetByCaller
magnitudes are rejected before creating assets. Capture defaults must be zero.

Execution calculations add one native output modifier:

```das
[ue_gas_calculation(kind="Execution",attributes="BattleStats",output="Health")]
class DamageExecution {
    @source="Source"
    Power:float=0.0f
    def static Calculate(context:GASCalculationContext):float {
        return -ue_gas_captured(context,"Power")
    }
}
[ue_gameplay_effect(attributes="BattleStats",execution="DamageExecution")]
class ExecutedDamage {}
```

These create real UGameplayEffectExecutionCalculation subclasses. Output
operation supports Add, Multiply and Override. Execution effects must be Instant
or periodic. Multiple output calculations can be composed as separate effects;
arbitrary dynamic output lists and execution-scoped modifiers are not included.
Ability costs still require strictly negative constant Instant Add modifiers.

GASCalculationContext contains Level, Values (named captures), SetByCaller (tag
names to supplied magnitudes), Source and Target. Target is available for native
executions; native MMC does not expose a target ASC, so its Target is empty.
Use captured values for MMC target-dependent work. No execution parameters,
aggregators or spec pointers escape the native evaluation call.

The GAS snapshot boundary is native GameplayEffectSpec creation. The existing
GASEffectSpec DAS value is a descriptor: its native spec is created on application.
Calling ue_gas_make_spec alone therefore does not freeze source attributes.
Non-snapshot captures retain native aggregator/recalculation behavior.

Calculate must be stateless and return a finite float; it may be evaluated more
than once by GAS. Its cached program is owned by the native calculation CDO, not
by an ability/actor. Source contract changes require reimport; body reload uses
the existing runtime validation. Game-thread execution is required. Script errors
are logged; MMC returns zero and execution omits its output on failure. This does
not roll back already applied native modifiers. Calculations must not perform
gameplay side effects or depend on mutable global state.

## Focused validation

`Scripts/TestGASAdvanced.ps1` builds the plugin modules and runs one commandlet
scenario. It covers native delay/event tasks, stored event snapshots, end/cancel/
revoke cleanup, actor/hit TargetData, stale targets, MMC source snapshots and live
target captures, and native execution output. Montage's invalid-input path is
checked; full animation playback, PIE and packaged validation are not part of this
short scenario. Result and source/DLL hashes: logs/gas-advanced-validation.json.

Latest focused run: PASS, 12 checks, commandlet exit code 0. The authored ASC uses a local effect context that explicitly retains the source ASC for Blueprint/DSL owners without the native C++ AbilitySystemInterface.
