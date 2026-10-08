options gen2
options gc
require ue_dsl
[ue_struct(id="nested.Leaf", module_id="nested", BlueprintType)]
struct Leaf {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count : int = 10
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title : string = "Initial"
}
[ue_struct(id="nested.Branch", module_id="nested", BlueprintType)]
struct Branch {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Inner : Leaf = Leaf()
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Weight : double = 1.25lf
}
[ue_struct(id="nested.Tree", module_id="nested", BlueprintType)]
struct Tree {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Left : Branch = Branch()
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Right : Branch = Branch()
}
[ue_class(module_id="nested", type_registry="REGISTRY", Blueprintable)]
class NestedActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Config : Tree = Tree()
    [ue_function(BlueprintCallable)]
    def Bump(amount:int):int {
        self.Config.Left.Inner.Count += amount
        return self.Config.Left.Inner.Count
    }
    [ue_function(BlueprintCallable)]
    def Echo(value:Tree):Tree {
        var result = value
        result.Right.Inner.Count += 5
        result.Right.Inner.Title = "Nested 世界"
        return result
    }
    [ue_function(BlueprintCallable)]
    def Replace(value:Tree):void { self.Config = value }
    [ue_function(BlueprintCallable)]
    def Get():Tree { return self.Config }
}
