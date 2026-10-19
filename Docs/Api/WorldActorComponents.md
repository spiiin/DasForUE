# World / Actor / Components Package (ue_api_world)

`ue_api_world` exposes the reflected methods of the core gameplay classes plus `GameplayStatics` and `KismetSystemLibrary` as typed daScript functions. It is installed in the plugin scripts and generated from `Bindings/Packages/WorldActorComponents.profile.json`.

```das
options gen2
require ue_api_world

[export]
def main(input:int):int {
    let actor = C_Actor_adf65d5b_from(ue_self_object())
    ue_actor_SetActorScale3D(actor,ue_vector(2.0lf,3.0lf,4.0lf))
    let scale : UEVector = ue_actor_GetActorScale3D(actor)
    let root = ue_actor_K2_GetRootComponent(actor)
    ue_scene_SetVisibility(root,true,true)
    let actors <- ue_gameplay_GetAllActorsWithTag(ue_name("Pickup"))
    return int(scale.z) + length(actors)
}
```

The example needs an Actor that owns the script.

## Contents

| Prefix | Class or library | Kind |
| --- | --- | --- |
| `ue_world_` | `World` | receiver methods |
| `ue_actor_` | `Actor` | receiver methods |
| `ue_component_` | `ActorComponent` | receiver methods |
| `ue_scene_` | `SceneComponent` | receiver methods |
| `ue_primitive_` | `PrimitiveComponent` | receiver methods |
| `ue_controller_` | `Controller` | receiver methods |
| `ue_movement_` | `MovementComponent` | receiver methods |
| `ue_gameplay_` | `GameplayStatics` | static functions |
| `ue_system_` | `KismetSystemLibrary` | static functions |

About 650 wrappers are generated. Each receiver contributes only its own declarations (inherited copies are not repeated): call `ue_actor_*` for Actor methods on any Actor view, converting with `_as_<Ancestor>` or via `_raw`/`_from` where needed. Some of these methods are also reachable through `self` and [native class bindings](../Reference/NativeClassBindings.md); the package does not add fields or native getters/setters.

## Typical calls

```das
let actor = C_Actor_adf65d5b_from(ue_self_object())
let bounds = ue_actor_GetActorBounds(actor,false,false)       // out_origin, out_boxextent
let player = ue_gameplay_GetPlayerCharacter(0)                 // C_Character_ab4a5baa, may be null
let controller = ue_gameplay_GetPlayerController(0)
let children <- ue_scene_GetChildrenComponents(ue_actor_K2_GetRootComponent(actor),true)
ue_actor_SetActorHiddenInGame(actor,false)
let name = ue_system_GetDisplayName(C_Object_55c3753a_from(C_Actor_adf65d5b_raw(actor)))
```

- All arguments are explicit (no C++ defaults). Multiple outputs return a `_Result` record with `out_*` fields; arrays are moved with `<-`.
- Object views have short typedefs (`Actor`, `Character`, `Controller`, …) and `_from`/`_raw`/`_valid` helpers. Null results are typed null references; check with `_valid`.
- `WorldContext` parameters are hidden and filled from the owner's live game world. Without a live Game world the call fails; the automatic argument cannot be overridden. Calls on a `World` receiver (for example `ue_world_K2_GetWorldSettings(world)`) are also supported.
- Vector/Rotator/Transform use the canonical `UEVector`/`UERotator`/`UETransform` shared with [Math](Math.md). Other structs (e.g. `LinearColor`) use generated `S_*` types.
- Functions marked construction-unsafe in Unreal work during gameplay and are rejected in a ConstructionScript before dispatch.
- Null, wrong-class and destroyed receivers are rejected.

## Importing with other packages

Object views and `UEName` are generated per package. When importing `ue_api_world` together with other packages, qualify names: `ue_api_world::ue_name(...)`, `ue_api_world::C_Actor_adf65d5b_from(...)`. Converting a view from another package goes through `UObjectHandle` (`_raw` then `_from`), which is a checked conversion of the same object.

## Not included

- Functions with unsupported signatures, internal/protected/deprecated functions, latent and custom-thunk functions.
- Soft object/class reference parameters: they are supported elsewhere in the binding but have no generic wrapper ABI yet; the exporter reports an explicit reason.
- `PrimitiveComponent`'s async physics tick handle (use [Async physics](../Guide/Gameplay/AsyncPhysics.md)) and a few Dialogue functions with an unsupported struct-array shape.

The full named list is in `Bindings/Packages/ue_api_world.summary.json`.

Try it: `.\Scripts\TestWorldApiPackage.ps1 -SkipBuild` (add `-Publish` to reinstall after a successful probe).

See also: [API packages](README.md) · [Collision / Physics](CollisionPhysics.md) · [Spawning and queries](../Guide/Gameplay/SpawnAndQueries.md) · [Native class bindings](../Reference/NativeClassBindings.md)
