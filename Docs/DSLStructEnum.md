# Authored enum members in authored structs

An authored ue_struct can contain a ue_enum declared in the same reflected type
module. Use its natural enum type and an explicit named default:

```das
[ue_enum(module_id="game", BlueprintType)]
enum State {
    Idle = 7
    Active = 42
}

[ue_struct(module_id="game", BlueprintType)]
struct Status {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Mode:State = State.Active
}
```

The named initializer is required. The collector checks its declared enum and
member; an integer initializer or an implicit zero default is not used. Enum
definitions need not include zero. The reflected type model validates the enum
reference and default against the module's declarations.

Unreal stores a real enum-backed property. Creation prepares that enum variable
description with its default before its first structure compilation, avoiding
an invalid intermediate zero for enums such as 7/42. Reuse checks both the native
enum identity and the stored member value. No struct-valued runtime JSON is added.

Natural pack/unpack helpers transfer enum members through the existing snapshot
path, validating enum identity and member values. This includes nested authored
structs and arrays of authored structs, whole properties and method input/result/
ref/out. A different enum with identical numbers remains a different type.

```das
var status = api.Status
status.Mode = State.Idle
api.Status = status
```

Inside a ue_class method, direct nested enum assignment also works:

```das
self.Settings.Point.Mode = MethodState.Idle
```

The generated commit helper accepts the exact source enum type before packing;
another enum or an integer does not silently become the declared enum. A commit
reads the latest native snapshot after evaluating the assigned value, changes
the enum member and assigns the whole property back. Other members remain intact.
Readonly roots reject writes; compound enum arithmetic is unsupported. Runtime
snapshot assignment checks exact enum identity and valid member values.

For another object's typed view, use local-copy/whole-property assignment.
Direct containers of enums, native generated enums and cross-module reflected
enum dependencies remain separate work. Existing explicit registry/folder and import rules remain.

Examples: MethodPoint.Mode in ue_method_values.das and its use in
dsl_method_value_target.das / dsl_method_value_driver.das.
Validation: [DSLStructEnumValidation.md](DSLStructEnumValidation.md).

Self-write validation: [DSLSelfEnumWritesValidation.md](DSLSelfEnumWritesValidation.md).
