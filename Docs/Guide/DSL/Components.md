# Components

An authored Actor declares its default components as typed fields. On import each
field becomes an SCS (Simple Construction Script) node, so Unreal creates a real
component for every instance. Fields can name native engine components or
authored `ActorComponent` classes. Components can also be looked up, created and
destroyed at runtime.

## Declaring components

```das
require ue_dsl
require ue_component_types      // declares ScoreComponent (see below)
require ue_scene_components     // SceneComponentRef and scene APIs

[ue_class(Blueprintable)]
class ComponentOwner : Actor {
    components = ue_components(VisibleAnywhere, BlueprintReadOnly, Category="Components") {
        // Score handling.
        var Behavior:ScoreComponent
        var Child:SceneComponentRef
    }
    @ue_component
    @RootComponent
    Pivot:SceneComponentRef

    @ue_component
    @Attach="Child"
    Grandchild:SceneComponentRef
}
```

- The field name is the component name. The field type selects the class: there
  are no name/script/type strings in the declaration.
- Do **not** write an initializer on a component field; Unreal creates the
  subobject. Configure values in [`ue_defaults`](Defaults.md#component-templates).
- `ue_components(...) { var ... }` is a [declaration group](Classes.md#declaration-groups)
  that marks every member as a component. Groups are supported only in Actor classes.
- Several fields may use the same type; each gets its own template:

```das
@ue_component
Primary : ScoreComponent
@ue_component
Secondary : ScoreComponent
```

### Flags and metadata

| Option | Values | Default |
| --- | --- | --- |
| `@ue_component` | singleton marker (individual fields) | - |
| visibility | `VisibleAnywhere`, `EditAnywhere` | `VisibleAnywhere` |
| Blueprint access | `BlueprintReadOnly`, `BlueprintReadWrite` | `BlueprintReadOnly` |
| `@Category`, `@DisplayName`, `@ToolTip` | strings (or a comment above the field) | - |
| `@ue_id` | explicit stable ID | automatic |

`VisibleAnywhere` shows the reference in Details without making it replaceable;
`BlueprintReadOnly` prevents replacing the reference from Blueprint, not editing the
component's own properties. Repeated or conflicting flags, unknown metadata,
unsupported types and explicit initializers are rejected.

### Component types

| Field type | Component |
| --- | --- |
| Native wrapper (`[ue_native_object(path=...)]`), e.g. `SceneComponentRef`, `CameraComponentRef`, `TextRenderComponentRef`, `StaticMeshComponentRef` | Concrete native ActorComponent subclass |
| Authored reference (`[ue_object_ref(...)]`) to a `ue_class ... : ActorComponent` | Authored component Blueprint (created or reused on import) |

Native and authored components can coexist. The generated gameplay module
`ue_native_gameplay` provides wrappers for common engine components; you can
declare your own:

```das
[ue_native_object(path="/Script/Engine.RotatingMovementComponent")]
struct RotatingComponentRef { handle:UObjectHandle }
```

For authored components, keep the reference type in a **declaration-only module**
so the Actor and component sources do not import each other:

```das
options gen2
module ue_component_types public
require ue_dsl public
// Declaration reference only: the Actor importer creates the Component Blueprint.
[ue_object_ref(script="dsl_component_behavior.das", type="ScoreBehavior")]
struct ScoreComponent { handle : UObjectHandle }
```

The reference may also resolve through a source catalog instead of `script=`; see
[Modules and type identity](ModulesAndTypeIdentity.md#source-catalog). A derived
authored component class is accepted as a component type.

## Authored ActorComponent classes

```das
require ue_dsl
[ue_class(Blueprintable)]
class ScoreBehavior : ActorComponent {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Bonus:int=10
    }
    [ue_function(BlueprintCallable)]
    def Evaluate(input:int):int { return self.Bonus + input }
    [ue_function(BlueprintCallable)]
    def Accumulate(var value:int&):int { value += self.Bonus; return value }
    def BeginPlay() { }
}
```

The importer produces an ordinary Component Blueprint on the `UDasScriptComponent`
host; no generated C++ is needed per declaration. Fields, defaults, callable
methods, out/ref parameters, lifecycle hooks and events work as in Actors. `self`
refers to the component's fields. Reach the owner with a typed view:

```das
let actor = ue_owner(type<ComponentOwnerRef>)   // authored owner view
actor.Score += 1
let raw = ue_ref(type<ActorRef>, ue_component_owner(ue_self_object()))
```

## Scene hierarchy

- `@RootComponent` selects the root. Without one, the first native
  SceneComponent with no explicit parent becomes root.
- Scene components without `@Attach` attach to the root (including an inherited
  root). `@Attach="Name"` picks a parent; inside a group write
  `var @Attach="Extension" Marker:SceneComponentRef`.
- `@AttachSocket="SocketName"` stores the attachment socket.
- Non-scene components have no scene parent.
- Cycles, missing parents and multiple roots are rejected before assets are created.
- On Character, the native capsule stays root; replacing it is rejected.

## Using components in methods

A component field gives a typed reference to the actual instance on the current
Actor:

```das
[ue_function(BlueprintCallable)]
def ConfigureBehavior(bonus:int):void { self.Behavior.Bonus = bonus }

[ue_function(BlueprintCallable)]
def AddBehaviorBonus(amount:int):int {
    self.Behavior.Bonus += amount
    let behavior = self.Behavior          // alias of the same component
    return behavior.Bonus
}

[ue_function(BlueprintCallable)]
def CallBehavior(input:int):int { return self.Behavior.Evaluate(input) }
```

- Every access checks the component's authored identity, liveness, type and
  permissions. Destroyed or null components fail instead of reading stale data.
- Compound assignments evaluate the receiver once.
- Native components expose their generated fields and methods, for example
  `self.Label.K2_SetText(...)` or `self.Camera.RelativeLocation`.
- Unknown component fields and replacing a component reference fail at compile
  time; replacing a writable component reference is also refused.

Method calls, argument types and reentry rules for authored references are in
[Authored references](AuthoredReferences.md).

## Runtime creation and lookup

Typed operations take a component type token, the owning Actor and an **instance
name** (the UObject name, not a DSL field name):

```das
let movement = ue_component_get_or_create(type<RotatingComponentRef>,actor,"Movement")
let found = ue_component_get(type<RotatingComponentRef>,actor,"Movement")
let removed = ue_component_destroy_handle(movement.handle)
```

| Function | Behavior |
| --- | --- |
| `ue_component_create(type<T>, actor, name[, class])` | Always creates; occupied names are rejected |
| `ue_component_get(type<T>, actor, name)` | Nullable typed reference; a same-named component of another type is an error |
| `ue_component_get_or_create(type<T>, actor, name[, class])` | Reuses a matching instance or creates one |
| `ue_component_destroy_handle(handle)` | Destroys only components created by the same script owner |

- Native types create loaded concrete native ActorComponent classes; no class is
  loaded by these calls.
- Authored types need a fourth argument naming an already loaded generated class,
  either a path or a `ue_class_loaded(type<T>, path)` handle:

```das
let behavior = ue_component_create(type<ScoreComponent>, actor,
    "RuntimeBehavior", "/Game/Components/BP_Score.BP_Score_C")
```

  The class must be concrete and its authored type ID must match `T`. Creation
  uses the class defaults. Arbitrary Blueprint classes without authored identity
  are not supported.
- The Actor must be live, non-template and in the script owner's game world.
  Creation is forbidden in `bind_events`, `EndPlay` and construction.
- Scene subclasses attach to the root, or become root if none exists. The Actor
  owns the registered transient instance; components survive ordinary VM
  replacement. Do not recreate the same name on every reload.
- A component created during play gets nested `BeginPlay`/`Tick`/`EndPlay`
  delivered for it. Creation nesting is limited to 16.
- Destroy is limited to components created by the same script-owner UObject
  (tracked weakly, up to 1024 live entries). Lookup never grants destroy rights.
  Detach scene children first.

### Scene component helpers (`ue_scene_components`)

| Function | Purpose |
| --- | --- |
| `ue_scene_create(actor,name)`, `ue_scene_get(actor,name)`, `ue_scene_get_or_create(actor,name)` | Named SceneComponent creation and lookup |
| `ue_scene_destroy(component):bool` | Destroy an owned component |
| `ue_scene_actor_root(actor)`, `ue_scene_parent(component)`, `ue_scene_valid(component)` | Nullable queries |
| `ue_scene_relative_transform`, `ue_scene_world_transform` | Copied `UETransform` values |
| `ue_scene_set_relative_transform`, `ue_scene_set_world_transform(c,t,teleport=false)` | Finite, normalized transforms only |
| `ue_scene_attach(child,parent,socket="",rule="KeepRelative",weld=false):bool` | Rules: KeepRelative, KeepWorld, SnapIncludingScale, SnapNotIncludingScale |
| `ue_scene_detach(component,rule="KeepWorld")` | KeepWorld or KeepRelative |

Unknown sockets, attachment cycles and cross-world mutation are rejected; Unreal
mobility rules can still refuse an attachment (returns false). There is no
sweep/hit-result variant.

Examples: `dsl_native_components.das`, `dsl_scene_components.das`,
`dsl_dynamic_owner.das`, `dsl_dynamic_lifecycle.das`.

## Limits

- At most 32 components per effective class. Names must not collide with fields,
  methods, inherited members or other components.
- Changing component declarations, metadata or template defaults on an already
  imported Actor is refused rather than silently modified; body-only reimport
  works. Use a new Blueprint for structural changes. Save the Actor and its
  component Blueprints after import.
- Replacing an inherited component with a subclass is not possible (engine
  constraint, see [Inheritance](Inheritance.md#components-in-a-hierarchy)).
- Replication and save/cook of runtime-created components are not covered by the
  runtime API.

See also: [Classes](Classes.md) · [Defaults](Defaults.md) · [Inheritance](Inheritance.md) ·
[Lifecycle](Lifecycle.md) · [Authored references](AuthoredReferences.md) ·
[Spawn and queries](../Gameplay/SpawnAndQueries.md) ·
[World/Actor/Component API](../../Api/WorldActorComponents.md)
