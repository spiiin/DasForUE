# Navigation queries

`require ue_navigation` provides synchronous queries against the current script
owner's Unreal world. The level must contain usable navigation data. The wrapper
does not create a NavMesh or wait for its build.

```das
let distance = ue_navigation_distance(start, finish)
if (distance.Status == NavigationQueryStatus.Success && distance.Length <= budget) {
    // The native corridor estimate fits this connection's budget.
}

let path = ue_navigation_path(start, finish, ue_self_actor())
if (ue_is_valid(path) && path.IsValid() && !path.IsPartial()) {
    let length = path.GetPathLength()
}
```

`NavigationDistance` preserves the native `GetPathLength` status. Use its length
only on `Success`. `Fail` means that the query did not find a route; `Error` or
`Invalid` must not be treated as a zero-length connection. This is the engine's
corridor estimate, matching the API used by the course patrol graph; it is not a
straight-line distance or a promise of an exact future movement distance.

`NavigationPathRef` is a native object reference created with `UENativeObjectRef`.
A valid object can still contain a partial route: inspect `IsValid()` and
`IsPartial()` before requiring full reachability. The optional querier selects
the actor context for the synchronous path query. These helpers use the default
navigation filter and default navigation data.

Queries do not move actors. Use the lifecycle-managed `ue_ai_move_to_actor` or
`ue_ai_move_to_location` helpers for movement and cancel their named request when
changing behavior. Avoid running synchronous graph queries on every frame;
rebuild when navigation or the set of destinations changes.

The native registry discovery includes literal `UENativeObjectRef` declarations,
so `NavigationPathRef` receives generated native methods. After changing native
reference declarations, regenerate the native gameplay registry and rebuild.
`navigation-queries` verifies the compact and generated query APIs against a real
Recast mesh with an obstacle and a disconnected island.
