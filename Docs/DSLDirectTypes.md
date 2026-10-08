# Direct authored daScript types

`ue_class` can now use annotated plain daScript enums and structs directly as
properties, method parameters and results. See
`Plugins/DasForUE/Content/Das/Scripts/dsl_direct_types.das`.

```das
[ue_class(module_id="authoredvalues", type_folder="/Game/DasAuthoredTypes", Blueprintable)]
class TypedActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Config : Settings = Settings()

    [ue_function(BlueprintCallable)]
    def Bump(amount:int):int {
        self.Config.Count += amount
        return self.Config.Count
    }

    [ue_function(BlueprintCallable)]
    def Evaluate(mode:State; data:Settings):Settings {
        var result = data
        result.Count += int(mode)
        return result
    }
}
```

Declare `State` with `ue_enum` and `Settings` with `ue_struct` before the class.
First apply the types from the same source using
`Das.DSL.Types.Apply dsl_direct_types.das /Game/DasAuthoredTypes`, then apply
the class through the existing DSL import workflow. Assets remain dirty until
saved. Type collection compiles without running script code and skips class
adapter generation so the combined source works before type assets exist.

The compiler generates hidden natural-type implementations and exported bridge
entry points. They retain the existing typed Unreal call contract. Enum conversion
checks the exact native enum identity and value before exposing a daScript enum.
Struct adapters build and read owned native snapshots field by field; they do not
assume binary layout compatibility. Strings cross the boundary as copied values.
The compiler checks loaded enum members/values, struct fields/storage and defaults
against the declarations before accepting an adapter.

Reflected `self.Config` reads materialize a local value. Whole struct assignment
packs every field. Direct scalar leaf assignments and compound `+=`, `-=`, `*=`,
`/=` update the native property. Their commit helper reads the latest snapshot
after the RHS has been evaluated, preserving RHS changes to other fields.
BlueprintReadOnly on either the property or the struct member rejects writes.

Current bounds: explicit `type_folder` or `type_registry`, authored assets and
32-bit daScript enums with explicit integer members 0..254. Nested structs,
authored struct arrays, primitive arrays and supported maps/sets are now covered
by DSLNestedTypeAuthoring.md, DSLArrayTypeAuthoring.md, DSLPrimitiveArrays.md,
DSLMapAuthoring.md, DSLSetAuthoring.md and DSLScalarMapsValidation.md. Class struct
defaults use an empty constructor or no initializer; overrides still reject.
Explicit out/ref supports primitives and standalone containers (DSLOutRef.md);
direct enum/struct reference parameters remain later work. Existing supported
definition changes have dedicated policies; universal migration stays deferred.
Stable ID resolution and cooked direct invocation are covered by DSLTypeRegistry.md.
Existing wrapper syntax remains supported.

Validation: `Scripts/TestDSLDirectTypesAsset.ps1`,
`Scripts/TestDSLDirectTypesPIE.ps1` and `Docs/DSLDirectTypesValidation.md`.
