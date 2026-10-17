options gen2
options gc
require ue_dsl

[ue_class(module_id="dispatchprobe",Blueprintable)]
class DispatchProbe {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) { var Score:int=0 }

    def BeginPlay():void { self.ResetScore() }
    def ResetScore():void { self.Score=3 }
    def Add(value:int):int { self.Score+=value; return self.Score }
    def Adjust(var value:int&):void { value+=5 }
    def Sum(values:array<int>):int {
        var total=0
        for (value in values) { total+=value }
        return total
    }
    def Recurse(depth:int):int {
        if (depth<=0) { return self.Score }
        return self.Recurse(depth-1)
    }
    [ue_function(BlueprintCallable)]
    def Twice(input:int):int { return input*2 }
    [ue_function(BlueprintCallable)]
    def Exercise(input:int):int {
        let score=self.Add(input)
        var value=2
        self.Adjust(value)
        var values:array<int>
        push(values,1); push(values,2)
        if (self.Recurse(4)!=score || self.Twice(3)!=6) { return -1 }
        return score+value+self.Sum(values)
    }
    [ue_function(BlueprintEvent)]
    def Hook(input:int):int { self.Score+=input; return self.Score }
    [ue_function(BlueprintCallable)]
    def Query(input:int):int { return self.Hook(input) }
}
