options gen2
module identity_defaults public
require ue_dsl
require ue_object_types
[ue_class(module_id="identitydefaults", Blueprintable)]
class IdentityDefaults {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Kind:ActorClass = ue_class_default(type<ActorRef>)
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Caption:UETextValue = ue_loctext(Caption, "Локализовано 世界")
    [ue_function(BlueprintCallable)]
    def Read(captionArg:UETextValue=ue_loctext(Param, "Argument")):int { return 1 }
}
