# Gameplay demo (source example)

A compact source example that combines an authored Actor and an authored ActorComponent with timers,
an owner delegate, Enhanced Input, a runtime SceneComponent, a latent delay and an async asset load.
It ships as daScript sources plus a setup description (`manifest.json`); you create the Blueprints,
input assets and map yourself as described below. For a ready-made scene, use the
[playable Character example](../../Docs/Guide/GettingStarted/Examples.md#playable-character) instead.

## Source files

| File | Content |
| --- | --- |
| `Plugins/DasForUE/Content/Das/Scripts/dsl_gameplay_actor.das` | `GameplayActor`: Score, Move, Ready, async load, input setup, pivot movement |
| `Plugins/DasForUE/Content/Das/Scripts/dsl_gameplay_behavior.das` | `GameplayBehavior` component: score timer, signal handler, Seconds |
| `Plugins/DasForUE/Content/Das/Scripts/ue_gameplay_refs.das` | Typed reference views of both authored classes |
| `Plugins/DasForUE/Content/Das/Scripts/ue_gameplay_catalog.json` | Source catalog shared by the reference views |

The Actor has one `Behavior` component field declared through `ue_components`; shared field flags use
`ue_properties`. The Actor uses typed native input, scene and object fields. The component's timer
and owner-delegate handler update `Actor.Score` through ordinary typed access. The two sources refer
to each other through the forward-declaration-aware source catalog.

## What it does

1. Actor `BeginPlay` creates an Actor-owned `GameplayPivot` SceneComponent and starts a 0.25
   world-second latent delay. If `AssetPath` is not empty, it also starts an async asset load.
2. Component `BeginPlay` starts a repeating score timer (`Pulse` with argument 1, once per second).
3. The component subscribes to the owner's `OnDasSignal`. Jump emits that signal, and the
   component's event method adds another point and increments `SignalCount`.
4. The Axis2D move action writes `Move`; Actor `Tick` applies the XY displacement to the pivot's
   relative location, keeping its rotation and scale.
5. The latent completion sets `Ready`. The async completion assigns the nullable `Loaded` and sets
   `LoadSucceeded`.

Blueprint-callable control methods: `SetupControls`, `StartLoad`, `CancelPending`, `PauseScore`,
`ResumeScore`, `GetSeconds`.

The pivot is a plain SceneComponent without a mesh. Add a mesh child through normal Unreal setup if
you want to see the movement. The code moves the pivot, not a CharacterMovement component and not
necessarily the Actor root.

## Setup

`manifest.json` is a project-specific setup description (not an Unreal manifest or an importer). It
lists the source IDs, asset names, input actions and key mappings used below.

1. Use a new folder such as `/Game/DasGameplayDemo` in a test project.
2. Import the Actor with the DSL service, e.g.
   `Das.DSL.Apply dsl_gameplay_actor.das /Game/DasGameplayDemo/BP_GameplayActor gameplaydemo.GameplayActor`.
   The declared component source is imported automatically as `BP_GameplayBehavior`. Check that both
   classes exist and that the Actor's components contain `Behavior` before placing the Actor on a new
   map. Save the Blueprints with the map.
3. Configure the project for `EnhancedPlayerInput` / `EnhancedInputComponent`. Create:
   - `IA_Jump` (Boolean): SpaceBar;
   - `IA_Move` (Axis2D): D/A → +X/−X, W/S → +Y/−Y using the usual Negate/Swizzle modifiers;
   - `IMC_Gameplay` with both mappings (priority 0).
4. On the placed Actor assign `Player` (the local PlayerController of that world), `Controls`,
   `JumpAction` and `MoveAction`. After the player and component are ready, call `SetupControls` from
   Blueprint. An already installed external context is used without taking ownership. Make sure the
   player has a usable view target/camera.
5. Optional: set `AssetPath` to an asset that is included in the cook. A string path alone does not
   make the asset part of the package; add a real soft/primary asset reference or a cook rule.

## Expected behavior

- `Score` grows by one per world-second while the score timer runs.
- Jump adds one point and increments `Behavior.SignalCount`.
- Moving changes `Move` and the pivot's XY location.
- `Ready` becomes true 0.25 world-seconds after BeginPlay.
- After an async load completes, `Loaded` and `LoadSucceeded` are set.

## Hot reload

- Accepted body reload restores the last installed input configuration (mapping context and
  handlers). Editing `SetupControls` does not rerun it: call the revised setup explicitly.
- Accepted component reload cancels the old score timer; call `ResumeScore` to restart it.
- Accepted Actor reload cancels latent and async work; `StartLoad` restarts the asset load.
- A failed compilation keeps the old live version, its timers and bindings.
- The Actor keeps its pivot across logic reload; BeginPlay is not replayed.

See also: [Examples](../../Docs/Guide/GettingStarted/Examples.md) ·
[Apply and hot reload](../../Docs/Guide/Workflow/ApplyAndHotReload.md) ·
[Components](../../Docs/Guide/DSL/Components.md) · [Input](../../Docs/Guide/Gameplay/Input.md) ·
[Timers](../../Docs/Guide/Gameplay/Timers.md)
