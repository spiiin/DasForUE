# Authored class inheritance and Super

Added 2026-10-11. This is the first authored inheritance slice, with an actual
Unreal Blueprint parent rather than duplicated parent members.

```das
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

The parent must be a declared `ue_class`. A native `parent=Actor` or
`parent=ActorComponent` annotation still selects the root adapter; do not combine it
with `class Child : Parent`. The IR keeps `parent_id` and only owned declarations.
ResolveClass computes the effective fields/methods/interfaces/components, rejects
cycles/depth >32, field shadowing, final-method overrides and incompatible event
signatures. Overriding requires BlueprintEvent on both methods and unchanged
parameter names/types/directions/return type. Callable methods remain final.

The editor creates parent assets in the child's folder as `BP_<ParentName>`, verifies
their stable identities, and saves newly created parents for subsequent package
loading/cooking. Name/path conflicts fail through existing asset ownership checks.
The child Blueprint derives from the parent's GeneratedClass and emits only owned
fields and methods. Parameter names remain canonical on override graph pins.

`self.Event(...)` dispatches normally to the most derived implementation.
`Super.Method(...)` explicitly selects the immediate authored parent's UFunction
and executes its implementation on the same UObject. This bypasses the child's
override and uses the existing typed checked call frame (including outputs), not
a raw VM function pointer. Parent and child script contexts remain separate, share
the reflected object fields, and are released on Destroy/EndPIE. Normal inherited
reflected methods are callable as well. Typed base-object accessors accept derived
objects by checking the native class/CDO identity chain.

Imported authored modules are supported. Generated free functions clear
isClassMethod as well as classParent; retaining the former caused daScript's module
symbol-use pass to dereference a null class parent.

## Current limits

- Actor and ActorComponent inheritance are supported. An Actor inherits its parent's
  SCS components and root; the child does not recreate those nodes. Component fields
  can reference a derived authored ActorComponent, including defaults on inherited
  editable scalar fields.
- Parent source is recorded as `source_script` in IR. A parent no longer needs an
  owned reflected method. Canonical parent package naming remains deterministic;
  arbitrary parent asset selection through another registry is not implemented.
- Derived declarations can add new native scene or authored ActorComponent nodes.
  Child defaults can override inherited templates. Replacing inherited node types
  or migrating composition on an existing imported class remains deferred.
- Parent/child managed event subscriptions are supported. Replacing a parent
  handler with the same name, constructors and general native virtual overrides
  are not included. Script-only helper inheritance is supported within a compiled
  script context, as described below.
- Changing the parent or applying explicit field structural migration to an authored
  hierarchy is rejected. Broad dependent Blueprint/map migration remains deferred.
- Base compatibility currently covers typed object checks/accessors. Broader
  ancestor-aware class-handle filtering and actor/component queries are separate.

## Examples and validation

`ue_inheritance_base.das`, `dsl_inheritance_child.das` and
`inheritance_reference_probe.das`. Base Score starts at 10; Child.Compute(2) calls
the base once (Score=12) and adds Bonus=3 (result=15). Probe adds inherited ReadScore
(12), yielding 27. A subsequent call through a typed BaseCounterRef still dispatches
to the child (Compute(1)=16). Only Bonus is owned by the child Blueprint.

Foundation covers owned/effective declarations and refusal to override a final
callable method. The authored Create/Reopen tests check actual inheritance,
reflected dispatch, Super, base references and a fresh process after saving.

Final validation: TestEditor passed with zero failures; authored Create/Reopen, TestMethodDispatchPIE regression, gameplay PIE and packaged Development all passed. The map retains the parent/child assets as cook dependencies; the packaged game returned 27 through the same Probe/override/Super chain and released its contexts. Rendering was disabled with NullRHI.

## Inherited lifecycle

```das
require ue_life_actor_base
[ue_class(module_id="lifefamily", Blueprintable)]
class ChildLifeActor : BaseLifeActor {
    [ue_lifecycle]
    def override BeginPlay():void {
        Super.BeginPlay()
        // Child initialization follows parent initialization.
    }
    [ue_lifecycle]
    def override EndPlay(reason:int):void {
        // Child cleanup can precede parent cleanup.
        Super.EndPlay(reason)
    }
    // Omitted Tick automatically invokes the authored parent's Tick.
}
```

The same syntax works for derived authored ActorComponents. An overridden hook
replaces the parent's hook; call `Super` where parent behavior is needed. Omitted
hooks forward automatically, including through multiple ancestors. Parent/child
contexts share UObject fields but keep separate session state. BeginPlay and
EndPlay are delivered at most once per session. Tick runs for each explicit call;
call it once if one parent tick is intended. `Super.Tick(delta)` requires the
parent's BeginPlay to have been delivered; it does not implicitly initialize a
parent skipped by the child. A parent that never began receives no EndPlay.

Parent and child lifecycle sources must be separate files because each compiled
script has a single exported lifecycle entry per kind. `ue_life_actor_base.das`,
`dsl_life_actor_child.das`, `ue_life_behavior_base.das` and
`dsl_life_behavior_child.das` form the Actor/component example. Base Actor owns
Pivot and Behavior; its child inherits both and reads Behavior's inherited fields.
The component derives from BaseBehavior and calls Super for all three hooks.

Tests cover owned/effective callbacks and IR source roundtrip, saved assets reopened
in a fresh process, actual PIE and cooked Development lifecycle delivery, component
root/template identity and cleanup. Runtime tests use NullRHI.

Final validation passed: foundation (zero failures), fresh-process Create/Reopen,
PIE (one success, zero warnings/errors), native-component regression and packaged
Development. Tick-only inheritance without a parent UFunction is covered by
`ue_life_tick_base.das` / `dsl_life_tick_child.das`. PIE duplication exposed a cycle
between the native fallback ProbeRoot and the inherited SCS root; root promotion
now leaves that fallback detached, so Unreal can restore its native root during
PostLoadSubobjects without creating a reverse attachment edge.

## Adding components in a child

A child emits only its own SCS nodes. Scene nodes attach implicitly to the effective
root, including an inherited root. `@Attach="Name"` can select either an owned or
inherited scene component. Inherited nodes stay in their parent's construction
script; the child stores Unreal's cross-Blueprint parent reference.

```das
[ue_class(module_id="lifefamily", Blueprintable)]
class ChildLifeActor : BaseLifeActor {
    components=ue_components(VisibleAnywhere, BlueprintReadOnly) {
        var Extension:SceneComponentRef
        var @Attach="Extension" Marker:SceneComponentRef
        var ExtraBehavior:ChildBehaviorRef
    }
    defaults=ue_defaults() {
        self.Extension.RelativeLocation=ue_vector(0.0lf,0.0lf,25.0lf)
        self.ExtraBehavior.Bonus=77
    }
    // Lifecycle overrides use the same Super syntax.
}
```

The example inherits Pivot/Behavior and adds Extension, Marker and ExtraBehavior.
Extension attaches to inherited Pivot; Marker attaches to Extension. The parent's Behavior Bonus remains 42; the child's override sets it to 55,
and ExtraBehavior's Bonus is 77 despite sharing the same authored class.
Validation considers the complete hierarchy: one unattached root, at most 32 total
components, no cycles, missing attachment parents or inherited member collisions.
A child may select a root if its ancestors declare none. Changing an existing
imported composition still requires a future migration policy.

Validation of new child components passed: foundation (zero failures), fresh-process Create/Reopen, PIE (one success, zero warnings/errors) and packaged Development. Both inherited/owned scene attachments, separate authored-component template defaults and BeginPlay/EndPlay delivery are covered.

## Defaults on inherited components

Use the same `ue_defaults` block for owned and inherited component fields:

```das
defaults=ue_defaults() {
    self.Behavior.Bonus=55
    self.Pivot.bHiddenInGame=true
    self.ExtraBehavior.Bonus=77
}
```

No component redeclaration is needed. The IR keeps owned nodes in `components` and
inherited changes in `component_overrides`, keyed by the parent's stable node ID.
The effective view merges defaults, while the parent's declaration remains intact.
The editor creates override templates through `UInheritableComponentHandler` and
`FComponentKey`; it never writes to the parent's SCS template. Other fields inherit
from the best ancestor archetype. Native scene defaults and editable authored
scalar defaults use the existing value/type validation.

Root and attachment remain inherited; an override cannot replace a node's type,
identity or topology. Field metadata/visibility still belong to the declaring
parent. Overrides on existing imported assets currently support body-only reimport;
changing the declared override values is explicitly refused until the composition
migration policy is implemented. This is an initial-import capability.

Inherited-default override validation passed: foundation with zero failures, PIE with one successful test and zero warnings/errors, fresh-process Create/Reopen and packaged Development. Parent templates retain Bonus=42 and native visibility; the child gets Bonus=55 and bHiddenInGame=true while its owned ExtraBehavior retains Bonus=77. The PIE launcher now waits for the editor process itself instead of persistent helper descendants.

## Inherited event subscriptions

Parent and child can each declare a handler for the same Unreal event:

```das
[ue_event(source="self",name="OnDasSignal")]
def ChildSignal(value:int):void { self.ChildSignals+=value*10 }
```

The parent handler lives in its own script session. Calling Super.BeginPlay starts
that session and its subscriptions; omitted BeginPlay forwards automatically. A
parent with events but no BeginPlay/EndPlay hook still receives generated forwarding
in the child, so subscriptions are initialized and cleared. An overridden BeginPlay
that omits Super deliberately skips parent initialization/subscriptions. EndPlay
uses the same explicit/automatic chaining policy; Destroy releases all sessions.
Handlers on both levels are additive and run once per subscription. Do not depend
on their relative delivery order. Redefining the same inherited handler name is
explicitly rejected; use a distinct name to add a handler.

The example subscribes both Actor levels to self.OnDasSignal and both authored
component levels to owner.OnDasSignal. Signal 4 yields BaseSignals=4 and
ChildSignals=40 independently on the Actor and each component. The TickOnly example
also covers a parent without explicit BeginPlay/EndPlay and unsubscribe on EndPlay.

## Component class replacement: engine constraint

Changing an inherited SCS component to a subclass is not implemented. UE 5.8's
InheritableComponentHandler::IsRecordValid requires the override template's class
**to equal** the original template's class. The engine discards mismatched records.
The native ComponentClassOverrides path only supports native default subobjects;
ChangeSubobjectClass tests IsNativeComponent, and Kismet's override validator looks
for the subobject in the parent CDO. Our authored component nodes are SCS nodes.

Verified locally in Engine/Source/Runtime/Engine/Private/InheritableComponentHandler.cpp
(IsRecordValid), Engine/Source/Editor/SubobjectDataInterface/Private/SubobjectDataSubsystem.cpp
(ChangeSubobjectClass), and Engine/Source/Editor/KismetCompiler/Private/KismetCompiler.cpp
(ValidateComponentClassOverrides).

This needs a separate construction backend investigation, e.g. native component
slots/factory data available before actor construction. It cannot be implemented
by changing the handler record or mutating the parent's SCS node. No engine patch,
duplicate hidden node or spawn-time replacement is added in this slice.

Inherited-event validation passed: foundation (zero failures), Create/Reopen, PIE (one success, zero warnings/errors) and packaged Development. Covered additive Actor self events, component owner events, one delivery per handler and cleanup. An imported-parent binder was initially resolved in a child without its own handlers, causing duplicate subscriptions; an explicit empty child binder now prevents this, and the no-BeginPlay/EndPlay parent regression passes.

## Script-only helpers in an authored hierarchy

Unannotated instance methods remain ordinary DAS helpers. No UFunction, Blueprint
function graph or IR method entry is created for them. Each compiled script lowers
its helper hierarchy into internal functions; self calls select the most derived
helper visible in that script and Super selects the next ancestor implementation.
Return and parameter types must match on override; static and ref-result helpers
remain unsupported.

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

With Score=10, Bonus=3 and GrandBonus=5, LocalProbe(2) returns 21 and leaves Score=12.
The inherited Template calls GrandCounter.Offset, then Super follows Child and Base.
Each helper body shares the caller's persistent context and the real UObject fields.
Generated names contain an owner hash to distinguish ancestor implementations; user
code keeps normal method names. Imported helper calls are module-qualified and
already lowered parent Super calls are remapped when cloned into a descendant.

Dispatch is **within the compiled script context**. An inherited reflected UFunction
still executes the parent script and uses the helper table of that parent script.
Calling an own child/grandchild reflected entry executes its effective helper table.
Cross-context virtual dispatch for script-only methods is not implemented; external
typed object references expose reflected methods, not helpers.

## Constructor follow-up contract

Native UObject constructors and a script-session initializer are distinct. AActor's
ExecuteConstruction executes SCS/user Blueprint construction before OnConstruction
(Engine/Source/Runtime/Engine/Private/ActorConstruction.cpp). Construction can repeat
when an editor actor is reconstructed, so running gameplay initialization there
unconditionally would repeat side effects.

The construction contract separates a once-per-instance runtime initializer from
a repeatable editor construction callback. The runtime part is implemented as
Initialize (see below), including parent chaining, instance values and component
availability. Repeatable Actor construction and its side-effect policy are described
in [DSLConstruction.md](DSLConstruction.md). Ordinary DAS class constructors are not UObject constructors.

Helper hierarchy validation passed: foundation with zero failures, three-level Create/Reopen, PIE and packaged Development. LocalProbe(2) returned 21 and Score became 12; Offset/Template remain absent from UFunctions and reflected IR. Temporary AST diagnostics were removed.


## Runtime initializer implementation

The once-per-instance runtime half of the constructor follow-up is implemented as
`Initialize():void`, with omitted-hook forwarding and explicit `Super.Initialize()`.
Delivery survives VM/session replacement. See [DSLInitialization.md](DSLInitialization.md)
for ordering, skipped parent hooks, failures and tests. Repeatable Actor construction
is implemented separately as ConstructionScript; native UObject constructor integration
and component construction callbacks remain separate work.
