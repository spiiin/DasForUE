options gen2
options gc
require ue_dsl

[ue_struct(module_id="setdemo", BlueprintType)]
struct Tags {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Ids : table<int>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Names : table<string>
}
[ue_class(module_id="setdemo", type_registry="REGISTRY", Blueprintable)]
class SetActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Config : Tags = Tags()
    [ue_function(BlueprintCallable)]
    def Add(name:string):void {
        var data <- clone(self.Config)
        insert(data.Names, name)
        self.Config <- data
    }
    [ue_function(BlueprintCallable)]
    def Remove(name:string):void {
        var data <- clone(self.Config)
        erase(data.Names, name)
        self.Config <- data
    }
    [ue_function(BlueprintPure)]
    def Contains(name:string):bool {
        var data <- clone(self.Config)
        return key_exists(data.Names, name)
    }
    [ue_function(BlueprintCallable)]
    def Echo(value:Tags):Tags {
        var result <- clone(value)
        return <- result
    }
}
