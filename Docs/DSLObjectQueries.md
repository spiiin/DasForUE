# Actor and component queries by type

```das
let actor = ue_actor_find(type<SpawnTargetRef>)
let actors = ue_actor_find_all(type<SpawnTargetRef>)

let component = ue_component_find(type<SpawnLifeRef>, actorRef)
let components = ue_component_find_all(type<SpawnLifeRef>, actorRef)
for (value in components) {
    print("component found")
}
```

Actor queries search the script owner's current game world. Component queries
search only the supplied Actor, which must be live and in the same world. Native
queries use IsA and include subclasses; abstract native base types are valid query
constraints. Authored queries additionally require the exact daScript stable type
ID and therefore exclude other authored classes sharing the same native host.
Blueprint subclasses inheriting that identity are included. Interface queries are
not provided by this API.

`find` returns a nullable typed reference. `find_all` returns an owned ordinary
`array<Reference>` snapshot; a miss yields an empty array. Result ordering is
unspecified and must not be used for deterministic gameplay. Actors/components
that are templates, obsolete or being destroyed are excluded. Components whose
owner is being destroyed are also excluded.

Queries do not retain Unreal objects or grant destruction rights. Array entries
are weak context-scoped object handles: recheck ue_object_valid (or ue_scene_valid)
after object destruction/world changes. The returned snapshot is not updated when
objects spawn or disappear. Each find_all is limited to 4096 results, with an error
rather than silent truncation. Internal native snapshots are released after
materialization; callback-scope cleanup also releases them on errors.

These scans are intended for explicit gameplay queries, not cached engine indexes.
Tags, radius filters, explicit generated-class filters, cross-world queries and
reflected function/property arrays are separate capabilities.

Integration coverage is in dsl_spawn_driver.das / DasForUE.Actors.SpawnPIE:
missing results, two authored Actor matches, native queries, authored component
identity, native scene subclasses, same authored type in a foreign world, rejected
foreign component owner, and invalidation of stored array entries after destruction.


Validation: TestActorSpawnPIE.ps1 PASS, one successful automation test with zero failures/warnings. Seventy deliberately interrupted internal queries verify callback-scope cleanup beyond the 64-snapshot limit.
