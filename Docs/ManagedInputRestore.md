# Desired input state restoration

Added 2026-10-10. **Not built or tested**. No delegate registration/build, reload,
component replacement, input dispatch, world/GC or packaged test was run.

Successful legacy Action/Axis and Enhanced Input binds now retain a session-local
recipe: mapping/action, phase/trigger, exported handler, and legacy consume/paused
flags. Enhanced assets are held strongly while a recipe remains. Successfully
acquired Mapping Contexts retain subsystem/asset/priority recipes separately.

An idle game-thread OnWorldTickStart pass reconciles desired state with the current
owner's InputComponent. Old component bindings are removed; missing compatible
bindings are rebuilt through the existing checked adapters. Missing components or
unready local-player input defer work to a later world tick. Restore errors retain
recipes, retry after one second, and log only when the error changes.

Accepted reload still revokes old proxy IDs/native handles before swapping the VM,
but retains desired recipes. A subsequent world tick restores contexts and handlers
against the new validated exports. Rejected reload retains old live bindings. Editor
reinstancing keeps recipes through context preservation. BeginPlay is not replayed.

This restores the **last successfully installed configuration**. Editing bind/setup
calls in a script does not rerun them or infer a new configuration: explicitly invoke
the revised setup method to change mappings, priorities, callbacks or flags. Required
callback contracts continue protecting reload; removed/incompatible handlers are not
silently rebound. Recipes are not arbitrary script closures.

Manual unbind/remove/clear cancels corresponding desired records too, including when
the native component is currently absent. Session-wide unbind/clear counts desired
records released. EndPlay/owner release/world teardown/shutdown discard desired
state and owned assets; they do not trigger resurrection on a later tick.

Context ownership still follows ManagedMappingContexts.md. Installed external
contexts are not acquired during recovery; priority conflicts defer recovery rather
than overwrite another system. Separate local players/worlds remain explicit. If
your context is externally managed, keep it external and restore only action handlers.

No controllers/InputComponents/mappings/assets are created. Enhanced recipes wait
for EnhancedInputComponent rather than being converted into legacy bindings. Manual
native removal of bindings on an unchanged component is outside current tracking;
use these APIs for managed unbinding or explicitly rebind after external changes.

Examples remain dsl_legacy_input.das, dsl_enhanced_input.das and
dsl_enhanced_contexts.das. They require initial setup; no example was run. This is
runtime reconciliation, not a claim that input/play testing has passed.

Pending checks: build and world-delegate signature; successful/failed reload;
component destruction/absence/replacement; owner component/Actor behavior; callback
contracts; explicit cancellation prevents recovery; contexts and priority conflicts;
asset/subsystem GC; world/local-player isolation; retries and reentry; EndPlay before
BeginPlay; reinstancing and rollback; real PIE and packaged execution/performance.


## Playable Character integration validation — 2026-10-17

The earlier "not tested" notes describe the original implementation slice.
The playable Character integration now verifies Boolean Started/Completed and
Axis2D Triggered callbacks, Mapping Context installation and synthetic W routing,
unpossess/repossess setup and EndPlay cleanup in real PIE and packaged Development.
The separate saved-map run verifies auto-possession and working native movement,
jump and camera after cook/load. This does not validate every trigger/value shape,
multi-player ownership scenario or hot reload combination listed above.
See PlayableCharacter.md for reproduction and the exact scope.
