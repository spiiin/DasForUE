options gen2
module ue_library_values_types public
require ue_dsl public
[ue_enum(BlueprintType)]
enum LibraryState {
 Idle=7
 Active=42
}
[ue_struct(BlueprintType)]
struct LibrarySettings {
 @ue_property=(EditAnywhere,BlueprintReadWrite)
 Count:int=10
 @ue_property=(EditAnywhere,BlueprintReadWrite)
 Title:string="Initial"
}
