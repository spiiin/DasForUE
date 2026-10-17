# FName containers in authored properties and functions

`table<UENameValue; V>` represents a real Unreal `TMap<FName, V>`;
`table<UENameValue>` represents `TSet<FName>`. Both work as direct `ue_class`
properties and as `ue_function` inputs, results, out and inout parameters.

```das
[ue_class(Blueprintable)]
class Counter {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Scores:table<UENameValue;int>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Tags:table<UENameValue>

    [ue_function(BlueprintCallable)]
    def CopyScores(values:table<UENameValue;int>):table<UENameValue;int> {
        return <- clone(values)
    }
    [ue_function(BlueprintCallable)]
    def AddTag():void {
        self.Scores[ue_name_value("Player")]=10
        insert(self.Tags,ue_name_value("Active"))
    }
}
```

Map values use the existing carrier families: int, float, double, bool, string,
UENameValue, supported UE math values, authored structs, reflected enums and
typed native object/interface references. Set elements now include int, string,
UENameValue and native/authored enums (see [DSLEnumSets.md](DSLEnumSets.md)).
General struct/object Set keys and nested containers remain separate work.

FName keys retain Unreal identity: case aliases are equal, None and empty share
one identity, and numeric suffixes retain native semantics. Names are represented
by context-owned interned tokens; raw token integers are not persistent identifiers.

Properties have implicit empty defaults. Readonly checks apply to assignment,
indexed writes and insert/erase/clear. Reads and results are independent snapshots;
use `:=` to copy a local container back into a property. Container declaration
migration remains deferred; body-only reimport is supported. `ue_struct` member
containers retain their separate supported-type restrictions.

Regression examples: `ValueDriver.NameScores`, `NameTags`, `NameVectors`,
`CopyNameScores`, `CopyNameTags`, `CopyNameVectors`, `RefNameScores`, `RefNameTags`
and `exercise_names` in `dsl_method_value_driver.das`. The shared C++ probe checks
the actual FName key/element properties and input/result UFunction signatures.

## Verification — 2026-10-17

Editor Create/Reopen and packaged Win64 Development passed
`DAS_METHOD_VALUES_RESULT=PASS`, including the C++ native property/signature/equality
check. PIE passed 1 test, 0 failures, 0 warnings. Runtime checks cover input/result,
out/inout, empty values, copy isolation, case aliases, None/empty, Unicode,
numbered names and math-valued maps. Compilation rejects wrong key types,
readonly mutation and explicit container defaults. Existing method/container
regressions and body-only reimport passed. `git diff --check` passed.

Logs: `logs/name-authoring-{create,reopen,pie,packaged}.log`,
`logs/name-authoring-package.log`, `logs/name-authoring-pie-report/index.json`.
Probe map: `/Game/DasMethodValues/f82b1c3025774273861619367cf820be/Map`.
Builds used NullRHI; rendered play, Shipping and other platforms were not tested.
Repeat the suite with `Scripts/TestMethodValues.ps1 -Cooked -SkipZenStore`.

The compiler's per-source descriptor now records both map key and value source
types. This preserves math/enum/struct/reference identity when generating accessors
and method wrappers through authored references. It is internal compilation
metadata; the persisted declaration JSON format is unchanged.
