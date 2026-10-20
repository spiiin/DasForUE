options gen2
options gc
require ue_dsl

// Self container operations: scalar shapes are lowered to in-place externs, the rest to snapshot helpers.
[ue_class(module_id="selfcontainers", Blueprintable)]
class SelfContainerActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Ints:array<int>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Labels:array<string>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Names:array<UENameValue>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Flags:array<bool>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Bytes:array<uint8>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Weights:table<string;double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Counts:table<int;int>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    NameScores:table<UENameValue;float>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    ByteLabels:table<uint8;string>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Keywords:table<string>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Ids:table<int>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Points:array<UEVector>

    [ue_function(BlueprintCallable)]
    def Arrays(value:int):int {
        let at = 2l
        push(self.Ints, value)
        push(self.Ints, value, 0)
        push(self.Ints, value, at)
        push_clone(self.Labels, "label")
        push(self.Names, ue_name_value("Name"))
        push(self.Flags, true)
        push(self.Bytes, uint8(7))
        self.Ints[0] = 4
        self.Ints[1l] += value
        self.Labels[0] = "first"
        self.Flags[0] = false
        pop(self.Ints)
        erase(self.Ints, 0)
        erase(self.Ints, 0l, 0l)
        let total = length(self.Ints)
        clear(self.Labels)
        return total
    }
    [ue_function(BlueprintCallable)]
    def Tables(key:string; id:int):bool {
        insert(self.Weights, key, 1.5lf)
        self.Weights[key] += 2.0lf
        self.Counts[id] = 3
        self.Counts[id] *= 2
        self.NameScores[ue_name_value(key)] = 1.0
        insert(self.ByteLabels, uint8(1), key)
        insert(self.Keywords, key)
        insert(self.Ids, id)
        let removed = erase(self.Weights, key) && erase(self.Keywords, key) && erase(self.Ids, id)
        clear(self.Counts)
        return removed
    }
    [ue_function(BlueprintCallable)]
    def Fallback():int {
        push(self.Points, ue_vector(1.0lf, 2.0lf, 3.0lf))
        self.Points[0].x += 1.0lf
        return length(self.Points)
    }
}
