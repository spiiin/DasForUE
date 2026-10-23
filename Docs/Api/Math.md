# Math Package (ue_api_math)

`ue_api_math` wraps Unreal's `KismetMathLibrary` as plain daScript functions, using the canonical math types shared with the rest of the plugin. It is installed in `Plugins/DasForUE/Content/Das/Scripts/` and generated from `Bindings/Packages/CoreMath.profile.json`.

```das
options gen2
require ue_api_math

[export]
def main(input:int):int {
    let position : UEVector = ue_vector(1.0lf,2.0lf,3.0lf)
    let doubled : UEVector = position + position
    return int(doubled.x)
}
```

## What you get

- About 620 of the 741 reflected `KismetMathLibrary` functions, all with the `ue_math_` prefix and the reflected function name: `ue_math_Add_IntInt`, `ue_math_MakeRotator`, `ue_math_VInterpTo`, `ue_math_RandomFloatInRange`, `ue_math_MaxOfIntArray`, …
- Scalar, vector, rotator, transform, color, array and multi-output functions.
- The module publicly imports `ue_foundation`, so `UEVector`, `ue_vector`, `ue_rotator`, `ue_transform_translation` etc. are available after one `require`.

## Canonical math types

The profile uses `value_types=canonical_math`:

| Unreal struct | daScript type | Fields / constructor |
| --- | --- | --- |
| `FVector2D` | `UEVector2D` | `x`, `y`; `ue_vector2d(x,y)` |
| `FVector` | `UEVector` | `x`, `y`, `z`; `ue_vector(x,y,z)` |
| `FRotator` | `UERotator` | `ue_rotator(pitch,yaw,roll)` |
| `FQuat` | `UEQuat` | `x`, `y`, `z`, `w`; `ue_quat(rotator)` or `ue_quat(x,y,z,w)` |
| `FLinearColor` | `UELinearColor` | `r`, `g`, `b`, `a` floats; `ue_linear_color(r,g,b,a)` |
| `FColor` | `UEColor` | `r`, `g`, `b`, `a` bytes; `ue_color(r,g,b,a)` |
| `FTransform` | `UETransform` | `ue_transform_translation(t)` and other foundation helpers |

These are the same types used by native class bindings, authored properties, defaults and `self`, in arguments, results, multiple outputs, ref/inout, arrays and fields of other structs. No duplicate `S_Vector_*`/`S_Transform_*` types are generated. Values must be finite; a Transform must have a normalized rotation, and its quaternion is copied as a whole (no Euler round-trip).

Other native structs such as Box keep generated transport types with `f_` fields.
Quat, LinearColor and Color now use their canonical foundation types. For example,
`ue_math_MakeColor(...)` returns `UELinearColor` with `.r` / `.a`, and
`ue_math_Conv_RotatorToQuaternion(...)` returns `UEQuat`. Their array elements and
nested properties use the same copied native snapshots. Existing generated transport
mode remains an explicit generator choice, without implicit conversions between views.
See [built-in type operations](../Guide/DSL/BuiltinTypes.md#quaternion-conversions-uequat).

## Examples

```das
let a = ue_math_MakeVector(1.0lf,2.0lf,3.0lf)
let split = ue_math_BreakVector(ue_math_Add_VectorVector(a,a))     // split.out_x == 2.0lf
let unit = ue_math_Vector_Normalize(ue_math_MakeVector(3.0lf,0.0lf,0.0lf),0.0001)

let transform = ue_math_MakeTransform(a,ue_math_MakeRotator(0.0,0.0,0.0),ue_vector(1.0lf,1.0lf,1.0lf))
let parts = ue_math_BreakTransform(transform)                      // out_location, out_rotation, out_scale

var values : array<int>
push(values,3)
push(values,42)
push(values,7)
let maximum = ue_math_MaxOfIntArray(values)                        // out_indexofmaxvalue == 1, out_maxvalue == 42

var vectors : array<UEVector>
push(vectors,ue_vector(2.0lf,4.0lf,6.0lf))
push(vectors,ue_vector(4.0lf,8.0lf,12.0lf))
let average : UEVector = ue_math_GetVectorArrayAverage(vectors)
```

Note the parameter types follow Unreal reflection: some functions take `double` (`FInterpTo`), others `float` (`VInterpTo`'s `DeltaTime`).

The built-in types also expose ordinary operators and a small checked helper API;
see [operators and math helpers](../Guide/DSL/BuiltinTypes.md#operators-and-math-helpers).
The generated Kismet functions remain available for the wider API.

## Default arguments

The profile uses `default_arguments=explicit`: pass every argument, including those that have a C++ default (for example the tolerance of `Vector_Normalize`). The original default texts are recorded in the package report; no default is invented or evaluated.

## Name conflicts

If another imported module defines the same name (for example `UEName` or a shared object view), qualify it: `ue_api_math::ue_math_Add_VectorVector(...)`.

## Not included

- Spring-state functions (Float/Vector/Quaternion `SpringState`) and `RuntimeFloatCurve` functions: their structs are opaque to the generator.
- About 110 functions rejected by the reflection exporter (unsupported signatures and policy); each is listed by name with its reason in `Bindings/Packages/ue_api_math.summary.json`.

Regenerate and reinstall with `.\Scripts\GenerateApiPackage.ps1 -SkipBuild -Publish` (Math is the default profile). For the standalone generator, canonical math is enabled with `GenerateBindings.py --canonical-math`.

See also: [API packages](README.md) · [Built-in types](../Guide/DSL/BuiltinTypes.md) · [World / Actor / Components](WorldActorComponents.md) · [Generated bindings](../Reference/GeneratedBindings.md)
