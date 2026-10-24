# Generated Bindings (ue_generated)

The binding generator turns an explicit selection of reflected `UFUNCTION`s into typed daScript wrappers in the `ue_generated` module. Each wrapper checks the receiver, builds a validated call frame, calls `ProcessEvent` and returns ordinary daScript values. The generic [API packages](../Api/README.md) use the same generator with whole-library profiles; this page describes the type mapping and the hand-picked `Bindings/Selection.json` workflow.

## Pipeline

```
Bindings/Selection.json → UE reflection (DasExport commandlet) → Bindings/Api.json (IR)
    → Scripts/GenerateBindings.py → Plugins/DasForUE/Content/Das/Scripts/ue_generated.das
```

Everything is based on loaded Unreal reflection; UE C++ sources are never parsed. The exporter uses the same parameter-support validator as the runtime, so an unsupported signature is rejected at export time rather than at call time.

```powershell
.\Scripts\GenerateBindings.ps1          # build editor, export, generate, then replace the working files
.\Scripts\GenerateBindings.ps1 -Check   # regenerate and fail if Api.json or ue_generated.das is stale
```

The script writes both results to `Build/Bindings` first and copies them over the working files only after a successful run (`-Python` and `-Engine` override the tool paths). `-Check` compares the full IR, including metadata and diagnostics, and never updates files. The generator can also run without Unreal against an existing IR:

```powershell
python Scripts/GenerateBindings.py Bindings/Api.json Plugins/DasForUE/Content/Das/Scripts/ue_generated.das --check
```

Export runs only in the Editor: packaged builds use the already generated bindings, and editor metadata may be missing there. Generation is deterministic (no timestamps). An unknown IR schema version is rejected: regenerate with `GenerateBindings.ps1`.

## Selecting functions

```json
{
  "policy": {"allow_deprecated": false, "allow_internal": false},
  "functions": [
    {"class": "/Script/DasRuntime.DasObjectFixture", "function": "AddHealth", "alias": "fixture_AddHealth"}
  ],
  "class_rules": [
    {
      "class": "/Script/Engine.Actor",
      "prefix": "actor_",
      "include": ["GetOwner", "SetOwner", "K2_GetActorLocation"],
      "exclude": ["SetOwner"],
      "include_inherited": false
    }
  ]
}
```

- `functions` entries are required by default: a missing function or unsupported signature aborts the export and keeps the previous output. `required: false` allows skipping with a diagnostic.
- `class_rules` select function names with Unreal `MatchesWildcard` patterns (case-sensitive). `include` is mandatory and non-empty; `exclude` wins inside a rule. The alias is `prefix` + the real reflected function name. A rule that matches nothing is an error.
- Only functions declared by the class itself are selected unless `include_inherited: true`.
- Matches are sorted. Automatic candidates with unsupported signatures are skipped and listed in the IR `diagnostics` (`excluded_by_rule`, `deprecated_function`, `internal_function`, or the marshalling error).
- `DeprecatedFunction` and `BlueprintInternalUseOnly` are excluded unless `allow_deprecated` / `allow_internal` is set; signature checks still apply. `BlueprintProtected`, `Latent` and `CustomThunk` functions need dedicated adapters and are not exported this way. RPC and delegate functions are not exported.
- Unknown keys, non-boolean flags, duplicate aliases, unloaded classes, alias collisions and invalid alias prefixes (currently `actor_`, `fixture_`, `ue_`) are errors.
- The exporter does not crawl the whole engine or load assets: classes must already be loaded. Ancestors of selected classes are added automatically.

Descriptive metadata (Category, DisplayName, ToolTip, Units, Clamp/UI ranges, deprecation keys) is recorded in the IR but does not change execution. Only `CPP_Default_*` text is used, for [defaults](#default-arguments).

## Type mapping

| Unreal type | daScript type in `ue_generated` | Notes |
| --- | --- | --- |
| `int32`, `float`, `double`, `bool` | `int`, `float`, `double`, `bool` | Exact match required |
| `FString` | `string` | UTF-8 ↔ FString copy |
| `FName` | `UEName` (`value:string`) | Construct with `ue_name("...")`; a `string` is rejected |
| `UEnum` / `TEnumAsByte` | `enum E_<Name>_<hash>`, members `v_<Symbol>` | Converted by symbol, not ordinal |
| `UScriptStruct` | `struct S_<Name>_<hash>`, fields `f_<name>` | Value copy, nominal identity |
| `UObject*` (UCLASS) | `struct C_<Name>_<hash>` | Weak checked reference |
| `TArray<T>` | `array<T>` | Move results with `<-` |
| `TMap<K,V>` / `TSet<T>` | `H_Map_<...>` / `H_Set_<...>` wrappers | `entries` / `values` arrays |

`<hash>` is a short SHA-256 suffix of the full UE type path, so identity is defined by the type, not by its field list. A user-defined `FakeVector` with the same fields is a different type.

A class whose short name is unique also gets a short alias: `typedef Light = C_Light_<hash>`. There is none when the name is taken in a module a script may require together with the bindings: a DSL base class (`[ue_native_base]` `class Actor` in `ue_dsl`: `Actor`, `Pawn`, `Character`, `ActorComponent`, `SaveGame`, `DataAsset`, ...) or a native bind type (`Class<T>(TEXT("Name"))` in a `DAS_NATIVE_BINDS` module, `DasNativeBinds.h`; `ue_engine` names its types as in C++: `AActor`, `APawn`, ...). Otherwise `var a : Actor` would be ambiguous. The generated module then has a comment in place of the alias (`// Actor: no short alias, the name belongs to the DSL base class ue_dsl::Actor; use C_Actor_<hash>`), the generator prints `GENERATE_BINDINGS_SKIPPED_ALIASES=...`, and the full name works as before. `GenerateBindings.py` scans the plugin's script modules and native sources for these names; a project with its own native modules adds its source folders with `--native-binds-source <folder>`. `TestBindingsGenerator.py` and the `DasNativeBindsManifest` commandlet fail if a checked-in module still declares such an alias.

### Enums and names

Enum wrappers are real daScript enums. Conversion to Unreal uses registered symbols, so `int(value)` is not a promise of the Unreal numeric value. `int` instead of an enum, or a different enum type, does not compile. Symbols ending in `_MAX` are reserved and rejected in both directions; other `Hidden` members are not filtered.

FName and FString remain distinct: `UEName` is a separate struct and `ue_name(...)` makes the conversion explicit. Empty text and `None` mean `NAME_None`; FName equality is case-insensitive and Unreal may normalize the returned spelling.

### Structs

```das
let scale = S_Vector_4bd4f956(f_x = double(2), f_y = double(3), f_z = double(4))
actor_SetActorRelativeScale3D(actor, scale)
let observed = actor_GetActorRelativeScale3D(actor)
verify(observed.f_x == scale.f_x)
```

`S_Vector_4bd4f956` is `/Script/CoreUObject.Vector` (double fields). Each UE struct is declared once; nested structs come first. Fields get the `f_` prefix to avoid daScript keywords. The value is plain daScript data; no UE memory address is exposed. daScript field defaults apply to newly constructed values: C++ constructors/defaults of `UScriptStruct` are not exported, so set important fields explicitly.

API packages generated with `canonical_math` map Vector2D/Vector/Rotator/Transform to the shared `UEVector2D`/`UEVector`/`UERotator`/`UETransform` instead (see [Math](../Api/Math.md)); `ue_generated` keeps the `S_*` form.

### Object references

```das
let actor = ue_ref(type<C_Actor_adf65d5b>, ue_self_object())
let owner = actor_GetOwner(actor)            // also C_Actor_adf65d5b
if (C_Actor_adf65d5b_valid(owner)) {
    let position = actor_K2_GetActorLocation(owner)
}
```

For every `C_<Name>_<hash>` the generator emits:

| Function | Meaning |
| --- | --- |
| `_from(UObjectHandle)` | Checked, nullable conversion from a low-level handle |
| `_raw(value)` | Back to `UObjectHandle`, re-checking the class |
| `_null()` | Empty reference, usable for global initialization |
| `_valid(value)` | `false` for null, destroyed or wrong-class objects |
| `_as_<Ancestor>(value)` | Explicit upcast, e.g. `C_Actor_adf65d5b_as_C_Object_55c3753a(actor)` |

There are no implicit conversions between classes. Downcast through the handle: `ue_ref(type<C_Actor_adf65d5b>, C_Object_55c3753a_raw(object))`. A function declared to return `UObject*` returns the Object view even when the object is an Actor.

The reference holds a weak, context-local token: it does not own the UObject or protect it from GC. Null is allowed for object arguments and results but not for the receiver. A non-null expired token raises an error on conversion or call; it never silently becomes null. The `handle` field is public but is not a security boundary: every wrapper re-checks the receiver and object arguments.

### Arrays

```das
let self = ue_ref(type<C_DasObjectFixture_5f019a81>, ue_self_object())
let input <- [1, 2, 3]
let result <- fixture_TransformInts(self, input)
verify(input[0] == 1)                         // input is never modified in place

let children <- actor_GetAttachedActors(ue_ref(type<C_Actor_adf65d5b>, ue_self_object()))
for (child in children) {
    if (C_Actor_adf65d5b_valid(child)) { let p = actor_K2_GetActorLocation(child) }
}
```

Supported elements: `int32`, `float`, `double`, `bool`, `FString`, `FName` (`array<UEName>`), reflected value structs, enums and typed object references (`array<C_Actor_...>`). Arrays and result records containing arrays are returned by move. There is no covariance between `array<C_Actor_...>` and `array<C_Object_...>`; convert elements explicitly. Each array is limited to 65,536 elements: an oversized input is rejected before the call; an oversized output is detected after `ProcessEvent`, so the function's side effects are not rolled back.

Object array elements are pinned by the call frame while the UE function runs and until results are read; returned elements are weak again.

### Maps and sets

```das
var map : H_Map_int_string_aa6f8c57
map.entries <- [
    H_Map_int_string_aa6f8c57_Entry(key=1, value="hello"),
    H_Map_int_string_aa6f8c57_Entry(key=2, value="Привет")]
let result <- fixture_TransformMap(self, map)

var set : H_Set_int_bf4eb51c
set.values <- [1, 2, 3]
let changed <- fixture_TransformSet(self, set)
```

Keys, values and elements may be `int32`, `float`, `double`, `bool`, `FString`, `FName` or a reflected enum (the UE property must be hashable). The wrappers are transport records, not a full map/set API: there is no insert/remove/find, and output order is not guaranteed. Duplicate map keys or set elements are rejected before the call (using Unreal hash/equality, so `Mixed_17` and `mixed_17` are the same FName). Float/double keys and set elements must be finite; `+0` and `-0` are normalized and count as duplicates. The same 65,536-entry limit applies.

## Calls, outputs and inout

- A single output (including a struct return) is returned directly.
- Several outputs return `<alias>_Result` with fields `out_<path>` (dots replaced by `_`), e.g. `changed.out_payload.f_count`, `state.out_returnvalue`.
- Inout parameters are passed as input and returned in the result; the script's argument is never modified in place.
- Strings and names in results are copied into daScript memory and outlive the call frame; results can be passed straight into the next wrapper.

The wrapper verifies a control signature (types, directions, field names, struct/enum/object identity) before `ProcessEvent`. If the bindings are stale, the call fails before any side effect; run `GenerateBindings.ps1`.

## Default arguments

`CPP_Default_*` metadata becomes typed daScript defaults on trailing input-only parameters:

```das
verify(fixture_PolicyDefault(self) == 7)
verify(fixture_PolicyDefault(self, 23) == 23)
verify(fixture_DefaultScalars(self, 8, false, 2.0, 3.0lf, "override") == "override")
```

| Kind | Accepted default text |
| --- | --- |
| `int` | Decimal within int32 |
| `float` / `double` | Finite decimal (exponent allowed); overflow or underflow-to-zero rejected; doubles get `lf` |
| `bool` | `true` / `false` |
| `FString` | Text as a string value (escaped, never executed); NUL rejected |
| `FName` | `ue_name("...")` |
| enum | Existing symbol or `Type::Member`; numbers are not accepted |
| `UObject` | Only `None` / `nullptr` / `NULL` → typed null |

After the first optional input all later inputs must have defaults. Return/out/inout, structs, arrays, maps/sets and arbitrary C++ expressions get no default; generation fails with the alias and parameter name instead of silently dropping it. The value comes from reflection, so precision lost by UHT (for example a double stored as `1.234568`) cannot be recovered. Changing a default changes the generated signature; `-Check` detects stale output. API packages use `default_arguments=explicit` and never emit defaults.

## Examples

All under `Plugins/DasForUE/Content/Das/Scripts/`:

- `generated_actor.das` — Actor wrappers (tags, tick group, owner, scale, attached actors); expects a test actor tagged `DasEnumTag_17`.
- `generated_calls.das`, `generated_arrays.das`, `generated_complex_arrays.das`, `generated_defaults.das`, `generated_hash_containers.das`, `generated_hash_numbers.das`, `class_roundtrip.das`, `class_persistent.das`, `enum_names.das` — fixture-based examples (they need the test `UDasObjectFixture`).

Files named `generated_*_bad_*`, `class_bad_*`, `class_forged_receiver`, `class_null_receiver` and similar are intentional compile/runtime failure tests.

## Limitations

- No overloads and no AOT for the wrappers; checks run in the VM backend.
- Rejected shapes (reported with a reason in the export diagnostics): nested containers, fixed C++ arrays, maps/sets of structs or objects, containers of weak object references, container defaults, soft object/class parameters (no generic wrapper ABI yet), opaque or empty structs without a dedicated adapter, recursive struct graphs, latent/RPC/delegate/CustomThunk functions without a dedicated adapter. Opaque engine values that do have adapters (HitResult, constraint handles, Geometry, input events and others) are described on the [API package](../Api/README.md) pages.
- Enum bitflag combinations are not synthesized; pass a single declared member.
- The early hand-picked `Bindings/Selection.json` contains mostly test-fixture functions; for real engine API use the [API packages](../Api/README.md) or [native class bindings](NativeClassBindings.md).

See also: [Low-level calls](LowLevelCalls.md) · [Native class bindings](NativeClassBindings.md) · [API packages](../Api/README.md) · [Type support](TypeSupport.md) · [Limitations](Limitations.md)
