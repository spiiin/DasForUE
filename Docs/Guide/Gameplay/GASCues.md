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

Magnitudes/coordinates must be finite, normalized magnitude is 0..1, Normal must be zero or unit length, levels are 0..10000. Optional actors and objects must be live; objects associated with another world are refused. Source assets with no world are accepted. Owned context/hit and canonical source/target tag-container snapshots are described below.

GASCueEvent adds Tag, MatchedTag, Event and Target. Event is Executed, OnActive, WhileActive or Removed. Native events that occur while the owning DAS VM is executing are queued using the existing event dispatcher; no synchronous VM re-entry is introduced. Stored snapshots remain independent of subsequent input mutations/events; object refs retain the binding's checked-reference semantics. Releasing the VM removes its subscription and discards obsolete callbacks.

## Persistent cue lifetime

Add permits one manually added cue per tag on the ASC; duplicate adds and already-active matching cue tags are refused. Active uses native IsGameplayCueActive. Remove only accepts a tag recorded by this adapter and repeated removal fails. Remove uses the native tag-wide loose-cue operation: do not independently add the exact same loose-cue tag through another path while this adapter owns it. GameplayEffect-owned cues follow their native effect lifetime.

Remove a manually added active cue before unwatching its tag; unwatch is refused while it is still recorded as active. EndPlay removes the adapter's remaining manual cues and clears watches. It does not broadcast new DAS callbacks into a tearing-down actor. Native notify actors and the existing owned VFX helpers retain their normal lifecycle cleanup.

Authored cue assets and effect cue lists are supported below. Networking and prediction remain separate work. Unrelated existing notify assets are not automatically observed by the DAS bridge. Ordinary event handlers can call the existing sound/Niagara APIs.

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

Cue assets currently declare their tag and finite level bounds (`0 <= min_level < max_level`, defaults 0 and 1). The native static notify is shared. Optional ordinary DAS methods run on a separate instance for each target ASC and cue class (see below); an empty declaration body remains supported. Cosmetic DAS logic can also use the target's ordinary `[ue_event(source="Abilities",name="GameplayCueReceived")]` handler. No explicit `ue_gas_cue_watch` is required for an authored asset route. It still uses owned snapshots and the existing deferred dispatcher when the VM is busy. `min_level`/`max_level` map native effect level to NormalizedMagnitude; RawMagnitude is native GAS's modified-attribute magnitude, zero when no magnitude attribute is configured. EffectLevel carries the native spec level.

`ue_gas_spec`/MakeAuthoredSpec and authored ability grants register referenced notify classes before native application. Registration preflights all references, refuses another notify assigned to that tag and does not install a partial batch on a conflict. Routes are shared for the manager lifetime. A manually watched tag already assigned to the plain bridge cannot then be claimed by a different cue asset. Prepare routes outside a synchronous cue callback. Native C++ callers bypassing the authored spec/ability helpers must call the effect's `PrepareCues()` themselves or use normal UE cue-library discovery.

GAS controls event phases: instant/periodic execution emits Executed, persistent effects emit OnActive/WhileActive and removal emits Removed. A cue-only infinite effect needs no dummy attribute modifier. Remove the GameplayEffect using `ue_gas_remove`; do not remove its cues through the loose-cue helper. Authored asset callbacks remain eligible regardless of manual watch/unwatch state. GameplayCue suppression, tag hierarchy and stacking rules remain native GAS behavior.

The `gas-cue-assets` suite is one short commandlet after an Editor Development build via the build agent. It checks DSL collection, saved native fields/class references, repeat import, route-conflict preflight, native effect execution/normalization, two active cues, effect-owned removal and a retained snapshot after GC. It does not validate rendered effects, network delivery, full PIE or packaged output.

Multiple cues with identical level bounds share one native FGameplayEffectCue tag container. This avoids redundant whole-spec callbacks in UE 5.8's Full-replication add/remove path. Different bounds require separate native entries; that engine path can then deliver repeated static-notify phases. Cosmetic handlers should be idempotent, as with other native GAS repeated/join-in-progress delivery.

Verified by build-agent job `gascueassets-4f3b765ddc`: Editor Development build and `DAS_GAS_CUE_ASSETS_RESULT=PASS checks=7` in one short commandlet (1.62 seconds of commandlet logic). Full PIE and packaged tests were not run for this increment.

## Owned extended payloads and cue methods

[gas_cue_hooks.das](../../../Plugins/DasForUE/Content/Das/Scripts/gas_cue_hooks.das) is the combined example and focused probe. The same cue declaration can contain ordinary instance methods:

```das
[ue_gameplay_cue(tag="GameplayCue.Das.Sample.Hooks")]
class ImpactCue {
    Count:int=0
    First:GASCueEvent

    def Execute(cue:GASCueEvent) {
        self.Count+=1
        if(self.Count==1) { self.First <- clone(cue) }
        if(cue.Parameters.Context.HasHit) {
            let point=cue.Parameters.Context.Hit.ImpactPoint
            // Use point and your loaded sound/Niagara assets for cosmetic logic.
        }
    }
    def OnActive(cue:GASCueEvent) { }
    def WhileActive(cue:GASCueEvent) { }
    def Removed(cue:GASCueEvent) { }
}
```

Each method is optional, returns void and accepts one by-value GASCueEvent. Execute receives native Executed; the other names match native phases. No ue_function/event annotation is required on these methods. Own fields retain state for that target/notify pair until target EndPlay. A second target has a separate VM and instance. Shared static notify CDOs never hold mutable DAS instance state. Lifecycle methods can run more than once under native GAS rules, so looping cosmetic logic should be idempotent.

Instances use transient native components owned by the target, allowing the existing effect/timer owner rules to apply. `cue.Target` explicitly names the recipient; `self` is the DAS cue instance. Pending hook payloads live in GC-visible owned storage, share the dispatcher budget with ordinary events and never re-enter an executing VM. Calls made from a hook queue subsequent cue hooks until that hook returns. EndPlay closes the session, discards pending work, releases its VM and owned helpers. A failing hook reports DAS_GAS_CUE_HOOK_FAILED and closes that session.

GASCueParameters adds SourceTags and TargetTags using the canonical ue_gameplay_tags::GameplayTagContainer type; normal `ue_tags_has`, `ue_tags_names` and other helpers work directly. Context is a GASCueEffectContext snapshot with Valid, Instigator, OriginalInstigator, EffectCauser, SourceObject, Actors, HasOrigin/Origin and HasHit/Hit. It contains no borrowed native FGameplayEffectContextHandle or HitResult pointer. Copies survive callback return, changes to the caller's input and later GC; object access retains the binding's checked-reference rules.

GASCueHit is the reflected HitResult adapter: BlockingHit, StartPenetrating, Time, Distance, PenetrationDepth, FaceIndex, Item/MyItem, ElementIndex, BoneName/MyBoneName, Location, ImpactPoint, Normal, ImpactNormal, TraceStart/TraceEnd and weak Actor/Component/PhysMaterial. Native physics/instance-handle internals are intentionally not exposed. Check Context.HasHit before using these fields. Normal calls/effect TargetData populate this adapter from the real native context.

The execute/add helpers accept the same owned parameter type. SourceTags/TargetTags round-trip as explicit cue parameters; they do not grant tags to the ASC. Context.Valid reconstructs its native instigator/causer/source provenance; OriginalInstigator is informational and is derived by native GAS. Actors, origin and the supported hit fields reconstruct a fresh native context. This does not clone arbitrary custom context subclasses or serialized native internals. Inputs must reference live same-world actors/components (worldless source assets are permitted), have finite coordinates and valid registered tags; arrays/tag containers are bounded. `ue_gas_cue_params()` initializes empty canonical containers. Parameters captured from a GameplayEffect preserve that effect's native captured tags and hit provenance.

The focused `gas-cue-hooks` build-agent suite builds and runs one short commandlet covering four saved methods, per-target state, deferred nested delivery, native effect TargetData/captured tags, ordinary event compatibility, snapshot retention after GC and EndPlay cleanup. Full PIE, packaged and network checks are separate work.

Because context now contains an Actors array, use normal DAS deep-clone/move syntax when retaining or copying the complete event: `self.First <- clone(cue)` or `var copy <- clone(cue.Parameters)`. The examples and cue-parameter constructor use these semantics.

Verified by build-agent job `gascuehooks-run-93bc5ca9`: Editor Development build and `DAS_GAS_CUE_HOOKS_RESULT=PASS checks=9` in one short runtime process. The scenario includes native effect TargetData/captured tags, deferred nested hooks, per-target state, snapshots after GC and EndPlay cleanup. Full PIE/packaged/network checks were not run for this increment.

## Playable impact and aura example

[gas_cue_playground.das](../../../Plugins/DasForUE/Content/Das/Scripts/gas_cue_playground.das) implements two authored cues. Impact Execute uses the native hit's ImpactPoint, or the target location when there is no hit, and starts an owned sound + Niagara burst. Aura OnActive/WhileActive starts a named fountain once; Removed stops that name. An instant damaging effect and a cue-only infinite effect drive these native phases.

[dsl_gas_cue_playground.das](../../../Plugins/DasForUE/Content/Das/Scripts/dsl_gas_cue_playground.das) is a placeable actor. Enter its sphere with a Pawn to take one point of demo damage, hear the ping, see the impact burst and start the fountain. Leave to remove the infinite GameplayEffect, which calls Removed and destroys the fountain component. Multiple overlapping Pawns keep the aura until the last leaves. Hit, StartAura and StopAura are also BlueprintCallable, for connecting input or actual combat. The marker is stationary; this world-space fountain does not follow a moving target.

The build-agent `gas-cue-playground` suite prepares `/Game/DasExamples/GASCues/BP_CuePlayground`, assigns its generated AttributeSet/effect classes and saves two editable Niagara assets: NS_Impact from RadialBurst and NS_Aura from FountainLightweight. The original engine/plugin templates stay untouched; existing project copies are reused. Drag BP_CuePlayground into a level that already has your controllable Pawn. This step does not create a new game mode or change an existing map. The sound is the engine's 1kSineTonePing. For manual setup, import gas_cue_playground.das with Das.GASImport into the same folder, import the actor DSL, assign Attributes/HitEffect/AuraEffect and duplicate the named templates there.

This small demo synchronously loads its fixed soft paths on first use. Production games should preload assets before combat and use their own sounds/systems. The paths are in ordinary DAS code, not reflected cook dependencies: explicitly include the example asset folder, sound and any referenced template dependencies in a packaged game's cook rules. The offscreen commandlet creates real rendering/audio components, but does not validate packaging or human-perceived sound/particle appearance.

The single short runtime scenario checks real native sound/Niagara component creation, an actual effect-context hit position, bounded repeated impacts, idempotent aura phases, retention across GC, effect-owned Removed cleanup and target EndPlay cleanup. It runs after the Editor Development build via the shared build agent, using the suite's opt-in rendering mode (AllowCommandletRendering/Audio + RenderOffscreen instead of NullRHI). Native Niagara refuses IsReadyToRun under NullRHI. No full PIE/packaged suite is requested for this step.

Verified by build-agent job `gascueplayground-20261020i`: Editor Development build and `DAS_GAS_CUE_PLAYGROUND_RESULT=PASS checks=9` in a single offscreen runtime scenario. The commandlet provisions a native FX system explicitly for its game world, because UE skips normal FX-system creation inside commandlets. Full PIE/packaged and perceptual audio/visual checks were not run.
