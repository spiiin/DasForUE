# Built-in Types

This page lists the daScript value types that map directly to Unreal property and Blueprint pin types: scalars, strings, names, text, bytes, the core math structs and gameplay tags. You can use these types in `ue_class` properties, `ue_struct` members, method parameters and results, and in containers, without declaring or registering anything.

## Type mapping

| daScript type | Module to `require` | Unreal C++ type / FProperty | Blueprint pin | Notes |
| --- | --- | --- | --- | --- |
| `int` | `ue_dsl` | `int32` / `FIntProperty` | Integer | |
| `float` | `ue_dsl` | `float` / `FFloatProperty` | Float (single precision) | 32-bit. Different from `double`. |
| `double` | `ue_dsl` | `double` / `FDoubleProperty` | Float (double precision) | Write literals with the `lf` suffix (`1.5lf`). Kept at full 64-bit precision. |
| `bool` | `ue_dsl` | `bool` / `FBoolProperty` | Boolean | Read and written through the normal `FBoolProperty` API, so bitfield bools work. |
| `uint8` | `ue_dsl` | `uint8` / `FByteProperty` (no enum) | Byte | The built-in daScript unsigned byte. Enums are a separate nominal type, see [Enums](Enums.md). |
| `string` | `ue_dsl` | `FString` / `FStrProperty` | String | Copied in both directions. Embedded NUL characters are rejected. |
| `UENameValue` | `ue_dsl` | `FName` / `FNameProperty` | Name | Nominal type, not a string alias. |
| `FText` (alias of `UETextValue`) | `ue_text` | `FText` / `FTextProperty` | Text | Keeps the native FText history (namespace, key, source). |
| `UEVector` | `ue_dsl` | `FVector` (`/Script/CoreUObject.Vector`) | Vector | Double-precision `x`, `y`, `z`. |
| `UEVector2D` | `ue_dsl` | `FVector2D` (`/Script/CoreUObject.Vector2D`) | Vector 2D | Double-precision `x`, `y`. |
| `UERotator` | `ue_dsl` | `FRotator` (`/Script/CoreUObject.Rotator`) | Rotator | `pitch`, `yaw`, `roll` in degrees. |
| `UETransform` | `ue_dsl` | `FTransform` (`/Script/CoreUObject.Transform`) | Transform | Opaque. Rotation is stored as a quaternion. Use the helpers below. |
| `GameplayTag` | `ue_gameplay_tags` | `FGameplayTag` | Gameplay Tag | Edited with the standard tag picker. See [GameplayTags](../Data/GameplayTags.md). |
| `GameplayTagContainer` | `ue_gameplay_tags` | `FGameplayTagContainer` | Gameplay Tag Container | |
| `SoftObject`, `SoftActorClass`, `$UESoftObject<T>`, `$UESoftClass<T>` | `ue_soft_references` | `TSoftObjectPtr` / `TSoftClassPtr` | Soft Object / Soft Class Reference | See [Soft references](SoftReferences.md). |
| Other loaded native value structs (for example `LinearColor`) | `ue_dsl` | the native `UScriptStruct` | that struct | Use `UEStructValue` snapshots, or the generated wrappers from `ue_api_math`. See [Structs](Structs.md#native-unreal-structs-uestructvalue). |

Object and class references are covered in [Object and class references](ObjectAndClassReferences.md), and enums and authored structs on their own pages.

**Not available as authored types:** `int64`, `uint32` and other integer widths besides `int`/`uint8`, `FQuat`, `FColor` as a built-in, fixed-size C++ arrays, and arbitrary pointers. Unreal `int64` properties are not exposed in the generated native API either.

## Where the types can be used

All the types in the first table (except as noted on their own pages) can be used in:

| Position | Supported |
| --- | --- |
| `ue_class` properties (`@ue_property` or `properties=ue_properties(...) { ... }`) | Yes |
| `ue_struct` members | Yes |
| `ue_function` parameters and return values | Yes, up to 16 parameters per method |
| Mutable `var x:T&` (inout) and `@ue_out var x:T&` (out) parameters | Yes |
| Authored interfaces, delegates, multicast events, `ue_event` subscriptions, captured timer arguments | Yes for the scalar, string, name, text, byte and math types |
| Arrays, map values | Yes. See [Containers](Containers.md) |
| Map keys and set elements | `uint8`, `int`, `string`, `UENameValue` only (plus enums and object keys). `FText`, math types and soft references cannot be keys. |

Parameter names and order from the `.das` source become the Blueprint pin names and order. The return type must be written explicitly, including `:void`. A method can mix any of these types:

```das
options gen2
require ue_dsl

[ue_class(Blueprintable)]
class MathBridge {
    [ue_function(BlueprintCallable)]
    def Shift(value:UEVector; delta:double):UEVector {
        return ue_vector(value.x + delta, value.y, value.z)
    }
    [ue_function(BlueprintCallable)]
    def Build(translation:UEVector; rotation:UERotator; scale:UEVector):UETransform {
        return ue_transform(translation, rotation, scale)
    }
}
```

## Scalars and strings

```das
options gen2
require ue_dsl

[ue_class(module_id="text", Blueprintable)]
class TextCounter {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Label : string = "Hello"
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Precision : double = 1.23456789012345lf
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Key : UENameValue = ue_name_value("Initial_7")

    [ue_function(BlueprintCallable)]
    def Echo(value:string; suffix:string):string {
        self.Label = value + suffix
        return self.Label
    }
    [ue_function(BlueprintCallable)]
    def Rename(value:UENameValue):UENameValue {
        self.Key = value
        return self.Key
    }
}
```

- **Strings.** An `FString` argument is copied into the script's string heap. A returned string is copied back into an `FString`. Changing the C++ buffer later does not affect a stored script string. Actor `FString` properties own their memory.
- **Doubles** travel in a 64-bit slot with no intermediate `float`. Default values must be finite.
- **Defaults** for these types are literals: string literals, numeric literals (`lf` for double), `true`/`false`, and `ue_name_value("literal")` for names. For computed defaults and the `ue_defaults()` block, see [Defaults](Defaults.md).

Examples: `Plugins/DasForUE/Content/Das/Scripts/dsl_scalar_methods.das`, `dsl_mixed_methods.das`, `dsl_text_methods.das`.

## Names (`UENameValue`)

`UENameValue` stands for an Unreal `FName`. Because it is a separate type, you cannot pass a `string` where a name is expected by accident.

- `ue_name_value(text)` creates a name. `ue_name_text(value)` returns its text. `ue_names_equal(a, b)` compares with Unreal name rules: equality ignores case, number suffixes such as `Item_7` work as in Unreal, and `None` and the empty name are the same.
- The `==` and `!=` operators are not defined for `UENameValue`. Use `ue_names_equal`.
- A name is a token owned by the current script context, and works only while that context is active. One context can hold up to 65,536 distinct names. The token is not a pointer, and another context does not accept it. Do not store raw token integers as persistent IDs.
- The generated `ue_generated::UEName` wrapper is a separate type. Convert with `ue_name_text` / `ue_name_value`.

## Text (`FText`)

`require ue_text` to get the `FText` alias and its constructors:

```das
require ue_text

[ue_class(Blueprintable)]
class Example : Actor {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Priority:uint8 = uint8(255)
        var Title:FText = ue_loctext(ExampleTitle, "Example")
    }

    [ue_function(BlueprintCallable)]
    def Update(var priority:uint8&; @ue_out var title:FText&) {
        priority = uint8(42)
        title = self.Title
    }
}
```

- `ue_loctext(Key, "source")` creates localized text using the module namespace and an identifier key. `ue_text_invariant(source)` creates culture-invariant text. `ue_text_to_string(value)` returns the display string. Use it only when you really need a string.
- A text default must be a literal `ue_loctext(...)` or `ue_text_invariant(...)`. Defaults are stored as `NSLOCTEXT(...)` / `INVTEXT(...)`, so the localization identity survives save, reopen and cook. Translation gathering is a separate Unreal localization step. Declaring text does not create translations.
- Blueprint frames carry a real native `FText` that the frame owns, never a display-only string.

## Bytes (`uint8`)

`uint8` maps to a raw `FByteProperty` with no enum attached. A byte default can be an integer literal or `uint8(literal)` in the range 0–255. An out-of-range default is rejected when the declaration is checked. A runtime `uint8(...)` conversion follows ordinary daScript narrowing rules. Typed accessors work from ordinary functions as well, for example `actor.Priority = uint8(7)`.

Examples: `Plugins/DasForUE/Content/Das/Scripts/dsl_text_byte.das`, `dsl_text_byte_calls.das`, `dsl_text_byte_containers.das`.

## Math types

```das
[ue_class(Blueprintable)]
class Mover {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Position : UEVector = ue_vector(1.0lf, 2.0lf, 3.0lf)
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Orientation : UERotator = ue_rotator(10.0lf, 20.0lf, 30.0lf)
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Pose : UETransform = ue_transform_identity()
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Aim : UEVector2D = ue_vector2d(0.0lf, 0.0lf)

    [ue_function(BlueprintCallable)]
    def Step():void {
        self.Position.x += 4.0lf
        self.Orientation.yaw += 5.0lf
        self.Pose = ue_transform(ue_vector(8.0lf, 9.0lf, 10.0lf),
                                 ue_rotator(0.0lf, 0.0lf, 0.0lf),
                                 ue_vector(2.0lf, 2.0lf, 2.0lf))
    }
}
```

Constructors and helpers:

- `ue_vector(x, y, z)`, `ue_vector2d(x, y)`, `ue_rotator(pitch, yaw, roll)`. All arguments are `double`.
- `ue_transform(translation, rotation, scale)` uses the native Unreal constructor. `ue_transform_identity()`, `ue_transform_translation()`, `ue_transform_rotation()`, `ue_transform_scale()` and `ue_transform_position(transform, point)` call the Unreal math API.
- No arithmetic operators (`+`, `*`, ...) are defined for these types. Work on components, or `require ue_api_math` for the generated KismetMathLibrary wrappers, such as `ue_math_Add_VectorVector(a, b)`. See [API packages](../../Api/README.md).

Semantics:

- These are real native struct properties and pins. Each type has its own identity: you cannot pass a Vector where a Rotator is expected, even though both have three doubles.
- Values are copies, never references into Unreal memory. Inside a `ue_class` method, `self.Position.x = ...` and compound assignments on Vector/Vector2D/Rotator components update the property directly. Through another object's typed reference, read a copy, edit it and assign the whole value back (`var p = api.Position; p.x += 1.0lf; api.Position = p`).
- `UETransform` internals are opaque. Change a transform by assigning a whole value built with the helpers.
- Defaults: if you omit the initializer, Vector, Vector2D and Rotator start at zero and Transform starts at identity. An explicit default must be a literal `ue_vector`/`ue_vector2d`/`ue_rotator` with finite components, or `ue_transform_identity()`. Initialize local variables explicitly, especially transforms. An uninitialized local `UETransform` is not guaranteed to be identity.
- Validation: NaN/Inf components, transforms with an unnormalized rotation, and transformed-position overflow are rejected at the boundary. Invalid values are never returned to Blueprint. `readonly`/`BlueprintReadOnly` fields reject writes.
- Enhanced Input `Axis2D` actions deliver `UEVector2D`. Bind them to a `void` callback that takes `(value:UEVector2D)`. See [Input](../Gameplay/Input.md).

Examples: `Plugins/DasForUE/Content/Das/Scripts/dsl_math_methods.das`, `dsl_vector2d.das`, `dsl_method_value_target.das`.

## Gameplay tags

`require ue_gameplay_tags`. `GameplayTag` and `GameplayTagContainer` fields are real tag properties. They always start empty: initializers are rejected. Set them in code with `ue_tag("Item.Weapon.Sword")` / `ue_tags(...)`. Tags work as fields, parameters, results and `ref` parameters. Map values of tag type are not supported. For the full API, see [GameplayTags](../Data/GameplayTags.md).

## Ownership and reload

- Values that cross the Unreal boundary are copied into storage owned by the host. A destroyed or reloaded script context does not change values already returned to Unreal.
- A VM global that holds a string, name or math value is re-initialized normally when a new valid version of the script is loaded. Arbitrary globals are not migrated. If a reload fails, the previous version and its globals stay in use. See [Apply and hot reload](../Workflow/ApplyAndHotReload.md).
- Changing the type or default of a property that already exists needs a migration. See [Migrations](../Workflow/Migrations.md).

See also: [Structs](Structs.md) · [Enums](Enums.md) · [Containers](Containers.md) · [Soft references](SoftReferences.md) · [Defaults](Defaults.md) · [Functions](Functions.md) · [Type support matrix](../../Reference/TypeSupport.md)
