# Native math fields in authored classes

UEVector, UERotator and UETransform may now be declared as ue_class properties:

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Position : UEVector = ue_vector(1.0lf,2.0lf,3.0lf)

@ue_property=(EditAnywhere, BlueprintReadWrite)
Orientation : UERotator = ue_rotator(10.0lf,20.0lf,30.0lf)

@ue_property=(EditAnywhere, BlueprintReadWrite)
Pose : UETransform = ue_transform_identity()
```

These are real native FVector/FRotator/FTransform Blueprint struct properties.
The class uses its existing field annotations, metadata and declaration/default
checks; no type registry is needed for these built-in math structs. Omitting an
initializer gives zero Vector/Rotator and identity Transform. Explicit defaults
currently accept finite literal ue_vector/ue_rotator components and
ue_transform_identity(). Arbitrary runtime expressions/custom Transform default
construction are not compile-time property defaults.

Own-object reads, whole assignment and scalar Vector/Rotator component writes work:

```das
self.Position.x += 4.0lf
self.Position.y = 8.0lf
self.Orientation.yaw += 5.0lf
self.Pose = ue_transform(ue_vector(8.0lf,9.0lf,10.0lf),
                         ue_rotator(0.0lf,0.0lf,0.0lf),
                         ue_vector(2.0lf,2.0lf,2.0lf))
let position = self.Position
```

Existing scalar snapshot commit helpers handle the component writes. readonly
fields reject writes. Transform changes use whole assignment and existing math
helpers rather than exposing raw quaternion/storage fields.

Typed authored views return the corresponding math type and support whole writes
through their standard property operators:

```das
var position = api.Position
position.x += 1.0lf
api.Position = position
api.Pose = ue_transform_identity()
```

Values are copies, not references to Unreal memory. Use the local copy and whole
assignment pattern when changing components through another object's view.
The same math snapshot pack/unpack converters are shared by self and reference
property access. They validate reflected struct identity, finite values and
normalized transforms. Defaults survive Blueprint save/reopen/cook.

Example: dsl_method_value_target.das declares Position, Orientation, Pose and a
readonly zero Vector. dsl_method_value_driver.das exercises self modifications,
typed view reads/writes, native math signatures and all earlier value/container
checks. TestMethodValues.ps1 -Cooked runs Create/Reopen/actual PIE/packaged tests.

This guide covers ue_class fields. Native math members in author-defined
ue_struct are described in [DSLStructMath.md](DSLStructMath.md). Automatic
imports, direct class containers and general migration remain separate.

Validation: [DSLMathFieldsValidation.md](DSLMathFieldsValidation.md).
