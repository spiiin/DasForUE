options gen2
require ue_dsl
require ue_probe_actor
require ue_actor_refs
require ue_component_types
[ue_class(Blueprintable)]
class ScoreBehavior : ActorComponent {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Bonus:int=10
        var SignalTotal:int=0
        var BeginCount:int=0
        var TickCount:int=0
        var EndCount:int=0
        var Seconds:float=0.0
    }
    [ue_function(BlueprintCallable)]
    def Evaluate(input:int):int { return self.Bonus + input }
    [ue_function(BlueprintCallable)]
    def Accumulate(var value:int&):int { value += self.Bonus; return value }
    [ue_function(BlueprintCallable)]
    def MakeScore(input:int; @ue_out var value:int&):void { value=self.Bonus+input }
    [ue_function(BlueprintCallable)]
    def Bounce(input:int):int {
        if (input<=0) { return self.Bonus }
        let actor=ue_owner(type<ComponentOwnerRef>)
        return actor.Bounce(input-1)+1
    }
    [ue_function(BlueprintCallable)]
    def EvaluateViaHelper(input:int):int {
        let component=ue_ref(type<ScoreComponent>,ue_self_object())
        return evaluate_component(component,input)
    }
    [ue_function(BlueprintCallable)]
    def EvaluateThroughOwner(input:int):int {
        let actor=ue_owner(type<ComponentOwnerRef>)
        return actor.Behavior.Evaluate(input)
    }
    [ue_function(BlueprintCallable)]
    def Adjust(var ratio:float&; var precise:double&; var enabled:bool&;
               var caption:string&; @ue_out var key:UENameValue&):void {
        ratio += 0.5
        precise *= 2.0lf
        enabled = !enabled
        caption += "!"
        key = ue_name_value("Changed")
    }
    [ue_function(BlueprintCallable, arg_value_enum="EDasCallMode", return_enum="EDasCallMode")]
    def EchoMode(value:UEEnumValue):UEEnumValue { return value }
    [ue_function(BlueprintCallable, arg_value_struct="DasCallPayload", return_struct="DasCallPayload")]
    def EchoPayload(value:UEStructValue):UEStructValue { return value }
    [ue_function(BlueprintCallable, arg_value_class="Actor", return_class="Actor")]
    def EchoActor(value:UObjectHandle):UObjectHandle { return value }
    [ue_function(BlueprintCallable)]
    def FailAfterWrite(var value:int&):void { value=999; panic("method fixture failure") }
    [ue_function(BlueprintCallable)]
    def EchoComponent(value:ScoreComponent):ScoreComponent { return value }
    [ue_event(source=owner,source_type=ProbeActorRef, name="OnDasSignal")]
    def OnOwnerSignal(amount:int) { self.SignalTotal += amount }
    def BeginPlay() { self.BeginCount += 1 }
    def Tick(delta:float) {
        self.TickCount += 1
        self.Seconds += delta
        let actor = ue_owner(type<ComponentOwnerRef>)
        actor.Score += 1
    }
    def EndPlay(reason:int) { self.EndCount += 1 }
}

// Ordinary DAS function uses the same overload as a DSL method.
def evaluate_component(component:ScoreComponent; input:int):int {
    return component.Evaluate(input)
}
