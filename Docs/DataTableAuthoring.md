# Create an authored DataTable from JSON

The editor service creates a new ordinary UDataTable using an existing compiled
DSL-authored UUserDefinedStruct. The data asset is usable by the existing typed
DataTable runtime API. No new runtime row layout/JSON metadata carrier is added.

## Example

The public schema is `datatable_item_types.das` and the input file is
`Examples/DataTables/Items.json`:

```json
[
  {"Name":"Potion", "Label":"Health Potion", "Price":25, "Weight":0.5},
  {"Name":"Sword", "Label":"Iron Sword", "Price":125, "Weight":3.0}
]
```

Run these commands in the editor console:

```text
Das.DSL.Types.Apply datatable_item_types.das /Game/ItemTypes
Das.DSL.Table.Create /Game/ItemTypes/ItemRow.ItemRow /Game/Items/DT_Items C:/src/DasForUE2/Examples/DataTables/Items.json
```

A package path `/Game/ItemTypes/ItemRow` is also accepted for the first argument;
the command normalizes it to the object path. Quote the JSON filename if it
contains spaces. The table appears in Content Browser as a dirty asset. Save
both the row struct and table explicitly in the editor. The service never saves
files, overwrites a package or changes AssetManager/cook/project settings.

Then use the existing typed API:

```das
require ue_data_tables
require datatable_item_types

let item = ue_data_table_row(type<ItemRow>, items_table, "Potion", "/Game/ItemTypes")
print("{item.Label}: {item.Price}")
```

## Validation and publication

The native editor API is:

```cpp
UDataTable* Table = nullptr;
FString Error;
bool Ok = FDasDataTableService::CreateFromJson(RowStruct, Json, NewPackage, Table, Error);
```

Header: `DasDataTableService.h` in DasEditor. Call on the game thread outside PIE.
The output pointer is cleared on failure. Only a new valid `/Game/...` package is
accepted; existing in-memory packages and existing files are refused before import.
No update/merge/replacement policy is implicit in this operation.

The row struct must be an up-to-date authored DSL struct with matching saved type
identity/model, member stable IDs, names and pin types. Native or manually-created
structs without authored identity are rejected. This validates the loaded
reflection/schema; it does not reimport a changed DAS source or perform migration.
Existing snapshot field support/limits apply to the row type.

Input must be a JSON array (an empty array creates an empty table), at most 4096
rows and 16M characters. Each row must be an object with a nonempty string `Name`
that UE can use unchanged as an FName, up to 1024 characters and not None.
Duplicate row names are rejected using case-insensitive FName identity.
`Name` is reserved for the row key; a row struct column that uses that import
name is refused. Use Label or another column name for item display text.

Every reflected top-level column must have exactly one recognized key in the
parsed JSON object, using one of UE's accepted import names. This supports the short authored column names without
requiring internal GUID-suffixed property names. Multiple aliases for one column
are rejected; identical duplicate JSON object keys follow UE JSON parser semantics. Direct numeric columns require finite JSON numbers; int32/byte
range and integer precision, and float storage range, are checked before UE can
narrow them. Enum and other property conversions use the native UE JSON importer.
Unknown/missing fields and native import problems reject the candidate. Nested
value syntax and conversion diagnostics follow UE DataTable JSON rules; the
service does not promise stricter nested numeric narrowing than UE itself.

JSON is imported into a transient candidate table with missing/extra-field
ignoring disabled. Any native import diagnostic rejects that candidate. Every
imported row is then validated by the common snapshot validator. Only after
validation does the service create the destination package/table, copy the
candidate rows with UScriptStruct lifecycle APIs, notify AssetRegistry and mark
the new asset dirty. Invalid JSON does not create a destination asset or package.

This does not register the source file with UE automatic reimport. Updating an
existing table, CSV import, custom row key names, structural migration and cook
policy are separate steps.

## Focused verification

`Scripts/TestDataTables.ps1` still runs one combined commandlet scenario. It now
creates the item table from the checked-in JSON and exercises malformed/nonobject
input, wrong Name/scalar types, integer overflow/fractional values, missing/extra
columns, duplicate row names and existing-package refusal. Rejections check that
no destination asset/package was created. The successful table is read through
the public DAS example, then the existing row-removal/GC and empty-table checks
run. An empty JSON array is also accepted.

Only uniquely named temporary fixtures and in-memory test assets are cleaned up;
no user asset or project setting is saved or changed. Evidence is recorded in
`logs/data-tables-validation.json`. Full Foundation/PIE/packaged tests are outside
this step.

Accepted JSON-creation validation on 2026-10-18: Editor Development build PASS
and one combined runtime scenario PASS (17 checks), stable source/DLL hashes and
engine BuildId, no engine Error/ensure. Temporary fixture files removed; no full
Foundation/PIE/packaged run and no commit.
