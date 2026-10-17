# Pawn and Character DSL parents

Supported native roots are Actor, Pawn, Character and ActorComponent. Both parent
spellings select the same native host and use the same source type resolver:

```das
options gen2
require ue_dsl

[ue_class(Blueprintable)]
class Player : Character {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=7

    defaults=ue_defaults() {
        self.BaseEyeHeight=55.0f
        self.bUseControllerRotationYaw=true
        self.CharacterMovement.MaxWalkSpeed=450.0f
    }

    def BeginPlay():void { self.Score+=3 }

    [ue_function(BlueprintPure,BlueprintEvent)]
    def const Query(value:int=2):int { return self.Score+value }
}
```

The alternative declaration is `[ue_class(parent="Character", Blueprintable)]`
with `class Player { ... }`. Explicit `/Script/Engine.Character` paths also work;
they are not required. `parent="Pawn"` and `class Player : Pawn` are equivalent.
Do not combine a `parent` option and class inheritance in the same declaration.

Authored inheritance keeps ordinary daScript syntax:

```das
[ue_class]
class SpecialPlayer : Player {
    def override BeginPlay():void {
        Super.BeginPlay()
        self.Score+=10
    }

    [ue_function(BlueprintPure,BlueprintEvent)]
    def override const Query(value:int=2):int {
        return Super.Query(value)+5
    }
}
```

## Native implementation

Generated Blueprints derive from ADasPawn : APawn or ADasCharacter : ACharacter.
They are real Unreal Pawns/Characters, with native collision, movement and
component ownership. A small native adapter supplies the script bridge; it does
not replace the selected engine class with the experimental Actor host.

Actor/Pawn/Character share one lifecycle and frame-dispatch implementation.
Initialize, ConstructionScript, BeginPlay, Tick and EndPlay use the existing
callback contracts, including authored parent lifecycle calls. Native Super
callbacks still execute. Without DAS lifecycle callbacks, Pawn/Character preserve the native tick state
selected by the class defaults.
EndPlay releases persistent contexts, including classes that only expose methods.

Pawn/Character use the full frame ABI for all reflected methods. Supported value
types, const/pure/event methods, defaults, out/ref and typed authored-reference
calls reuse the existing implementations. Native inherited fields such as
BaseEyeHeight use the checked native-self accessors. Source defaults resolve the
correct native CDO for each root, including short explicit parent names.

Character keeps its native capsule as root, along with its native skeletal mesh
and CharacterMovement component. Replacing that root through an authored root
component is rejected. Editable native movement/subobject defaults follow the
existing default-subobject identity rules. Native object-valued fields still
require the corresponding imported typed wrappers when used in method bodies.

## Limits

- Arbitrary native parents are not automatically supported. They require their
  own native adapter/generation strategy; unsupported roots fail explicitly.
- This does not expose arbitrary C++ virtual overrides. Existing reflected API
  bindings and explicit adapters remain the route for native function calls.
- Structural/data migration of existing Pawn/Character definitions is deferred;
  creating a new Blueprint and unchanged/body-only reimport are supported.
  Changing an existing Blueprint's native parent is rejected.
- Possession, input bindings, movement/jump gameplay and replication are separate
  integration steps. This stage validates parent/lifecycle/interop behavior, not
  a playable Character controller or a networked game.

## Examples and validation

- `dsl_pawn_player.das`, `dsl_character_player.das`: authored fields, native and
  movement defaults, lifecycle, pure virtual methods and out/ref.
- Corresponding `_child.das` files: inherited lifecycle and Super calls.
- Corresponding `_calls.das` files: typed weak references, property access,
  omitted/default arguments and ref/out calls from free DAS functions.
- `dsl_pawn_explicit_parent.das`: short/full explicit native parents without DAS
  lifecycle callbacks, including native defaults with a short parent name.

Validated 2026-10-17: Editor build and strict RuntimePIE, Win64 Development
build/cook/stage/Pak/archive and packaged gameplay, packaged foundation failures=0.
The same native probe runs in PIE and the separate packaged process. It checks
native class ancestry, Initialize/ConstructionScript/BeginPlay effects, Tick/EndPlay,
inherited Super and virtual dispatch, Blueprint consumers, typed references,
live native fields, ref/out, capsule/mesh/movement defaults, explicit parents,
native tick without script callbacks and context cleanup. Unsupported/conflicting
parents and capsule replacement are rejected before creating the fixtures.

Run `Scripts/TestGameplayRuntimePIE.ps1` and `Scripts/TestCookedGameplay.ps1`.
Logs: `pawn-character-pie-build.log`, `pawn-character-pie.log`,
`pawn-character-pie-report.json`, `pawn-character-cooked.log`,
`cooked-gameplay-runtime.log`, `smoke-packaged.log`. Game runs used NullRHI;
Shipping and rendered gameplay were not exercised.


Playable possession/input/movement integration was added subsequently; see PlayableCharacter.md for the authored camera, input assets, generated map and validated runtime coverage.
