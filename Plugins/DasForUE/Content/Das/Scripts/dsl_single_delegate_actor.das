options gen2
require ue_delegate_examples
[ue_class(module_id="singledelegate",Blueprintable)]
class SingleDelegateActor {
    @ue_property=(BlueprintReadWrite)
    Callback:FScoreQuery
    @ue_property=(BlueprintReadWrite)
    Copy:FScoreQuery
    @ue_property=(BlueprintReadWrite)
    Score:int=0
    [ue_function(BlueprintCallable)]
    def Handler(value:int):int { self.Score+=value; return self.Score }
    [ue_function(BlueprintCallable)]
    def Bind():void {
        self.Callback=ue_bind(type<FScoreQuery>,ue_self_object(),"Handler")
        self.Copy=self.Callback
    }
    [ue_function(BlueprintCallable)]
    def Invoke(value:int):int { return self.Callback.Execute(value) }
    [ue_function(BlueprintCallable)]
    def Clear():void { self.Callback=FScoreQuery() }
    [ue_function(BlueprintCallable)]
    def InvokeCopy(value:int):int { return self.Copy.Execute(value) }

    // Accepts a binding value from Blueprint or another DAS context.
    [ue_function(BlueprintCallable)]
    def ExecuteCallback(callback:FScoreQuery, value:int):int {
        return callback.Execute(value)
    }
    // Returns an independent copy of the received binding.
    [ue_function(BlueprintCallable)]
    def EchoCallback(callback:FScoreQuery):FScoreQuery { return callback }
    [ue_function(BlueprintCallable)]
    def GetCallback():FScoreQuery { return self.Copy }
    [ue_function(BlueprintCallable)]
    def StoreCallback(callback:FScoreQuery):void { self.Callback=callback }

    @ue_property=(BlueprintReadWrite)
    Mutation:FValueMutation
    @ue_property=(BlueprintAssignable)
    OnCountChanged:FCountChanged

    [ue_function(BlueprintCallable)]
    def Mutate(var Count:int&; var Text:string&;
               @ue_out var Accepted:bool&; @ue_out var Description:string&):int {
        Count+=3
        Text+=" edited"
        Accepted=true
        Description=Text
        return Count*2
    }
    [ue_function(BlueprintCallable)]
    def FirstCount(var Counter:int&):void { Counter+=2 }
    [ue_function(BlueprintCallable)]
    def SecondCount(var Counter:int&):void { Counter+=4 }
    [ue_function(BlueprintCallable)]
    def ProbeRefOut():int {
        self.Mutation=ue_bind(type<FValueMutation>,ue_self_object(),"Mutate")
        var count=5
        var text="start"
        var accepted=false
        var description="old"
        let answer=self.Mutation.Execute(count,text,accepted,description)
        if(count!=8 || text!="start edited" || !accepted || description!=text || answer!=16) {
            panic("authored delegate ref/out failed")
        }
        self.OnCountChanged.Clear()
        self.OnCountChanged.AddUFunction(ue_self_object(),"FirstCount")
        self.OnCountChanged.AddUFunction(ue_self_object(),"SecondCount")
        self.OnCountChanged.Broadcast(count)
        // Both handlers update the same value; listener order does not matter.
        self.OnCountChanged.Clear()
        return answer+count
    }

    [ue_function(BlueprintCallable)]
    def MutateArray(var Values:array<double>&; @ue_out var Previous:array<double>&):void {
        Previous <- clone(Values)
        Values[0]+=10.0lf
    }
    [ue_function(BlueprintCallable)]
    def ProbeRefArray():int {
        let callback=ue_bind(type<FArrayMutation>,ue_self_object(),"MutateArray")
        var values:array<double>
        push(values,1.0lf)
        push(values,2.0lf)
        var previous:array<double>
        callback.Execute(values,previous)
        if(length(values)!=2 || values[0]!=11.0lf || values[1]!=2.0lf ||
           length(previous)!=2 || previous[0]!=1.0lf || previous[1]!=2.0lf) {
            panic("delegate array ref/out failed")
        }
        return 1
    }
}
