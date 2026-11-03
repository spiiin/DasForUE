options gen2
module ue_int64_types public
require ue_dsl public

[ue_struct(module_id="int64test", BlueprintType)]
struct Int64Record {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Value : int64 = 9007199254740993l
}
