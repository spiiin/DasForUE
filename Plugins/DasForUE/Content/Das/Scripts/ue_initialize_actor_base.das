options gen2
module ue_initialize_actor_base public
require ue_dsl public
[ue_class(module_id="initactors",Blueprintable)]
class InitActorBase {
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
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    BaseBegins:int=0
    def BeginPlay():void { self.BaseBegins+=1 }
    def Initialize():void { self.InitTrace=1; self.InitCount+=1; self.SeenEditable=self.EditableValue }
}
