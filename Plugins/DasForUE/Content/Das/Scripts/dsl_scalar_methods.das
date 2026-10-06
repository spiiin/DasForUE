options gen2
require ue_dsl
[ue_class(module_id="typed",id="typed.fixture",Blueprintable)]
class TypedFixture {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    health : int = 10
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    speed : float = 1.5
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    bScalar : bool = true

    [ue_function(id="typed.fixture.check",BlueprintCallable)]
    def Check(input:int):int { return self.health + input }
    [ue_function(id="typed.fixture.scale",BlueprintCallable)]
    def Scale(input:float):float { self.speed += input; return self.speed }
    [ue_function(id="typed.fixture.toggle",BlueprintCallable)]
    def Toggle(input:bool):bool { self.bScalar = input; return self.bScalar }
    [ue_function(id="typed.fixture.reset",BlueprintCallable)]
    def Reset():void { self.health = 0; self.bScalar = false }
}
