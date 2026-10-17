# Playable Character and Enhanced Input

`dsl_playable_character.das` is a first-person Character example using the real
ACharacter host. `ue_character_control.das` adds checked typed gameplay calls:

```das
let pawn=ue_character_self()
ue_character_move(pawn,float(value.y),float(value.x))
ue_character_look(pawn,float(value.x),float(value.y))
ue_character_jump(pawn,true)
ue_pawn_possess(player,pawn)
self.CharacterMovement.MaxWalkSpeed=450.0f
```

Movement uses controller yaw (forward/right). Look calls Unreal's controller input
functions, respecting native input scaling/ignore settings. Jump uses Jump and
StopJumping; native CharacterMovement performs movement, collision and gravity.
Possession takes an explicit PlayerController and Pawn/Character; it requires
authority. Input helpers require a locally controlled Character. Object operations
validate live game-world identity and reject cross-world, template, destroyed,
construction, bind_events and EndPlay mutations. They do not expose raw pointers.

CharacterRef, CharacterMovementRef, CapsuleComponentRef, SkeletalMeshComponentRef
and CameraComponentRef are checked weak native views. Native reflected properties
reuse the existing property/method operator generators. Native gameplay generation
now exposes supported reflected members of Character, CharacterMovementComponent,
PlayerController and CameraComponent. The same syntax works in ordinary functions
and authored methods; the input example uses self.Jump()/StopJumping() and native
controller-input calls. See NativeGameplayGeneration.md for generation and limits.
This does not add animation, a skeletal mesh asset or arbitrary native methods.

The example uses grouped property declarations and an authored Camera component.
Its Tick waits for a local controller, EnhancedPlayerInput and the possessed Pawn's
EnhancedInputComponent before installing its context/handlers. It releases its
owned input state when local control is lost, then can bind after repossession.
Managed runtime teardown removes owned handlers/context on EndPlay. The sample's
session owns all of its input recipes; do not share that session with unrelated
bindings if reusing its unbind-all approach.

The example sets movement properties during BeginPlay: mixed native subobject CDO
defaults and SCS composition remain conservatively refused by the existing defaults
policy. Camera defaults use the established SCS component defaults path.
State modified by DAS currently uses BlueprintReadWrite (BlueprintReadOnly is also
enforced by DAS accessors). Each grouped block needs a unique placeholder name.

## Create/open the demo

Run `Scripts/CreatePlayableCharacter.ps1`. It builds the Editor target and creates
a fresh generated folder under `/Game/DasExamples/Playable/<id>` with the Character
Blueprint, pickup Blueprint, three saved InputActions, Mapping Context, a separately streamed pickup mesh and `PlayableMap`, plus the
Character's gameplay dependencies.
Use `-SkipBuild` when the Editor target is already built. The standalone export
does not create the regression fixture Blueprints. Existing generated folders are
preserved. The printed map/source paths are also saved in `logs/playable-example.json`.
Open the printed PlayableMap and press Play:

- WASD: movement relative to view yaw.
- Mouse: view rotation.
- Space: jump / release.
- E: interact with the object under the camera trace.
- H: receive 25 damage.
- Tab: toggle mouse/UI interaction.

The map has a floor and several cube obstacles. The Character auto-possesses
Player 0 and uses its camera. No asset paths are needed in user callback bodies.
The smoke project config already selects EnhancedPlayerInput/EnhancedInputComponent.
For another project configure these classes and assign the mapping context, six input actions and HUD class.

`Scripts/TestGameplayRuntimePIE.ps1` and `Scripts/TestCookedGameplay.ps1` share a
native probe of this exact authored Character. It verifies possession through DAS,
context installation, Axis2D dispatch, keyboard W mapping, native movement, look/
camera rotation, jump Started/Completed, typed native movement fields and EndPlay
cleanup. Tests inject input into EnhancedPlayerInput; they do not use physical
keyboard/mouse or visually inspect rendered output. The packaged script also cooks
PlayableMap and all of its assets. Multiplayer, replication, Shipping and general
possession lifecycle hooks remain separate work.

Validated 2026-10-17 in strict RuntimePIE and Win64 Development packaged gameplay,
including a separate launch of the saved map (auto-possession, movement on floor,
jump, camera, no runtime error). Packaged foundation failures=0. See Docs/Plan.md
for logs and limits.

Current generated map:
`/Game/DasExamples/Playable/f11090b4a75e4fe79c1836d70c429fc4/PlayableMap`.
This is generated test content; rerunning the scripts creates another folder.


Intentional negative-speed rejection lives in the test-only
`playable_character_negative_probe.das`, not in the playable Character API.

Standalone export verification (2026-10-17): Editor Development, exactly six saved
assets and a fresh Editor `-game -DasPlayableMap` launch passed. The focused probe
checked input readiness, movement on the floor, jump, camera and empty DAS error.
This iteration did not rerun Foundation, the PIE suite or packaged Development.
See `logs/playable-example-validation.json`. The full cooked regression exporter
still uses its existing `/Game/DasCookedGameplay/<id>` path.

The standalone scene now includes a [respawning score pickup](PlayablePickup.md).
Its mesh/collision/event/score/timer logic lives in `dsl_playable_pickup.das`.
The focused `TestPickup.ps1` runner validates the saved scene without running the
Foundation/PIE/packaged suites. Earlier six-asset validation above predates this addition.

Traces, UMG HUD, native damage and HealthComponent: [GameplayServices](GameplayServices.md).
