# Arrays of native math values

array<UEVector>, array<UERotator> and array<UETransform> are supported as
ue_class properties, ue_struct members and reflected method input/result/ref/out
values. Unreal sees native arrays of FVector, FRotator and FTransform; internal
array carrier structs are transport only.

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Positions : array<UEVector>

[ue_function(BlueprintCallable)]
def CopyPositions(values : array<UEVector>) : array<UEVector> {
    return <- clone(values)
}

// Inside a ue_class method:
push(self.Positions, ue_vector(1.0lf, 2.0lf, 3.0lf))
self.Positions[0].x += 4.0lf

// Ordinary functions with an imported authored reference:
var copy = api.CopyPositions(api.Positions)
api.Positions := copy
```

The same syntax applies to rotators and transforms. Entire transform values can
be assigned; transform subfield projection remains limited to the existing API.
Nested array members of authored structs use the same converter, including paths
such as self.Groups[0].Path[0].y.

Dynamic array carriers reuse the existing struct-array mechanism with an explicit
allowlist for the three supported native math paths. Carrier names encode the
element path and retain exact native layout/identity checks. Arbitrary native
UScriptStruct arrays are still rejected. Existing authored struct arrays are
unchanged.

Getters/results are owned values. Exact nominal element types prevent mixing
vectors, rotators and transforms. Readonly fields reject assignments/operations.
Packing validates each candidate before publication; non-finite math failure
preserves earlier native data. Existing aggregate element/storage limits apply.

Defaults remain implicit empty values. Enum/object/interface arrays, additional
math types, new map/set families, structural migrations, Shipping and broad AOT
coverage are separate work.

Fixtures: ValueTarget.MathArrays, CopyVectors/Rotators/Transforms and
RefVectors/Rotators/Transforms; MethodGroup.Path.
Validation: DSLMathArraysValidation.md.
