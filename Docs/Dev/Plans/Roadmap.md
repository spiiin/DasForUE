# Roadmap

Open work only. Implemented features are listed in the
[documentation index](../../index.md) and the [changelog](../../../CHANGELOG.md).
Being listed here does not mean a feature is implemented or scheduled.
Foundation/refactoring work (value-kind registry, string-free DSL, performance,
test infrastructure) is in the [improvement plan](ImprovementPlan.md).

## Current priorities

1. **Functional completeness and usability over optimization.** Extend the existing
   playable Character / pickup example instead of creating another equivalent demo.
   New engine APIs are added for the next concrete gameplay mechanic, not to grow
   a list of supported classes.
2. **Remove low-level code from user scripts.** User methods should rely on typed
   references, ordinary fields/methods and public managed helpers. Manual call
   frames and internal `__ue_*` helpers are acceptable only where no public API
   exists yet. Renaming helpers without adding capability is not a goal.
3. **Treat production capabilities as separate milestones.** Shipping, full AOT of
   the gameplay path, clean-machine installs, networking (RPC/replication) and
   extended migrations each need their own acceptance gates.
4. **Deferred until there is a measurable gameplay problem:** further snapshot
   storage optimizations and source-check optimizations.

## Versions

Decided 2026-10-21. The gates below say what closes each item.

### v1 — first release (Win64)

- **Shipping with the interpreter.** Gameplay methods and hooks run in the daScript VM in
  Win64 Shipping, as the GAS ability map already does; pure-DAS AOT stays optional. Gate:
  the playable Character and GAS maps pass packaged Shipping, plus a recorded per-frame
  script budget on both maps.
- **Clean machine.** The plugin installs and runs without the source tree; dependency
  licenses are listed.
- **Reload matrix.** Every structural change listed under "Full DSL and structural reload"
  has a documented outcome: live migration, controlled refusal or required restart. Plan:
  [Structural changes for every entity kind](StructuralChanges.md); current state:
  [Reload matrix](../../Reference/ReloadMatrix.md).
- **Save-game schema compatibility.**
- **User scripts without low-level code.** Examples and guides use no manual call frames
  or internal `__ue_*` helpers (priority 2).

### v2

- **AOT of the gameplay path**: object/snapshot ABI and authored callbacks for the
  [UE API AOT bridge](UEApiAot.md) (scalar/string calls exist), then full release
  validation.
- **C++ interop without the daScript VM**, as in AngelScript for Unreal: AOT script
  methods are native functions that C++ calls directly (UFunction thunks and plain C++
  entry points, no `Context` evaluation), and scripts call C++ directly instead of going
  through interpreted extern nodes. The editor keeps the VM for hot reload. Gate: call
  overhead in both directions measured against an equivalent native call; extern guard,
  execution-role and lifetime checks behave as in the VM. Builds on the AOT item above.
- Networking (replication, RPC), other platforms, extended migrations and the
  integrations below stay unscheduled.

## Open acceptance gates

These gates come from the original plan (after the NimForUE audit) and are still open.

### Full DSL and structural reload

- A generated C++/UHT backend for cook, with the same semantics as the dynamic
  editor backend; never rely on daScript memory layout.
- Complete reload matrix: for each of add/remove/rename member, metadata-only
  change, type/default change, base-class change, enum values, struct/DataTable
  rows, components, Blueprint pins, references, save/reopen, decide between live
  migration, controlled refusal or required restart.
- Still deferred: structural migration of classes with delegate/event fields,
  container schema migration, hierarchy (base class) migration, replacing an
  inherited SCS component type, authored interface contract changes.
- Detect schema mismatches in multi-user/network sessions.

### Production and networking

- Shipping, cook and AOT; preserve type identity and metadata needed at runtime.
- Dedicated server plus two clients, replicated fields, RepNotify, conditions and
  Server/Client/Multicast RPC (network work is deferred by decision).
- Save-game schema compatibility.
- Profile reflected calls against native adapters, compile/reload time, memory and GC.
- Clean machine without a source tree; dependency licenses.
- Platforms other than Win64; CPUs without AVX2.

### Additional integrations (on demand)

- GAS: the [authored data-only AttributeSet / GameplayEffect DSL](../../Guide/Gameplay/GASAuthoring.md) now imports real native attributes and effects. Duration, periodic execution and stacking now use native GAS policies. [Declarative abilities](../../Guide/Gameplay/GASAbilities.md) now support grants, native cost/cooldown, activation, cancellation and managed snapshots. Ordinary DAS Activate/End/Cancel methods now own explicit commit/completion and receive checked activation context. Native wait/event/montage tasks, actor/hit TargetData and authored MMC/execution capture calculations are described in [GAS advanced APIs](../../Guide/Gameplay/GASAdvanced.md). Local trace/overlap targeting, filters and confirm/cancel sessions are described in [GAS targeting](../../Guide/Gameplay/GASTargeting.md). Multiple execution outputs, required named SetByCaller inputs and native scoped capture modifiers are described in [GAS execution](../../Guide/Gameplay/GASExecution.md). Native cue execute/add/remove and DAS event snapshots are described in [Gameplay Cues](../../Guide/Gameplay/GASCues.md). Authored cue-notify Blueprints and repeatable effect cue links now use native effect-owned lifetimes. Cue assets now declare ordinary DAS Execute/OnActive/WhileActive/Removed methods with per-target state and owned context/hit/tag snapshots. A playable impact sound/Niagara and infinite-aura example now uses these hooks; Removed releases its owned VFX. The targeted strike example now combines input confirmation/cancel, a fresh trace, commit, target damage and a timed cue aura that outlives ability completion and cleans up on native effect expiry. A saved first-person GAS ability map now provides mapped movement/look/jump and aim/confirm/cancel controls. Its focused build-agent real-PIE check covers input, native hit/cue lifetimes, viewport capture and EndPIE cleanup. The same map now passes a real Win64 packaged Development run through the build agent (12 checks): bundled DAS/cooked assets, mapped controls, targeting/damage, live audio playback, Niagara, native aura expiry/Removed and context release. Cooked FName spelling is handled without requiring Editor casing. The same map also passes instrumented Win64 Shipping with a strict linked pure-DAS AOT check; gameplay/cue hooks remain interpreted. The first [guard-preserving scalar/string AOT bridge](UEApiAot.md) now generates and links native UE calls, with 15 focused guard/runtime checks; object/snapshot ABI and authored callback build/link support remain next, then full release validation. Gameplay hooks still use the VM; replication/prediction remain deferred.
- UMG MVVM / FieldNotify; Slate and editor UI extensions.
- GameplayMessageRouter typed messages with listener lifetime.
- PCG adapters and daScript kernels with explicit threading rules.
- StateTree tasks/evaluators/conditions; Behavior Tree.

### Tooling beyond NimForUE parity

- Extend the implemented [VM DSL diagnostics](../../Guide/DSL/Diagnostics.md) to full authored gameplay AOT/native glue as that backend lands.
- Static checks of lifetime/thread/replication contracts where the IR allows.
- Script time/memory limits and compiler macro limits (research; not a sandbox
  for untrusted code).
- LSP integration and profiler markers; debugger integration as research.

## Subsystem follow-ups

- **Authored services:** WorldSubsystem / GameInstanceSubsystem are implemented. Focused checks pass: 20 native collection/reload/travel checks and two real offline PIE worlds with separate GameInstances, inline timers and GC cleanup. See [Subsystems](../../Guide/Gameplay/Subsystems.md).


- **AI / Perception:** sight occlusion, team filters, additional perception
  delegates and reload lifecycle; direct inheritance from `AIPerceptionComponent`.
- **Animation / effects:** attached effects, fade, attenuation, Niagara user
  parameters; visible poses and root motion are unverified.
- **Async physics:** generic async handle transport stays closed; skeletal,
  welded, network and resimulation bodies are out of scope so far.
- **Physics simulation:** contact/friction, ragdoll, swing limits, drives/motors,
  force-threshold breaking.
- **UMG:** grouped pointer and focus-path notifications, a general widget
  lifecycle/override bridge (user `Destruct`, `OnAnimationFinished`),
  `GetBoundActionValue` polling, arbitrary native/Blueprint drag-drop operation
  subclasses and authored operation classes.
- **Gameplay services:** multi-hit/object queries, point/radial damage.
- **Construction:** native UObject constructors.

## API expansion

API surface grows by declarative subsystem packages generated from reflection
rather than hand-written wrappers. Each package member must end in one of:
generated, verified policy exclusion, documented adapter, or a concrete technical
blocker. Open items:

- Raise Math coverage and extend canonical views for remaining struct types as needed.
- Remaining named blockers per package (Animation/Audio/Niagara async callbacks and
  modulation object sets; Pawn/AI `Client*` RPCs, latent force feedback, raw MoveTo
  proxy; UMG technical blockers).
- Unify object/enum/name views across packages.
- Candidate next packages: SaveGame/AssetManager engine classes and libraries.
- Out of scope of the current measurement: non-reflected C++ methods, arbitrary
  virtual overrides, Slate, disabled plugins and modules outside the profiles.

Details: [ApiExpansionPlan](ApiExpansionPlan.md).
