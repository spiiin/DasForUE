options gen2
require ue_gameplay_services
require ue_health_signals

[ue_class(Blueprintable)]
class HealthComponent : ActorComponent {
    settings=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Maximum:float=100.0f
    }
    state=ue_properties(BlueprintReadWrite) {
        var Current:float=100.0f
        var DamageEvents:int=0
    }
    events=ue_properties(BlueprintAssignable) { var Changed:HealthChangedSignal }
    def BeginPlay():void {
        verify(self.Maximum>0.0f && self.Maximum<=3.402823466e38f,"Maximum health must be finite and positive")
        self.Current=self.Maximum
    }
    [ue_event(source="owner",name="OnTakeAnyDamage")]
    def OnDamage(damaged:ActorRef; amount:float; kind:ObjectRef; instigator:ObjectRef; source:ActorRef):void {
        if(!(amount>0.0f && amount<=3.402823466e38f) || self.Current<=0.0f) { return }
        self.DamageEvents+=1
        if(amount>=self.Current) { self.Current=0.0f } else { self.Current-=amount }
        self.Changed.Broadcast(self.Current,self.Maximum)
    }
    [ue_function(BlueprintCallable)]
    def Heal(amount:float):void {
        verify(amount<=3.402823466e38f,"Healing amount must be finite")
        if(amount<=0.0f) { return }
        self.Current+=amount
        if(self.Current>self.Maximum) { self.Current=self.Maximum }
        self.Changed.Broadcast(self.Current,self.Maximum)
    }
}
