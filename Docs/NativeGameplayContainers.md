# Native structs and arrays

The gameplay generator exports recursive types in manifest schema 4. The selected five receiver classes currently expose 337 fields, 643 methods, 36 enums and 8 value structs. Only dependencies of accepted members are emitted.

```das
require ue_native_gameplay

let character=ue_character_self()
let movement=character.CharacterMovement
var group=movement.AvoidanceGroup
group.bGroup1=true
movement.SetAvoidanceGroupMask(group)

var tags <- character.Tags
push(tags,ue_name_value("Player"))
character.Tags:=tags

var children:array<ActorRef>
character.GetAllChildActors(children)

let player=ue_ref(type<PlayerControllerRef>,character.GetController().handle)
var shapes <- player.StreamingSourceShapes
var shape=StreamingSourceShape()
shape.Radius=1000.0f
shape.LoadingRangeScale=1.0f
shape.Location=ue_vector(0.0lf,0.0lf,100.0lf)
push_clone(shapes,shape)
player.StreamingSourceShapes:=shapes
```

These are owned copies. Change a local value and assign the whole writable field back; reading a native field does not expose native memory. Existing nested-write rejection and readonly policies remain in force. Native source declarations use the internal generated `ue_native_struct` annotation, with canonical module/name, exact reflected member kinds/identities and a compiled schema signature. User code imports the generated types directly, without annotations or string paths.

The generator uses the existing checked struct/container snapshot adapters for fields, arguments, return values and out/ref parameters. It does not copy the DAS memory layout into a C++ struct. Native fields are copied through reflection into owned `FInstancedStruct` snapshots, and the shared transport checks container element identity, enum identity and object classes. Ordinary reference wrappers remain weak checked handles. Native mutation/world/receiver/teardown policies and call-frame cleanup apply unchanged.

Generated structs: PlatformUserId, Quat, Vector4, LightingChannels, NavAvoidanceMask, StreamingSourceShape, WalkableSlopeOverride and Key. Existing Vector, Vector2D, Rotator and Transform keep their established native value wrappers. Structs must pass the existing whole-struct snapshot validator and recursive source-type support checks. Native structs containing unsupported members, opaque native object-reference tracing or unsupported recursive layouts are excluded. This is not arbitrary C++ struct support.

Arrays support the existing exact carriers for int, float, double, bool, string, name, supported enum, native object reference, supported math and supported generated struct elements. Nested containers and byte/text/class elements remain excluded in this generator step. Existing snapshot limits (4096 aggregate container elements and 16 MiB payload) remain in effect. No container covariance or implicit element downcasts are introduced. Native TMap/TSet export is now covered separately in [NativeGameplayHashContainers.md](NativeGameplayHashContainers.md). Native structs embedded inside authored ue_struct declarations require a separate adapter and are explicitly rejected.

Cpp defaults unsupported by the existing literal parser remain explicit. This step does not generate non-reflected native members, serializer hooks or constructors. A freshly constructed DAS value uses its source defaults; to preserve Unreal defaults or existing state, begin with a native field/result snapshot and modify that copy.

Validation completed: 17 generator tests, fresh export/artifact comparison, strict PIE (1 success, 0 warnings/failures/skips), Win64 packaged Development foundation/gameplay and a separate saved playable-map launch all passed under NullRHI. Logs: native-struct-array-check.log, native-struct-array-pie.log and native-struct-array-cooked.log; stable runtime copies use native-struct-array-cooked-runtime.log, native-struct-array-foundation.log and native-struct-array-map.log. Physical rendering, physical input devices and Shipping were not exercised. The shared playable-character probe exercises name-array field round trips, object-array out/return, float-array inputs, native struct fields/inputs/results, enum members, struct arrays and nested math. Separate negative fixtures reject wrong array elements, object-array out covariance, wrong struct identity and forged struct layouts.
