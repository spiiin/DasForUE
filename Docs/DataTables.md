# Typed DataTable reads

`require ue_data_tables` provides read-only access to an already loaded UE
DataTable. It never loads an asset implicitly and never returns native row memory.

```das
require ue_data_tables
require datatable_item_types

if (ue_data_table_has_row(items_table, "Potion")) {
    let item = ue_data_table_row(type<ItemRow>, items_table, "Potion", "/Game/ItemTypes")
    print("{item.Label}: {item.Price}")
}
let names = ue_data_table_names(items_table)
```

- `ue_data_table_row(type<Row>, table, name [, scope])` returns a typed value.
  Row must be an existing authored `ue_struct` or canonical generated
  `ue_native_struct`. Scope is an explicit type folder or exact type-registry
  object path for authored structs, as with other authored type resolution.
  Native canonical structs do not need scope. The compiler validates declaration
  identity/layout/defaults using the existing struct machinery; runtime requires
  the exact table row struct path. A mismatched row type is an error, including
  a type with the same field layout but different identity.
- `ue_data_table_has_row(items_table, name)` returns false for an absent row.
- `ue_data_table_names(items_table)` returns a detached array of strings sorted by
  FName lexical order. Listing is limited to 4096 rows. Single-row reads do not
  impose a whole-table row-count limit.

Names follow UE FName lookup semantics (case insensitive). Empty/None names and
names longer than 1024 characters are rejected. A missing row in a direct typed
read raises a script error; use `has_row` for optional lookup. Invalid/dead/non-table
handles are errors rather than an empty result.

Native rows first become owned snapshots in the existing VM table. Unpacking
creates the normal DAS struct value: changes/removal of the native row do not
change a previously read value. Strings/containers are copied through existing
struct adapters; object reference fields retain the existing checked weak-handle
semantics. This API does not turn object references into ownership/pins. Existing
snapshot limits, field support and validation still apply. Reads require an active
game-thread VM context and follow the common UE call guards.

## Item table example

Sources:
- `Plugins/DasForUE/Content/Das/Scripts/datatable_item_types.das`: authored
  ItemRow with Label, Price and Weight.
- `Plugins/DasForUE/Content/Das/Scripts/datatable_items.das`: item_price using
  ordinary typed field access and -1 for an absent item.

Import the reflected row schema in the editor:

```text
Das.DSL.Types.Apply datatable_item_types.das /Game/ItemTypes
```

Save the generated ItemRow struct. In Content Browser, create a DataTable using
that row struct and add, for example:

| Row name | Label | Price | Weight |
| --- | --- | ---: | ---: |
| Potion | Health Potion | 25 | 0.5 |
| Sword | Iron Sword | 125 | 3.0 |

Alternatively, create the table directly from the checked-in JSON example:

```text
Das.DSL.Table.Create /Game/ItemTypes/ItemRow.ItemRow /Game/Items/DT_Items C:/src/DasForUE2/Examples/DataTables/Items.json
```

This creates a new dirty DataTable asset in Content Browser. Save both the authored
row struct and the table in the editor, then pass the loaded table handle to
`item_price`. Loading can use an existing authored asset reference or the existing
asset-loading API. See [DataTable authoring/import](DataTableAuthoring.md) for
validation rules. Existing tables are never overwritten. Schema migration,
automatic reimport and cook settings are separate work. CurveTable reading is
covered in [CurveTables](CurveTables.md).

## Focused validation

`Scripts/TestDataTables.ps1` builds the Editor and runs one `DasDataTables`
commandlet scenario. It creates a unique authored row struct and a new DataTable asset from the public
`Examples/DataTables/Items.json`, checks import refusals before asset creation,
then compiles/runs a temporary copy of the public example against that type folder. It checks names, case-insensitive lookup,
missing rows, exact-type refusal, typed values and a retained copy after native
row removal/GC, then an empty table. Only its own temporary DAS files are removed;
no user assets/settings are saved or modified.

Evidence: `logs/data-tables-validation.json` and `logs/data-tables-runtime.log`.
PIE and packaged tests are intentionally outside this focused step.

Accepted on 2026-10-18: Editor Development build PASS, one combined runtime
scenario PASS (5 grouped checks); stable source/DLL hashes and engine BuildId,
no engine Error/ensure. Initial fixture syntax/import issues were corrected before
this accepted run. No full Foundation/PIE/packaged execution and no commit.

Accepted JSON-creation validation on 2026-10-18: Editor Development build PASS
and one combined runtime scenario PASS (17 checks), stable source/DLL hashes and
engine BuildId, no engine Error/ensure. Temporary fixture files removed; no full
Foundation/PIE/packaged run and no commit.
