# Spawning Actors and Finding Objects

Scripts can spawn Actors by class, destroy the Actors they spawned, and find Actors in the current world or components on an Actor by typed reference. All results are typed, nullable references that never keep objects alive.

Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_spawn_driver.das` (with `dsl_spawn_target.das`).

## Spawning

```das
let klass=ue_class_loaded(type<ActorRef>,"/Script/Engine.Actor")
let actor=ue_actor_spawn(type<ActorRef>,klass)
let destroyed=ue_actor_destroy_handle(actor.handle)
```

Full form, with transform, owner and collision handling:

```das
let pose=ue_transform(ue_vector(125.0lf,250.0lf,375.0lf),ue_rotator(0.0lf,30.0lf,0.0lf),ue_vector(1.0lf,1.0lf,1.0lf))
self.Spawned=ue_actor_spawn(type<SpawnTargetRef>,self.SpawnClass,pose,owner,"AlwaysSpawn")
```

| Argument | Default | Rules |
| --- | --- | --- |
| type token | required | Must describe an Actor type. |
| class handle | required | Must already be loaded (a class property can hold the serialized reference). Rechecked for native inheritance and the authored stable ID. Native classes, compatible Blueprint subclasses and authored Actor classes are accepted. |
| transform | identity | Finite, with normalized rotation. |
| owner | null | When given, a live Actor in the script owner's game world. |
| collision rule | `"AlwaysSpawn"` | `AlwaysSpawn`, `AdjustIfPossibleButAlwaysSpawn`, `AdjustIfPossibleButDontSpawnIfColliding`, `DontSpawnIfColliding`. |

Unreal applies the collision rule; a native spawn failure returns a null reference.

### Lifecycle and ownership

- Construction finishes before `ue_actor_spawn` returns. The spawned Actor's BeginPlay (and that of its daScript components) may run nested inside the call; nesting depth is limited to 16.
- Spawning is refused during event binding and EndPlay, on templates, and in non-game worlds.
- Spawned Actors are world-owned and tracked weakly per script owner (up to 1024 live entries). They survive VM replacement and session release.
- `ue_actor_destroy_handle` only destroys Actors spawned by the **same** owner. Level-placed/native Actors and Actors spawned by another owner are refused. Handles become invalid after destruction.
- Instigator, deferred-spawn customization, network roles and replication options are not provided.

## Finding Actors and components

```das
let actor=ue_actor_find(type<SpawnTargetRef>)
let actors=ue_actor_find_all(type<SpawnTargetRef>)

let component=ue_component_find(type<SpawnLifeRef>,actorRef)
let components=ue_component_find_all(type<SpawnLifeRef>,actorRef)
for(value in components) {
    print("component found\n")
}
```

- Actor queries search the script owner's current game world. Component queries search only the given Actor, which must be live and in the same world.
- Native types match with `IsA`, including subclasses; abstract native bases are valid filters. Authored types also require the exact daScript stable type ID, so other authored classes on the same native host are excluded; Blueprint subclasses inheriting that identity are included.
- `find` returns a nullable typed reference. `find_all` returns an owned `array<Ref>` snapshot; a miss gives an empty array. The snapshot is not updated when objects spawn or disappear.
- Templates, obsolete objects and objects being destroyed are excluded, as are components whose owner is being destroyed.
- Ordering is unspecified; do not rely on it for deterministic gameplay.
- Each `find_all` returns at most 4096 results and raises an error instead of truncating silently.
- Results are weak, context-scoped handles that grant no destruction rights. Recheck `ue_object_valid` (or `ue_scene_valid`) after destruction or world changes.

These are explicit scans, not cached engine indexes. Interface queries, tag and radius filters, explicit generated-class filters and cross-world queries are not provided here. For radius queries use `ue_sphere_overlap` from [Gameplay services](GameplayServices.md); the generated World/Actor API also exposes engine functions such as `GetAllActorsOfClass` (see [World, Actor and Components API](../../Api/WorldActorComponents.md)).

See also: [Object and class references](../DSL/ObjectAndClassReferences.md), [Lifecycle](../DSL/Lifecycle.md), [Gameplay services](GameplayServices.md)
