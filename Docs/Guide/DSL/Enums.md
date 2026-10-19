# Enums

Declare an ordinary daScript `enum` with `[ue_enum]` to create a real Unreal enum asset. Use `[ue_native_enum]` to give an existing C++ `UENUM` a typed daScript view. Both kinds are used by their natural type in properties, struct members, method signatures and containers, and the enum's identity is checked everywhere.

## Authored enums

```das
options gen2
require ue_dsl

[ue_enum(module_id="game", BlueprintType)]
enum State {
    Idle = 7
    Active = 42
}
```

Rules:

- Every member needs an explicit, unique integer value in **0..254**. Value 255 and member names ending in `_MAX` are reserved. The asset gets a hidden `MAX` entry.
- Values may be sparse and need not include zero. They are treated as values, never as ordinal indexes.
- The asset is a `UDasDefinedEnum`, a runtime `UEnum` subclass that keeps the explicit values through save, reload and cook. Plain `UUserDefinedEnum` is not used because the editor renumbers its entries. Unreal stores the property as a byte (`FByteProperty`).
- Apply the type assets with `Das.DSL.Types.Apply <script.das> /Game/<Folder>` before applying classes that use them. See [Structs](Structs.md#creating-the-assets) for how that command behaves.

### Member stable IDs

Each member has a stable ID, by default `<EnumStableId>.<MemberName>`. You can set an explicit ID through an annotation argument. The daScript enum grammar has no per-entry annotations, so the argument goes on the enum itself:

```das
[ue_enum(module_id="memberids", BlueprintType,
         member_id_Active="memberids.State.running")]
enum State {
    Idle = 7
    Active = 42
}
```

An unknown member key, an invalid ID or a duplicate ID is an error. Member IDs make a rename possible later (see below). Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_enum_member_ids.das`.

## Native enums

```das
[ue_native_enum(path="/Script/Engine.ESpawnActorCollisionHandlingMethod")]
enum SpawnCollision {
    Undefined = 0
    AlwaysSpawn = 1
    AdjustIfPossibleButAlwaysSpawn = 2
    // ...
}
```

This declares a nominal daScript view of an existing `UEnum`. No asset is created. Member names and numbers must match the native enum, and coverage of its values is checked at compile time. Only byte-sized enums are supported: `enum class : uint8` (`FEnumProperty`) and `TEnumAsByte` (`FByteProperty`). Wider underlying types are rejected.

Native self fields and generated native APIs need exactly one visible `ue_native_enum` declaration for each native enum. A missing or ambiguous declaration is a compile error. The generated native API modules already contain these declarations, for example `ue_native_engine_values.das` and `ue_native_gameplay.das`. Native enums cannot be members of an authored `ue_struct`.

## Using enums

```das
[ue_class(module_id="authoredvalues", type_folder="/Game/DasAuthoredTypes", Blueprintable)]
class TypedActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Mode : State = State.Idle

    [ue_function(BlueprintCallable)]
    def SetMode(value:State):void { self.Mode = value }
    [ue_function(BlueprintCallable)]
    def GetMode():State { return self.Mode }
    [ue_function(BlueprintCallable)]
    def ModeNumber():int { return int(self.Mode) }
}
```

- Enum fields, parameters, results, `ref` and `@ue_out` parameters keep the exact `UEnum` identity on the Blueprint pin.
- Struct members of enum type need a named default (`Mode:State = State.Active`). Integer initializers are not accepted, and there is no implicit zero default.
- **Type safety.** Another enum with identical numbers, a raw integer, or a `UEEnumValue` cannot be assigned to an enum field or passed as an enum argument. This is a compile-time error. At runtime, values outside the declared members, including the reserved `MAX`, are rejected before any write, even if unsafe code forged them.
- In a `ue_class` method you can assign a nested enum member directly, for example `self.Settings.Point.Mode = MethodState.Idle`. The assigned value must be the exact enum type. Compound arithmetic on enum members is not supported, and `readonly` roots reject the write. Through another object's typed reference, edit a local copy and assign the whole property back.

## Enum containers

| Shape | Supported in |
| --- | --- |
| `array<E>` (authored or native) | `ue_class` properties, `ue_struct` members, method input/result/ref/out, native self fields |
| `table<E>` (Unreal `TSet`) | `ue_class` properties, native self fields, method input/result/ref/out |
| `table<E; V>` (enum keys) | `ue_class` properties, native self fields, method input/result/ref/out. Values: int, float, double, bool, string, name, math, authored structs, enums, typed object/interface references |
| `table<int or string; E>` (enum values) | `ue_class` properties, `ue_struct` members (authored enums), method signatures |

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
States : array<MethodState>
@ue_property=(EditAnywhere, BlueprintReadWrite)
Modes : table<SpawnCollision>
@ue_property=(EditAnywhere, BlueprintReadWrite)
Counts : table<MethodState; int>

[ue_function(BlueprintCallable)]
def Touch():void {
    push(self.States, MethodState.Active)
    insert(self.Modes, SpawnCollision.AlwaysSpawn)
    self.Counts[MethodState.Active] += 1
}
```

Sets support `insert`, `erase`, `clear`, `key_exists`, `length`, local `clone` and whole-property `:=`. Inserting a duplicate leaves one element. Native and authored enum storage keeps its actual property form (`FEnumProperty` for native enum classes, `FByteProperty` otherwise). Containers start empty unless a class default is given. See [Defaults](Defaults.md). The general container rules are on the [Containers](Containers.md) page.

Examples: `Plugins/DasForUE/Content/Das/Scripts/dsl_method_value_driver.das` (enum arrays, sets and key maps), `dsl_native_array_self.das` (native enum fields and containers).

## Changing an enum

Enum assets are immutable unless you run one of the explicit migration commands. Each one preflights everything before any change, rolls back fully on failure, and leaves assets dirty for you to save. None of them edits source files. General requirements: the editor is idle, PIE is stopped, and no script contexts or subscriptions are retained. The scope is one imported DSL Actor root and its ordinary Blueprint, map and catalog dependencies (up to 256 packages). World Partition/streaming maps and independent DSL roots are refused.

| Change | Command |
| --- | --- |
| Append new members (existing names, values and order unchanged) | `Das.DSL.Types.ExtendEnums Script.das /Game/Folder/TypesRegistry.TypesRegistry` |
| Append and repair enum nodes in graphs | `Das.DSL.Types.ExtendEnums Script.das /Game/Registry.Registry graphs=repair` |
| Rename members (same member ID and value) | `Das.DSL.Types.RenameEnumMembers Script.das /Game/Registry.Registry` |
| Renumber or remove members | `Das.DSL.Types.MigrateEnumValues Script.das /Game/Registry.Registry Rules.json` |

**Append.** The enum UObject keeps its identity and stable ID. Existing byte values and graph defaults keep their meaning. By default, graphs that contain enum-dependent nodes are refused. With `graphs=repair`, the command refreshes and recompiles Switch on Enum, Enum Literal, Select, Get Num Enum Entries, safe Byte to Enum and For Each Enum nodes. Existing links are kept, and new members get unconnected pins. Unsafe cast nodes are not supported.

**Rename.** Keep the old member ID explicit under the new name, then update source references:

```das
[ue_enum(id="direct.State", module_id="direct", BlueprintType,
         member_id_Running="direct.State.Active")]
enum PlayState {
    Idle = 7
    Running = 42
    Paused = 200
}
```

Member names, enum defaults, Blueprint variable and pin defaults, and Switch/Select option names are updated. Stored numeric values do not change. If you leave out the old automatic ID, the member gets a different ID and the rename is rejected.

**Renumber or remove.** Put a JSON rule file in `Plugins/DasForUE/Content/Das/Scripts`:

```json
{ "schema_version": 1, "rules": [ { "enum_id": "direct.State", "from": 42, "to": 50 } ] }
```

Every changed or removed value needs exactly one rule, and its target must exist in the new enum. Unknown, duplicate or unused rules are rejected. Stored values are remapped for scalar enum fields of the imported Actor, its instances and its CDO. Enum values inside containers or nested structs are refused. Removing a member whose Switch output or Select option is connected is also refused. The command does not combine with additions, renames or reordering.

More detail: [Migrations](../Workflow/Migrations.md).

See also: [Structs](Structs.md) · [Containers](Containers.md) · [Built-in types](BuiltinTypes.md) · [Self access](SelfAccess.md) · [Modules and type identity](ModulesAndTypeIdentity.md)
