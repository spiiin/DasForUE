options gen2
require ue_initialize_actor_middle public
require ue_dsl
require ue_object_types
[ue_object_ref(script="dsl_dynamic_lifecycle.das",type="DynamicLife")]
struct SpawnLifeRef { handle:UObjectHandle }
[ue_class(module_id="spawntarget",Blueprintable)]
class SpawnTarget : InitActorMiddle {
    @ue_component
    Life:SpawnLifeRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Begins:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Ticks:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Ends:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    InitComponentsReady:bool=false
    def override Initialize():void { Super.Initialize(); self.InitTrace=self.InitTrace*10+3; self.InitCount+=1; self.InitComponentsReady=ue_object_valid(self.Life.handle) && self.Life.InitTrace==12 }
    def override BeginPlay() { self.BeginSawTrace=self.InitTrace; self.Begins += 1 }
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

    [ue_function(BlueprintCallable)]
    def TryReinitialize(input:bool):bool { Super.Initialize(); return true }
}
