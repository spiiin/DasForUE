options gen2
require ue_object_types
require ue_dsl
[ue_object_ref(script="dsl_dynamic_lifecycle.das",type="DynamicLife")]
struct DynamicLifeRef { handle:UObjectHandle }
[ue_class(module_id="dynamicowner",Blueprintable)]
class DynamicOwner {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    ComponentClass:string=""
    [ue_function(BlueprintCallable)]
    def Create(input:bool):bool {
        let actor=ue_ref(type<ActorRef>,ue_self_object())
        let component=ue_component_create(type<DynamicLifeRef>,actor,"DynamicLife",self.ComponentClass)
        return component.Begins==1
    }
    [ue_function(BlueprintCallable)]
    def Remove(input:bool):bool {
        let actor=ue_ref(type<ActorRef>,ue_self_object())
        let component=ue_component_get(type<DynamicLifeRef>,actor,"DynamicLife")
        return ue_component_destroy_handle(component.handle)
    }
}
