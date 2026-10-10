options gen2
require ue_dsl
[ue_class(module_id="refscalar", Blueprintable)]
class ScalarTarget {
 @ue_property=(EditAnywhere, BlueprintReadWrite)
 Number:int=10
 @ue_property=(EditAnywhere, BlueprintReadWrite)
 Ratio:float=1.5
 @ue_property=(EditAnywhere, BlueprintReadWrite)
 Precise:double=2.25lf
 @ue_property=(EditAnywhere, BlueprintReadWrite)
 Enabled:bool=true
 @ue_property=(EditAnywhere, BlueprintReadWrite)
 Label:string="Initial"
 @ue_property=(EditAnywhere, BlueprintReadWrite)
 Key:UENameValue=ue_name_value("Initial")
 @ue_property=(EditAnywhere, BlueprintReadOnly)
 Locked:int=9
 @ue_property=(EditAnywhere, BlueprintReadWrite)
 Payload:UEStructValue=ue_struct_default("DasCallPayload","(Count=10)")
 @ue_property=(EditAnywhere, BlueprintReadWrite)
 Mode:UEEnumValue=ue_enum_value("EDasCallMode","Idle")
 @ue_property=(EditAnywhere, BlueprintReadWrite)
 Target:UObjectHandle=ue_object_value("Actor")
 def BeginPlay() {}
}
