# Integrated gameplay source example

Status: **source-only, validation pending** (2026-10-10). This is a cohesive source
example and setup specification. Blueprint/InputAction/MappingContext assets and a
playable map have not been created. Source compilation is validated; this demo map
has not been imported, played or cooked. Runtime API coverage is described in
Docs/GameplayRuntimePIEValidation.md.

## Source files

- Plugins/DasForUE/Content/Das/Scripts/dsl_gameplay_actor.das
- Plugins/DasForUE/Content/Das/Scripts/dsl_gameplay_behavior.das
- Plugins/DasForUE/Content/Das/Scripts/ue_gameplay_refs.das
- Plugins/DasForUE/Content/Das/Scripts/ue_gameplay_catalog.json

Actor/Component reference views share the source catalog; the Actor has one
Behavior component field declared through ue_components. Shared field flags use
ue_properties; the Actor uses typed native input/scene/object fields.
Component timer and owner delegate update Actor.Score through ordinary typed access.
Source references follow the existing forward-declaration/cycle-aware compiler path;
that path was compile-validated after conversion to declaration groups on 2026-10-11.

## Intended gameplay

1. Actor BeginPlay creates an Actor-owned GameplayPivot SceneComponent and starts
   a 0.25-world-second latent delay. It starts asset loading if AssetPath is nonempty.
2. Component BeginPlay starts a repeating score timer with saved integer argument 1.
3. Component subscribes to owning Actor's OnDasSignal. Jump emits that signal and
   adds another point through the component's annotated event method.
4. Axis2D input writes Move. Actor Tick applies XY displacement to the pivot's
   relative transform, preserving native rotation and scale.
5. Latent completion sets Ready. Asset completion assigns nullable Loaded and
   LoadSucceeded. Blueprint can inspect state and invoke exposed control methods.

The pivot is a plain SceneComponent; it has no visible mesh. Add a configured mesh
child through ordinary Unreal setup if a visual movement demo is desired. The code
moves the pivot, not a Character movement component and not necessarily Actor root.

## Isolated setup specification

manifest.json is a **project-specific setup description**, not an Unreal standard
manifest or an executable importer. It declares source IDs, intended asset names,
input shapes/key mappings and observable states for the future fixture setup.

Use a new folder such as /Game/DasGameplayDemo in the existing isolated smoke project.
After compiling the plugin, import GameplayActor through the existing DSL service;
its declared component source is handled by the existing component import workflow.
Confirm both generated classes and SCS Behavior before placing an actor on a new map.

Configure EnhancedPlayerInput/EnhancedInputComponent using the test project's input
setup, create IA_Jump (Boolean), IA_Move (Axis2D) and IMC_Gameplay. Map Space to Jump;
D/A to positive/negative X, W/S to positive/negative Y using normal Enhanced Input
negate/swizzle modifiers. No such assets or project setting changes were performed.

Assign Player explicitly to the local PlayerController of that actor's world, and
assign Controls/JumpAction/MoveAction assets. Enable input/finish component readiness
then call SetupControls from Blueprint. Existing external contexts are queried and
used without taking ownership. Ensure a usable view target/camera separately.

Optional AssetPath must identify a cook-included asset. A string alone does not
ensure packaging. Configure a real soft/primary asset reference or cook rule in the
isolated project. Keep asset selection out of unrelated user projects.

## Reload behavior

Current input/context recipes restore after accepted body reload/component change.
Editing SetupControls does not replay it; invoke the revised setup explicitly.
Accepted component logic reload cancels the old score timer; ResumeScore explicitly
restarts it. Latent/async work is cancelled at accepted Actor reload; StartLoad can
restart the asset operation. Failed compilation keeps the old live context/records.
Actor owns the existing pivot across ordinary logic reload; BeginPlay is not replayed.
Structural reinstancing/creation tracking and broad migration remain outside this demo.

## Pending verification package

First perform one build of accumulated code, without repeatedly testing each source
change. Then import/setup this isolated scene and check the coupled behavior:
- catalog/forward-source resolution and actual SCS component identity;
- score timer, signal delivery, owner property writes and cross-context calls;
- bool/Axis2D input, pivot transform and native ownership;
- delayed Ready and nullable async asset result;
- accepted/rejected reload and input restoration versus explicit task restart;
- EndPlay/Destroy/GC/world cleanup with no remaining callbacks or context leases;
- save/reopen, actual PIE, and packaged Development with required cooked assets.

These are future checks, not observed passes. Test timing should use world updates,
not fragile wall-clock sleeps; distinguish user input from replayed/native broadcast
fixtures. No existing projects/assets should be overwritten for setup.
