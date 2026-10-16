options gen2
module ue_initialize_component_base public
require ue_dsl public
[ue_class(module_id="initcomponents",Blueprintable)]
class InitComponentBase : ActorComponent {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    InitTrace:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    InitCount:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    EditableValue:int=7
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    SeenEditable:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    BeginSawTrace:int=0
    def Initialize():void { self.InitTrace=1; self.InitCount+=1; self.SeenEditable=self.EditableValue }
}
