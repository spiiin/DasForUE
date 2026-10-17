# Enum sets

`table<MyEnum>` maps to a real Unreal `TSet` whose element property retains the
exact reflected enum identity. Both `[ue_enum]` and `[ue_native_enum(path=...)]`
source enums work in direct `ue_class` properties, `self` operations and
`ue_function` inputs, results, out and inout.

```das
require ue_native_engine_values

[ue_class(Blueprintable)]
class CollisionRules {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Modes:table<SpawnCollision>

    [ue_function(BlueprintCallable)]
    def AddMode():void {
        insert(self.Modes,SpawnCollision.AlwaysSpawn)
    }
    [ue_function(BlueprintCallable)]
    def CopyModes(values:table<SpawnCollision>):table<SpawnCollision> {
        return <- clone(values)
    }
    [ue_function(BlueprintCallable)]
    def ChangeModes(var values:table<SpawnCollision>&;
                   @ue_out var previous:table<SpawnCollision>&):void {
        previous := values
        insert(values,SpawnCollision.DontSpawnIfColliding)
    }
}
```

`SpawnCollision` is the sample native projection of
`/Script/Engine.ESpawnActorCollisionHandlingMethod`. An authored enum uses the
same syntax, with the existing registry/type-module requirements.

Supported operations are insert, erase, clear, key_exists, length, local clone
and whole-property clone assignment (`:=`). Duplicate insertion preserves one
element. Iteration uses local snapshots; no native container pointers escape.
Readonly checking and enum type identity apply to mutations and function calls.
Passing an integer or a different enum is a compilation error, even if its
numeric values match. Unknown/reserved values forged through unsafe code fail
before publication. Runtime snapshot validation also checks native enum values.

Native `self` fields support both `TSet<EnumClass>` with byte underlying storage
and `TSet<TEnumAsByte<Enum>>`. Distinct transient carriers preserve the actual
element property shape. Carrier reconstruction after loading resolves the enum
path and validates its member fingerprint through the existing enum-array cache.
Source native enum inference requires one visible matching ue_native_enum.

Limits remain 4096 total snapshot elements and 16 MiB. Properties start empty;
explicit defaults and structural migration of container-bearing classes remain
deferred. Wide native enum storage, arbitrary struct/object Set elements and
containers nested directly in containers are outside this stage. Authored
`ue_struct` members keep their existing separate container restrictions.

Examples: `ValueDriver.EnumStates`/`NativeStates`, `CopyEnumSet`,
`CopyNativeEnumSet`, `RefEnumSet`, `RefNativeEnumSet`, `SeedEnumSets` in
`dsl_method_value_driver.das`; `NativeArraySelf.EnumSets` in
`dsl_native_array_self.das` exercises actual native strong and byte enum sets.

## Validation — 2026-10-17

Editor builds and Win64 Development game build passed. Native self fixtures
passed Create/Reopen/Recompile and final Reopen. The method-value suite passed
Create/Reopen, including the actual enum property and UFunction input/result
identity checks and raw snapshot validation. Natural PIE passed 1 test, 0 failures,
0 warnings. Both maps passed in the packaged executable with exit code 0:
`DAS_NATIVE_DEFAULTS_PACKAGED=PASS` and `DAS_METHOD_VALUES_RESULT=PASS`.

Coverage includes native enum-class and TEnumAsByte storage, authored sparse values
7/42 without a zero member, duplicate insertion, membership, erase/missing erase,
clear, local-copy isolation, assignment, input/return, out/inout, empty values,
forged invalid values with recovery and preservation of the previous property.
Negative compilation tests reject other enum identities, integer keys, readonly
mutations, wrong method container types and explicit defaults. Existing array,
map, reference, nested-struct and method failure/recovery regressions also passed.

Kismet creates FEnumProperty for a native enum class in authored properties and
method signatures. The canonical enum-set carrier follows that form; regular
native enums and authored enums use FByteProperty. The native-field path follows
the actual FProperty shape, including TEnumAsByte of an enum class. No IR/JSON
format change or daScript SDK change is required.

Logs: `logs/enum-sets-native-{Create,Reopen,Recompile}.log`,
`logs/enum-sets-native-final-reopen.log`, `logs/enum-sets-methods-{Create,Reopen,pie}.log`,
`logs/enum-sets-methods-pie-report.json`, `logs/enum-sets-package.log`,
`logs/enum-sets-{native,methods}-packaged.log`. Targeted `git diff --check` passed.

Native map: `/Game/DasNativeDefaults/903ba0ea08e84c39a34b1d5a851ca4a5/Map`.
Authored map: `/Game/DasMethodValues/c2a6c6432abf409cb28acf856368aa29/Map`.
Validation used NullRHI on Windows; rendered gameplay, Shipping and other
platforms were not tested.

Repeat Editor checks with `Scripts/TestNativeDefaults.ps1` and
`Scripts/TestMethodValues.ps1`; the latter accepts `-Cooked -SkipZenStore`.
