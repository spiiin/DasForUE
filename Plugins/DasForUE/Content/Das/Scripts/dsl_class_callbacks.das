options gen2
require ue_dsl
require dsl_class_callback_interface
require ue_class_callback_signals
[ue_class(Blueprintable,type_folder="/Game/DasClassCallbacks/Types"), ue_implements(interface=ClassContract)]
class ClassCallbackActor : Actor {
    properties=ue_properties(BlueprintReadWrite) {
        var Calls:int=0
        var Events:int=0
        var MixedEvents:int=0
        var Next:int=0
        var MixedNext:int=0
        var Repeats:int=0
        var Nulls:int=0
        var NullEvents:int=0
        var Rejected:int=0
        var Errors:int=0
    }
    @ue_property=(BlueprintAssignable)
    Changed:ClassSignal
    @ue_property=(BlueprintAssignable)
    MixedChanged:MixedClassSignal
    [ue_function(BlueprintCallable)]
    def EchoClass(value:ActorClass):ActorClass { self.Calls+=1;return value }
    [ue_function(BlueprintCallable)]
    def ChangeClass(var value:ActorClass&; @ue_out var previous:ActorClass&):ActorClass {
        previous=value;value=default<ActorClass>;return previous
    }
    def BeginPlay():void {
        let actor=ue_class_cast(type<ActorClass>,ue_class_loaded(type<ActorRef>,"/Script/Engine.StaticMeshActor"))
        let callback=ue_bind(type<ClassQuery>,ue_self_object(),"EchoClass")
        if(ue_class_path(callback.Execute(actor))!="/Script/Engine.StaticMeshActor") { self.Errors+=1 }
        if(ue_class_valid(callback.Execute(default<ActorClass>))) { self.Errors+=1 }
        let mutate=ue_bind(type<ClassMutation>,ue_self_object(),"ChangeClass")
        var value=actor;var previous:ActorClass
        let result=mutate.Execute(value,previous)
        if(ue_class_valid(value) || ue_class_path(previous)!=ue_class_path(actor) || ue_class_path(result)!=ue_class_path(actor)) { self.Errors+=1 }
        self.Changed.Broadcast(actor)
        self.Changed.Broadcast(default<ActorClass>)
        self.MixedChanged.Broadcast(actor,17)
        ue_timer_next_tick("class-next","NextClass",actor)
        ue_timer_next_tick("class-mixed","NextMixed",actor,17)
        ue_timer_next_tick("class-null","NullClass",default<ActorClass>)
        ue_timer_start("class-repeat",0.002f,"RepeatClass",true,actor)
        try { ue_timer_start("class-repeat",0.0f,"RepeatClass",true,actor) } recover { self.Rejected+=1 }
        ue_timer_start("class-cancel",1000.0f,"Cancelled",false,actor)
        ue_timer_cancel("class-cancel")
    }
    [ue_event(name="Changed")]
    def ClassChanged(value:ActorClass):void {
        if(!ue_class_valid(value)) { self.NullEvents+=1;return }
        if(ue_class_path(value)!="/Script/Engine.StaticMeshActor") { self.Errors+=1 };self.Events+=1
    }
    [ue_event(name="MixedChanged")]
    def MixedClassChanged(value:ActorClass; number:int):void {
        if(ue_class_path(value)!="/Script/Engine.StaticMeshActor" || number!=17) { self.Errors+=1 };self.MixedEvents+=1
    }
    [ue_function(BlueprintCallable)]
    def NextClass(value:ActorClass):void {
        if(ue_class_path(value)!="/Script/Engine.StaticMeshActor") { self.Errors+=1 };self.Next+=1
    }
    [ue_function(BlueprintCallable)]
    def NextMixed(value:ActorClass; number:int):void {
        if(ue_class_path(value)!="/Script/Engine.StaticMeshActor" || number!=17) { self.Errors+=1 };self.MixedNext+=1
    }
    [ue_function(BlueprintCallable)]
    def NullClass(value:ActorClass):void { if(ue_class_valid(value)) { self.Errors+=1 };self.Nulls+=1 }
    [ue_function(BlueprintCallable)]
    def RepeatClass(value:ActorClass):void {
        if(ue_class_path(value)!="/Script/Engine.StaticMeshActor") { self.Errors+=1 };self.Repeats+=1
        if(self.Repeats==3) { ue_timer_cancel("class-repeat") }
    }
    [ue_function(BlueprintCallable)]
    def Cancelled(value:ActorClass):void { self.Errors+=1 }
}
