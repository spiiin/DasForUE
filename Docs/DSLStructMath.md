# Native math members in authored structs

Author-defined ue_struct supports native UEVector, UERotator and UETransform
members. Import ue_dsl as usual; builtin math types do not need registry entries.
The containing authored struct still uses the explicit type registry/folder.

```das
[ue_struct(module_id="game", BlueprintType)]
struct Placement {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Position:UEVector = ue_vector(1.0lf, 2.0lf, 3.0lf)
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Orientation:UERotator = ue_rotator(0.0lf, 90.0lf, 0.0lf)
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Pose:UETransform = ue_transform_identity()
}
```

The declaration collector accepts finite numeric
literals for vector/rotator defaults and only identity for transform defaults;
it does not execute arbitrary default expressions. Omitted reflected defaults
are zero vectors/rotators and identity transforms, as for ue_class fields.
For a Transform without an initializer, the macro supplies
ue_transform_identity() so ordinary DAS construction has the same default.

Unreal stores these members as native struct properties inside the authored
UUserDefinedStruct. Default text, reflected layout and exact native identities
are checked on reuse. Nested authored structs and arrays of authored structs
can contain these math members. Method input/result/ref/out adapters reuse the
existing owned snapshots and checked native math pack/unpack operations.

For local values, ordinary member operations work:

```das
var placement = Placement()
placement.Position.x += 1.0lf
placement.Pose = ue_transform_identity()
```

For a struct property accessed through another object's typed reference, edit a
local copy and assign the whole struct back. This stage does not add direct
arrays/maps/sets of native math types. Whole struct assignment remains available.

Example fixtures: ue_method_values.das, dsl_method_value_target.das and
dsl_method_value_driver.das. Their nested MethodPoint.Position and
MethodSettings.Orientation/Pose exercise defaults and reflected method copying.

Validation: [DSLStructMathValidation.md](DSLStructMathValidation.md).

## Nested self component writes

Inside a ue_class method, scalar components of math values in authored struct
properties support direct assignment and compound operations:

```das
self.Settings.Point.Position.x += 1.0lf
self.Settings.Point.Position.y = 8.0lf
self.Settings.Orientation.yaw *= 2.0lf
```

Vector components are x/y/z; Rotator components are pitch/yaw/roll. The macro
resolves every authored struct member and preserves the full reflected path.
It commits through the existing owned snapshot path rather than modifying a
temporary returned value. Readonly roots/members and incompatible scalar types
are rejected. Transform internals are opaque; use its constructors/helpers and
whole value assignment through a local struct copy.

Snapshots validate native math values recursively, including authored children
and array children. Invalid/nonfinite math values and unnormalized transforms
are rejected before publishing a changed snapshot. A failed scalar update leaves
the property unchanged. This does not make earlier statements in a function
transactional. Indexed self container writes still require a local copy and
whole property assignment. Other-object getters for authored struct properties now return their natural
source types; see [DSLPropertySourceValues.md](DSLPropertySourceValues.md).
Use local-copy/whole-assignment syntax for those values.

Validation: [DSLNestedMathWritesValidation.md](DSLNestedMathWritesValidation.md).
