options gen2
require ue_dsl
require ue_component_types
[ue_class(module_id="actorcomponent", Blueprintable)]
class ComponentOwner {
    components = ue_components(VisibleAnywhere, BlueprintReadOnly, Category="Components") {
        // Подсчёт очков
        var Behavior:ScoreComponent
    }
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        // Текущие очки владельца.
        var Score:int=0
        var Begins:int=0
        var Ticks:int=0
        var Ends:int=0
    }

    // Возвращает текущие очки владельца.
    [ue_function(BlueprintCallable)]
    def GetScore():int { return self.Score }

    [ue_function(BlueprintCallable)]
    def ReadBehaviorBonus():int { return self.Behavior.Bonus }
    [ue_function(BlueprintCallable)]
    def ConfigureBehavior(bonus:int):void { self.Behavior.Bonus = bonus }
    [ue_function(BlueprintCallable)]
    def AddBehaviorBonus(amount:int):int {
        self.Behavior.Bonus += amount
        let behavior = self.Behavior
        return behavior.Bonus
    }
    [ue_function(BlueprintCallable)]
    def CallBehavior(input:int):int { return self.Behavior.Evaluate(input) }
    [ue_function(BlueprintCallable)]
    def CallBehaviorRef(input:int):int {
        var value=input
        let result=self.Behavior.Accumulate(value)
        return result+value
    }
    [ue_function(BlueprintCallable)]
    def CallBehaviorOut(input:int):int {
        var value=-1
        self.Behavior.MakeScore(input,value)
        return value
    }
    [ue_function(BlueprintCallable)]
    def Bounce(input:int):int { return self.Behavior.Bounce(input) }
    [ue_function(BlueprintCallable)]
    def CallBehaviorHelper(input:int):int { return self.Behavior.EvaluateViaHelper(input) }
    [ue_function(BlueprintCallable)]
    def CallValues():int {
        var ratio=1.5
        var precise=2.25lf
        var enabled=true
        var caption="Initial"
        var key=ue_name_value("Initial")
        self.Behavior.Adjust(ratio,precise,enabled,caption,key)
        let mode=self.Behavior.EchoMode(ue_enum_value("EDasCallMode","Active"))
        let payload=self.Behavior.EchoPayload(ue_struct_default("DasCallPayload","(Count=31)"))
        let actor=self.Behavior.EchoActor(ue_self_object())
        return (ratio==2.0 && precise==4.5lf && !enabled && caption=="Initial!" &&
                ue_names_equal(key,ue_name_value("Changed")) && ue_enum_number(mode)==7 &&
                ue_struct_get_int(payload,"Count")==31 && ue_object_is_a(actor,"Actor")) ? 1 : 0
    }
    [ue_function(BlueprintCallable)]
    def FailurePreservesRef():int {
        var value=7
        try { self.Behavior.FailAfterWrite(value) } recover { return value }
        return -1
    }
    [ue_function(BlueprintCallable)]
    def RepeatFailure():int {
        for (index in range(300)) {
            var value=7
            try { self.Behavior.FailAfterWrite(value) } recover {}
            if (value!=7) { return 0 }
        }
        return 1
    }
    [ue_function(BlueprintCallable)]
    def CallViaOwner(input:int):int { return self.Behavior.EvaluateThroughOwner(input) }
    [ue_function(BlueprintCallable)]
    def RoundtripComponent():int { return self.Behavior.EchoComponent(self.Behavior).Bonus }
    def BeginPlay() { self.Begins += 1; self.Score += 100 }
    def Tick(delta:float) { self.Ticks += 1 }
    def EndPlay(reason:int) { self.Ends += 1 }
}
