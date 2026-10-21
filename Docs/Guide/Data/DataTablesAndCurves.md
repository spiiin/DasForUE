# DataTables and CurveTables

Scripts can read rows of a loaded DataTable as typed structs and evaluate curves of a loaded CurveTable with Unreal's own interpolation. The editor can also create a new DataTable from JSON using a DSL-authored row struct.

Both readers are read-only: they never load assets implicitly and never hand out pointers into native rows or curves. Pass in a table you already loaded (an asset reference property or [asset loading](AssetLoading.md)).

## Reading DataTable rows

```das
require ue_data_tables
require datatable_item_types

if(ue_data_table_has_row(items_table,"Potion")) {
    let item=ue_data_table_row(type<ItemRow>,items_table,"Potion","/Game/ItemTypes")
    print("{item.Label}: {item.Price}\n")
}
let names=ue_data_table_names(items_table)
```

| Function | Behavior |
| --- | --- |
| `ue_data_table_row(type<Row>, table, name [, scope])` | Returns a typed copy of the row. A missing row raises a script error. |
| `ue_data_table_has_row(table, name):bool` | `false` for an absent row. |
| `ue_data_table_names(table):array<string>` | Detached array sorted by FName lexical order; at most 4096 rows. |

- `Row` must be an authored `ue_struct` or a canonical generated `ue_native_struct`. For authored structs, `scope` is the type folder or exact type-registry object path (as for other authored type resolution); native structs need no scope.
- The compiler checks the struct declaration (identity, layout, defaults). At runtime, the table's row struct must be **exactly** that type: a struct with the same fields but a different identity is an error.
- Names use FName semantics (case-insensitive). Empty/`None` names and names over 1024 characters are rejected. Invalid, dead or non-DataTable handles are errors, not empty results.
- The row becomes an owned snapshot and then a normal DAS struct. Later native changes or row removal do not affect a value you already read. Strings and containers are copied; object reference fields keep checked weak-handle semantics (no pinning). The usual snapshot field support and limits apply.
- Reads need an active game-thread VM context. Single-row reads have no table-size limit.

### Example: item table

`Plugins/DasForUE/Content/Das/Scripts/datatable_item_types.das` defines the row:

```das
options gen2
require ue_dsl

// Reflected authoring schema; import once with Das.DSL.Types.Apply.
[ue_struct(BlueprintType)]
struct ItemRow {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Label:string=""
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Price:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Weight:double=0.0lf
}
```

`datatable_items.das` reads it:

```das
def item_price(items_table:UObjectHandle;name:string):int {
    if(!ue_data_table_has_row(items_table,name)) {return -1}
    let item=ue_data_table_row(type<ItemRow>,items_table,name,"/Game/ItemTypes")
    return item.Price
}
```

Import the row schema in the editor console and save the generated `ItemRow` struct:

```text
Das.DSL.Types.Apply datatable_item_types.das /Game/ItemTypes
```

Then either create a DataTable in the Content Browser with that row struct, or create it from JSON as below.

| Row name | Label | Price | Weight |
| --- | --- | ---: | ---: |
| Potion | Health Potion | 25 | 0.5 |
| Sword | Iron Sword | 125 | 3.0 |

## Creating a DataTable from JSON

`Examples/DataTables/Items.json`:

```json
[
  {"Name":"Potion", "Label":"Health Potion", "Price":25, "Weight":0.5},
  {"Name":"Sword", "Label":"Iron Sword", "Price":125, "Weight":3.0}
]
```

```text
Das.DSL.Table.Create /Game/ItemTypes/ItemRow.ItemRow /Game/Items/DT_Items <full path to>/Examples/DataTables/Items.json
```

- The first argument also accepts the package path `/Game/ItemTypes/ItemRow`. Quote the JSON path if it contains spaces.
- The new table appears as a dirty asset. Save both the row struct and the table yourself. The command never saves files, overwrites a package, or changes AssetManager/cook/project settings.
- C++ editor API (`DasDataTableService.h`, DasEditor module; game thread, outside PIE): `FDasDataTableService::CreateFromJson(RowStruct, Json, NewPackage, Table, Error)`. `Table` is cleared on failure.

Validation rules:

| Area | Rule |
| --- | --- |
| Destination | Must be a new valid `/Game/...` package; existing in-memory packages and files are refused. No update/merge mode. |
| Row struct | An up-to-date DSL-authored struct whose saved identity/model, member stable IDs, names and pin types match. Native or hand-made structs are rejected. The DAS source is not reimported. |
| Document | A JSON array (empty array gives an empty table), at most 4096 rows and 16M characters. |
| Row key | Each row is an object with a non-empty string `Name`, usable unchanged as an FName, at most 1024 characters, not `None`. Duplicates (case-insensitive) are rejected. A struct column whose import name is `Name` is refused; use `Label` or similar for display text. |
| Columns | Every top-level column must appear exactly once under one of UE's accepted import names (short authored names work). Missing, unknown or multiple-alias fields are rejected. |
| Numbers | Direct numeric columns need finite numbers; int32/byte range, integer precision and float range are checked before UE narrows them. |
| Other values | Enums and nested values use the native UE DataTable JSON importer and its diagnostics. |

Import first goes into a transient candidate table; any native import diagnostic or snapshot validation failure rejects it before the destination package is created. Invalid JSON never creates an asset.

Changing the row struct goes through the struct migration (`Das.DSL.Types.MigrateStructFields`, see
[Migrations](../Workflow/Migrations.md)): added, renamed (same `@ue_id`), dropped (with a rule) and converted
fields are applied to every row of the tables in place, and the tables are marked dirty.

Not provided: updating rows from the JSON file, CSV import, custom row key names and automatic reimport from the
JSON file.

## CurveTables

```das
require ue_curve_tables

if(ue_curve_table_has_row(curves,"Damage")) {
    let damage=ue_curve_table_eval(curves,"Damage",0.5)
    print("Damage at time 0.5: {damage}\n")
}
let names=ue_curve_table_names(curves)
```

| Function | Behavior |
| --- | --- |
| `ue_curve_table_has_row(curves:UObjectHandle, name:string):bool` | `false` when the curve does not exist. |
| `ue_curve_table_names(curves:UObjectHandle):array<string>` | Detached, FName lexical order, at most 4096; empty for an empty table. |
| `ue_curve_table_eval(curves:UObjectHandle, name:string, time:float, default_value:float=0.0):float` | Calls the row's native `FRealCurve::Eval`. |

- Simple and rich CurveTables use the same API. Interpolation, tangents and extrapolation come from the native curve settings; each evaluation sees the table's current contents.
- `time` is any curve input (seconds, level, charge).
- `default_value` is used only when the curve has neither keys nor a stored default; a stored default wins. It does not hide missing rows or invalid handles.
- FName lookup is case-insensitive. Empty/`None` names, names over 1024 characters, invalid/non-CurveTable handles, missing rows, and non-finite input or results raise script errors.

### Example: damage by level

`Plugins/DasForUE/Content/Das/Scripts/curvetable_items.das`:

```das
options gen2
require ue_curve_tables

// Time can also be an item level, charge amount or other curve input.
def item_damage(curves:UObjectHandle;level:float):float {
    if(!ue_curve_table_has_row(curves,"Damage")) {return 0.0}
    return ue_curve_table_eval(curves,"Damage",level)
}
```

With a simple CurveTable whose `Damage` row has keys (0, 10) and (2, 30) and linear interpolation, `item_damage(curves,0.5)` returns 15. Create, save and cook the CurveTable as a normal project asset.

See also: [Structs](../DSL/Structs.md), [Asset loading](AssetLoading.md), [Data assets](DataAssets.md), [Gameplay tags](GameplayTags.md), [Struct snapshots](../../Reference/StructSnapshots.md)
