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
a fresh generated folder under `/Game/DasCookedGameplay/<id>` with the Character
Blueprint, saved InputActions, Mapping Context and `PlayableMap`. Existing generated
test folders are not removed. Open the printed PlayableMap and press Play:

- WASD: movement relative to view yaw.
- Mouse: view rotation.
- Space: jump / release.

The map has a floor and several cube obstacles. The Character auto-possesses
Player 0 and uses its camera. No asset paths are needed in user callback bodies.
The smoke project config already selects EnhancedPlayerInput/EnhancedInputComponent.
For another project configure these classes and assign the four input assets.

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
`/Game/DasCookedGameplay/94e91ee1d4414c4b8b5211844f8801f5/PlayableMap`.
This is generated test content; rerunning the scripts creates another folder.


Intentional negative-speed rejection lives in the test-only
`playable_character_negative_probe.das`, not in the playable Character API.
