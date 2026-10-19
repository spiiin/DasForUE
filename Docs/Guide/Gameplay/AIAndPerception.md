# AI and Perception

The `ue_ai` module gives scripts a typed `AIControllerRef` and managed MoveTo requests toward an Actor or a location. The `ue_perception` module delivers AI Perception updates to ordinary `[ue_event]` handlers as a typed, owned `AIStimulus` value.

## Getting the controller

`ue_ai_self_controller()` returns the AIController that already possesses the script's Pawn. If the script belongs to an AIController, it returns that controller; for an ActorComponent, the component's owner is used. The helper never creates a controller: set `AIControllerClass` and `AutoPossessAI` on the Blueprint.

## Managed MoveTo

```das
require ue_ai

// Inside a ue_class method; Target is a typed ActorRef.
let controller=ue_ai_self_controller()
ue_ai_move_to_actor("chase",controller,self.Target,"OnMoveFinished",40.0f)

[ue_function(BlueprintCallable)]
def OnMoveFinished(result:MoveResult):void {
    if(result.Code==MoveCode.Success) {
        // Arrived at the target.
    }
}
```

| Function | Purpose |
| --- | --- |
| `ue_ai_move_to_actor(name, controller, target:ActorRef, handler="", radius=50.0f, pathfinding=true, partial=false):bool` | Move toward an Actor. |
| `ue_ai_move_to_location(name, controller, location:UEVector, handler="", radius=50.0f, pathfinding=true, partial=false):bool` | Move toward a location. |
| `ue_ai_move_active(name):bool` | Is this script instance's named request active? |
| `ue_ai_move_cancel(name):bool` | Remove the callback and stop the movement; no callback is delivered. |
| `ue_apply_damage(target, amount, source:ActorRef, instigator:AIControllerRef):float` | Native ApplyDamage with an AI instigator. |

Arguments and semantics:

- `radius` (acceptance radius) must be finite and non-negative. UE includes the agent radius when checking arrival.
- With `pathfinding=true` a location target is projected onto the NavMesh. With `pathfinding=false` projection is disabled and obstacle avoidance is not provided.
- The `bool` result means UE accepted the request or is already at the goal. A pathfinding failure returns `false` and queues `MoveResult` with `Invalid`; AlreadyAtGoal queues `Success`.
- Type, owner, authority, argument and callback errors are rejected **before** the previous request is replaced.
- `MoveResult` contains `Code` (`MoveCode`: `Success`, `Blocked`, `OffPath`, `Aborted`, `Invalid`) and the UE `RequestId`.
- All callbacks are deferred; they never run inside the starting daScript call. Without a handler no callback runs, but the subscription is still released automatically.

### One movement per controller

A controller runs one movement at a time. A new managed request replaces the previous one in the current session even if the name differs. An external MoveTo can preempt a managed request; the managed result is then `Aborted`. Cleaning up the old subscription never cancels the newer external movement and never removes other listeners.

### Lifetime

The managed proxy stores the UE request ID and weak references, never VM pointers. On Destroy/EndPlay and on a successful hot reload, only its own delegate is removed and only its own current request is cancelled; a queued callback is revoked. A rejected reload keeps the working request and handler.

Movement requires authority, a live Pawn in the current game world, and a controller that matches the script's owner. You cannot drive another actor's controller with this API.

## Example: chasing enemy

`Plugins/DasForUE/Content/Das/Scripts/dsl_enemy_character.das` is a real Character with an AIController, a visible mesh and an "Enemy" label. Every 0.25 s it checks its target and starts MoveToActor; within attack range it cancels the chase and calls `ue_apply_damage` with the AI instigator. `AttackDamage`, `AttackRange`, `AttackInterval` and `Target` are editable in the Blueprint.

```das
[ue_function(BlueprintCallable)]
def Think():void {
    if(!self.Enabled || !ue_is_valid(self.Target)) { ue_ai_move_cancel("chase");return }
    let controller=ue_ai_self_controller()
    if(!ue_is_valid(controller)) { return }
    if(self.GetDistanceTo(self.Target)<=self.AttackRange) {
        ue_ai_move_cancel("chase")
        ue_apply_damage(self.Target,self.AttackDamage,ue_self_actor(),controller)
    } elif(!ue_ai_move_active("chase")) {
        ue_ai_move_to_actor("chase",controller,self.Target,"OnMoveFinished",40.0f)
    }
}
```

`StopBrain`/`ResumeBrain` control the timer and movement. After a successful logic reload `BeginPlay` is not called again, so call `ResumeBrain` to restart the timer.

Try it: `.\Scripts\CreateAIEnemy.ps1 -SkipBuild` creates a separate `/Game/DasExamples/AI/<id>/PlayableMap` with the playable Character/HUD/pickup plus the enemy, an obstacle and a saved NavMesh. Open the map, press Play and move away with WASD. By default the enemy is slower than the player and attacks once per second. (The ordinary `CreatePlayableCharacter.ps1` does not add the enemy.)

## Navigation queries

The static `NavigationSystemV1` queries (projection, random/reachable points, NavMesh raycast, path to location/Actor with cost and length, partial paths) are available as generated bindings in the Pawn/Character/AI/Navigation API package. See [Pawn, Character, AI and Navigation API](../../Api/PawnCharacterAINavigation.md).

## Perception events

`require ue_perception` imports `AIStimulus`, `AISense`, `PerceptionComponentRef` and the ordinary `ActorRef`. Bind `OnTargetPerceptionUpdated` with a normal `[ue_event]`; the compiler generates the binder automatically.

```das
require ue_perception

[ue_class]
class PerceptionProbe : Actor {
    @ue_component
    Perception:PerceptionComponentRef

    [ue_event(source="Perception",name="OnTargetPerceptionUpdated")]
    def OnTarget(actor:ActorRef; stimulus:AIStimulus):void {
        if(stimulus.Sense==AISense.Sight && stimulus.SuccessfullySensed) {
            ue_state_set_int("seen",ue_state_get_int("seen",0)+1)
        }
    }
}
```

Stimuli are produced by UE itself (sight registration, `AISense_Hearing.ReportNoiseEvent`, aging by MaxAge); the handler simply receives them. The generated `ue_perception_*` queries for current/known perceived actors remain available.

### AIStimulus fields

`AIStimulus` is an owned snapshot captured when UE emits the event, not a pointer into the AIPerceptionComponent.

| Field | Type / meaning |
| --- | --- |
| `Sense` | `AISense.Invalid`, `Sight`, `Hearing` or `Other` |
| `SuccessfullySensed` | native `WasSuccessfullySensed()` |
| `Expired` | native `IsExpired()` |
| `Valid` | native `IsValid()`; not a synonym for "currently perceived" |
| `Age` | native `GetAge()` at callback time |
| `ExpirationAge` | reflected UE `ExpirationAge` |
| `Strength` | `float` |
| `StimulusLocation`, `ReceiverLocation` | `UEVector` |
| `Tag` | `UENameValue` |

- The snapshot's `Age` does not advance on its own; editing the local value never changes native perception.
- `Sense` is classified natively from the sense ID at capture time, not from Tag, actor name or a fixed number. An invalid ID gives `Invalid`; any other valid sense (e.g. Damage) gives `Other`.
- The projection is allowed only for the exact native `FAIStimulus` to `AIStimulus` pair in this callback. A different struct with matching fields is rejected. Writing back into `FAIStimulus` is not supported.

## Limitations

- Behavior Tree, StateTree, EQS, networking and full combat animation are not provided by these modules.
- Deriving a DSL class directly from `AIPerceptionComponent` is not supported; use an Actor with a native perception component as shown above.
- The typed stimulus projection covers `OnTargetPerceptionUpdated`. Other perception delegates (forgotten/info updates), single-parameter forms, sight occlusion and team-affiliation policies are not specifically covered.

See also: [Timers](Timers.md), [Callback arguments](CallbackArguments.md), [Gameplay services](GameplayServices.md), [Delegates and events](../DSL/DelegatesAndEvents.md), [Components](../DSL/Components.md)
