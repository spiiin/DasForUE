# Native Class Bindings (ue_native_gameplay)

`require ue_native_gameplay` lets scripts use reflected fields and methods of native Unreal classes with ordinary member syntax on typed references and on `self`. The set of classes is discovered from `[ue_native_object]` declarations in your DAS sources; there is no C++ class list to edit.

```das
require ue_native_gameplay
let character=ue_character_self()
let movement=character.CharacterMovement
movement.MaxWalkSpeed=450.0f
let maxSpeed=movement.GetMaxSpeed()
character.Jump()
character.StopJumping()
```

The same syntax works in free functions, authored methods and lifecycle callbacks. In a DSL `Character` class, inherited native members are available through `self` (`self.Jump()`, `self.AddControllerYawInput(value)`); authored methods take precedence over native members of the same name. See `Plugins/DasForUE/Content/Das/Scripts/dsl_playable_character.das` and `native_gameplay_reference_probe.das` (camera fields, controller rotation, typed object results, omitted defaults and out parameters such as `player.GetPlayerViewPoint(location,view)`).

Native receiver types are reference types, not authored class parents: exposing `PlayerController` members does not make it a supported `[ue_class]` base.

## Adding a native class

Declare a reference wrapper in an independent module:

```das
options gen2
module game_native_types public
require ue_object_types public

[ue_native_object(path="/Script/Engine.PointLightComponent")]
struct PointLightRef { handle:UObjectHandle }

```

Import it together with `ue_native_gameplay` in gameplay code:

```das
require game_native_types
require ue_native_gameplay

def set_brightness(light:PointLightRef):void {
    light.SetIntensity(1500.0f)
}
```

Then regenerate:

```powershell
.\Scripts\GenerateNativeGameplay.ps1                                   # default sources and their imports
.\Scripts\GenerateNativeGameplay.ps1 -Entry my_game -ScriptDirectory C:\mygame\Das
.\Scripts\GenerateNativeGameplay.ps1 -Check -SkipBuild                 # verify stored artifacts only
```

Without `-SkipBuild` the command builds the exporter, discovers types, exports reflection, generates files and rebuilds the C++ member registry. With `-SkipBuild` you must build before using new members. Generation is an explicit command; it does not run when a DAS file is saved. Every discovered class exports its whole supported API, including inherited members.

Rules for discovery:

- `Bindings/NativeGameplay.sources.json` lists entry modules, search directories and external modules (override with `-Sources`). It configures sources, not classes; newly imported modules are found automatically. Modules map to `<module>.das` files.
- A lexical pre-pass reads `require` lines and literal native annotations (skipping comments and strings), so it works before generated members exist. Missing imports, ambiguous files, duplicate type paths and invalid paths are errors.
- `ue_native_gameplay.das` is generated output and never a source of new classes. The module declaring canonical wrappers must compile without `ue_native_gameplay` (cycles are rejected); put code that calls generated members in a gameplay module.
- Classes must be loadable native `/Script/` classes. Unknown modules/classes, Blueprint classes, interfaces declared as objects and duplicates fail without writing a partial manifest. Plugins must be enabled in the project; generation does not edit `Build.cs` or plugin settings.
- Parameter and return classes get *dependency views* (checked typed references). Their own members are exported only when their declarations are in the discovered sources.

Artifacts: `Bindings/NativeGameplay.json` (build-time audit of selected and skipped members with reasons), `Plugins/DasForUE/Source/DasRuntime/Private/Generated/DasNativeGameplayRuntime.generated.h` (compiled registry; no JSON is read at runtime; `GenerateNativeGameplay.ps1 -Check` fails if it is stale) and `ue_native_gameplay.das`. Skipped members and reasons are listed in the JSON; see also the [API coverage audit](../Dev/Audits/ApiCoverage.md).

## Fields

- Require `BlueprintVisible`; `BlueprintReadOnly` stays read-only. Hidden, editor-only and deprecated fields are excluded.
- Native C++ `Getter`/`Setter` and `BlueprintGetter`/`BlueprintSetter` are honoured: a field access calls the accessor, so normalization and dependent state updates happen as in Blueprint. Example: `actor.NetUpdateFrequency=37.5` calls `SetNetUpdateFrequency`; `actor.RootComponent` reads through `K2_GetRootComponent` and stays read-only. A C++ setter does not lift read-only.
- A write is validated in temporary storage first; wrong type, non-finite numbers/math and read-only writes never reach the setter. A setter may normalize the value; the next read returns the actual value. Objects inside the value are kept alive while the setter runs.
- Accessors must take/return exactly one value of the field type; static, network, editor-only, latent and custom-thunk accessors are rejected. The accessor policy is baked in at generation time, so cooked builds cannot bypass it when metadata is stripped.
- Numeric writes reject NaN/Inf; movement speed and jump velocity also reject negative values.

```das
options gen2
require ue_native_gameplay

[export]
def main(input:int):int {
    let actor=ue_ref(type<ActorRef>,ue_self_object())
    actor.NetUpdateFrequency=37.5
    return int(actor.NetUpdateFrequency)
}
```

Full example: `Plugins/DasForUE/Content/Das/Scripts/native_accessors_example.das` (needs an Actor owner).

## Methods

- Public native `BlueprintCallable` instance functions. Static, net/RPC, delegate, editor-only, deprecated, protected, internal, custom-thunk, latent and `WorldContext` methods are excluded with reasons.
- Reflected names are used (`K2_GetPawn`), not Blueprint display names.
- Inputs, returns and native mutable out/ref parameters use the shared call frame. Outputs are read before ref/out changes are published; frames are released on success and error.
- Only trailing scalar literal defaults and native enum constants become source defaults (e.g. `SetMovementMode(EMovementMode.MOVE_Custom)` uses `NewCustomMode=0`). Other parameters must be passed explicitly; metadata expressions are never evaluated.
- Overloads: an exact overload wins; otherwise the unique concrete overload with the shortest summed native ancestry distance is chosen; ties stay unresolved.
- `UnsafeDuringActorConstruction` methods are generated with a guard: they work in normal gameplay and fail in a ConstructionScript before dispatch, also in cooked builds.

## Object references and upcasts

```das
let character=ue_character_self()
let player=ue_ref(type<PlayerControllerRef>, character.GetController().handle)
player.Possess(character)            // CharacterRef -> NativePawnRef, automatic upcast
let camera=ue_component_get(type<CameraComponentRef>, ue_ref(type<ActorRef>,character.handle), "Camera")
camera.SetProjectionMode(ECameraProjectionMode.Orthographic)
```

Automatic checked upcasts apply to value arguments of concrete functions/methods, typed local initialization, copy/move assignment and function returns. Authored references also convert to their native host's ancestor views. Null stays null. Downcasts, unrelated types and interfaces never convert implicitly. Mutable `ref`/`out` arguments require the exact view type (a `var NativePawnRef` parameter cannot accept a `CharacterRef`), and containers are not covariant. Returned references are weak and checked.

## Value types

| Kind | Source type and rules |
| --- | --- |
| `int32`, `float`, `double`, `bool` (incl. bitfields), `FString`, `FName` | Plain values; FName via `ue_name_value("...")` |
| raw `uint8` | DAS `int`; values outside 0..255 rejected before the write/call |
| native enum | Generated `[ue_native_enum]` type (e.g. `EMovementMode`); unique values 0..254, `_MAX` excluded; int or another enum rejected at compile time |
| `FText` | `UETextValue` (see below) |
| Vector2D / Vector / Rotator / Transform | `UEVector2D`, `UEVector`, `UERotator`, `UETransform` |
| other native value structs | Generated owned struct values (e.g. `Quat`, `Vector4`, `StreamingSourceShape`, `Key`) when every member is supported |
| object / class references | Typed references; class references supported |
| soft object/class references | Typed soft references, see [Soft references](../Guide/DSL/SoftReferences.md) |
| `TArray` | `array<T>` of int, float, double, bool, string, name, enum, object reference, math or generated struct |
| `TMap` / `TSet` | `table<K;V>` / `table<K>`; keys int, string, name, enum or object (`ue_key(ref)`); map values as array elements |

Struct members and container elements are restricted further: nested containers, byte/text/class elements, float/bool/struct keys, interfaces as hash keys, weak/interface references, fixed arrays and opaque structs are excluded and reported as skipped. `PerPlatformInt/Float/Bool/FrameRate` are not treated as value structs because their data is absent in cooked games.

### Structs and containers are owned copies

```das
var tags <- character.Tags
push(tags,ue_name_value("Player"))
character.Tags:=tags                 // assign the whole writable field back

var group=movement.AvoidanceGroup
group.bGroup1=true
movement.SetAvoidanceGroupMask(group)

var children:array<ActorRef>
character.GetAllChildActors(children) // out parameter
```

Reading a native field copies it through reflection; no native memory is exposed. Modify a local copy and assign the whole field with `:=`. Nested writes on value-struct, array and map fields go through the same root-snapshot helper and are published by the checked setter; read-only roots (`movement.GravityDirection.x`) stay rejected. Native math roots use whole-value writes only: `movement.Velocity.x=1.0lf` is rejected. A freshly constructed DAS struct uses DAS defaults: to keep Unreal defaults or existing state, start from a field/result snapshot. Native structs inside authored `ue_struct` declarations are rejected.

Maps and sets:

```das
var labels:table<ActorKey;string>
labels[ue_key(actor)]="owner"
api.ActorLabels:=labels
```

Object keys use `$UEObjectKey<Reference>` / `ue_key(reference)`, never raw addresses; null keys are allowed and identical objects collapse to one entry. Iteration order is unspecified. Destroyed object keys are rejected when a snapshot is read; assigning an empty container clears such a field. Example: `native_gameplay_hash_probe.das` (uses a test component).

Snapshot limits: 4096 aggregate container elements and 16 MiB payload per value.

### Text

`UETextValue` is an opaque, immutable, context-local snapshot of a real `FText`; history and localization identity survive copies between Unreal and DAS.

| Function | Meaning |
| --- | --- |
| `ue_loctext(Key, "source")` | Localizable text; namespace comes from the defining module |
| `ue_text_invariant(value)` | Culture-invariant text |
| `ue_text_to_string(value)` | Current display string (never used to send text back) |
| `ue_text_namespace(value)`, `ue_text_key(value)` | Identity, or empty |
| `ue_text_is_invariant(value)` | Invariant flag |

```das
let label=ue_component_get(type<TextRenderComponentRef>,ue_ref(type<ActorRef>,ue_self_object()),"Label")
label.K2_SetText(ue_loctext(PlayerLabel, "Player"))   // Text itself is BlueprintReadOnly
let shown=ue_text_to_string(label.Text)
```

A context holds up to 4096 distinct text snapshots; they are released with the context. Localization gathering from DAS sources is not automatic: keyed text needs matching localization resources to translate. Dedicated format/string-table construction APIs are not provided.

## Runtime checks

Every access checks the live owner/world context and the expected receiver and member shape. Field and function pointers are resolved per access, so nothing survives reinstancing. Authority-only flags are enforced. Mutations and method calls are rejected in `bind_events` and `EndPlay`. In a ConstructionScript (game or editor world) ordinary methods and accessors work, while construction-unsafe ones are refused before dispatch. Missing, stale or unselected members fail explicitly rather than falling back to raw reflection.

## Limitations

- Only reflected members; no C++ parser, arbitrary C++ methods, virtual overrides or memory-layout copying.
- No general support for every engine type; see the skipped list in `Bindings/NativeGameplay.json` and [Type support](TypeSupport.md).
- Construction guards are not transactions: earlier allowed operations in the same ConstructionScript are not rolled back.
- Generated artifacts belong to one engine/API version; regenerate after changing engine version or sources.

See also: [Generated bindings](GeneratedBindings.md) · [API packages](../Api/README.md) · [Self access](../Guide/DSL/SelfAccess.md) · [Object and class references](../Guide/DSL/ObjectAndClassReferences.md) · [Limitations](Limitations.md)

The full test registry is generated into `Plugins/DasForUE/Tests/Runtime/Private/Generated/DasNativeGameplay.generated.h`.
Ordinary games compile the production registry; Editor and explicitly opted-in smoke games compile the test registry.
