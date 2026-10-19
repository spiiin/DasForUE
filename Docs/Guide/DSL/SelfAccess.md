# Self access

Inside methods, helpers and lifecycle hooks of an authored class, `self` refers
to the real Unreal object. Reads and writes of `self.Field` go to the object's
reflected properties through checked accessors; the script holds no separate copy
of the object. This page covers authored fields, nested struct and container
members, and inherited native fields and methods.

## Authored fields

```das
[ue_function(BlueprintCallable)]
def AddScore(amount:int):void {
    self.Score += amount
    self.Label.K2_SetText(ue_text_invariant("Score: {self.Score}"))
}
```

- Plain assignment and `+=`, `-=`, `*=`, `/=` work on supported scalar and value fields.
- `BlueprintReadOnly` fields can be read but not assigned in methods (compile error).
- `def const` methods reject any write to `self`
  ([Functions](Functions.md#const-methods)).
- Component fields return typed references to the live components
  ([Components](Components.md#using-components-in-methods)).
- The same accessors work through typed references to other objects
  (`actor.Score += 1`); see [Authored references](AuthoredReferences.md).

## Nested struct members

Authored struct properties can be modified in place, member by member:

```das
self.Settings.Point = point                    // whole authored struct member
self.Settings.Point.Position = ue_vector(5.0lf, 8.0lf, 3.0lf)
self.Settings.Orientation = rotation           // UERotator
self.Settings.Pose = transform                 // UETransform
self.Settings.Items[0].Position.x += 4.0lf     // leaf of a math value
self.Settings.Point.Owner = actor              // ActorRef member
self.Settings.Point.Events = default<DasEventInterface>
```

- Structs containing containers are noncopyable in daScript; assign them with the
  clone operator: `self.Envelope.Payload := settings`.
- Object and interface members accept their exact wrapper types: `ObjectRef` does
  not implicitly become `ActorRef` or an interface wrapper. Omit initializers for
  null defaults. A member referencing a destroyed Actor reads as empty and can be
  cleared by assignment.
- Enum, native reference, authored struct and math leaves share one typed commit
  path: the RHS type is checked, the exact nested member updated and the root
  property committed. Sibling fields stay intact.
- Reference arithmetic and compound arithmetic on whole struct values are rejected.

## Containers inside self

Arrays, maps (`table<K;V>`) and sets (`table<K>`) inside authored struct properties
or declared directly on the class support standard daScript operations:

```das
self.Settings.Samples[0] = 5.0lf
self.Settings.Samples[1l] += 3.0lf
self.Settings.Items[0].Count = 17
self.Settings.Items[0].Mode = MethodState.Idle
self.Settings.Weights["score"] = 7.0lf
push(self.Settings.Samples, 2.0lf)
push(self.Settings.Samples, 4.0lf, 0l)        // insert at index
push_clone(self.Settings.Items, MethodPoint())
pop(self.Settings.Samples)
insert(self.Settings.Tags, "ready")
insert(self.Settings.Weights, "score", 8.5lf)
insert_clone(self.Settings.Weights, "copy", 8.5lf)
let removed = erase(self.Settings.Tags, "ready")   // bool for sets/maps
erase(self.Settings.Samples, 0l, 1l)               // index, optional count
clear(self.Settings.Weights)
self.Settings.Items := items                       // whole-member replacement
```

Multi-level paths through supported shapes work too:

```das
self.Groups[0].Scores[1l] += 3.0lf
self.Groups[0l].Weights["score"] = 8.0lf
self.Groups[0].Points[0].Position.x += 4.0lf
push(self.Groups[0].Scores, 2.0lf)
self.Groups[0].Scores := replacement
```

| Operation | Arrays | Maps | Sets |
| --- | --- | --- | --- |
| indexed read/assign/compound | yes (`int` or `int64` index) | yes (exact key type) | - |
| `push`, `push_clone` (optional index), `pop` | yes | - | - |
| `insert`, `insert_clone` | - | yes | `insert` |
| `erase` | index [, count], void | key, returns bool | key, returns bool |
| `clear`, `:=` whole replacement | yes | yes | yes |

- An empty value assigned with `:=` clears the member. Plain `=` is not used for
  noncopyable containers.
- These operations follow the declared container shapes; they do not add new ones
  (for example `array<array<T>>`). See [Containers](Containers.md).
- Array-to-array `push`, consuming moves, live iterators and passing a member as a
  mutable function argument are not supported. Clone into a local, work on it,
  then assign the whole member or property back. Iterate keys/values on a local clone.

The same operations are available through typed references in ordinary functions:

```das
def update(api : ValueTargetRef) {
    push(api.Settings.Samples, 2.0lf)
    api.Settings.Items[0].Count += 1
    api.Settings.Weights["score"] = 7.5lf
    let removed = erase(api.Settings.Weights, "score")
}
```

### Write semantics

- Receiver, indices, keys and values are evaluated **once**, before native state is
  read.
- Each operation reads an owned snapshot of the root property, applies the change,
  validates and packs it, then publishes the whole root property. An index, type or
  limit failure publishes nothing.
- Atomicity is per operation: earlier statements and side effects of argument
  evaluation are not rolled back.
- Readonly roots and readonly struct members crossed by the path reject mutation.
- Limits per snapshot: 4096 elements and 16 MiB of tracked container storage.

Examples: `ue_method_values.das`, `dsl_method_value_target.das`
(`ValueTarget.ReplaceSelfValues`, `MutateSelfContainers`, `MultiIndexContainers`).

## Native fields of the host class

Authored Actors, Pawns, Characters and ActorComponents can use inherited
BlueprintVisible native fields directly:

```das
options gen2
require ue_scene_components
[ue_class(Blueprintable)]
class Example : Actor {
    [ue_function(BlueprintCallable)]
    def SetSpeed(input:float):float {
        self.CustomTimeDilation=input
        self.CustomTimeDilation*=2.0f
        return self.CustomTimeDilation
    }
    [ue_function(BlueprintPure)]
    def const Root():SceneComponentRef { return self.RootComponent }
    [ue_function(BlueprintPure)]
    def const OwningActor():ActorRef { return self.Owner }
    [ue_function(BlueprintCallable)]
    def Mark():void { push(self.Tags,ue_name_value("Marked")) }
}
```

No Blueprint variables or authored properties are created for these fields.

| Native field type | daScript type |
| --- | --- |
| int32, float, double, bool (incl. bitfields), FString, FName | `int`, `float`, `double`, `bool`, `string`, `UENameValue` |
| strong UObject / UClass reference | the unique visible nominal wrapper for that exact class (e.g. `SceneComponentRef`, `ActorRef`, a Pawn wrapper) |
| byte-sized UEnum | the visible `ue_native_enum` declaration (e.g. `SpawnCollision`) |
| FVector, FVector2D, FRotator, FTransform | `UEVector`, `UEVector2D`, `UERotator`, `UETransform` |
| other safe structs | detached `UEStructValue` snapshot |
| TArray of the above / of objects | `array<...>` of the corresponding type |
| TMap with int32, FString or FName keys | `table<int / string / UENameValue; value>` |
| TSet of int32, FString, FName | `table<int / string / UENameValue>` |

Enum-key maps, enum sets and object-key maps/sets are covered in [Enums](Enums.md)
and [Containers](Containers.md).

Rules:

- Names are case-sensitive. Only BlueprintVisible fields of the engine host class
  are visible; bridge bookkeeping, hidden fields (`ReplicatedMovement`,
  `PrimaryActorTick`) and opaque/unsafe structs are not.
- `BlueprintReadOnly` native fields are read-only in methods. Some may still be set
  as class defaults (for example `InitialLifeSpan`, see [Defaults](Defaults.md)).
- Properties with native Getter/Setter or BlueprintGetter/BlueprintSetter go through
  those accessors: writes are validated before the setter runs, the setter may
  normalize the value, and a C++ setter does not lift `BlueprintReadOnly`.
- `self.Owner` is a read-only facade over `GetOwner()`; assigning it is rejected.
  `self.RootComponent` is a read-only `SceneComponentRef`; use
  `ue_scene_valid(self.RootComponent)` to test it.
- Wrapper types must match exactly: a missing or ambiguous visible wrapper is a
  compile error, and `ActorRef`, `SceneComponentRef` or a raw `UObjectHandle` cannot
  be assigned to a Pawn field. The real object class is checked again before writing.
  Import one canonical module per native type (`ue_scene_components`,
  `ue_native_gameplay`).
- Native enums are checked for member coverage when compiled; values are limited to
  0..254 without `_MAX`, and integers or other enums cannot be assigned. Native enums
  inside authored structs are rejected.
- Authored fields and components cannot shadow native field names.
- Storage is resolved on every access with the object, type and access flags checked
  again; no pointer survives compilation or reinstancing. Taking a writable reference
  to a native field is not supported.

### Native containers

Native arrays, maps and sets follow the same snapshot rules as authored containers:

```das
self.Tags[0]=ue_name_value("Updated")
self.Tags := values                       // commit a local array
let count = int(length(self.Tags))
self.Scores[ue_name_value("Score_1")] += 5
insert(self.Labels, ue_name_value("Visible"))
let present = key_exists(self.Labels, ue_name_value("VISIBLE"))
```

`Actor.Tags` and `ActorComponent.ComponentTags` are `array<UENameValue>`. A read
returns a detached copy: changing a local copy affects Unreal only after it is
assigned back. Direct mutations: indexed assignment, `push`, `push_clone`, `erase`,
`pop`, `clear` (arrays), indexed/compound writes, `insert`, `erase`, `clear` (maps
and sets), and `:=`. Vector/rotator components of indexed elements can be updated;
transforms and generic structs use whole-element assignment.

FName keys compare like Unreal names: case variants are the same key, numeric
suffixes stay distinct (`Unit_1` vs `Unit_01`), and `None`, empty and default
names are the same. Name tokens are VM-local and cannot be moved between contexts.

### Math adapters

```das
let position=self.RootComponent.RelativeLocation   // UEVector, read-only
let scaleX=self.RootComponent.RelativeScale3D.x    // nested leaf read
let transform=self.ActorTransform                  // UETransform snapshot
self.ActorTransform=newTransform                   // whole value only
```

`ActorTransform` wraps the root's world-transform getter/setter. It needs a valid
root and a game-world, non-template owner; partial leaf writes are rejected.

## Native methods through self

Inherited native functions available in the generated gameplay bindings can be
called on `self`, and native object fields expose their own members:

```das
def OnJump(value:bool):void {
    if (value) { self.Jump() } else { self.StopJumping() }
}
def BeginPlay():void {
    self.CharacterMovement.MaxWalkSpeed=450.0f
    self.AddControllerYawInput(0.5f)
}
```

Authored methods take precedence over native ones with the same name. The set of
callable native members is determined by the generated bindings; see
[Native class bindings](../../Reference/NativeClassBindings.md). Methods marked
unsafe during Actor construction are rejected in `ConstructionScript`.

## Limits

- Weak/soft native references, UClass container elements, nested native containers,
  struct keys and arbitrary native object property projection are not exposed
  through `self`.
- Arbitrary C++ parent classes are not supported as hosts.

Examples: `dsl_native_defaults.das`, `dsl_native_defaults_component.das`,
`dsl_native_defaults_child.das`, `dsl_playable_character.das`.
Try it: `Scripts/TestNativeDefaults.ps1`.

See also: [Classes](Classes.md) · [Functions](Functions.md) ·
[Containers](Containers.md) · [Structs](Structs.md) ·
[Authored references](AuthoredReferences.md) ·
[Object and class references](ObjectAndClassReferences.md) ·
[Native class bindings](../../Reference/NativeClassBindings.md)
