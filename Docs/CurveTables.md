# CurveTable reads and evaluation

`require ue_curve_tables` reads an already loaded CurveTable on the active game
thread. It does not load assets, expose native curve pointers or edit table rows.

```das
require ue_curve_tables

if (ue_curve_table_has_row(curves, "Damage")) {
    let damage = ue_curve_table_eval(curves, "Damage", 0.5)
    print("Damage at time 0.5: {damage}")
}
let names = ue_curve_table_names(curves)
```

## API

- `ue_curve_table_has_row(curves:UObjectHandle, name:string):bool`: false when the
  named curve does not exist.
- `ue_curve_table_names(curves:UObjectHandle):array<string>`: detached string array
  in FName lexical order, including an empty array for an empty table. Listing is
  limited to 4096 rows. Names remain valid after native table changes or GC.
- `ue_curve_table_eval(curves:UObjectHandle, name:string, time:float,
  default_value:float=0.0):float`: calls the row's native `FRealCurve::Eval`.

Simple and rich CurveTables use the same API without manual casts. Interpolation,
tangents and extrapolation come from native UE curve settings. No linear-only
script approximation or layout copy is introduced. The output is an ordinary
float. A later evaluation observes the table's current native contents.

`time` is a general curve input; it may represent seconds, level or charge.
`default_value` is UE's fallback when the curve has neither keys nor a stored
default value. A curve's stored default takes precedence. This argument does not
hide missing rows or invalid handles. Use `has_row` for optional lookup.

Row lookup follows FName case-insensitive semantics. Empty/None names and names
longer than 1024 characters are errors. Invalid/dead/non-table handles, missing
rows, nonfinite time/default input and nonfinite evaluation results raise script
errors through the existing runtime mechanism. The common UE call/context guards
remain in effect. Curve pointers are used only during the synchronous native call.

## Item damage example

`Plugins/DasForUE/Content/Das/Scripts/curvetable_items.das` defines
`item_damage(curves, level)`. It looks up the Damage row and evaluates it using the
requested level, returning zero if the row is absent. The caller supplies the loaded
CurveTable handle through existing object/asset-loading facilities.

For example, create a simple CurveTable in the UE editor with a Damage row:

| Time / level | Value |
| ---: | ---: |
| 0 | 10 |
| 2 | 30 |

With linear interpolation, evaluation at 0.5 returns 15. Adjust extrapolation in
UE if values outside the key interval should follow a different rule. Asset
creation, saving and cook inclusion remain normal project responsibilities.

## Focused validation

`Scripts/TestCurveTables.ps1` builds Editor Development and runs one
`DasCurveTables` commandlet scenario. It uses real in-memory simple and rich
CurveTables, compiles the public example and exercises names/lookup, exact keys,
simple linear and rich cubic interpolation, linear/constant extrapolation, empty
curve fallback and stored default precedence, absent/invalid row names, wrong
object type, table clearing and preservation of previously returned names after
GC. It does not save user assets or modify project settings.

Temporary probe scripts are uniquely named and removed. Evidence is recorded in
`logs/curve-tables-validation.json` and `logs/curve-tables-runtime.log`, including
source/DLL hashes and engine BuildId. No full Foundation/PIE/packaged run is part
of this focused step.

Accepted on 2026-10-18: Editor Development build PASS and one focused runtime
scenario PASS (5 grouped checks), stable source/DLL hashes and engine BuildId,
no engine Error/ensure. Initial test-fixture issues and parallel-build conflicts
were resolved before this accepted run. No full Foundation/PIE/packaged and no commit.
