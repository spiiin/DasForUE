options gen2
options gc
require ue_dsl
[ue_enum(id="direct.State", module_id="direct", BlueprintType)]
enum PlayState {
    Idle = 7
    Active = 42
}
[ue_struct(id="direct.Settings", module_id="direct", BlueprintType)]
struct GameSettings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count : int = 15
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title : string = "Initial"
}
[ue_class(parent=Actor, module_id="authoredvalues", type_registry="/Game/DasRegisteredTypes/TypesRegistry.TypesRegistry", Blueprintable)]
class TypedActor {
    [ue_function(BlueprintCallable, arg_value_enum="direct.State", return_enum=PlayState)]
    def RawMode(value:UEEnumValue):UEEnumValue { return value }
    [ue_function(BlueprintCallable, arg_value_struct=GameSettings, return_struct="direct.Settings")]
    def RawConfig(value:UEStructValue):UEStructValue { return value }

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Mode : PlayState = PlayState.Idle
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Config : GameSettings = GameSettings()
    [ue_function(BlueprintCallable)]
    def SetMode(value:PlayState):void { self.Mode = value }
    [ue_function(BlueprintCallable)]
    def GetMode():PlayState { return self.Mode }
    [ue_function(BlueprintCallable)]
    def ModeNumber():int { return int(self.Mode) }
    [ue_function(BlueprintCallable)]
    def SetConfig(value:GameSettings):void { self.Config = value }
    [ue_function(BlueprintCallable)]
    def GetConfig():GameSettings { return self.Config }
    [ue_function(BlueprintCallable, arg_mode_enum=PlayState, arg_data_struct=GameSettings, return_struct="direct.Settings")]
    def Evaluate(mode:PlayState; data:GameSettings):GameSettings {
        var result = data
        result.Count += int(mode)
        return result
    }
    [ue_function(BlueprintCallable)]
    def Bump(amount:int):int {
        self.Config.Count += amount
        return self.Config.Count
    }
    [ue_function(BlueprintCallable)]
    def SetTitle(value:string):void { self.Config.Title = value }
}
