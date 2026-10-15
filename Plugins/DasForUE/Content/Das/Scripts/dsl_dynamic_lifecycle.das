options gen2
require ue_initialize_component_base public
require ue_dsl
require ue_object_types
[ue_class(module_id="dynamiclife",Blueprintable)]
class DynamicLife : InitComponentBase {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Begins:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Ticks:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Ends:int=0
    def override Initialize():void { Super.Initialize(); Super.Initialize(); self.InitTrace=self.InitTrace*10+2; self.InitCount+=1 }
    def BeginPlay() { self.BeginSawTrace=self.InitTrace; self.Begins += 1 }
    def Tick(delta:float) { self.Ticks += 1 }
    def EndPlay(reason:int) { self.Ends += 1 }

    @ue_property=(EditAnywhere,BlueprintReadWrite)
    SelectedClass:ActorClass
    [ue_function(BlueprintCallable)]
    def EchoClass(value:ActorClass):ActorClass { self.SelectedClass=value; return self.SelectedClass }
    [ue_function(BlueprintCallable)]
    def EchoAnyClass(value:ObjectClass):ObjectClass { return value }
    [ue_function(BlueprintCallable,out="output",ref="changed")]
    def ClassOutputs(value:ActorClass; var output:ActorClass&; var changed:ActorClass&):ActorClass {
        output=value
        changed=default<ActorClass>
        return value
    }
    [ue_function(BlueprintCallable,return_class="/Script/Engine.Actor")]
    def BadClassResult():UEClassHandle { return ue_class_loaded(type<ObjectRef>,"/Script/Engine.SceneComponent") }
    [ue_function(BlueprintCallable,arg_output_class="/Script/Engine.Actor",out="output")]
    def BadClassOutput(var output:UEClassHandle&):void { output=ue_class_loaded(type<ObjectRef>,"/Script/Engine.SceneComponent") }
}
