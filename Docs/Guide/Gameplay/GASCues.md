# Gameplay Cues

`require ue_gas` exposes native GameplayCue sending and owned callback snapshots on the authored ASC. A compact actor/handler example and focused probe is [gas_cues.das](../../../Plugins/DasForUE/Content/Das/Scripts/gas_cues.das).

```das
let hit=ue_tag("GameplayCue.Das.Sample.Impact")
let aura=ue_tag("GameplayCue.Das.Sample.Aura")
verify(ue_gas_cue_watch(self.Abilities,hit),"Cannot register cue handler route")
verify(ue_gas_cue_watch(self.Abilities,aura),"Cannot register cue handler route")
var params=ue_gas_cue_params(7.0f)
params.Location=ue_vector(100.0lf,0.0lf,50.0lf)
params.Normal=ue_vector(0.0lf,0.0lf,1.0lf)
ue_gas_cue_execute(self.Abilities,hit,params)
ue_gas_cue_add(self.Abilities,aura,params)
// Later:
ue_gas_cue_remove(self.Abilities,aura)
```

A normal DSL actor uses its existing reflected event mechanism:

```das
[ue_event(source="Abilities",name="GameplayCueReceived")]
def OnCue(cue:GASCueEvent) {
    let phase=ue_name_text(cue.Event)
    if(phase=="Executed") {
        // Given the game's loaded sound asset:
        ue_sound_play("hit",self.ImpactSound,cue.Parameters.Location)
    } elif(phase=="WhileActive") {
        // Given the game's loaded Niagara asset:
        ue_niagara_spawn("aura",self.AuraSystem,cue.Parameters.Location)
    } elif(phase=="Removed") {
        ue_effect_stop("aura")
    }
}
```

The latter fragment assumes `require ue_effects` and reflected ImpactSound/AuraSystem properties of SoundBaseRef/NiagaraSystemRef. Several cue tags can share the component event; filter `cue.Tag` or `cue.MatchedTag` in the handler. Gameplay state/damage belongs to ability/effect logic, because cue handling is cosmetic and may be suppressed by native GAS (for example on a dedicated server).

## Native delivery and registration

Execute/add/remove call the real UAbilitySystemComponent GameplayCue methods. Register a watched tag with ue_gas_cue_watch when DAS callbacks are needed. The tag must already exist in the GameplayTag registry and be a child of GameplayCue. Two native sample tags above are included; projects register their own tags through their normal tag sources.

Register routes during actor/component setup. Creating a new route synchronously inside a native cue callback is refused, because native CueSet parent routing still holds references to the route array; retry from a later update. Repeated watch of an already-installed route remains safe.

Watching installs a shared native UGameplayCueNotify_Static bridge for that exact tag in the current native runtime cue set, and enables delivery on this ASC. The bridge runs in Shipping too; it does not depend on the CueManager debug event. Repeated watch is idempotent. A tag already assigned to another native/asset notify is refused without modifying that entry. Use a dedicated cue namespace for DAS routes; a tag has one native notify route. The bridge does not override parent notify routes (IsOverride=false), and native GameplayCue tag hierarchy/translation/suppression remains in force. A parent route may receive a child cue; Tag preserves OriginalTag while MatchedTag identifies the bridge route.

The global route persists for the cue manager's lifetime. Unwatch removes this ASC's interest, not a route shared with other actors. There are at most 64 watched tags per ASC and 1024 bridge routes per native cue set. New native cue-set instances need registration again. The manual watch/send calls do not modify project configuration. For imported cue assets and effect-owned routes, see below.

Sending does not require watch. Existing native/asset cue routes continue to run normally; those tags cannot also be claimed by the DAS bridge. A successful send means native submission, not proof that a visual/audio asset was played. This increment covers the current authoritative authored ASC lifecycle; network/prediction and client registration need a separate pass.

## Parameters and snapshots

GASCueParameters carries RawMagnitude, NormalizedMagnitude, Location, Normal, Instigator, EffectCauser, SourceObject, AbilityLevel and EffectLevel. The helper sets levels to 1; optional object refs default to null. Native sending supplies the current avatar as default Instigator/EffectCauser and creates a native effect context. The callback snapshots the native parameters into owned storage; it never retains a borrowed FGameplayCueParameters pointer.

Magnitudes/coordinates must be finite, normalized magnitude is 0..1, Normal must be zero or unit length, levels are 0..10000. Optional actors and objects must be live; objects associated with another world are refused. Source assets with no world are accepted. This does not yet expose full effect-context/hit/tag-container payloads.

GASCueEvent adds Tag, MatchedTag, Event and Target. Event is Executed, OnActive, WhileActive or Removed. Native events that occur while the owning DAS VM is executing are queued using the existing event dispatcher; no synchronous VM re-entry is introduced. Stored snapshots remain independent of subsequent input mutations/events; object refs retain the binding's checked-reference semantics. Releasing the VM removes its subscription and discards obsolete callbacks.

## Persistent cue lifetime

Add permits one manually added cue per tag on the ASC; duplicate adds and already-active matching cue tags are refused. Active uses native IsGameplayCueActive. Remove only accepts a tag recorded by this adapter and repeated removal fails. Remove uses the native tag-wide loose-cue operation: do not independently add the exact same loose-cue tag through another path while this adapter owns it. GameplayEffect-owned cues follow their native effect lifetime.

Remove a manually added active cue before unwatching its tag; unwatch is refused while it is still recorded as active. EndPlay removes the adapter's remaining manual cues and clears watches. It does not broadcast new DAS callbacks into a tearing-down actor. Native notify actors and the existing owned VFX helpers retain their normal lifecycle cleanup.

Authored cue assets and effect cue lists are supported below. Full hit/context payloads, networking and prediction remain separate work. Unrelated existing notify assets are not automatically observed by the DAS bridge. Ordinary event handlers can call the existing sound/Niagara APIs.

## Focused verification

The gas-cues suite builds and runs a single short commandlet via the build agent. It covers native execute/active/while-active/remove, queued callbacks, snapshot retention across GC, native callers bypassing the sending helper, validation, route-conflict preservation, subscription release and EndPlay cleanup. NullRHI does not verify rendered Niagara or audio playback. Full PIE and packaged tests are outside this step.

Verified in agent job `gascues-run-2ecbc19c0f`: exit 0, Editor Development build, `DAS_GAS_CUES_RESULT=PASS checks=10` in one commandlet process (0.52 seconds for the commandlet logic). Callback registration guards and correct test-world EndPlay were included. Full PIE/packaged were not run for this block.

## Cue assets and GameplayEffect authoring

[gas_cue_assets.das](../../../Plugins/DasForUE/Content/Das/Scripts/gas_cue_assets.das) is a complete declaration/handler example:

```das
[ue_gameplay_cue(tag="GameplayCue.Das.Sample.Impact",min_level=1,max_level=5)]
class ImpactCue {}
[ue_gameplay_cue(tag="GameplayCue.Das.Sample.Aura",min_level=1,max_level=5)]
class AuraCue {}

[ue_attribute_set]
class CueStats { Health:float=100.0f }

[ue_gameplay_effect(attributes="CueStats",cue="ImpactCue")]
class CueStrike { Health:float=-5.0f }

[ue_gameplay_effect(attributes="CueStats",duration="Infinite",cue="ImpactCue",cue="AuraCue")]
class CueAura {}
```

`Das.GASImport gas_cue_assets.das /Game/GAS` creates `GCN_ImpactCue` and `GCN_AuraCue` Blueprint assets derived from the native static notify bridge, plus the AttributeSet and effects. `cue=` resolves simple names or explicit stable IDs in this script's registry; repeat the option for multiple cues (up to 16). Unknown/repeated cues are rejected. Tags must be registered GameplayCue child tags and unique within the cue registry. A cue-only effect is permitted; the existing explicit `attributes=` registry is still required.

The effect stores real `FGameplayEffectCue` entries and reflected hard references to generated notify classes. Cook can follow those references when the effect itself is included. No runtime JSON metadata, generated transient notify class or project config edit is used. Reimport into the same folder is idempotent; changed definitions require a new folder until schema migration is implemented. Use one asset route per cue tag: importing the same tag into another folder is not a hot-reload mechanism.

Cue assets currently declare their tag and finite level bounds (`0 <= min_level < max_level`, defaults 0 and 1). They are shared static notifies, not per-actor DAS instances. Their declaration bodies are empty; cosmetic DAS logic uses the target's ordinary `[ue_event(source="Abilities",name="GameplayCueReceived")]` handler. No explicit `ue_gas_cue_watch` is required for an authored asset route. It still uses owned snapshots and the existing deferred dispatcher when the VM is busy. `min_level`/`max_level` map native effect level to NormalizedMagnitude; RawMagnitude is native GAS's modified-attribute magnitude, zero when no magnitude attribute is configured. EffectLevel carries the native spec level.

`ue_gas_spec`/MakeAuthoredSpec and authored ability grants register referenced notify classes before native application. Registration preflights all references, refuses another notify assigned to that tag and does not install a partial batch on a conflict. Routes are shared for the manager lifetime. A manually watched tag already assigned to the plain bridge cannot then be claimed by a different cue asset. Prepare routes outside a synchronous cue callback. Native C++ callers bypassing the authored spec/ability helpers must call the effect's `PrepareCues()` themselves or use normal UE cue-library discovery.

GAS controls event phases: instant/periodic execution emits Executed, persistent effects emit OnActive/WhileActive and removal emits Removed. A cue-only infinite effect needs no dummy attribute modifier. Remove the GameplayEffect using `ue_gas_remove`; do not remove its cues through the loose-cue helper. Authored asset callbacks remain eligible regardless of manual watch/unwatch state. GameplayCue suppression, tag hierarchy and stacking rules remain native GAS behavior.

The `gas-cue-assets` suite is one short commandlet after an Editor Development build via the build agent. It checks DSL collection, saved native fields/class references, repeat import, route-conflict preflight, native effect execution/normalization, two active cues, effect-owned removal and a retained snapshot after GC. It does not validate rendered effects, network delivery, full PIE or packaged output.

Multiple cues with identical level bounds share one native FGameplayEffectCue tag container. This avoids redundant whole-spec callbacks in UE 5.8's Full-replication add/remove path. Different bounds require separate native entries; that engine path can then deliver repeated static-notify phases. Cosmetic handlers should be idempotent, as with other native GAS repeated/join-in-progress delivery.

Verified by build-agent job `gascueassets-4f3b765ddc`: Editor Development build and `DAS_GAS_CUE_ASSETS_RESULT=PASS checks=7` in one short commandlet (1.62 seconds of commandlet logic). Full PIE and packaged tests were not run for this increment.
