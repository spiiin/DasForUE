# Actor spawning from daScript

```das
let klass = ue_class_loaded(type<ActorRef>, "/Script/Engine.Actor")
let actor = ue_actor_spawn(type<ActorRef>, klass)
let destroyed = ue_actor_destroy_handle(actor.handle)
```

Full form:

```das
let actor = ue_actor_spawn(type<SpawnTargetRef>, self.SpawnClass,
    pose, owner, "AlwaysSpawn")
```

The type token must describe an Actor. The class handle is rechecked for native
inheritance and authored stable ID. Native classes, compatible Blueprint subclasses
and authored Actor classes are accepted. The class must already be loaded; a class
property can provide the serialized reference. Default transform is identity,
default owner is null, default collision rule is AlwaysSpawn.

Collision rules: AlwaysSpawn, AdjustIfPossibleButAlwaysSpawn,
AdjustIfPossibleButDontSpawnIfColliding, DontSpawnIfColliding. Unreal applies the
selected rule; native spawn failure returns a nullable reference. Transform must
be finite with normalized rotation. Owner, when provided, must be a live Actor in
the script owner's game world. Spawn is forbidden in bind_events/EndPlay, templates
and non-game worlds. Instigator, deferred caller customization, network roles and
replication-specific adapters are not provided.

Creation defers construction until the runtime installs a narrowly scoped lifecycle
permission. BeginPlay/EndPlay may nest for the spawned Actor and its daScript
components. Actor construction finishes before returning. Actors are tracked weakly
by script-owner UObject, with 1024 live entries and spawn nesting limited to 16.
They survive ordinary VM replacement/session release and remain world-owned.
ue_actor_destroy_handle only accepts Actors spawned by that same owner. Existing
level/native Actors and Actors spawned by another owner cannot be destroyed through
this operation. Stale handles become invalid after destruction.

TestActorSpawnPIE.ps1 runs DasForUE.Actors.SpawnPIE. It covers native spawn/destroy,
Blueprint subclass of an authored Actor, transform/owner, BeginPlay/Tick/EndPlay of
Actor and SCS component, stale handles, rejected class/collision and foreign destroy.
Physical collision outcomes, replication and packaged builds remain separate tests.

Validation: TestActorSpawnPIE.ps1 PASS; one successful test without warnings/errors.

Packaged validation: these features passed TestCookedCalls.ps1 in Win64 Development NullRHI on 2026-10-13. See PackagedCallsValidation.md for coverage and remaining limits.
