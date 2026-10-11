options gen2
require ue_object_types
require ue_dsl
[ue_object_ref(script="dsl_dynamic_lifecycle.das",type="DynamicLife")]
struct DynamicLifeRef { handle:UObjectHandle }
[ue_class(module_id="dynamicowner",Blueprintable)]
class DynamicOwner {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_class_base="/Script/DasRuntime.DasScriptComponent"
    ComponentClass:UEClassHandle
    [ue_function(BlueprintCallable)]
    def Create(input:bool):bool {
        let actor=ue_ref(type<ActorRef>,ue_self_object())
        let componentClass=self.ComponentClass
        let component=ue_component_create(type<DynamicLifeRef>,actor,"DynamicLife",componentClass)
        return ue_class_valid(componentClass) && ue_class_path(componentClass)==ue_class_path(self.ComponentClass) && component.Begins==1
    }
    [ue_function(BlueprintCallable)]
    def WrongBase(input:bool):bool {
        let wrong=ue_class_loaded(type<ActorRef>,ue_class_path(self.ComponentClass))
        return ue_class_valid(wrong)
    }
    [ue_function(BlueprintCallable)]
    def WrongIdentity(input:bool):bool {
        let wrong=ue_class_loaded(type<DynamicLifeRef>,"/Script/DasRuntime.DasScriptComponent")
        return ue_class_valid(wrong)
    }
    [ue_function(BlueprintCallable)]
    def MissingClass(input:bool):bool {
        let wrong=ue_class_loaded(type<DynamicLifeRef>,"/Game/Missing/BP_Missing.BP_Missing_C")
        return ue_class_valid(wrong)
    }
    [ue_function(BlueprintCallable)]
    def WrongCreate(input:bool):bool {
        let actor=ue_ref(type<ActorRef>,ue_self_object())
        let wrong=ue_class_loaded(type<ActorRef>,"/Script/Engine.Actor")
        let component=ue_component_create(type<DynamicLifeRef>,actor,"WrongClass",wrong)
        return ue_object_valid(component.handle)
    }
    [ue_function(BlueprintCallable)]
    def NullClass(input:bool):bool {
        var missing:UEClassHandle
        return !ue_class_valid(missing)
    }
    [ue_function(BlueprintCallable)]
    def WrongWrite(input:bool):bool {
        self.ComponentClass=ue_class_loaded(type<ActorRef>,"/Script/Engine.Actor")
        return true
    }
    [ue_function(BlueprintCallable)]
    def ClassRoundtrip(input:bool):bool {
        let previous=self.ComponentClass
        var empty:UEClassHandle
        self.ComponentClass=empty
        let cleared=!ue_class_valid(self.ComponentClass)
        self.ComponentClass=previous
        return cleared && ue_class_valid(self.ComponentClass)
    }
    [ue_function(BlueprintCallable)]
    def Remove(input:bool):bool {
        let actor=ue_ref(type<ActorRef>,ue_self_object())
        let component=ue_component_get(type<DynamicLifeRef>,actor,"DynamicLife")
        return ue_component_destroy_handle(component.handle)
    }
}
