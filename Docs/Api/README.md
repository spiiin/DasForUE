# Generated API Packages

API packages are daScript modules generated from Unreal reflection for whole engine libraries and receiver classes. A profile selects the classes, the exporter lists their reflected API, and the generator builds typed wrappers with all type dependencies, then compiles and probes the whole module before installing it. Wrappers use the existing `ProcessEvent` call frames, signature checks and GC rules; there is no hand-written C++ wrapper per function.

## Packages

| Module | Profile (`Bindings/Packages/`) | Contents | State | Page |
| --- | --- | --- | --- | --- |
| `ue_api_math` | `CoreMath.profile.json` | KismetMathLibrary | Installed | [Math](Math.md) |
| `ue_api_world` | `WorldActorComponents.profile.json` | World, Actor, components, Controller, GameplayStatics, KismetSystemLibrary | Installed | [World / Actor / Components](WorldActorComponents.md) |
| `ue_api_collision` | `CollisionPhysics.profile.json` | Traces, sweeps, overlaps, HitResult, physics bodies, constraints, PhysicsHandle | Installed | [Collision / Physics](CollisionPhysics.md) |
| `ue_api_umg_input` | `UMGInput.profile.json` | UMG widgets and libraries, input keys, Enhanced Input | Installed | [UMG / Input](UMGInput.md) |
| `ue_api_pawn_ai` | `PawnCharacterAINavigation.profile.json` | Pawn, Character, controllers, movement, AIController, perception, navigation | Generate with `-Publish` | [Pawn / Character / AI / Navigation](PawnCharacterAINavigation.md) |
| `ue_api_animation_audio_vfx` | `AnimationAudioVFX.profile.json` | AnimInstance, montages, skeletal mesh, audio, Niagara | Generate with `-Publish` | [Animation / Audio / Niagara](AnimationAudioNiagara.md) |

Installed modules live in `Plugins/DasForUE/Content/Das/Scripts/` and are used with an ordinary `require`. The two staged packages compile and pass their probes but are not installed by default; publish them yourself (below) before requiring them.

Each package publicly re-exports `ue_foundation`, so `ue_vector`, `ue_rotator` and the canonical math types are available after one `require`. Packages are independent: `ue_api_collision` does not need World or Math.

```das
options gen2
require ue_api_world

[export]
def main(input:int):int {
    let actor = C_Actor_adf65d5b_from(ue_self_object())
    ue_actor_SetActorScale3D(actor,ue_vector(2.0lf,3.0lf,4.0lf))
    let scale : UEVector = ue_actor_GetActorScale3D(actor)
    return int(scale.z)
}
```

## Naming and calling conventions

- Wrapper name = profile prefix + reflected function name: `ue_actor_SetActorScale3D`, `ue_math_Add_VectorVector`, `ue_collision_LineTraceSingle`.
- Receiver methods take the typed receiver first (`target`). Parameters are named `arg_<lowercase name>`.
- Object types are views `C_<Class>_<hash>` with `_from`, `_raw`, `_null`, `_valid` and `_as_<Ancestor>` helpers; short typedefs (`Actor`, `Character`, …) are also emitted. Enums are `E_<Name>_<hash>` with `v_<Symbol>` members; other structs are `S_<Name>_<hash>` with `f_<field>` fields. See [Generated bindings](../Reference/GeneratedBindings.md).
- `value_types=canonical_math`: Vector2D/Vector/Rotator/Transform use `UEVector2D`/`UEVector`/`UERotator`/`UETransform`, shared with native class bindings and authored code.
- `default_arguments=explicit`: every argument must be passed, including those with C++ defaults. The original default texts are listed in the package report.
- Several outputs return `<wrapper>_Result` with `out_<name>` fields (`out_returnvalue` for the return value). Results containing arrays are moved: `let hits <- ...`.
- `WorldContext` parameters are hidden and filled from the script owner's live game world. Calling without a game world fails, and the automatic argument cannot be overridden.
- Construction-unsafe functions keep a guard: they work in gameplay and are refused in a ConstructionScript before dispatch.
- Receivers are checked on every call: null, wrong class and destroyed receivers are rejected.

### Importing several packages

Object views, `UEName` and some enums are generated per package and are not yet unified across packages. When importing more than one package, qualify ambiguous names with the module:

```das
require ue_api_collision
require ue_api_world
require ue_api_math

let actor=ue_api_world::C_Actor_adf65d5b_from(ue_self_object())
let scale=ue_api_world::ue_actor_GetActorScale3D(actor)
```

Convert between packages through `UObjectHandle` (`_raw` then `_from`); this is a checked conversion, not a new object. A copy of a package module in the project's `Content/Das/Scripts` takes precedence over the plugin copy.

## Generating and publishing

```powershell
.\Scripts\GenerateApiPackage.ps1 -SkipBuild                       # default profile (Math), staged only
.\Scripts\GenerateApiPackage.ps1 -Profile .\Bindings\Packages\PawnCharacterAINavigation.profile.json -SkipBuild -Publish
```

Without `-SkipBuild` the export commandlet is built first. Without `-Publish` the result stays in a unique folder under `Build/ApiPackages/<module>/`. Before publishing, the whole module is compiled under a unique content-hashed name (so an installed old version cannot mask it) and the profile's `[export] main(int):int` probe must return 42; some profiles also run a world probe. Only then is the module installed. A failed check leaves the installed module untouched; see the run's `preflight.log`.

Each run writes `report.json` (diagnostics and default-argument notes) and `latest-report.json`; the compact published snapshot is `Bindings/Packages/<module>.summary.json`. After installing a new module, rebuild so the file is included in the plugin's staged scripts.

Package-specific test runners such as `Scripts/TestWorldApiPackage.ps1 -SkipBuild -Publish` or `Scripts/TestCollisionApiPackage.ps1 -SkipBuild -Publish` regenerate, probe and publish in one step.

## Profiles

```json
{
  "schema_version": 1,
  "module": "ue_api_world",
  "ue_modules": ["Engine"],
  "value_types": "canonical_math",
  "default_arguments": "explicit",
  "libraries": [ {"class": "/Script/Engine.GameplayStatics", "prefix": "ue_gameplay_", "include": ["*"]} ],
  "receivers": [ {"class": "/Script/Engine.Actor", "prefix": "ue_actor_", "include": ["*"]} ],
  "exclusions": [],
  "probe_world": true,
  "probe": "[export]\ndef main(input:int):int { ... return 42 }"
}
```

- `ue_modules` are loaded before export.
- `libraries` select static functions; `receivers` select instance methods and native static `BlueprintCallable` methods of any loaded native class (called through its CDO). Both use `class`, `prefix`, `include`/`exclude` wildcards and optional `include_inherited`. A class may appear in only one surface.
- `include: ["*"]` selects the class's own declarations; inherited copies are not duplicated unless `include_inherited` is set.
- `exclusions` list functions deliberately left out; each needs a `reason` (and usually an `alternative`).
- If a selected library disappears, or two wrappers collide, the package fails.

Every candidate ends in one state: `generation_ready`, `export_blocked`, `generation_blocked` or `policy_excluded`. Blocked candidates keep their reasons in the summary; nothing is silently counted as working. "Ready" means the wrapper was generated and the whole module compiled, not that every function was executed. Common reasons: unsupported signature, internal/protected/deprecated, latent or CustomThunk, RPC, delegate signatures, soft references without an adapter, opaque structs.

Coverage of all packages and the native registry: [API coverage audit](../Dev/Audits/ApiCoverage.md); named blockers: [Dev/Api/Blockers](../Dev/Api/Blockers/).

## Packages vs. native class bindings

Packages expose functions only. Fields, `self` member syntax and native getters/setters come from [native class bindings](../Reference/NativeClassBindings.md) (`ue_native_gameplay`); generating a package does not change that registry. Both layers share the call frames, construction guards and canonical math types, so they can be used together.

See also: [Generated bindings](../Reference/GeneratedBindings.md) · [Native class bindings](../Reference/NativeClassBindings.md) · [Type support](../Reference/TypeSupport.md) · [Limitations](../Reference/Limitations.md)
