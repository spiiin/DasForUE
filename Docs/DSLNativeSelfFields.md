# Native fields in instance methods

An authored Actor or ActorComponent can read inherited native Blueprint-visible
fields using `self`, without declaring another `ue_property`.

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
    def const StartingLife():float { return self.InitialLifeSpan }
    [ue_function(BlueprintPure)]
    def const HasRoot():bool { return ue_scene_valid(self.RootComponent) }
}
```

The same lowering applies to script-only instance helpers, inherited method
implementations and lifecycle callbacks. Source templates are lowered separately
after runtime clones are generated, so ordinary daScript type checking still runs.
No fake native fields are added to source class storage or authored JSON/IR, and
the importer does not create Blueprint variables for these properties.

Supported native field types: int32, float, double, bool (including bitfields),
FString, FName, strong UObject references and UClass references. Native reference
reads and assignments use the unique visible nominal wrapper matching the exact
native property class (or UClass MetaClass). The raw handle remains the internal
checked ABI; ordinary method code receives the typed value.
Primitive native arrays project to typed daScript arrays; see the native array
section below. Native enums and safe struct snapshots are supported as described in
the native enum/struct section. RootComponent relative math values support readonly
member reads, including `self.RootComponent.RelativeLocation.x`. Native maps/sets and object arrays are described below. Weak/soft references and
arbitrary native object property projection remain separate follow-up work.

Native property names are case-sensitive. Only BlueprintVisible properties of
the declared Engine host are eligible; bridge bookkeeping is not exposed.
Actor `Owner` is a deliberate exception: a readonly facade calls the public
`GetOwner()` adapter, rather than reading the non-Blueprint-visible field directly.
Assigning `self.Owner` is rejected; use the public owner API when needed.
BlueprintReadOnly fields can be read but cannot be assigned in gameplay methods
(including compound assignments). Editable defaults have their separate policy:
`InitialLifeSpan` may be set in `defaults` but only read in methods.
Authored properties/components cannot shadow native field names. Accessor-backed
properties require explicit accessor calls, so this syntax never silently bypasses
their getter/setter. RootComponent reads use the existing native GetRootComponent adapter.
The Actor accessor policy is explicit because Unreal strips accessor metadata when
cooking; new host adapters must carry their corresponding policy. Const methods retain the existing source-level write check.

Assignment and `+=`, `-=`, `*=`, `/=` use the existing checked self runtime ABI.
Native storage is resolved on every access, with the active game-thread object,
type and access flags checked again. No FProperty pointer survives compilation or
Blueprint reinstancing. Other mutation operators and taking a writable field
reference are not supported.

Examples: `dsl_native_defaults.das`, `dsl_native_defaults_component.das`,
`dsl_native_defaults_child.das`. Regression: `Scripts/TestNativeDefaults.ps1`;
add `-Package` for the cooked Win64 Development runtime probe. The probe checks
live native writes, compound operations, script-only helpers, inherited methods,
const reads, native component handles, rejection of readonly/hidden/wrong-type/
case-mismatched/unsupported access, and unchanged authored property counts.

Validation (2026-10-16): Editor build and TestNativeDefaults.ps1 -Package passed
Create/Reopen/Recompile and the Win64 Development NullRHI game probe. Nine invalid
native-method variants, the positive control and the const-write check passed.
Editor and packaged foundation each report DAS_FOUNDATION_RESULT=PASS failures=0.
Logs: logs/native-self-verified-suite.log,
logs/native-self-foundation-editor-final.log, logs/native-defaults-packaged.log,
and logs/smoke-packaged.log. Shipping and rendered gameplay were not exercised.

## Typed native reference projection

```das
options gen2
require ue_scene_components
[ue_class(Blueprintable)]
class ExampleRefs : Actor {
    [ue_function(BlueprintPure)]
    def const Root():SceneComponentRef { return self.RootComponent }
    [ue_function(BlueprintPure)]
    def const OwningActor():ActorRef { return self.Owner }
    [ue_function(BlueprintPure)]
    def const RootScale():UEVector {
        return ue_transform_scale(ue_scene_relative_transform(self.RootComponent))
    }
}
```

`ue_scene_components` publicly imports the canonical Actor wrapper module.
`self.RootComponent` has type SceneComponentRef, and `self.Owner` has type ActorRef.
Native references can therefore be returned from ue_function or passed directly
to the existing typed API. Empty native references become null typed values.

Writable references use the same contract: with a visible native Pawn wrapper,
`self.Instigator=value` accepts that nominal wrapper and returns a typed Pawn value
when read. Passing ActorRef, SceneComponentRef or a raw UObjectHandle is a compile
error. Construct null in the intended nominal type, not with a raw handle alone.
The wrapper converter checks the actual UObject class again before writing, so a
manually forged wrapper cannot place an Actor into a Pawn field.

Wrapper resolution scans only visible modules and excludes imported private
structures. It matches ue_native_object.path (or ue_class_ref.path) exactly;
authored object/interface views and broader parent wrappers are not substitutes.
Aliases to the same wrapper are fine. Missing or multiple distinct matching
wrappers produce explicit diagnostics. Import one canonical module for each
native type; fallback to raw handles would make source typing depend on imports.
Custom native wrappers follow the existing `_from(UObjectHandle)` / `_raw(Type)`
checked converter contract; see ue_native_self_refs.das for the Pawn example.

This changes the previous raw-reference spelling. Replace
`ue_object_valid(self.RootComponent)` with `ue_scene_valid(self.RootComponent)`;
use an explicit typed raw conversion only when calling the low-level handle API.
Native reference metadata stays compilation-local and does not create authored
property entries or additional Blueprint variables.

Typed-reference validation (2026-10-16): Editor Create/Reopen/Recompile and
Win64 Development NullRHI packaged probe passed. Covered typed SceneComponent
returns and direct scene API calls, inherited methods, null/live Actor owners,
Pawn assignment/return/null/recovery, wrong nominal types, readonly Owner,
missing/ambiguous wrappers and forged wrapper rejection before UE storage changed.
Editor and packaged foundation each report PASS failures=0.
Logs: logs/typed-native-refs-editor-2.log, typed-native-refs-foundation.log,
typed-native-refs-package.log, typed-native-refs-packaged.log and smoke-packaged.log.
Native UClass wrapper projection shares the implementation but the current
Actor/ActorComponent host surface has no native UClass property fixture; it has
not been separately exercised. Shipping and rendered gameplay remain untested.

## Native arrays: Tags and ComponentTags

```das
options gen2
require ue_dsl
[ue_class(Blueprintable)]
class TaggedActor : Actor {
    [ue_function(BlueprintCallable)]
    def Mark():void {
        push(self.Tags,ue_name_value("Marked"))
        self.Tags[0]=ue_name_value("Updated")
    }
    [ue_function(BlueprintCallable)]
    def Replace(values:array<UENameValue>):void { self.Tags := values }
    [ue_function(BlueprintPure)]
    def const Count():int { return int(length(self.Tags)) }
}
```

`Actor.Tags` and `ActorComponent.ComponentTags` have type `array<UENameValue>`.
The compiler supports primitive native arrays with int32, float, double, bool,
FString or FName elements when the existing carrier matches the property exactly.
Name arrays are exercised against the actual Actor and ActorComponent host fields;
the other primitive paths reuse existing carriers but lack native host fixtures.
Native object arrays, maps and sets are described in the later sections; native
container defaults remain separate work. No additional authored IR properties or Blueprint variables
are generated.

Reading a native array produces a separate snapshot. Use `length`, indexed reads
or clone it into a local array. Changes to that local copy do not affect Unreal
until `self.Tags := values` explicitly commits it. Supported direct mutations are
indexed assignment, `push`, `push_clone`, `erase`, `pop` and `clear`. Other APIs
(including resize or arbitrary mutable-ref helpers) should operate on a local
clone followed by explicit assignment; they are not direct native write-through
operations. Container keys/values iterators over self keep the existing local-clone
requirement.

Mutation helpers evaluate arguments, read the latest native snapshot, modify it,
and commit through the checked property setter. RHS side effects therefore survive
an indexed write. A failing index/type operation never commits its partial snapshot;
effects already performed while evaluating arguments are not rolled back.
Const methods reject direct mutation. Readonly/accessor/visibility/type checks
from the native field policy still apply, including runtime setter checks.

Read and mutation helpers are shared with authored containers, not a separate tag
API. They resolve native storage for each call and never retain element pointers.
The declaration-only compilation path also publishes generated array helpers.
Examples and runtime probes are in dsl_native_defaults.das,
dsl_native_defaults_component.das and DasNativeDefaultsProbe.cpp.

Native-array validation (2026-10-16): Editor build, Create/Reopen/Recompile and
Win64 Development NullRHI packaged probe passed. Verified live Actor.Tags and
ActorComponent.ComponentTags mutation, inherited methods, whole-array assignment,
snapshot return/isolation, indexed reads/writes, clear/push/erase/pop, preservation
of RHS side effects, bounds failure without partial commit and recovery. Wrong
array element/assignment types and const writes were rejected. Declaration-only
compilation with the native array methods also passed. Editor and packaged
foundation each report DAS_FOUNDATION_RESULT=PASS failures=0.
Logs: logs/native-arrays-editor-verified.log, native-arrays-foundation-editor.log,
native-arrays-package.log, native-arrays-packaged.log and smoke-packaged.log.
Only the FName native arrays have live host fixtures; other primitive arrays share
the carrier implementation. Shipping and rendered gameplay were not exercised.

## Native enums and value structs through self

Native enum fields require one visible canonical declaration of the existing UEnum:

```dascript
require ue_native_engine_values public

[ue_function(BlueprintCallable)]
def SetCollision(value:SpawnCollision):SpawnCollision {
    self.SpawnCollisionHandlingMethod=value
    return self.SpawnCollisionHandlingMethod
}
```

`ue_native_engine_values.das` defines `[ue_native_enum(path="/Script/Engine.ESpawnActorCollisionHandlingMethod")] enum SpawnCollision`.
This is a nominal daScript view, not a new Unreal enum asset. Explicit native member
names/numbers and complete coverage of supported values are checked on compilation.
The existing byte-safe enum ABI permits 0..254, excludes MAX sentinels and validates
runtime values before writes. Integers, raw UEEnumValue and other enum types cannot
be implicitly assigned to this field. Missing or ambiguous visible declarations
fail with a diagnostic. Native enum parameter/result type paths are inferred.
Native enums nested in authored ue_struct declarations are currently rejected;
the authored source-value registry adapter remains for authored types only.

Native FStructProperty fields follow the same visibility, readonly, accessor and
safe-value validation policy. Supported math structs use UEVector, UEVector2D,
UERotator or UETransform; other accepted structs use detached UEStructValue snapshots.
Opaque/reference-bearing/unsafe structs are not exposed. Hidden fields such as
Actor.ReplicatedMovement and PrimaryActorTick remain inaccessible.

The actual Actor/ActorComponent base fields do not provide a BlueprintVisible
writable struct fixture. The following explicit component/math adapters are tested:

```dascript
let position=self.RootComponent.RelativeLocation   // UEVector, readonly UE field
let rotation=self.RootComponent.RelativeRotation   // UERotator, readonly UE field
let scaleX=self.RootComponent.RelativeScale3D.x    // nested leaf read
let transform=self.ActorTransform                 // UETransform snapshot
self.ActorTransform=newTransform                  // whole-value, checked public API
```

Root relative fields preserve BlueprintReadOnly, including nested leaf writes.
ActorTransform is an explicit adapter over root world-transform getter/setter, not
an FProperty or generated Blueprint variable. It requires a valid root component;
mutation checks the script owner's world, template status and transform validity.
Partial ActorTransform leaf assignment is rejected: modify a local value and assign
the whole transform. This facade is for ordinary methods, not a new defaults path.
Const method writes are rejected before lowering. No native storage pointers or fake
authored properties are added to the DSL class or persisted IR.

Live fixtures exercise the Engine collision enum and root math structs. Generic safe
struct snapshots share the existing runtime path but have no additional native base
host fixture in this stage. Arbitrary native object dot-property access, automatic
nominal wrappers for all native structs and native struct/enum map keys or set
elements are separate work.


Native enum/struct validation (2026-10-16): Editor build, Create/Reopen/Recompile,
Editor foundation and Win64 Development NullRHI package passed. Native UEnum
parameters/results and all five values, inherited methods, forged enum rejection
without partial write and recovery, missing/ambiguous/wrong nominal declarations,
declaration member mismatch, readonly/const/type rejection, nested math leaf reads,
exact nonzero rotation getters and whole-value world-transform writes were verified.
Packaged native probe reports DAS_NATIVE_DEFAULTS_PACKAGED=PASS; Editor and packaged
foundation report DAS_FOUNDATION_RESULT=PASS failures=0. Logs:
logs/native-enum-struct-editor-verified.log, native-enum-struct-foundation.log,
native-enum-struct-package.log, native-enum-struct-packaged.log and smoke-packaged.log.
Probe map: /Game/DasNativeDefaults/281e6c57275c4c8ea8e3b1609a0d38e8/Map.
Shipping and rendered gameplay were not exercised.


## Native enum and struct arrays

An accessible inherited native TArray of a byte-sized UEnum now projects to
`array<CanonicalNativeEnum>` using the unique visible ue_native_enum declaration.
Both FEnumProperty (enum class : uint8) and FByteProperty (TEnumAsByte) storage are
supported, with distinct exact snapshot carriers. Wider underlying enum integers
are rejected explicitly: Unreal SameType alone does not compare that storage width.
The value ABI still accepts only declared non-MAX enum values in 0..254.

Native TArray<FVector>, FVector2D, FRotator and FTransform project to their existing
math handle arrays. Other structs accepted by ValidateDasValueStruct project to
`array<UEStructValue>`; no automatic nominal struct declaration is invented. Opaque,
recursive and otherwise unsupported struct layouts retain the snapshot rejection
policy. A generic UEStructValue element has its actual UScriptStruct checked when
packed; a different struct cannot partially modify the native array.

Supported direct operations match native primitive arrays: indexed assignment,
whole-array clone assignment, push/push_clone, clear, erase and pop. Vector x/y/z and
rotator pitch/yaw/roll can be updated through an indexed element. Generic structs
use snapshot accessors, then explicit element or whole-array assignment. Transform
elements use whole-value assignment. Readonly and const checks apply to mutations.
Reads are detached; local changes affect Unreal only when committed through self.
Arguments are evaluated before the latest native snapshot is read. Bounds, enum,
struct identity or snapshot validation failure prevents that snapshot's commit.

The fixed Engine Actor/ActorComponent hosts have no suitable BlueprintVisible native
enum/struct array fields. dsl_native_array_self.das therefore uses actual C++ UPROPERTY
arrays on the existing test adapter. Their descriptor extension is explicitly scoped
by SetDasNativeArraySelfProbeEnabled during the regression harness only and disabled
by default. It is not user DSL syntax, a production field whitelist, a fake authored
property or permission to inspect other bridge bookkeeping. Normal Engine host and
property visibility rules are unchanged. Supporting arbitrary C++ parent hosts is
separate work.


Native value-array validation (2026-10-17 Europe/Budapest): Editor build and
Create/Reopen/Recompile passed. Verified real FEnumProperty and FByteProperty
UPROPERTY array storage; FVector, FRotator, FTransform and generic FIntPoint arrays;
indexed and vector/rotator leaf writes, clone assignment, clear/push/pop/erase,
detached snapshots, invalid enum and bounds recovery, wrong generic struct identity
and whole-array failure after a valid element without partial commit. Wrong nominal
enum/math types and readonly mutation are rejected at compilation. No authored
Blueprint variables were generated for the fixture. Win64 Development NullRHI
cook/package and packaged native probe passed with exit 0; Editor and packaged
foundation each report DAS_FOUNDATION_RESULT=PASS failures=0. Logs:
logs/native-value-arrays-editor-verified.log, native-value-arrays-foundation.log,
native-value-arrays-package.log, native-value-arrays-packaged.log and
native-value-arrays-foundation-packaged.log. Probe map:
/Game/DasNativeDefaults/19ec0214580b4e8a9c45e72bf24aaf87/Map.
Vector2D shares the math array path but has no additional live fixture here.
Shipping and rendered gameplay were not exercised.


## Native maps, sets and object arrays through self

Accessible native containers now project to typed daScript containers without
declaring authored ue_property fields:

| Unreal field | daScript value |
| --- | --- |
| TArray<UObject subtype / TObjectPtr> | array<canonical native object wrapper> |
| TMap<int32 or FString, scalar/name> | table<int or string; scalar/name> |
| TMap<int32 or FString, byte-sized enum> | table<int or string; canonical ue_native_enum> |
| TMap<int32 or FString, safe struct> | table<int or string; math handle / UEStructValue> |
| TMap<int32 or FString, UObject subtype> | table<int or string; canonical native object wrapper> |
| TSet<int32 or FString> | table<int or string> (set) |

Map keys and set elements also support FName as described in the next section.
Enum, object or struct keys/set elements are not included. Weak/soft references,
UClass container elements and nested native containers are not included. Canonical
object wrappers must match the exact native class/interface; raw handles and
different nominal wrapper types cannot be passed implicitly. Null references use
the existing checked nullable wrapper converters. Pointer lifetime and class checks
remain in the existing object handle/snapshot machinery.

Object arrays support the existing array operations. Maps support indexed reads,
indexed/compound writes, insert, erase, clear and whole-table clone assignment.
Sets support insert, erase, clear, membership/length and whole-set clone assignment;
duplicate insertions remain one set element. Math members of indexed map values
follow the existing vector/rotator update rules. Other structs use explicit snapshot
accessors and value assignment. Iterate keys/values on a local clone.

All reads are detached snapshots. A mutable local copy affects native storage only
when assigned back. Mutation helpers evaluate arguments, read current storage and
commit a validated snapshot; snapshot packing failures cannot partially write the
native container. Previous side effects from argument evaluation are not rolled back.
BlueprintReadOnly, native accessor, visibility and const checks still apply.

Enum maps retain the exact FEnumProperty vs FByteProperty representation in separate
snapshot carriers. Wider-than-byte enum storage is rejected, independently of Unreal
SameType. Native enum names/numbers and native struct identity remain checked. Native
struct map carriers now accept the same validated native layouts as struct arrays.

The existing scoped C++ regression adapter is extended with real reflected map/set
and TObjectPtr array fields. The test DSL is dsl_native_array_self.das; the native
field descriptor extension stays disabled outside that harness. No bridge bookkeeping
is made available and no authored Blueprint variables are generated for these fields.


Native map/set/object-array validation (2026-10-17 Europe/Budapest): Editor build,
Create/Reopen/Recompile and Win64 Development NullRHI cook/package/runtime passed.
Live C++ fixtures verify Actor/Pawn TObjectPtr arrays with nulls, detached clones,
whole assignment and forged wrong-class rejection; int/string sets with duplicate
insertion, erase, membership and clone assignment; int/int, string/double and int/name
maps; string/vector, int/IntPoint, int/native-enum, string/byte-enum and int/Pawn maps.
Verified indexed/compound writes, map insert/erase, clear, clone assignment,
wrong enum/object/struct rejection with recovery and no partial native snapshot
commit. Wrong key/value/wrapper/element types and readonly mutation were rejected
during compilation. Actual native fields were compared against expected values.
Other supported primitive map value combinations and interface references reuse
existing carriers; they do not have additional native live fixtures in this stage.
Editor and packaged foundation report DAS_FOUNDATION_RESULT=PASS failures=0;
packaged native probe reports DAS_NATIVE_DEFAULTS_PACKAGED=PASS and exit 0.
Logs: logs/native-containers-editor-verified.log, native-containers-foundation.log,
native-containers-package.log, native-containers-packaged.log and
native-containers-foundation-packaged.log. Map:
/Game/DasNativeDefaults/877cccf7e8a74a9b873d717e293608cb/Map.
Shipping and rendered gameplay were not exercised.


## FName map keys and set elements

Native TMap<FName, Value> now projects to table<UENameValue; Value> and native
TSet<FName> to table<UENameValue>. The map value families remain scalar/name,
byte-sized enum, math/safe struct and strong native object/interface references.
This stage covers native fields through self and their local snapshots; new authored
ue_property declarations or ue_function container signatures using FName keys are
not part of this stage's public authoring contract.

```das
let key=ue_name_value("Score_1")
self.Scores[key]=10
self.Scores[ue_name_value("score_1")]+=5
insert(self.Labels,ue_name_value("Visible"))
let present=key_exists(self.Labels,ue_name_value("VISIBLE"))
```

These example field names denote native reflected properties, not fields the DSL
creates. The concrete regression fixture is dsl_native_array_self.das, NameKeys and
NameValues, using the existing scoped C++ test adapter.

Names remain UENameValue values rather than string keys. MakeDSLName interns names
in a context-owned TMap<FName,uint64>: equal FNames receive the same opaque token.
daScript's existing non-ref handle table keys use the annotation's uint64 value
representation for hashing/equality. Thus case variants have equal keys, while
Unreal's numeric suffix rules, including leading-zero distinctions, remain intact.
NAME_None/default UENameValue, empty text and "None" share the native None identity.
The numeric table hash/token is VM-local, not a serialized FName ID or a promise of
the same numeric GetTypeHash result as Unreal. Tokens cannot be transferred between
contexts; native conversion validates ownership. Name display spelling is not the
key identity and follows the existing name interning behavior.

Snapshot carriers use actual FNameProperty keys and preserve the exact value layout,
including enum-class versus TEnumAsByte storage. Packing resolves every key back
to FName before modifying the temporary native snapshot. The native setter commits
only after complete snapshot validation. Readonly, const, bounds/size, object class,
enum and struct identity checks continue to apply. Reading or iterating local copies
does not retain native container storage pointers. No parser or daScript SDK changes
were needed.


FName container validation (2026-10-17 Europe/Budapest): Editor build,
Create/Reopen/Recompile and Win64 Development NullRHI package/runtime passed.
Live native fixtures confirm CPP-origin name keys, case-insensitive aliases,
Unit/Unit_0/Unit_1/Unit_01 distinctions, None/empty alias, Unicode, set duplicate
handling, key membership, erase, compound writes, local snapshot isolation and
whole-container clone assignment. FName-key map values tested: int, FName, FVector,
FIntPoint, enum class, TEnumAsByte and Pawn references including null. Wrong enum,
struct and object values are rejected without partial native commit; wrong key
types and readonly mutations fail compilation. Other scalar map values reuse the
same scalar accessor templates and do not have additional native fixtures here.
Corrected new name/enum getter registration to the existing uint64 return ABI;
managed object/struct getters retain their copy/move return ABI. No SDK changes.
Editor and packaged foundation report DAS_FOUNDATION_RESULT=PASS failures=0;
native packaged probe reports DAS_NATIVE_DEFAULTS_PACKAGED=PASS and exit 0.
Logs: logs/native-name-containers-editor-final.log,
native-name-containers-foundation.log, native-name-containers-package.log,
native-name-containers-packaged.log, native-name-containers-foundation-packaged.log.
Probe map: /Game/DasNativeDefaults/edfcc276c80349b6a66c559081148910/Map.
Shipping and rendered gameplay were not exercised.


The playable Character step adds an explicit native-reference field policy for CharacterMovement MaxWalkSpeed/JumpZVelocity through the shared property operator generator. These accessors also work in free DAS functions. General native object property projection is still separate; see PlayableCharacter.md.

Update 2026-10-17: authored class properties and method signatures also support
FName-key maps and FName sets; see [DSLNameContainers.md](DSLNameContainers.md).

Update 2026-10-17: native and authored enum sets are supported in native self
fields, direct class properties and method inputs/results/out/inout; see
[DSLEnumSets.md](DSLEnumSets.md).


Native gameplay generation was added for Character, CharacterMovementComponent,
PlayerController and CameraComponent. It shares property/method compiler emitters,
and bakes reflection access policy into the runtime for cooked parity. See
NativeGameplayGeneration.md for source syntax, regeneration, coverage and limits.

Update 2026-10-17: UObject sets and object-key maps are supported through native
self fields, direct properties and method inputs/results/out/inout using typed
UEObjectKey values; see [DSLObjectHashContainers.md](DSLObjectHashContainers.md).
