# Static class and component defaults

`ue_defaults` describes reflected defaults. It is consumed before daScript type
inference; it does not run at Initialize, BeginPlay or ConstructionScript.

```das
properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
    var Score:int = 0
    var SpawnType:ActorClass
    var Visual:ObjectRef
    var Pose:UETransform
}
defaults = ue_defaults() {
    self.Score = 31
    self.SpawnType = ue_class_default("/Script/Engine.StaticMeshActor")
    self.Visual = ue_asset_default("/Engine/BasicShapes/Cube.Cube")
    self.Pose = ue_transform(ue_vector(10.0lf,20.0lf,30.0lf),
        ue_rotator(0.0lf,45.0lf,0.0lf), ue_vector(2.0lf,2.0lf,2.0lf))
    self.Mesh.StaticMesh = ue_asset_default("/Engine/BasicShapes/Cube.Cube")
}
```

`self.Property` configures an own reflected property on the generated class CDO.
It follows the existing property initializer contract: scalar literals, name/enum
literal forms, Vector2D/Vector/Rotator/Transform, and supported reflected structs.
A block assignment takes precedence over that property's inline initializer.
Repeated block assignments, unknown/private fields, component reference replacement
and inherited property overrides are rejected. Class inheritance still inherits the
parent's defaults; child overrides need a separate policy.

`self.Component.Property` configures an SCS component template. Native and authored
components accept editable supported scalar/math/struct values, class references
and asset references. It changes neither the component class CDO nor a sibling
template. Existing root/attachment declarations remain the component graph source.

Math constructors accept numeric literals; Transform accepts literal translation,
rotation and scale constructors. `ue_transform_identity()` remains supported.
For an ordinary typed reflected struct use `ue_default_text("(Field=Value,...)")`;
the type comes from the property, and the existing strict struct-default parser
validates fields, ranges and supported nested values. A UEStructValue property
continues to use `ue_struct_default("/Script/Module.Type", "(...)")`.

`ue_class_default` and `ue_asset_default` are declaration markers with one literal
top-level object path, not runtime loading functions. Preflight loads and validates
the target against the declared property class or MetaClass. Asset defaults require
assets, excluding Actors, ActorComponents, CDOs and UClass values. Class defaults
may name abstract classes, as class references can; creation still has its own
checks. Non-null authored-object reference defaults are deferred until an authored
identity/ownership contract exists. Empty reference defaults retain existing null
semantics; implicit class defaults retain the existing editor class-picker behavior.

References are written to ordinary reflected CDO/template properties. Unreal saves
hard references and discovers cook dependencies from them. They are not context-local
VM handles or a request to load a path during gameplay. Existing owner checks validate
the declaring CDO without forbidding legitimate per-instance edits.

The existing authoring model holds these values: own defaults in property `default`,
component defaults in the component defaults map. Vector/Rotator component defaults
keep their previous wire spelling for unchanged-source reimport compatibility.
This data-only stage does not execute arbitrary daScript code. The subsequent
computed-defaults stage adds audited pure scalar expressions/helpers; see
[DSLComputedDefaults.md](DSLComputedDefaults.md).
Asset loading follows ordinary Unreal asset-loading behavior.

Changing defaults on an already imported class still follows the existing migration
guards. This extension does not overwrite edited instances or implement default
migration for component-bearing classes by itself. The later safe-merge stage
implements that migration. Container default syntax remains separate work.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_extended_defaults.das.
Reproduction: Scripts/TestExtendedDefaults.ps1, with -PIE for real editor play and -Package for a
Win64 Development NullRHI cook/package and runtime probe.

## Validation — 2026-10-16

Create and fresh-process Reopen passed in Editor and Game worlds, including JSON
roundtrip, own CDO defaults, native/authored template values, unchanged component
class CDO/sibling, strong asset/class references and unchanged body-only reimport.
Six negative fixtures reject duplicate/unknown assignments, executable expressions,
incompatible classes/assets and missing assets before creating a Blueprint package.

Real DasForUE.Defaults.RuntimePIE passed: one success, zero warnings/failures.
Editor and packaged foundation passed with zero failures. The existing native
component automation passed with one success and zero warnings/failures.

The final Win64 Development NullRHI cook/package and runtime probe passed without
engine errors/ensures. Cube was included as a saved reflected dependency. The probe
checks both saved class defaults and newly spawned Actor/component values, including
Transform, reflected struct text, typed class values, native mesh defaults and a
reflected daScript call. Shipping/rendered behavior was not exercised.

Integration fixes: root Transform Blueprint defaults use the dedicated Euler pin
format; CDO matching accepts exactly the corresponding quaternion conversion,
without a numerical tolerance. Template writes notify native editor property handlers
so component caches stay consistent. Native asset markers require strong object/class
properties; soft/weak references need separate handling. Packaged test iteration uses
an Actor-array snapshot because its probe spawns an Actor.
Safe default-only reimport and preservation of manual values: see [DSLDefaultsMigration.md](DSLDefaultsMigration.md).

Defaults on inherited authored properties: see [DSLInheritedPropertyDefaults.md](DSLInheritedPropertyDefaults.md).

Computed scalar expressions/helpers: see [DSLComputedDefaults.md](DSLComputedDefaults.md).
