options gen2
require ue_dsl

// Reflected authoring schema; import once with Das.DSL.Types.Apply.
[ue_struct(BlueprintType)]
struct ItemRow {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Label:string=""
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Price:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Weight:double=0.0lf
}
