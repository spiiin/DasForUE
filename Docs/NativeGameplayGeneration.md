# Generated native gameplay members

Selected native classes: Character, CharacterMovementComponent, PlayerController,
CameraComponent. Their supported inherited fields/functions are included. Native
DSL roots and native object reference types remain distinct concepts: this does
not make PlayerController/CameraComponent supported authored class parents.

```das
require ue_native_gameplay
let character=ue_character_self()
let movement=character.CharacterMovement
movement.MaxWalkSpeed=450.0f
let maxSpeed=movement.GetMaxSpeed()
character.Jump()
character.StopJumping()
```

The same syntax works in free functions, authored methods and lifecycle callbacks.
For a DSL Character, inherited native calls also support `self.Jump()` and
`self.AddControllerYawInput(value)`. Authored methods take precedence over native
members. `dsl_playable_character.das` uses these calls in its input handlers.
`native_gameplay_reference_probe.das` demonstrates camera methods/fields,
controller rotation, typed object results, math, omitted scalar defaults and out
parameters (`player.GetPlayerViewPoint(location,view)`).

## Generation pipeline

`Scripts/GenerateNativeGameplay.ps1` builds the exporter, runs DasNativeGameplay,
and stages the reflection manifest, generated C++ registry and dependency views.
`-Check` regenerates and compares all three artifacts without replacing them.
The next C++ build compiles an updated member registry into the runtime.

- `Bindings/NativeGameplay.json`: deterministic audit of selected/supported and
  skipped members with reasons, declaring class and marshaling shape signatures.
- `DasNativeGameplay.generated.h`: compiled field/method/default/canonical-view
  registries, shared by compiler and runtime. No runtime JSON files are loaded.
- `ue_native_gameplay.das`: imports the existing canonical reference wrappers and
  adds checked nominal dependency views for other returned/parameter classes.
  Those dependency views do not automatically expose all of their own members.
- `FUEPropertyPass`: synthesizes receiver-first methods and property operators
  when used by source, sharing the existing authored property/method AST emitters.
  Native methods use the existing call frame, shape validation and ProcessEvent.

No C++ source parser, Unreal memory-layout copying or vtable replacement is used.
Generated native members do not become authored properties or authored IR methods.
Native classes are selected explicitly in DasNativeGameplayClasses; add a class
there and regenerate to extend this registry.

## Contracts and boundaries

Supported types: int32, float, double, bool/bitfields, FString, FName (UENameValue),
strong UObject references, UClass handles, Vector2D, Vector, Rotator and Transform.
Unsupported enum/byte, text, delegate, container, custom struct and weak/soft fields
or function signatures are reported as skipped. Existing authored/native-self
adapters for those types remain available; they are not generalized by this slice.

Fields require BlueprintVisible and preserve BlueprintReadOnly. Hidden, editor,
deprecated and accessor-backed fields are excluded. Accessor functions can be
generated separately when eligible. This policy is baked from editor metadata
so cooked builds cannot silently bypass BlueprintGetter/BlueprintSetter metadata.

Methods require public native BlueprintCallable instance functions. Static, net,
delegate, editor-only, deprecated, protected, internal, custom thunk, latent,
WorldContext and construction-unsafe methods are excluded with reasons. Arbitrary
C++ methods and virtual overrides remain separate work. Reflected names are used
(e.g. K2_GetPawn); Blueprint display names are not renamed into source identifiers.

Inputs, return values and native mutable out/ref parameters share the frame ABI.
Output values are read before publishing ref/out changes; frames are released on
normal/error paths. Only trailing supported scalar literal defaults become source
defaults. Other parameters must be supplied explicitly; metadata expressions are
not evaluated as source code. Object parameters use their declared nominal view;
use an explicit checked upcast for a different native reference type.

Every access checks live owner/world context and expected receiver/member shape.
Mutations/method calls are rejected in construction, bind_events and EndPlay.
Field/function pointers are resolved per access; no pointer survives reinstancing.
Authority-only function flags are enforced. Numeric property writes reject NaN/Inf;
the existing nonnegative policy for movement speed/jump velocity is retained.
Returned references remain checked and weak. Missing/stale/unselected members fail
explicitly, rather than becoming raw reflection calls.

## Verification

`Scripts/TestNativeGameplayGenerator.py` checks determinism, canonical view reuse,
invalid manifests, duplicate classes/members and unsupported kinds.
The shared RuntimePIE/cooked gameplay probe exercises the same playable Character
with native self methods, fields, object results, math, scalar defaults, out params
and a free camera function. Six invalid-source fixtures reject readonly/hidden/
missing-method/argument/RPC access and nested snapshot writes; a runtime fixture rejects a wrong receiver.

Validated 2026-10-17: reproducible -Check, six generator tests, strict PIE,
Win64 Development packaged foundation/gameplay and a separate saved-map launch.
See Docs/Plan.md for exact scope and logs. Tests use NullRHI and synthetic input;
rendering, physical devices, Shipping and networking are not validated.

Native math property reads return snapshots. Replace the entire property value
to write it; nested writes such as `movement.Velocity.x=1.0lf` are rejected.
