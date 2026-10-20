# Examples

These examples show complete gameplay written in daScript: a playable first-person Character with a
respawning pickup, HUD, damage and effects; an AI enemy; an Actor with an authored component; a
standalone project template; and a source-only gameplay demo.

| Example | How to get it | Main sources |
| --- | --- | --- |
| [Playable Character](#playable-character) | `Scripts/CreatePlayableCharacter.ps1` | `dsl_playable_character.das`, `dsl_playable_pickup.das`, `dsl_health_component.das` |
| [AI enemy](#ai-enemy) | `Scripts/CreateAIEnemy.ps1` | `dsl_enemy_character.das` |
| [Actor + Component](#actor-and-component) | `Das.DSL.Apply` | `dsl_actor_component_owner.das`, `dsl_component_behavior.das` |
| [DasStarter project](../../../Examples/Standalone/README.md) | `Scripts/NewDasProject.ps1` | `Examples/Standalone/Template/Content/Das/Scripts/*.das` |
| [Gameplay demo](../../../Examples/GameplayDemo/README.md) | Manual setup | `dsl_gameplay_actor.das`, `dsl_gameplay_behavior.das` |

Unless noted otherwise, source files are in `Plugins/DasForUE/Content/Das/Scripts/`.

## Playable Character

```powershell
.\Scripts\CreatePlayableCharacter.ps1
# If the Editor target is already built:
.\Scripts\CreatePlayableCharacter.ps1 -SkipBuild
```

The command creates a fresh folder `/Game/DasExamples/Playable/<id>` in the smoke project
(`-Project <path.uproject>` selects another project) with the Character and pickup Blueprints, the
HealthComponent, a Widget Blueprint HUD, six InputActions, a MappingContext, a separate pickup mesh,
two NiagaraSystems and `PlayableMap`. The interaction interface is stored once in
`/Game/DasExamples/Shared/Interactable` and reused by every export. Open the printed map and press
Play. Each run creates a new folder; changing the structure of an existing Blueprint requires
reimporting it.

| Input | Action |
| --- | --- |
| WASD | Move relative to view yaw |
| Mouse | Look |
| Space | Jump / release |
| E | Interact with the object under the camera trace |
| H | Take 25 damage |
| Tab | Toggle mouse / UI interaction |

The Character auto-possesses Player 0 and uses its camera. The map has a floor and a few cube
obstacles. The smoke project already selects `EnhancedPlayerInput` / `EnhancedInputComponent`; in
another project configure those classes and assign the mapping context, the six input actions and the
HUD class on the Character.

### Character

`PlayableCharacter` is an authored class on the real `ACharacter` host with a camera, a world-space
text label and a `Health` component. Native fields and reflected methods are used through ordinary
`self` syntax:

```das
def BeginPlay():void {
    self.Label.K2_SetText(ue_loctext(PlayerLabel, "Player"))
    self.CharacterMovement.MaxWalkSpeed=450.0f
    self.CharacterMovement.JumpZVelocity=500.0f
}

[ue_function(BlueprintCallable)]
def OnMove(value:InputActionValue):void {
    let axis=ue_input_axis2d(value)
    ue_character_move(ue_character_self(),float(axis.y),float(axis.x))
}

[ue_function(BlueprintCallable)]
def OnJump(value:bool):void {
    if (value) { self.Jump() } else { self.StopJumping() }
}
```

- `Tick` waits for a local controller, Enhanced Input and the pawn's input component, then calls
  `BindControls` (`ue_enhanced_add_context`, `ue_enhanced_bind`) and creates the HUD. When local
  control is lost it releases its input and HUD, and binds again after repossession. Managed teardown
  removes everything on EndPlay. The example's session owns all of its input, so do not share it with
  unrelated bindings if you reuse its unbind-all approach.
- Movement uses controller yaw; look uses the controller input functions (respecting native scaling);
  jump uses `Jump` / `StopJumping`; CharacterMovement does movement, collision and gravity.
- Movement properties are set in BeginPlay; camera defaults use the component defaults path.
- `AddScore(amount)` updates `Score`, the label and the HUD.

Helpers from `ue_character_control.das`: `ue_character_self`, `ue_character_move`,
`ue_character_look`, `ue_character_jump`, `ue_pawn_possess(player, pawn)` (requires authority). Input
helpers require a locally controlled Character; operations reject cross-world, template, destroyed
and EndPlay objects.

### Pickup

`PlayablePickup` has a root `SphereComponent` (radius 75) and a child `StaticMeshComponent`. It reacts
to a real Unreal overlap event; nothing is polled in Tick:

```das
[ue_event(name="OnActorBeginOverlap")]
def OnOverlap(overlapped:ActorRef; other:ActorRef):void {
    self.OverlapEvents+=1
    self.Interact(other)
}

[ue_function(BlueprintCallable)]
def Interact(interactor:ActorRef):void {
    if(!self.Available || !ue_is_valid(interactor) || !ue_is_valid(self.Collector)) { return }
    if(!ue_same_object(interactor,self.Collector)) { return }
    // Set the guard before changing collision, which can issue nested events.
    self.Available=false
    self.Trigger.SetCollisionEnabled(ECollisionEnabled.NoCollision)
    self.Visual.SetVisibility(false,true)
    self.Collector.AddScore(self.Points)
    self.Pickups+=1
    ue_timer_start("respawn",self.RespawnSeconds,"Restore",false)
}
```

- `Collector` is the Character the exporter assigns; other pawns are ignored. `Points` defaults to 10
  and `RespawnSeconds` to 3; both must be positive. Single-player only, no replication.
- `Restore` cancels the old named timer and re-enables state, visibility and collision. If the player
  is still inside the sphere, the pickup can be collected again immediately (once per period).
- The mesh is a typed soft reference, loaded asynchronously in BeginPlay:

  ```das
  var MeshAsset:SoftStaticMesh=ue_soft(type<SoftStaticMesh>,"/Engine/BasicShapes/Sphere.Sphere")
  // in LoadMesh():
  ue_soft_load_async("mesh",self.MeshAsset,"OnMeshLoaded")
  ```

  Until `OnMeshLoaded(mesh:StaticMeshRef)` arrives the pickup is hidden and not collectable
  (`MeshLoading=true`); `Restore` cannot enable it before `MeshReady`. An empty reference or a load
  error increments `LoadFailures` and leaves it unavailable without a script exception.
  `LoadMesh` is Blueprint-callable and replaces a pending request. The exporter saves a separate
  `PickupMesh` asset so a fresh game process really loads it asynchronously.
- `ue_is_valid(ref)` checks a weak reference; `ue_same_object(a, b)` compares two live references,
  including different typed views of the same object.
- The timer, overlap subscription, async request and pickup effects belong to the pickup and are
  removed by managed teardown on EndPlay/Destroy.

### HUD, interaction, damage and effects

- **E** traces from the camera and calls the pickup through the shared Blueprint interface
  `Interactable` (declared in `ue_interaction_contract.das`, stable ID
  `dasplayableinteraction.Interactable`). All exported maps use the single shared asset in
  `/Game/DasExamples/Shared`, which must be shipped with them.
- **H** applies native damage; `dsl_health_component.das` handles it and raises a `Changed` event the
  Character subscribes to. Sound and Niagara effects play on pickup and damage.
- The HUD is created with `ue_widget_create`, shows Score and Health, and has a Damage button bound
  with `ue_widget_bind_click`. **Tab** toggles the cursor for UI interaction.

Details: [Gameplay services](../Gameplay/GameplayServices.md), [Input](../Gameplay/Input.md),
[Animation and effects](../Gameplay/AnimationAndEffects.md).

## AI enemy

```powershell
.\Scripts\CreateAIEnemy.ps1 -SkipBuild
```

Creates `/Game/DasExamples/AI/<id>/PlayableMap`: the playable Character, HUD and pickup plus an enemy,
an obstacle and a saved NavMesh. `dsl_enemy_character.das` is a real Character with an AIController:
every 0.25 s it checks its target and runs `MoveToActor`; in attack range it stops and calls
`ApplyDamage`. `AttackDamage`, `AttackRange`, `AttackInterval` and `Target` are editable in Blueprint.
By default the enemy is slower than the player and attacks once per second. `StopBrain` /
`ResumeBrain` control its timer and movement; after an accepted logic reload call `ResumeBrain`
(BeginPlay is not replayed). See [AI and perception](../Gameplay/AIAndPerception.md).

## Actor and Component

An Actor Blueprint with an authored Component Blueprint declared as a field:

- `dsl_actor_component_owner.das` — `ComponentOwner` with `Score`/`Begins`/`Ticks`/`Ends`,
  `GetScore`, lifecycle methods and the component field `Behavior:ScoreComponent`;
- `dsl_component_behavior.das` — `ScoreBehavior`: fields, Blueprint methods, lifecycle and event
  methods, and typed access to its owner;
- `ue_component_types.das` and `ue_actor_refs.das` — imported reference types (declarations only,
  which avoids cyclic modules).

```das
class ComponentOwner : Actor {
    components = ue_components(VisibleAnywhere, BlueprintReadOnly, Category="Components") {
        var Behavior:ScoreComponent
    }
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Score:int=0
    }
    [ue_function(BlueprintCallable)]
    def ConfigureBehavior(bonus:int):void { self.Behavior.Bonus = bonus }
}
```

Applying the Actor also creates the component Blueprint from the imported reference and adds it to
the Actor's components; no manual setup is needed. Save both Blueprints with the map. Each object
owns its own VM session; Destroy and world/PIE cleanup release sessions and subscriptions. Metadata
(Category, ToolTip, access flags) survives compilation and save/reopen. See
[Components](../DSL/Components.md).

## Your own project and the gameplay demo

- [DasStarter](../../../Examples/Standalone/README.md) — a separate UE project with its own scripts,
  Blueprints, map and project-local bindings; the recommended starting point
  ([guide](StandaloneProject.md)).
- [Gameplay demo](../../../Examples/GameplayDemo/README.md) — Actor + component with timers, owner
  delegate, Enhanced Input, latent delay and async load; sources plus manual setup instructions.

See also: [Install](Install.md) · [Your own project](StandaloneProject.md) ·
[Apply and hot reload](../Workflow/ApplyAndHotReload.md) · [Classes](../DSL/Classes.md)
