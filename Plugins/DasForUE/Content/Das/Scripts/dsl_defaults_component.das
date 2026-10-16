options gen2
module dsl_defaults_component public
require ue_dsl public
[ue_class(Blueprintable)]
class DefaultComponent : ActorComponent {
    [ue_function(BlueprintPure)]
    def DefaultAdd(left:int=7; right:int=-2):int { return left+right }
    [ue_function(BlueprintPure)]
    def DefaultDouble(value:double=1.23456789012345lf):double { return value }
    [ue_function(BlueprintPure)]
    def DefaultFloat(value:float=-0.125):float { return value }
    [ue_function(BlueprintPure)]
    def DefaultBool(value:bool=false):bool { return value }
    [ue_function(BlueprintPure)]
    def DefaultText(value:string="Пример 世界"):string { return value }
    [ue_function(BlueprintPure)]
    def DefaultEmpty(value:string=""):string { return value }
    [ue_function(BlueprintCallable)]
    def CheckDefaults():int {
        if (self.DefaultAdd()!=5 || self.DefaultAdd(10)!=8 || self.DefaultAdd(10,3)!=13) { return -101 }
        if (self.DefaultDouble()!=1.23456789012345lf || self.DefaultFloat()!=-0.125) { return -102 }
        if (self.DefaultBool() || self.DefaultText()!="Пример 世界" || self.DefaultEmpty()!="") { return -103 }
        return 1
    }
    [ue_function(BlueprintPure,BlueprintEvent)]
    def const DefaultEvent(value:int=4):int { return value*2 }
    [ue_function(BlueprintCallable)]
    def EventProbe():int { return self.DefaultEvent() }
}
