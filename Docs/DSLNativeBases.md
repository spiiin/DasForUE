# Native Actor and ActorComponent base syntax

Require ue_dsl and use ordinary DAS class inheritance:

```das
require ue_dsl

[ue_class(Blueprintable)]
class Counter : Actor {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Score:int=0
    }
}

[ue_class(Blueprintable)]
class ScoreComponent : ActorComponent {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Bonus:int=10
    }
}
```

ue_dsl exports empty native-host marker classes Actor and ActorComponent. Their
ue_native_base annotation stores the native Unreal path. Recognition uses the
resolved DAS parent declaration and annotation, not a class-name comparison. A
same-named unannotated class is rejected. Native marker declarations must be empty
root classes, cannot combine declaration annotations, and support exactly the two
existing native hosts. The native UClass must exist and be native/non-interface.

A native root publishes native_parent in the same IR as the old parent annotation;
it does not create a fictitious authored parent_id or an extra Blueprint. Script
helper ancestry stops at native markers. Further authored inheritance still works:

```das
require counter_base
[ue_class(Blueprintable)]
class SpecialCounter : Counter {}
```

Author-defined Super methods/lifecycle, properties, components and inherited defaults
retain their existing semantics. Native C++ virtual methods are not made overridable
by this syntax, and Super at a native root does not acquire new native dispatch.
The existing native lifecycle bridge remains responsible for native chaining.

These DAS declarations are type/host markers, not UObject constructors or object
reference wrappers. Use existing Unreal spawn/component creation APIs and typed
references; constructing a DAS value does not spawn an Unreal Actor/component.
Use ActorRef/$UEObject for object references. Modules that also export short Actor
aliases may qualify the inheritance base as ue_dsl::Actor.

Legacy [ue_class(parent=Actor)]/[ue_class(parent=ActorComponent)] and the implicit
Actor default remain compatible. Combining parent= with any explicit DAS base is
rejected. Existing native-parent restrictions remain; arbitrary native subclasses
require their own backend adapters.

Updated principal examples: dsl_actor_component_owner.das,
dsl_component_behavior.das, dsl_gameplay_actor.das, dsl_gameplay_behavior.das,
ue_life_actor_base.das, ue_life_behavior_base.das, ue_inheritance_base.das,
initialization roots and construction fixtures. Negative/legacy compatibility
fixtures intentionally retain parent= syntax.


## Validation — 2026-10-16

Editor foundation passes with zero failures. Checks cover Actor/ActorComponent IR,
mixed parent syntax, same-named unannotated parents, stateful marker declarations,
unsupported native paths and conflicting declaration annotations. Existing authored
inheritance tests now run with native marker roots. Create/Reopen, real RuntimePIE
(one success, zero warnings/failures), and the Win64 Development NullRHI packaged
gameplay/foundation pipeline pass. Lifecycle, Super, inherited components and
ConstructionScript are exercised through the updated family examples.

Simple examples omit module_id: the default namespace is the source filename
without .das, and the default class ID is <filename>.<class>. An explicit module_id
is useful for intentionally shared/stable namespaces, especially when renaming
source files. Registry/identity/migration examples may retain it for that reason.
