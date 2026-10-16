# Standalone container parameters and results

Container types can now appear directly in BlueprintCallable instance methods:

```das
[ue_class(Blueprintable)]
class ListsActor {
    [ue_function(BlueprintCallable)]
    def Echo(input:array<string>):array<string> {
        var result <- clone(input)
        return <- result
    }
    [ue_function(BlueprintCallable)]
    def Tags(input:table<string>):table<string> {
        var result <- clone(input)
        insert(result, "世界")
        return <- result
    }
    [ue_function(BlueprintCallable)]
    def Weights(input:table<int;double>):table<int;double> {
        var result <- clone(input)
        result[-3] = 1.23456789012345lf
        return <- result
    }
}
```

Supported shapes are primitive arrays (int/float/double/bool/string/UENameValue),
int/string key-only tables (sets), and maps with int/string keys and
int/float/double/bool/string/UENameValue values: 20 shapes in total.
Several containers and scalars may share a method's existing 16-input frame.
No type_registry or authored enclosing ue_struct is needed for these signatures.

Unreal function parameters/results are real FArrayProperty, FSetProperty and
FMapProperty values, with ordinary native Blueprint pins. Float/double and FName
remain distinct. The authoring IR retains array/set/map shapes and identities.
Internally, the Actor adapter's scalar-frame ABI uses native private carrier
USTRUCTs with one Values field, passed as owned FInstancedStruct snapshots.
These carriers are compiled runtime types, not generated user assets or JSON
value payloads. Users do not declare them in the DSL or connect carrier pins.

The daScript implementation receives an owned container. Input snapshots,
unpacked VM containers, returned snapshots and native result storage are separate.
Use clone and move return syntax for owned results; mutations of that local copy
do not modify native input storage. Unreal K2 automatically marks array input
properties ref/out; the adapter accepts this native convention and still copies
values into the script. This does not introduce DSL out/inout parameters.

The current 4096 total elements per snapshot and 16 MiB fixed element storage
limits apply. FString heap contents are not part of that fixed-byte budget.
Wrong carrier/type, invalid indexes and oversized inputs reject. String-key
collisions under native equality follow DSLMapAuthoring.md / DSLSetAuthoring.md.
Packing currently copies snapshots per insertion; bulk optimization is later work.

Explicit ref/out/inout, parameter defaults, pure/static authoring methods, nested
container elements, object/enum/struct elements and more key types are not included
in this milestone. Class container properties and structural migration policies
remain separate work; the previous migration deferral is unchanged.

The source example is Plugins/DasForUE/Content/Das/Scripts/dsl_container_calls.das.
Validation: Scripts/TestDSLTypeRegistry.ps1 -ContainerCalls -Cooked.
Next: DSL out/ref semantics and interfaces.
