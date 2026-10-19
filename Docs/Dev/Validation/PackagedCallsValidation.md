# Packaged validation of classes, spawning, queries and libraries

Run Scripts/TestCookedCalls.ps1. It builds the editor, exports/generates the library
wrappers, saves a unique test map and dependent Blueprint assets, builds Win64
Development game target, cooks/stages/archives and launches the packaged executable
with -DasCookedCalls -nullrhi. The existing packaged foundation smoke also runs.

The spawn class is referenced only by the driver's reflected class-property default;
there is no pre-placed target Actor. This verifies class serialization and cook
dependency traversal. The target is a Blueprint child of an authored daScript Actor
with an authored SCS component.

Runtime checks:
- game target compiled without WITH_EDITOR;
- loaded class property, correct spawned class, owner and transform;
- native/authored Spawn, nullable/typed first/all Actor/component queries;
- Actor/component BeginPlay, ticks, EndPlay and stale snapshot handles;
- engine/user C++ Blueprint Function Libraries and WorldContext without editor metadata;
- class input/return/out/arrays, null and abstract class values;
- generated static GetActorOfClass and instance GetComponentsByClass;
- rejected WorldContext override, non-library and wrong MetaClass calls;
- interpreter context cleanup.

Outputs: logs/cooked-calls-console.log, cooked-calls-export.log,
cooked-calls-runtime.log and Build/Packaged/Windows. This is a Development NullRHI
check; it does not establish Shipping, graphics/physics or replication coverage.

Validation 2026-10-13: full Win64 Development packaged pipeline PASS. Foundation smoke PASS failures=0; DAS_COOKED_CALLS_RESULT=PASS. All requested feature checks passed without WITH_EDITOR. Test cook uses -SkipZenStore to avoid ZenServer staging availability; the general TestPackaged runner exposes this as an optional switch. Temporary UUID source assets are removed after success; packaged archive and logs remain. An initial inheritance smoke failure during parallel source changes disappeared after forcing runtime rebuild; final full run is clean.


2026-10-15: the same full pipeline also passed authored ue_function UEClassHandle
inputs/results/ref/out on Actors and ActorComponents, native ProcessEvent and typed
cross-context method accessors. Saved graphs were reopened and tested in a separate
editor process before packaging. Null/Blueprint subclasses and negative input,
result and output base checks passed. See [DSLClassMethods.md](DSLClassMethods.md).


2026-10-15: Initialize lifecycle coverage also passed. Actor/component initialization
precedes their daScript BeginPlay, hierarchy and once-per-instance guards survive
session recreation, errors latch without repeated side effects, and both placed
instance edits (49) and spawned Blueprint defaults (41) remain visible. The editor
export additionally verifies body reload without replay and restores the source
before cooking. See [DSLInitialization.md](DSLInitialization.md).
