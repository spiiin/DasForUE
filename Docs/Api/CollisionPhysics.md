# Collision / Physics Package (ue_api_collision)

`ue_api_collision` covers traces, sweeps, overlaps, hit results, physics body settings, physics constraints and `PhysicsHandleComponent`. It is installed in `Plugins/DasForUE/Content/Das/Scripts/` and generated from `Bindings/Packages/CollisionPhysics.profile.json`. It depends only on `ue_foundation` (re-exported) and does not need World or Math.

```das
options gen2
require ue_api_collision
```

## Contents

| Prefix | Source | Examples |
| --- | --- | --- |
| `ue_collision_` | `KismetSystemLibrary` | `LineTraceSingle`, `SphereTraceMulti`, `BoxTraceSingleForObjects`, `CapsuleTraceSingle`, `SphereOverlapActors`, `BoxOverlapComponents` |
| `ue_collision_gameplay_` | `GameplayStatics` | `BreakHitResult`, projectile path prediction (incl. `Blueprint_PredictProjectilePath_Advanced`) |
| `ue_body_` | `PrimitiveComponent` | `SetSimulatePhysics`, `AddImpulse`, `AddForce`, `SetLinearDamping`, `SetEnableGravity`, `SetCollisionEnabled`, `SetCollisionProfileName` |
| `ue_constraint_` | `PhysicsConstraintComponent` | `GetConstraint`, limits, drives, `BreakConstraint` |
| `ue_joint_` | `ConstraintInstanceBlueprintLibrary` | all methods operating on a constraint handle |
| `ue_handle_` | `PhysicsHandleComponent` | `GrabComponentAtLocation`, `SetTargetLocation`, `ReleaseComponent` |

Nearly every selected declaration is generated; only the documented exclusions below are missing. Native fields and inherited copies of methods are not added by this profile, and it does not cover all of Unreal physics.

## Traces and hit results

Generated traces return `UEHitResult`, an opaque value holding a real native `FHitResult` copy. Inspect it with `ue_hit_result_view`, which produces the same `UETraceHit` view used by the managed trace helpers:

```das
require ue_api_collision
require ue_gameplay_services

var ignored : array<C_Actor_adf65d5b>
let result <- ue_collision_LineTraceSingle(start, finish,
    E_ETraceTypeQuery_c61871df.v_TraceTypeQuery1, false, ignored,
    E_EDrawDebugTrace_59edfe33.v_None, true,
    S_LinearColor_f4abb14b(f_r=1.0, f_a=1.0), S_LinearColor_f4abb14b(f_g=1.0, f_a=1.0), 0.0)
if (result.out_returnvalue) {
    let hit = ue_hit_result_view(result.out_outhit)
    let actor = ue_hit_actor(hit)
    print("distance={hit.distance}, penetration={hit.penetration_depth}\n")
}
let details <- ue_collision_gameplay_BreakHitResult(result.out_outhit)   // pass the opaque value on
let empty = ue_hit_result_empty()
```

`UETraceHit` fields: `blocking_hit`, `start_penetrating`, `distance`, `time`, `location`, `impact_point`, `normal`, `impact_normal`, `trace_start`, `trace_end`, `penetration_depth`, `face_index`, `item`, `my_item`, `element_index`. Getters: `ue_hit_actor`, `ue_hit_component`, `ue_hit_physical_material` (nullable `PhysicalMaterialRef`), `ue_hit_physics_owner`, `ue_hit_bone_name`, `ue_hit_my_bone_name` (`UENameValue`). Numeric fields are copies.

Semantics:

- `UEHitResult` is a private token; neither memory addresses nor `ActorInstanceHandle` fields are exposed. It is detached from the call frame, immutable, and works as scalar, ref/out, array element and inside nested structs.
- Weak storage is preserved: component, physical material and actor instance handle are not pinned. After Destroy/GC the getters return null while the numeric copy stays available.
- The actor is read with `GetCachedActor()`, so viewing a hit never materializes a dehydrated actor (the actor may be null while the component is alive). The called native function keeps its own semantics.
- Unset inputs, foreign or zero tokens, wrong struct identity and non-finite outputs are rejected.
- Hit results share the context's snapshot table (4096 live values) and are collected like `UEStructValue`; they do not survive context destruction or hot reload.
- `UEHitResult` cannot be used in authored `ue_property`, `ue_function` signatures or JSON defaults.

The simpler managed helpers `ue_line_trace`, `ue_sphere_trace` and `ue_sphere_overlap` (module `ue_gameplay_services`) return `UETraceHit` / `UEOverlapResult` directly; see [Spawning and queries](../Guide/Gameplay/SpawnAndQueries.md). `UEOverlapResult` has `blocking_hit`, `item_index`, `my_item_index` and getters `ue_overlap_actor`, `ue_overlap_component`, `ue_overlap_physics_owner`; overlaps are channel-based, limited to 4096 input/output objects and not deduplicated by actor.

### Reference arrays inside structs

Structs may contain arrays of object or class references, including arrays of structs with their own reference arrays. `Blueprint_PredictProjectilePath_Advanced` passes the whole `FPredictProjectilePathParams` (with `ActorsToIgnore`) and returns `FPredictProjectilePathResult` with its `PathData` array and an opaque hit result. Element classes and `MetaClass` are checked; null and duplicates are allowed. Weak-object arrays are never turned into strong arrays.

## Physics bodies and PhysicsHandle

```das
let body = C_PrimitiveComponent_5efdd477_from(ue_get_self_object("Mesh"))   // a component property of the owner
ue_body_SetSimulatePhysics(body,true)
ue_body_SetLinearDamping(body,0.375)
ue_body_SetEnableGravity(body,false)
ue_body_SetCollisionEnabled(body,E_ECollisionEnabled_52584e7c.v_QueryOnly)
ue_body_AddImpulse(body,ue_vector(0.0lf,0.0lf,500.0lf),UEName(value="None"),true)
```

`ue_body_SetCollisionProfileName` applies a collision profile to a component. `ue_handle_*` grabs, moves, rotates and releases simulated components through a `PhysicsHandleComponent` (it ticks as a normal component).

### Collision profile name property

`ue_collision_SetCollisionProfileNameProperty(object, property_name, value)` assigns an `FCollisionProfileName` struct field on an object; it does **not** apply a profile to a body (use `ue_body_SetCollisionProfileName`). The field is looked up by name each time (inherited fields allowed), must be Blueprint-visible and writable, and is written through the property's setter. The call is refused for null/CDO/template objects, destroyed actors/components, foreign worlds and in ConstructionScript, `bind_events`, `EndPlay` and teardown. Errors are daScript errors rather than silently skipped writes.

## Constraints

`ue_constraint_GetConstraint` returns an opaque `UEConstraintHandle` that the `ue_joint_*` functions accept as their accessor:

```das
let handle=ue_constraint_GetConstraint(component)
if (ue_constraint_valid(handle)) {
    let updated=ue_joint_SetDisableCollision(handle,true)
    let state=ue_joint_GetDisableCollsion(updated)    // Unreal's own spelling
    verify(state.out_returnvalue)
}
```

A ref/out accessor comes back as an updated handle (`out_accessor` when there are other outputs). The handle stores only a token; owner and bodies are weak. It becomes stale on Destroy, when constrained bodies or bones change, when the broken state changes or when the physical constraint is recreated: call `GetConstraint` again. A pending call is rechecked just before dispatch, so an old frame never silently targets a new constraint. Errors do not roll back completed native operations.

Supported: component-owned `GetConstraint` and the `ConstraintInstanceBlueprintLibrary` accessors. Not supported: skeletal/PhysicsAsset accessors, other producers, arrays of handles, `ue_property`/`ue_function` use, and use during construction, `bind_events` or `EndPlay`. Handles are not carried over to a new VM or after hot reload.

## Weak object properties

`TWeakObjectPtr` fields use the same typed reference as a strong field of that class. Reads go through `FWeakObjectProperty`; expired references and references to actors/components being destroyed read as null. Call frames and snapshots do not pin weak leaves. Direct arrays, sets and maps of weak references are rejected; structs with their own weak fields can be stored in arrays.

## Exclusions

| Function | Reason | Use instead |
| --- | --- | --- |
| `KismetSystemLibrary.GetCollisionProfileNames` | Editor customization helper | Profile names from Project Settings |
| `PhysicsConstraintComponent.ReInitConstraintAndApplyPhysicsState` | Engine maintenance, not BlueprintCallable | Public constraint setup API |
| `PhysicsConstraintComponent.SetAngularOrientationDrive` | Deprecated | `SetOrientationDriveTwistAndSwing` / `SetOrientationDriveSLERP` |
| `PhysicsConstraintComponent.SetAngularVelocityDrive` | Deprecated | `SetAngularVelocityDriveTwistAndSwing` / `SetAngularVelocityDriveSLERP` |
| `PhysicsHandleComponent.GrabComponent` | Deprecated | `GrabComponentAtLocation` / `GrabComponentAtLocationWithRotation` |
| `PrimitiveComponent.GetBodyInstanceAsyncPhysicsTickHandle` | Raw async handle forbidden | [Async physics](../Guide/Gameplay/AsyncPhysics.md): `ue_async_physics`, `ue_physics_body`, `ue_physics_add_force` |
| `PrimitiveComponent.HandlePostMigrationPhysicsStateChanged` | Engine migration hook | — |

## Limitations

- Complex, welded and skeletal bodies, worker-thread VMs and network resimulation are outside this package.
- A project copy of `ue_api_collision.das` in `Content/Das/Scripts` overrides the installed version; keep a single source.

Try it: `.\Scripts\TestCollisionApiPackage.ps1 -SkipBuild -Publish` regenerates, probes and reinstalls the package.

See also: [API packages](README.md) · [Async physics](../Guide/Gameplay/AsyncPhysics.md) · [Spawning and queries](../Guide/Gameplay/SpawnAndQueries.md) · [World / Actor / Components](WorldActorComponents.md)
