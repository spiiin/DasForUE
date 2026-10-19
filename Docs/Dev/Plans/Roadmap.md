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

- GAS (attributes, effects, abilities, replication).
- UMG MVVM / FieldNotify; Slate and editor UI extensions.
- GameplayMessageRouter typed messages with listener lifetime.
- PCG adapters and daScript kernels with explicit threading rules.
- StateTree tasks/evaluators/conditions; Behavior Tree.

### Tooling beyond NimForUE parity

- Source mapping of generated DSL/glue errors back to the daScript source.
- Static checks of lifetime/thread/replication contracts where the IR allows.
- Script time/memory limits and compiler macro limits (research; not a sandbox
  for untrusted code).
- LSP integration and profiler markers; debugger integration as research.

## Subsystem follow-ups

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
