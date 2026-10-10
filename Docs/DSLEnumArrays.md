# Arrays of authored enum values

array<AuthorEnum> is supported for ue_class fields, ue_struct members and reflected
method input/result/ref/out values. Enum members in authored structs must resolve
within the existing reflected module/registry contract.

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
States : array<MethodState>

[ue_function(BlueprintCallable)]
def CopyStates(values : array<MethodState>) : array<MethodState> {
    return <- clone(values)
}

push(self.States, MethodState.Active)
self.States[0l] = MethodState.Idle
self.Groups[0].States[0] = MethodState.Active

var values = api.CopyStates(api.States)
api.States := values
```

Unreal receives a native array with the exact authored UEnum identity. Internal
carrier names encode the enum path; the cache checks member names, values and
stable member IDs. Declaration changes are still outside the existing carrier
layout/migration contract.

Arrays start empty. Enum values need not include zero: native snapshot resizing
initializes added elements with the first usable enum member before validation.
Actual source elements replace these temporary defaults during packing. Existing
elements are preserved; explicit enum array defaults remain unsupported.

Generated converters check the enum identity and every value. Another enum with
the same numbers and raw integers cannot substitute for the declared source type.
An unsafe invalid value, including a reserved sentinel, is rejected before native
publication. Reads reject malformed native enum elements as well. Readonly roots,
aggregate snapshot limits and ref/out atomic copyback remain in force.

This stage supports authored UDasDefinedEnum arrays only. Native enum arrays,
enum sets/maps, UObject/interface arrays, nonempty defaults, migration and broad
Shipping/AOT coverage remain separate work.

Fixtures: ValueTarget.EnumArrays/CopyStates/RefStates/FailStates and MethodGroup.States.
Validation: DSLEnumArraysValidation.md.
