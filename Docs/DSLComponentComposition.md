# Actor components declared as fields

```das
require ue_dsl
require ue_component_types
[ue_class(module_id="actorcomponent", Blueprintable)]
class ComponentOwner {
    components = ue_components(VisibleAnywhere, BlueprintReadOnly, Category="Components") {
        // Подсчёт очков
        var Behavior:ScoreComponent
    }
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Score:int=0
    }
    // Methods and lifecycle callbacks use the existing class DSL.
}
```

See [declaration groups](DSLDeclarationGroups.md) for grouped metadata and grammar limits. Individual annotations remain supported.

Name comes from Behavior; source and class come from the imported reference type.
There are no name/script/type strings in the Actor's component declaration.
ue_component_types is a declaration-only module, avoiding execution imports and
cyclic Actor/Component source dependencies:

```das
module ue_component_types public
require ue_dsl public
[ue_object_ref(script="dsl_component_behavior.das", type="ScoreBehavior")]
struct ScoreComponent { handle : UObjectHandle }
```

The type describes a reference to the authored Component class. The field does not
construct a separate daScript component instance. Import creates/reuses the Component
Blueprint and adds its generated class to the Actor SCS. A source reference starts
empty; the collector initializes its source representation automatically. Do not
write an initializer on a component field: Unreal creates the actual subobject.

@ue_component is a singleton true marker. @ue_property accepts VisibleAnywhere or
EditAnywhere and BlueprintReadOnly or BlueprintReadWrite for component references.
Defaults are VisibleAnywhere and BlueprintReadOnly. Category, DisplayName and
ToolTip are separate string annotations. Conflicting/repeated flags and metadata,
unknown metadata, unsupported field types and explicit initializers reject.
VisibleAnywhere makes the reference visible but noneditable in Details; ReadOnly
prevents replacing it from Blueprint, not changing the component's own properties.

Metadata/flags are stored on SCS nodes and transferred to generated properties by
a compiler extension after layout generation, including subsequent compilation.
Component declarations retain automatic stable IDs; optional @ue_id overrides one.
Their authoring JSON includes metadata/flags so reimport cannot silently change
permissions. Class-level ue_component(name/script/type) remains compatibility syntax
for old fixtures, but current examples use fields. Structural reimport stays guarded.

Several fields can reference the same type and produce distinct templates:

```das
@ue_component
Primary : ScoreComponent
@ue_component
Secondary : ScoreComponent
```

Limit: 32 components, direct Actor owner and direct authored ActorComponent type.
Names cannot collide with fields, methods, inherited members or other components.
The editor preflights sources/contracts and protects occupied asset paths.
Save the Actor and dependent Component Blueprints; the executable commandlet does
this before saving/reopening its map. Declaration/default migrations on composed
Actors remain deferred. Existing imported assets whose component metadata changes
require a migration policy rather than silent modification; probes use fresh assets.

Native/SceneComponent definitions, attachments/transforms, per-instance component
defaults, self.Behavior method dispatch and custom constructors remain next.
Examples: dsl_actor_component_owner.das, dsl_component_composition_dual.das,
ue_component_types.das and dsl_component_behavior.das under Content/Das/Scripts.

Validation: [ComponentFieldsValidation](ComponentFieldsValidation.md).

Direct component field access is supported; see [DSLComponentFieldAccess](DSLComponentFieldAccess.md).

Comments immediately above reflected declarations provide default `ToolTip` metadata; see [comment tooltips](DSLCommentTooltips.md). Explicit metadata takes priority.
