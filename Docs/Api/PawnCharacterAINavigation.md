# Pawn / Character / AI / Navigation Package (ue_api_pawn_ai)

`ue_api_pawn_ai` wraps the reflected API of pawns, characters, controllers, movement components, AI controllers, AI perception and the navigation system. It is generated from `Bindings/Packages/PawnCharacterAINavigation.profile.json` (UE modules Engine, AIModule, NavigationSystem). The package is not installed by default: publish it before use.

```powershell
.\Scripts\GenerateApiPackage.ps1 -Profile .\Bindings\Packages\PawnCharacterAINavigation.profile.json -SkipBuild -Publish
```

```das
options gen2
require ue_api_pawn_ai

[export]
def main(input:int):int {
    let pawn = C_Pawn_ba3732bb_from(ue_self_object())
    ue_pawn_AddMovementInput(pawn, ue_vector(1.0lf,0.0lf,0.0lf), 1.0, false)
    let pending : UEVector = ue_pawn_GetPendingMovementInputVector(pawn)
    return ue_ai_helpers_IsValidAILocation(ue_vector(0.0lf,0.0lf,0.0lf)) ? 42 : 0
}
```

## Contents

| Prefix | Class | Highlights |
| --- | --- | --- |
| `ue_ai_helpers_` | `AIBlueprintHelperLibrary` | `GetAIController`, `SimpleMoveToLocation/Actor`, `SpawnAIFromClass`, `SendAIMessage`, path queries, `IsValidAILocation/Direction/Rotation` |
| `ue_pawn_` | `Pawn` | movement input (add/pending/consume/last), controller input, `GetController`, `IsPlayerControlled`, `PawnMakeNoise`, `GetMovementBaseActor` |
| `ue_character_` | `Character` | `Jump`, `StopJumping`, `Crouch`/`UnCrouch`, `LaunchCharacter`, `PlayAnimMontage`/`StopAnimMontage`, root motion queries |
| `ue_controller_` | `Controller` | `Possess`/`UnPossess`, `K2_GetPawn`, control rotation, input ignore flags, `LineOfSightTo`, `StopMovement` |
| `ue_player_` | `PlayerController` | mouse/touch/key state, deproject screen→world, hit under cursor, viewport size, HUD, streaming sources |
| `ue_pawn_move_`, `ue_nav_move_` | `PawnMovementComponent`, `NavMovementComponent` | input vectors, falling/flying/swimming/crouching state |
| `ue_character_move_` | `CharacterMovementComponent` | `SetMovementMode`, `AddImpulse`/`AddForce`, floor queries, walkable angle, gravity direction, avoidance |
| `ue_ai_controller_` | `AIController` | `MoveToLocation/Actor`, `K2_SetFocus`/`K2_ClearFocus`, focal point, move status, `RunBehaviorTree`, `UseBlackboard` |
| `ue_perception_` | `AIPerceptionComponent` | currently/known/hostile perceived actors, `GetActorsPerception`, `SetSenseEnabled`, `ForgetAll` |
| `ue_navigation_` | `NavigationSystemV1` | `K2_ProjectPointToNavigation`, `K2_GetRandomLocationInNavigableRadius`, `K2_GetRandomReachablePointInRadius`, `NavigationRaycast`, `FindPathToLocationSynchronously`, `FindPathToActorSynchronously`, `GetPathCost`, `GetPathLength` |
| `ue_path_` | `NavigationPath` | `IsValid`, `IsPartial`, `GetPathLength`, `GetPathCost`, `GetLengthFromPosition` |

About 250 wrappers are generated. Each class contributes its own declarations only; part of this API is also available through `self` and [native class bindings](../Reference/NativeClassBindings.md).

## Semantics

- Native static `BlueprintCallable` methods of native classes (such as the `NavigationSystemV1` queries and two Pawn movement-base helpers) are called through the class default object with the usual call frame. Their `WorldContext` is hidden and filled from the script owner's live game world; it cannot be overridden manually. Calling an instance method as static, a non-native owner and a wrong world are rejected.
- Native failure semantics are preserved: an empty `NavigationPath` reports `IsValid=false` and length/cost `-1`; `GetPathCost`/`GetPathLength` return the native error enum and a zero out value; synchronous path functions may return null; `GetMovementBaseActor`/`Owner` are nullable.
- Perception arrays are empty when nothing is perceived. Sense classes are checked against their `MetaClass`.
- `LaunchCharacter` queues a pending launch velocity; the movement happens on later movement ticks.
- Null and wrong-class receivers are rejected. Several functions keep construction guards.
- All arguments are explicit; canonical math types are used (see [API packages](README.md)).

## Managed alternatives

- `AIBlueprintHelperLibrary.CreateMoveToProxyObject` is excluded on purpose: use the lifecycle-managed `ue_ai` MoveTo adapter, which owns callbacks and cancellation.
- Perception callbacks with a typed `AIStimulus` come from the `ue_perception` module and `ue_event(source="Perception",name="OnTargetPerceptionUpdated")`.

Both are described in [AI and perception](../Guide/Gameplay/AIAndPerception.md).

## Not included

- `PathFollowingComponent` methods (all blocked by unsupported signatures).
- `PlayerController` `Client*` RPC declarations: networking is not supported.
- `PlayDynamicForceFeedback` (needs a latent adapter).
- Many declarations excluded by visibility or deprecation policy, and functions using unsupported shapes such as `TArray<uint8>`.

Named blockers with native evidence: [PawnAINavigationBlockers](../Dev/Api/Blockers/PawnAINavigationBlockers.md) and `Bindings/Packages/ue_api_pawn_ai.summary.json`.

Try it: `.\Scripts\TestPawnAIPackage.ps1 -SkipBuild` (pawn input, movement mode, AI focus/possession, navigation paths and perception queries); `.\Scripts\TestNavigationQueries.ps1` runs the navigation queries on a real Recast NavMesh.

See also: [API packages](README.md) · [AI and perception](../Guide/Gameplay/AIAndPerception.md) · [World / Actor / Components](WorldActorComponents.md) · [Native class bindings](../Reference/NativeClassBindings.md)
