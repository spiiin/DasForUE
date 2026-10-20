# Structs

You can declare an ordinary daScript `struct` with `[ue_struct]` to get a real Unreal struct asset (`UUserDefinedStruct`), then use it by its natural type in `ue_class` properties, method signatures and other structs. Existing native Unreal structs that have no built-in mapping are available as `UEStructValue` snapshots.

## Declaring an authored struct

```das
options gen2
require ue_dsl

[ue_struct(module_id="game", BlueprintType)]
struct Settings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count : int = 15
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title : string = "Initial"
}
```

Annotation arguments: `module_id` (the reflected type module), optional `id` (an explicit stable ID such as `id="game.Settings"`), and `BlueprintType`. Each member takes the usual `@ue_property=(...)` flags and metadata. `BlueprintReadOnly` on a member makes writes to it fail to compile.

### Supported member types

| Member type | Default |
| --- | --- |
| `int`, `float`, `double`, `bool`, `string`, `UENameValue`, `uint8`, `FText` | Literal, or the type's zero/empty value |
| `UEVector`, `UEVector2D`, `UERotator` | Finite literal `ue_vector(...)` etc., or zero |
| `UETransform` | `ue_transform_identity()` only, or omitted (identity) |
| Authored `ue_enum` from the same module | **Required** named member, e.g. `Mode:State = State.Active` |
| Another authored `ue_struct` (nesting) | `Inner : Leaf = Leaf()` or omitted. Values come from the child declaration. |
| Typed native object/interface references (`ActorRef`, `ObjectRef`, generated wrappers, native interface types) | Implicit null only |
| `GameplayTag`, `GameplayTagContainer` | Implicit empty only |
| `SoftObject` / `SoftActorClass` and other soft aliases | Implicit empty path only |
| Arrays, maps and sets of supported elements | Implicit empty only. See [Containers](Containers.md#containers-inside-ue_struct). |

Not supported in `ue_struct`: native `[ue_native_enum]` members, references to authored classes (`ue_object_ref` views), weak/lazy/class references, explicit overrides of nested struct initializers, inheritance, and methods. These are refused when the type is applied.

Limits: depth 16 and 1024 scalar leaves per struct tree, 64 fields per struct, 256 reflected types per module. Cycles between by-value structs and missing references are rejected before any asset is created. All referenced types must be declared in the same type module.

### Creating the assets

Struct and enum declarations become Unreal assets through a separate editor command:

```text
Das.DSL.Types.Apply dsl_direct_types.das /Game/DasAuthoredTypes
```

The command compiles the source without running it and creates one asset per declared type in the folder (child structs before parents). It marks them dirty but does not save them. Repeating it with an identical declaration reuses the same assets. If a path is already taken, an identity conflicts, a definition has changed or the native asset has drifted, the command refuses. A struct's internal Unreal property names carry GUID suffixes, so do not assume they equal the source names when you access the native struct from C++.

Then apply the class that uses the types through the normal [Apply workflow](../Workflow/ApplyAndHotReload.md). A class that refers to authored types names the type location with `type_folder="/Game/..."` or `type_registry="/Game/.../TypesRegistry.TypesRegistry"`. See [Modules and type identity](ModulesAndTypeIdentity.md).

## Using structs in a class

```das
[ue_class(module_id="authoredvalues", type_folder="/Game/DasAuthoredTypes", Blueprintable)]
class TypedActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Config : Settings = Settings()

    [ue_function(BlueprintCallable)]
    def Bump(amount:int):int {
        self.Config.Count += amount
        return self.Config.Count
    }

    [ue_function(BlueprintCallable)]
    def Evaluate(mode:State; data:Settings):Settings {
        var result = data
        result.Count += int(mode)
        return result
    }
}
```

A struct property's default is either an empty constructor (`Settings()`) or no initializer. Values are taken from the struct declaration. Overriding a field in the class initializer is not supported.

Blueprint pins and properties use the real authored struct. Conversion is field by field, with no memory-layout equivalence assumed. Strings are copied, and enum identity and values are checked.

### Reading and writing through `self`

- `self.Config` returns an owned local copy.
- Leaf assignment and compound operators (`=`, `+=`, `-=`, `*=`, `/=`) update the native property directly, including deep paths and math components:

```das
self.Config.Inner.Count += amount
self.Settings.Point.Mode = MethodState.Idle
self.Settings.Point.Position.x += 1.0lf
self.Settings.Orientation.yaw *= 2.0lf
```

  The commit reads the latest native value after the right-hand side has been evaluated, so other fields changed by the right-hand side are preserved. A failed update leaves the property unchanged. Earlier statements in the same function are not rolled back.
- Whole assignment `self.Config = value` packs every field.
- Assigning a whole nested member (`self.Config.Inner = other`) is rejected. Copy the root, edit the copy, then assign it back: `var c = self.Config; c.Inner = other; self.Config = c`.
- Indexed writes and container operations inside struct members (`self.Settings.Items[0].Count = 17`, `push(self.Settings.Samples, 2.0lf)`) are supported. See [Self access](SelfAccess.md).
- For enum members, only plain assignment of the exact enum type is allowed. An integer or another enum with the same numbers fails to compile, as does compound arithmetic.
- `readonly` roots and read-only members reject every kind of write.

### Through another object's reference

Typed authored references return natural struct values. Edit a local copy and assign the whole property:

```das
var status = api.Status
status.Mode = State.Idle
api.Status = status
```

See [Authored references](AuthoredReferences.md).

### Local values

Local struct values behave like normal daScript structs. Structs that contain arrays or tables need clone/move semantics:

```das
var data <- clone(self.Config)
var item = Leaf()
item.Count = 42
push_clone(data.Items, item)
self.Config <- data
```

Use `return <- result` to return a moved local. Clone a const input before editing it.

## Value and ownership rules

- Every crossing of the boundary copies. Method inputs, results, `ref`/`out` copyback and property reads all produce independent values, and no reference into native struct memory reaches script.
- Object members share identity: copying a struct copies the reference, not the object. A struct snapshot holds a strong reference to its objects, and the script context reports snapshots to Unreal GC. A plain `ActorRef` variable is weak. Destroyed objects read back as null.
- Name members use context-owned `UENameValue` tokens. Use `ue_name_value`, `ue_name_text`, `ue_names_equal`.
- Math members are validated recursively: non-finite values or unnormalized transforms are rejected before publication.

## Native Unreal structs

Prefer the canonical typed native structs from generated modules:

```das
require ue_native_gameplay

@ue_property=(EditAnywhere, BlueprintReadWrite)
Payload : DasCallPayload = DasCallPayload(Count=10, Label="Initial")

[ue_function(BlueprintCallable)]
def Add(amount:int):int {
    var value = self.Payload
    value.Count = ue_struct_get(value, Count) + amount
    self.Payload = value
    return value.Count
}
```

`ue_struct_get(value, Field)` checks field names and types during compilation and expands to ordinary typed field access. Nested reads compose. Generic `UEStructValue` snapshots remain available for structs without a generated source type; their path-based transport is described in the [developer reference](../../Dev/Runtime/StructSnapshots.md). See [API packages](../../Api/README.md) for generated engine structs.

## Changing a struct later

After the assets exist, changing a definition (adding, removing or retyping members, changing defaults) needs an explicit migration policy. Changing a stored scalar leaf is supported, but changing nested-member or container definitions is refused. See [Migrations](../Workflow/Migrations.md).

Examples: `Plugins/DasForUE/Content/Das/Scripts/dsl_type_declarations.das`, `dsl_direct_types.das`, `dsl_nested_authoring.das`, `ue_method_values.das` (with `dsl_method_value_target.das` / `dsl_method_value_driver.das`), `dsl_struct_fields.das`, `dsl_authored_values.das`. Fixtures that contain `REGISTRY` need your own registry object path in its place.

See also: [Enums](Enums.md) · [Containers](Containers.md) · [Built-in types](BuiltinTypes.md) · [Self access](SelfAccess.md) · [Modules and type identity](ModulesAndTypeIdentity.md) · [Defaults](Defaults.md) · [Migrations](../Workflow/Migrations.md)
