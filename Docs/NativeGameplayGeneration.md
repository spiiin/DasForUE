# Generated native gameplay members

Current systematic coverage and exclusions: [API audit](ApiCoverage.md).
Subsystem package roadmap: [expansion plan](ApiExpansionPlan.md).

Native receiver classes are discovered from DAS declarations and their imports;
there is no C++ receiver class list. See [automatic discovery](NativeGameplayDiscovery.md).
Their supported inherited fields/functions are included. Native
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
The command also builds the updated member registry, unless `-SkipBuild` or
`-Check` is specified.

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
Native classes are selected by `[ue_native_object]` declarations reachable from
the configured DAS entry modules. Regenerate after adding/importing a declaration;
no C++ registration is required.

## Contracts and boundaries

Supported types currently include int32, raw uint8 (bounded DAS int), float,
double, bool/bitfields, FString, FName, FText, UObject and class references,
validated native enum/struct values, typed soft references and supported arrays,
maps and sets. The exact recursive policy is shared with the coverage audit in
DasNativeGameplayTypes.h. Enum values must be unique and within 0..254 (excluding
_MAX). Containers restrict element/key/value kinds; structs restrict their
members. Weak/interface references, fixed arrays and additional numeric/opaque
types still have explicit limitations. These are not all-UE type guarantees.
Detailed rejected shapes are in Build/ApiCoverage/report.json and members.csv.

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
normal/error paths. Only trailing supported scalar literals and native enum constants become source
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
and a free camera function. Sixteen invalid-source fixtures reject readonly/hidden/
missing-method/argument/RPC access, nested snapshot writes, enum identity/integer
mismatches, byte/text argument typing, readonly text, downcasts, unrelated references,
mutable ref upcasts and ambiguous overloads; a runtime fixture rejects a wrong receiver.

Validated 2026-10-17: reproducible -Check, eleven generator tests, strict PIE,
Win64 Development packaged foundation/gameplay and a separate saved-map launch.
See Docs/Plan.md for exact scope and logs. Tests use NullRHI and synthetic input;
rendering, physical devices, Shipping and networking are not validated.

Native value-struct, array and map nested writes use the same owned root-snapshot
mutation helper as authored properties and publish through the checked native
setter. Readonly roots such as `movement.GravityDirection.x` remain rejected.
Native math root properties still use whole-value writes; direct nested math-root
writes such as `movement.Velocity.x=1.0lf` remain rejected.

## Native enum and reference upcasts

Reflection manifest schema 3 includes native enum paths and explicit numeric
layouts. The generator emits `[ue_native_enum]` declarations, reusing canonical
imported enum declarations when available. Current selection: 35 enums,
323 fields, 606 methods. Generated names are the native UEnum names.

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
layouts remain skipped. Trailing native enum defaults now use typed daScript enum constants; raw uint8
inputs/results and fields use checked integer adapters. SetMovementMode is
generated, including its NewCustomMode=0 default. Other unsupported default
expressions remain explicit arguments. See [NativeGameplayValues.md](NativeGameplayValues.md).

Enum/upcast verification: native camera enum input/property read/write, movement
enum input/result and property roundtrip; Character-to-Pawn native Possess call,
free function, local initialization/assignment, function return, null propagation,
single evaluation, closest-parent and exact-overload selection. Thirteen negative
source fixtures run during fixture preparation. Strict RuntimePIE reports exactly
1 success, 0 warnings/failures/notRun. Win64 Development build/cook/stage,
packaged foundation (0 failures), gameplay with the same enum/upcast probe and
separate PlayableMap launch pass. Logs: native-enum-upcast-{check,pie,cooked}.log
and native-enum-upcast-pie-report.json. NullRHI/synthetic input only.


Native value extension validated 2026-10-17: 13 generator tests, reproducible
schema-3 export/generation -Check, strict PIE (1 pass, no warnings), Win64
Development build/cook/stage, packaged foundation (0 failures), gameplay and
separate playable-map launch. This adds uint8 endpoints/default/range checks,
typed enum defaults, text identity/flags and real Kismet text out/return tests.
See NativeGameplayValues.md and the native-byte-text-* logs.


## Native struct and array generation

Manifest schema 4 adds recursive array element descriptors and validated native struct definitions. Current export: 337 fields, 643 methods, 36 enums and 8 value structs across the five existing receiver roots. See [NativeGameplayContainers.md](NativeGameplayContainers.md) for syntax, snapshot semantics, supported elements and limits.


## Native hash containers (schema 5)

Native map/set fields and methods now use recursive key/value/element descriptors and shared typed snapshot adapters. The original five Engine receivers retain 337 fields/643 methods; the additional explicitly marked ABI fixture contributes 15 fields/27 methods. Combined registry: 352 fields, 670 methods, 37 enums, 9 structs. The fixture is not an authored gameplay root. See [NativeGameplayHashContainers.md](NativeGameplayHashContainers.md) for supported forms and executable examples.

See [generated native soft references](NativeGameplaySoftReferences.md) for typed soft fields, parameters and containers.

## Mesh and collision receivers — 2026-10-17

StaticMeshComponent and SphereComponent are now selected native receivers.
Canonical StaticMeshRef, StaticMeshComponentRef and SphereComponentRef come from
`ue_pickup_components.das`. Supported inherited primitive/scene component methods
share the existing generated frame/property bridge. This does not make these
classes authored class parents or expose unsupported physics signatures.
The [pickup example](PlayablePickup.md) uses typed mesh, collision profile/radius,
overlap events, visibility and timer restoration without manual call frames.
Targeted validation: 31 generator tests, exact artifact reproduction, Editor
Development and the saved-map pickup game probe; no full Foundation/PIE/package rerun.

## Native accessors и construction guard — 2026-10-18

[Общий object-property слой](NativeAccessAndConstruction.md) поддерживает native
FProperty Getter/Setter и BlueprintGetter/Setter, сохраняет readonly и проверяет
значение до вызова setter. Используются существующие value descriptors/carriers
и GC roots; bitfield bool ABI нормализуется отдельно. Контракты сериализованы
в native manifest/header; FProperty/UFunction pointers не кэшируются между reload.
UnsafeDuringActorConstruction больше не исключает методы целиком: native registry
и generic wrappers сохраняют guard, runtime проверяет фазу до dispatch.

160 Python-тестов, native artifacts check, Game/Editor ConstructionScript,
metadata-removed guards, getter/setter semantics, readonly/stale/type/infinity,
GC в setter и настоящий Engine accessor example — PASS. World пакет повторно
проверен: 652 wrappers, 88 guarded, blockers 246 export + 6 generation; Math 619.
Без полного Foundation/PIE/packaged прогона и без собственного коммита.
Далее — Collision/Physics/type blockers; унификация object/enum/UEName views
между пакетами остаётся отдельной задачей.
