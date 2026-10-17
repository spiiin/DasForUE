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
strong UObject references, UClass handles, native enum, Vector2D, Vector, Rotator and Transform.
Unsupported raw byte, text, delegate, container, custom struct and weak/soft fields
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
not evaluated as source code. Object value inputs accept automatic checked upcasts to a native parent view.
Mutable out/ref arguments require the exact view type.

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
and a free camera function. Thirteen invalid-source fixtures reject readonly/hidden/
missing-method/argument/RPC access, nested snapshot writes, enum identity/integer
mismatches, downcasts, unrelated references, mutable ref upcasts and ambiguous overloads; a runtime fixture rejects a wrong receiver.

Validated 2026-10-17: reproducible -Check, eleven generator tests, strict PIE,
Win64 Development packaged foundation/gameplay and a separate saved-map launch.
See Docs/Plan.md for exact scope and logs. Tests use NullRHI and synthetic input;
rendering, physical devices, Shipping and networking are not validated.

Native math property reads return snapshots. Replace the entire property value
to write it; nested writes such as `movement.Velocity.x=1.0lf` are rejected.

## Native enum and reference upcasts

Reflection manifest schema 2 includes native enum paths and explicit numeric
layouts. The generator emits `[ue_native_enum]` declarations, reusing canonical
imported enum declarations when available. Current selection: 21 enums,
232 fields, 442 methods. Generated names are the native UEnum names.

```das
require ue_native_gameplay
let character=ue_character_self()
let player=PlayerControllerRef_from(character.GetController().handle)
player.Possess(character) // CharacterRef -> NativePawnRef

let camera=ue_component_get(type<CameraComponentRef>, ue_ref(type<ActorRef>,character.handle), "Camera")
camera.SetProjectionMode(ECameraProjectionMode.Orthographic)
camera.ProjectionMode=ECameraProjectionMode.Perspective
let axis=character.CharacterMovement.GetPlaneConstraintAxisSetting()
```

`ue_component_get` is a separate macro with its own argument contract; use an
explicit ActorRef there when required. Automatic conversions apply to concrete
ordinary function/method value arguments, explicitly typed local initialization,
copy/move assignment and function returns. They use loaded UClass ancestry and
the existing checked source pack/target factory, without copying UObject state.
Authored references can also convert to their native host's ancestor views.
Null remains null. Downcasts, unrelated types and interfaces do not convert
automatically. Mutable ref/out references are invariant: a function accepting
`var NativePawnRef` cannot replace a caller's `CharacterRef` with an arbitrary Pawn.
Containers do not become covariant. Generic signatures and lambda/block return
type inference are left to daScript.

Ordinary exact overloads take precedence. When no exact overload resolves, the
compiler selects a unique concrete overload with the shortest summed native
ancestry distance; ties remain unresolved. Conversion evaluates each input once.

Enum reads/writes, inputs and return values use the existing typed enum carrier
and identity/number checks. An integer or a different enum is rejected by source
typing. Current shared enum ABI supports unique values 0..254, excludes `_MAX`
sentinels and rejects invalid values at runtime; larger/signed/aliased enum
layouts remain skipped. Enum defaults remain explicit arguments, as do other
defaults unsupported by the scalar literal adapter. Methods with raw byte or
other unsupported parameters remain skipped even when their other parameters
are enums (for example SetMovementMode with a raw uint8 custom-mode argument).

Enum/upcast verification: native camera enum input/property read/write, movement
enum input/result and property roundtrip; Character-to-Pawn native Possess call,
free function, local initialization/assignment, function return, null propagation,
single evaluation, closest-parent and exact-overload selection. Thirteen negative
source fixtures run during fixture preparation. Strict RuntimePIE reports exactly
1 success, 0 warnings/failures/notRun. Win64 Development build/cook/stage,
packaged foundation (0 failures), gameplay with the same enum/upcast probe and
separate PlayableMap launch pass. Logs: native-enum-upcast-{check,pie,cooked}.log
and native-enum-upcast-pie-report.json. NullRHI/synthetic input only.
