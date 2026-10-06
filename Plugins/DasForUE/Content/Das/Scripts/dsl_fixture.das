options gen2
require ue_dsl
[ue_class(module_id="fixture",id="fixture.scalar")]
class ScalarFixture {
    @ue_property=BlueprintReadWrite
    health : int = 10
    @ue_property=BlueprintReadWrite
    speed : float = 1.5
    [ue_function(BlueprintCallable)]
    def Update(input:int):int {
        self.health = input
        self.health += 7
        self.health *= 2
        self.health -= 4
        self.health /= 2
        self.speed = 2.5
        self.speed += 1.25
        return self.health
    }
}
