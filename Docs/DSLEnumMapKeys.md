# Enum keys in TMap

Native `[ue_native_enum]` and authored `[ue_enum]` declarations can be table keys:

```das
@ue_property=(EditAnywhere,BlueprintReadWrite)
Counts:table<MethodState;int>

[ue_function(BlueprintCallable)]
def Echo(values:table<MethodState;int>):table<MethodState;int> {
    return <- clone(values)
}
```

Use the enum directly: `self.Counts[MethodState.Active] += 1`.
No integer casts, string class hints, or new source annotation is needed.
The same representation supports parameters, results, `ref` and `@ue_out`,
and compatible native reflected fields through `self`.

Values use the existing map families: int, float, double, bool, string,
FName, supported math/authored structures, enums, and typed object/interface
references. Nested containers are outside this slice.

## Identity and storage

The key carries its exact UEnum identity in the declaration/IR. A different
enum remains a different key type even when its numeric members match.
Native enum classes preserve FEnumProperty; native TEnumAsByte and authored
enums preserve FByteProperty. Only byte-sized reflected key storage is
supported. Native self inference requires one visible matching enum projection.

The transient carrier duplicates a validated enum-array element and the
value field of an existing int-key map carrier. Its reconstructible path
records the enum path, storage representation, and baseline value carrier.
Cooked runs reconstruct the same carrier from loaded reflected types.
Caches retain existing enum member/layout validation, 1024 carrier limits,
and NAME_SIZE path limits. Persisted IR and JSON versions are unchanged.

Pack/unpack uses checked enum tokens at the runtime boundary and typed
daScript enum keys in the VM. Numeric values outside declared members and
reserved `_MAX` entries are rejected. Snapshot validation also rejects
duplicate native keys. Unsafe casts do not bypass boundary validation.

## Mutation and ownership

Readback returns a detached table. Local edits do not alter Unreal fields
until assignment; ordinary `self` mutations use existing snapshot/commit
lowering. Failed packing or a failing reflected method leaves native fields
and `ref/out` results unchanged. Readonly checks still apply to container
mutation. Object values retain the existing GC pinning contract.

Property defaults remain implicitly empty. Nonempty/computed defaults,
struct keys, nested containers, and structural hot-reload migration remain
separate stages. The authored `ue_struct` field declaration subset is not
expanded by this change; this slice targets ue_class fields and methods.

## Regression examples

- `dsl_native_array_self.das`, `EnumKeys`: native strong enum and TEnumAsByte
  keys; int, FVector, enum and UObject values; clone, update, erase and
  unknown-key rejection without partial commit.
- `dsl_method_value_driver.das`, `SeedEnumMaps` and `exercise_names`: sparse
  authored keys 7/42, native keys, properties and reflected calls, ref/out,
  failure rollback, empty maps, readonly and wrong-enum compile failures.
- `DasMethodValuesProbe.h`: actual FMapProperty/UFunction identity, sparse
  keys, unknown/reserved/duplicate native snapshot rejection.

## Validation (2026-10-17)

Passed Editor and Win64 Development game builds; native Create/Reopen/Recompile;
authored Create/Reopen; MethodValues PIE (1 success, 0 warnings/failures/notRun);
and combined cook/stage/pak/archive followed by both packaged map runs (exit 0).
The final native suite also confirms rejected Actor-to-Pawn, raw handle and
unrelated reference writes, and the Owner readonly policy. The native setter
keeps a typed parameter boundary before extracting a reference handle.

Native map: `/Game/DasNativeDefaults/06fa00f6563c4c75b2d2cab70e7710bc/Map`.
Authored map: `/Game/DasMethodValues/7f637ce1b2ec4290895b1873d057ac46/Map`.

Evidence in `logs/`:

- `enum-key-editor-nominal-build.log`, `enum-key-game-nominal-build.log`,
  `enum-key-game-receipt.log` (fresh runtime dependencies, no missing scripts).
- `enum-key-native-{Create,Reopen,Recompile}.log`.
- `enum-key-method-{Create,Reopen}.log`, `enum-key-method-final-reopen.log`.
- `enum-key-final-pie.log`, `enum-key-final-pie-report/index.json`.
- `enum-key-package.log`, `enum-key-package-run.log`,
  `enum-key-native-packaged.log`, `enum-key-methods-packaged.log`.

PIE logs contain two pre-existing startup `LogAutomationTest: Error: Condition
failed` entries before engine initialization, also present in the preceding
object-hash PIE log. They are outside the selected test; its automation report
contains no failed tests or warnings. Commandlet and packaged validation logs
contain no Error/Fatal entries.

The independent game runs also execute the previous container regression and
GC ownership probes. Validation used Windows NullRHI; rendered gameplay,
Shipping, other platforms and multiplayer were not tested.

Repeat with `Scripts/TestNativeDefaults.ps1` and
`Scripts/TestMethodValues.ps1 -Cooked -SkipZenStore`.



Update 2026-10-17: nonempty class defaults with computed scalar leaves are now
supported for scalar/name/enum/math container families, with whole-container
manual override preservation. See [DSLContainerDefaults.md](DSLContainerDefaults.md).
Nonempty object-reference/object-key/opaque-struct defaults remain deferred.
