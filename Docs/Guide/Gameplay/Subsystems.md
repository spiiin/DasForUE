# Authored WorldSubsystem and GameInstanceSubsystem

Import `ue_dsl` (or `ue_subsystems` for typed lookup). The same `ue_class`,
`ue_property` and `ue_function` declarations used for Actors produce a Blueprint
class whose native parent is a real Unreal subsystem. No Actor is spawned.

```dascript
options gen2
require ue_subsystems
[ue_class(module_id="missions", Blueprintable)]
class MissionService : WorldSubsystem {
    @ue_property=(BlueprintReadWrite)
    Completed:int = 0
    def Initialize() { self.Completed = 0 }
    def Deinitialize() { /* release application state here */ }
    [ue_function(BlueprintCallable)]
    def CompleteMission():int {
        self.Completed += 1
        return self.Completed
    }
}
```

Use `GameInstanceSubsystem` instead of `WorldSubsystem` for state that belongs to
one game session and survives replacement of its world. World-bound timers and
bindings are cancelled when that world is cleaned up; the session VM/state remains. Each PIE GameInstance
owns an independent instance. Initialize runs once for each native object;
a successful body reload replaces code without replaying Initialize. Failed
reload keeps the last working code. Deinitialize is called by the native
collection once, then all owned VM callbacks and resources are released. GC
fallback releases resources without executing script code during garbage collection.

GameInstance Initialize can run before its first world exists. Property/state
initialization and world-independent work are allowed; world-dependent operations
must wait until there is a live game world. Both bases currently expose Initialize
and Deinitialize only: Actor BeginPlay/Tick/EndPlay, construction, default Actor
components and async physics declarations are rejected for these roots. Reflected
ordinary methods and properties use the existing frame ABI and defaults pipeline.

## Loading and retrieval

Unreal creates subsystem instances only for classes loaded before the owner
collection initializes. Add imported Blueprint generated classes to the explicit
project allowlist in `Config/DefaultGame.ini`:

```ini
[/Script/DasRuntime.DasSubsystems]
+Classes=/Game/Services/BP_Missions.BP_Missions_C
+Classes=/Game/Services/BP_Session.BP_Session_C
```

The native collection bootstrap loads and retains these classes and initializes
them as collection dependencies, including before the first GameInstance world. Keep their
asset directory in the project's cook settings: this config list is not a cook
manifest. Already loaded classes, including imported test classes, are discovered
by Unreal's normal collections. Import/reinstance structural changes before PIE;
this block does not add classes to already initialized collections or override
Unreal's live Blueprint reinstancing rules.

Typed lookup takes an explicit concrete class handle and uses the current script
owner's world or GameInstance, never a process-global singleton:

```dascript
let service = ue_subsystem(type<WorldSubsystemRef>, missionClass)
let session = ue_subsystem(type<GameInstanceSubsystemRef>, sessionClass)
```

`missionClass`/`sessionClass` are the existing typed or untyped UE class handles.
For authored members, use the existing authored reference declaration:

```dascript
[ue_object_ref(script="dsl_world_subsystem.das",type="MissionService")]
struct MissionServiceRef { handle:UObjectHandle }
// Inside an ordinary method with a concrete service class handle:
// let missions = ue_subsystem(type<MissionServiceRef>, missionClass)
// missions.Advance()
```

Native Blueprint callers can use Unreal's ordinary Get World Subsystem and
Get Game Instance Subsystem nodes. Lookups fail clearly when a class is not a
subsystem, the owning collection has no instance, or the subsystem was deinitialized.

## Targeted validation

Submit `authored-subsystems` and `authored-subsystems-pie` through the shared
build agent. The commandlet checks Apply, native collection creation, independent
world/session state, good and failed body reload, and cleanup. The PIE automation
starts two real PIE worlds in one editor process with separate GameInstances,
checks per-owner instances and reload (including retained inline-callback timers),
ends PIE and checks Deinitialize before GC without retaining world objects, and VM
resource release. Play settings are restored after the test. Generated assets
stay under `/Game/DasSubsystems`; no user map or service is modified.

The native/PIE checks alone do not certify packaged startup, level travel/network replication,
subsystem dependencies, LocalPlayerSubsystem, EditorSubsystem, or automatic
registration of newly authored classes during a running session.

Inline timer callbacks with unchanged bodies retain their stable callback identity
through reload. Existing named-handler timers follow the shared runtime policy
and are cancelled on reload; call the service method again to rearm them.

The focused acceptance run `subsystems-check9-20261023` passed through the build
agent: 20 commandlet checks and one successful multi-world PIE automation test,
with zero automation warnings or failed tests. Local evidence is in
`logs/authored-subsystems-validation.json`,
`logs/authored-subsystems-pie-report/index.json` and
`logs/authored-subsystems-block-validation.json`. The native receipt freezes
source and binary hashes during its run; the block receipt verifies local files
again after PIE. Initial failed runs are retained in the agent logs.

## Focused packaged startup and shutdown

Queue `test-suite <id> authored-subsystems-packaged repo=<worktree>` through the
shared build agent. The runner exports two dedicated service Blueprints and an
empty map under `/Game/DasSubsystems/Packaged`. The map contains no service Actors
or hard references to the service classes. The runner temporarily supplies the
`DasSubsystems.Classes` allowlist in the smoke project's `DefaultGame.ini` and
cooks the dedicated asset directory explicitly. Original config bytes are restored
in `finally`, including on packaging failure. The archived game consumes the cooked
configuration, not the restored source config.

The Win64 Development probe never loads the classes itself. Twelve checks cover
the cooked process/config, classes already loaded by the native bootstrap,
collection-owned instances, Initialize once, staged scripts, reflected method
execution and live timers/VMs. It requests normal game exit and observes native
GameInstance/World Deinitialize before runtime module shutdown, requiring both
callbacks once and all VM/timer/subscription resources released. It never manually
calls Deinitialize or ReleaseObject. The runner enforces a three-minute runtime
deadline and checks a final marker emitted after native teardown, source/config
hash stability and the executable hash.

Receipt: `logs/authored-subsystems-packaged-validation.json`; runtime log:
`logs/subsystems-packaged.log`. This focused suite does not run Foundation, PIE,
networking, level travel or Shipping acceptance tests.

Verified 2026-10-23 by `subsystems-package3-20261023` through the build agent:
all twelve checks passed, the game exited with code 0 after normal native teardown,
and the agent exited 0. Scripts were resolved under the archived
`Windows/RemappedPlugins/DasForUE/Content/Das` directory. Source/config and executable
hashes matched before/after the run. Repeated export reopens/fully loads the dedicated
map package before saving; the native exit observer removes its callbacks while
global delegates are still alive. No runtime subsystem implementation changes were
needed for this gate.
