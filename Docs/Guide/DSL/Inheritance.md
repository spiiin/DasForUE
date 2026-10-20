# Inheritance

An authored class can derive from another authored `ue_class` with ordinary
daScript inheritance. The child Blueprint derives from the parent's generated
class, so Unreal sees a real class hierarchy; the child owns only the members it
declares. `Super.Method(...)` calls the parent's implementation.

## Example

```das
// ue_inheritance_base.das (public module) declares BaseCounter : Actor
require ue_inheritance_base

[ue_class(module_id="inheritance",Blueprintable)]
class ChildCounter : BaseCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Bonus:int=3

    [ue_function(BlueprintCallable,BlueprintEvent)]
    def override Compute(input:int):int {
        return Super.Compute(input)+self.Bonus
    }
}
```

With base `Score=10`, `Compute(2)` runs the base once (Score becomes 12) and adds
`Bonus=3`, returning 15. A call through a typed `BaseCounterRef` still dispatches
to the child override.

Examples: `ue_inheritance_base.das`, `dsl_inheritance_child.das`,
`dsl_inheritance_grandchild.das`, `inheritance_reference_probe.das` in
`Plugins/DasForUE/Content/Das/Scripts/`.

## Roots and parents

- The root of a hierarchy is a native host marker: `Actor`, `Pawn`, `Character` or
  `ActorComponent` (see [Classes](Classes.md#choosing-the-parent)). Authored
  Actor/Pawn/Character and authored ActorComponent hierarchies are supported.
- The parent is imported as a module (`require ue_inheritance_base`). Put parent
  and child in **separate source files**: each compiled script has one exported
  lifecycle entry per kind.
- On import the editor creates or reuses the parent Blueprint as `BP_<ParentName>`
  in the child's folder, verifies its stable identity and saves newly created
  parents so they can be loaded and cooked. Name/path conflicts fail through the
  normal asset-ownership checks.
- Maximum depth is 32; cycles are rejected. Changing the parent of an existing
  authored Blueprint, or applying explicit structural field migration to a
  hierarchy, is rejected.

## Overriding methods

| Method kind | Overridable? |
| --- | --- |
| `[ue_function(BlueprintCallable)]` | Final unless used as a callback in its declaring class (see [handler references](HandlerReferences.md)); `BlueprintEvent=false` keeps a callback explicitly final |
| `[ue_function(BlueprintEvent)]` (optionally with `BlueprintPure`) | Yes, by an authored child (`def override`) or a Blueprint child |
| lifecycle hooks | Yes, with `def override` |
| unannotated script-only helpers | Yes, within the compiled script (see below) |

Callback overrides inherit the reflected event contract and Blueprint visibility automatically; ordinary reflected overrides still declare their annotation. An override must keep `BlueprintEvent`, parameter names, types, directions,
return type, purity and `const`:

```das
[ue_function(BlueprintPure,BlueprintEvent)]
def override const Query(value:int=2):int {
    return Super.Query(value)+5
}
```

Field shadowing, overriding a final method and incompatible event signatures are
rejected when the effective class is resolved.

- `self.Event(...)` dispatches to the most derived implementation (including a
  Blueprint child's override).
- `Super.Method(...)` selects the immediate authored parent's UFunction and runs
  its body on the same UObject through the normal checked call path, including
  out/ref outputs. It bypasses the child's override.
- Parent and child script contexts are separate but share the object's reflected
  fields. They are released on Destroy/EndPIE.
- Inherited reflected methods are callable on the child; typed base references
  accept derived objects.

## Inherited lifecycle

```das
require ue_life_actor_base
[ue_class(module_id="lifefamily", Blueprintable)]
class ChildLifeActor : BaseLifeActor {
    def override BeginPlay():void {
        Super.BeginPlay()
        // Child initialization follows parent initialization.
    }
    def override EndPlay(reason:int):void {
        // Child cleanup can precede parent cleanup.
        Super.EndPlay(reason)
    }
    // Omitted Tick automatically invokes the authored parent's Tick.
}
```

- An override replaces the parent's hook; call `Super` where parent behavior is
  needed. An omitted hook forwards to the parent automatically, through any
  number of ancestors.
- `BeginPlay` and `EndPlay` are delivered at most once per session. `Tick` runs for
  every explicit call, so call `Super.Tick(delta)` once if you want one parent tick.
- `Super.Tick` requires the parent's `BeginPlay` to have run; it does not
  initialize a parent the child skipped. A parent that never began gets no `EndPlay`.
- The same rules apply to derived ActorComponents. `Initialize` and
  `ConstructionScript` have their own chaining rules; see [Lifecycle](Lifecycle.md).

Examples: `ue_life_actor_base.das` / `dsl_life_actor_child.das`,
`ue_life_behavior_base.das` / `dsl_life_behavior_child.das`,
`ue_life_tick_base.das` / `dsl_life_tick_child.das` (Tick-only parent).

## Inherited event subscriptions

Parent and child may each subscribe to the same Unreal delegate:

```das
[ue_event(source="self",name="OnDasSignal")]
def ChildSignal(value:int):void { self.ChildSignals+=value*10 }
```

- The parent handler lives in the parent's session, started by `Super.BeginPlay`
  (or by automatic forwarding when BeginPlay is omitted). A parent with events but
  no BeginPlay/EndPlay still gets its subscriptions initialized and cleared.
- An overridden `BeginPlay` that does not call `Super` deliberately skips parent
  subscriptions.
- Handlers on both levels are additive and run once per subscription; do not rely
  on their relative order. Redefining an inherited handler with the same name is
  rejected; use a distinct name.

## Components in a hierarchy

- A child inherits the parent's SCS components and root; it does not recreate them.
- A child can add its own native scene or authored components. Scene nodes attach
  to the effective root (possibly inherited); `@Attach="Name"` may name an owned or
  inherited scene component. A child may choose a root only if no ancestor declares one.
- Validation covers the whole hierarchy: one unattached root, at most 32 components,
  no cycles, no missing attachment parents and no member-name collisions.
- Defaults on inherited components use the ordinary `ue_defaults` block, without
  redeclaring the component. They are stored as overrides keyed by the parent's
  node ID; the parent's template is never modified. Root, attachment, type and
  metadata stay with the declaring parent.

```das
[ue_class(module_id="lifefamily", Blueprintable)]
class ChildLifeActor : BaseLifeActor {
    components=ue_components(VisibleAnywhere, BlueprintReadOnly) {
        var Extension:SceneComponentRef
        var @Attach="Extension" Marker:SceneComponentRef
        var ExtraBehavior:ChildBehaviorRef
    }
    defaults=ue_defaults() {
        self.Behavior.Bonus=55          // inherited component
        self.Pivot.bHiddenInGame=true   // inherited component
        self.Extension.RelativeLocation=ue_vector(0.0lf,0.0lf,25.0lf)
        self.ExtraBehavior.Bonus=77
    }
}
```

Not supported:

- Replacing an inherited component with a subclass. UE 5.8's
  `InheritableComponentHandler` requires an override template to have exactly the
  original template's class, and native class overrides only apply to native
  default subobjects.
- Changing inherited component composition or component-default overrides on an
  already imported class; body-only reimport works. Create a new Blueprint.

Defaults on inherited properties are covered in
[Defaults](Defaults.md#inherited-properties).

## Script-only helpers in a hierarchy

Unannotated instance methods are ordinary daScript helpers: no UFunction,
Blueprint graph or IR entry. They support `override` and `Super` too:

```das
// BaseCounter
def Offset(value:int):int { self.Score+=value;return self.Score }
def Template(value:int):int { return self.Offset(value)+1 }

// ChildCounter : BaseCounter
def override Offset(value:int):int { return Super.Offset(value)+self.Bonus }

// GrandCounter : ChildCounter
def override Offset(value:int):int { return Super.Offset(value)+self.GrandBonus }
[ue_function(BlueprintCallable)]
def LocalProbe(input:int):int { return self.Template(input) }
```

With `Score=10`, `Bonus=3`, `GrandBonus=5`, `LocalProbe(2)` returns 21 and leaves
`Score=12`: the inherited `Template` calls `GrandCounter.Offset`, whose `Super`
chain follows Child and Base.

- Return and parameter types must match on override. Static and ref-returning
  helpers are not supported.
- Dispatch happens **within one compiled script context**. An inherited reflected
  UFunction runs the parent script and uses the parent's helper table; calling
  the child's own reflected entry uses the child's effective helper table.
  External typed references expose reflected methods, not helpers.

## Limits

- Constructors: daScript class constructors are not UObject constructors. Use
  `Initialize` (once per instance) or `ConstructionScript` (repeatable); see
  [Lifecycle](Lifecycle.md).
- Arbitrary C++ virtual overrides are not exposed.
- Parent assets use deterministic package names; choosing an arbitrary parent asset
  from another registry is not supported.
- Ancestor-aware class-handle filtering in actor/component queries is limited to
  the typed object checks.

See also: [Classes](Classes.md) · [Lifecycle](Lifecycle.md) ·
[Components](Components.md) · [Defaults](Defaults.md) · [Functions](Functions.md) ·
[Delegates and events](DelegatesAndEvents.md)
