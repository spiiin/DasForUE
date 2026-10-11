# Typed native component operations

```das
[ue_native_object(path="/Script/Engine.RotatingMovementComponent")]
struct RotatingComponentRef { handle:UObjectHandle }
def RotatingComponentRef_from(value:UObjectHandle):RotatingComponentRef {
    return RotatingComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.RotatingMovementComponent"))
}

let movement = ue_component_get_or_create(type<RotatingComponentRef>,actor,"Movement")
let found = ue_component_get(type<RotatingComponentRef>,actor,"Movement")
let removed = ue_component_destroy_handle(movement.handle)
```

`ue_component_create(type<T>,actor,name)` always creates and rejects occupied names.
`get` returns a nullable typed reference; `get_or_create` reuses a matching instance.
The type token is expanded at compile time; the runtime repeats class validation.
Creation supports loaded concrete native ActorComponent classes. Authored component
classes and Blueprint classes are rejected for creation until their identity and
initialization contract is implemented. No class loading occurs during these calls.

Lookup uses UObject instance names with FName comparison, not DSL field identifiers.
An existing named component with an incompatible type is an error, not a miss.
Instances must belong to a live actor in the script owner's game world. Creation
is forbidden during bind_events/EndPlay. Scene subclasses attach to the actor root,
or become root when none exists. Actor owns the registered transient instance.

Destroy accepts a raw handle and checks component type and creator ownership at
runtime. Only instances created by the same script-owner UObject may be destroyed;
lookup does not grant that right. Scene children must be detached first. The shared
creation tracker is weak and bounded to 1024 live components. Components persist
across ordinary VM replacement; replication, structural reload and save/cook
behavior are not covered by this API.

Coverage: the native-components fixture creates RotatingMovementComponent, looks
it up, reuses it, destroys it and checks reference invalidation. It also creates a
SceneComponent through the typed API and exercises scene attachment operations.


## Authored component lookup

`ue_component_get(type<ScoreComponent>, actor, "InstanceName")` also accepts
`ue_object_ref` types whose declaration has ActorComponent as native parent.
The result supports existing authored property/method adapters. A missing name
returns a nullable reference. A matching non-authored component, destroying
component or different authored stable type ID is an error.

Names are actual UObject instance names, not necessarily SCS property names.
No Blueprint class resolution or asset loading is necessary: identity is checked
against the found instance's class CDO. Authored create/get_or_create still fail
at compile time; their generated class selection and initialization need a separate
contract. General Blueprint component references remain unsupported.

The integration fixture renames its test component to a known instance name and
checks successful typed lookup, Bonus=42 and null for a missing instance.


## Authored creation with an explicit generated class

```das
let behavior = ue_component_create(type<ScoreComponent>, actor,
    "RuntimeBehavior", "/Game/Components/BP_Score.BP_Score_C")
let same = ue_component_get_or_create(type<ScoreComponent>, actor,
    "RuntimeBehavior", "/Game/Components/BP_Score.BP_Score_C")
```

The path is an explicit example, not an automatically created asset location.
The generated class must already be loaded. There is no synchronous asset load,
class scanning or arbitrary choice between multiple imports of the same DAS type.
The class must be concrete and its CDO's AuthoredTypeId must match the requested
reference declaration. Existing instances must also satisfy IsA(requestedClass).
Creation uses the generated class defaults, not another SCS instance's overrides.
The normal Actor instance-component registration and owned-destruction path applies.

This supersedes the earlier authored-creation restriction above. Three arguments
remain sufficient for authored get; create/get_or_create require four. Native
operations continue to take three arguments. Arbitrary Blueprint types without
matching authored identity remain unsupported.

Integration coverage: create, reflected property write/read through a reused
reference, owned destruction and stale-reference invalidation in an Editor-hosted
game UWorld. Creation during an already-running PIE world, nested BeginPlay,
Tick/EndPlay behavior and packaged asset availability still need dedicated tests.

## Dynamic component PIE lifecycle

`Scripts/TestDynamicComponentsPIE.ps1` runs `DasForUE.Components.DynamicPIE` in
real PIE. Fixtures: `dsl_dynamic_owner.das` and `dsl_dynamic_lifecycle.das`.
The test checks synchronous nested BeginPlay, at least two ticks, a single EndPlay
on script-owned destruction, disabled ticking/unregistration, no lifecycle errors,
and release of interpreter sessions after ending PIE.

The runtime allows nested BeginPlay/EndPlay only for the concrete component being
registered/destroyed by the component adapter. The previous context is restored
by scope guards. Other unsolicited nested lifecycle calls remain disallowed.
Creation nesting is limited to 16 and checked before allocating the component.
This does not establish packaged-build or replication coverage.

Validation 2026-10-11: TestDynamicComponentsPIE.ps1 PASS; one successful automation test, zero warnings/errors.
