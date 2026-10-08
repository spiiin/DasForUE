options gen2
options gc
require ue_dsl
[ue_enum(module_id="direct", BlueprintType)]
enum State {
    Idle = 7
    Active = 42
}
[ue_struct(module_id="direct", BlueprintType)]
struct Settings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count : int = 15
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title : string = "Initial"
}
[ue_class(module_id="authoredvalues", type_folder="/Game/DasAuthoredTypes", Blueprintable)]
class TypedActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Mode : State = State.Idle
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Config : Settings = Settings()
    [ue_function(BlueprintCallable)]
    def SetMode(value:State):void { self.Mode = value }
    [ue_function(BlueprintCallable)]
    def GetMode():State { return self.Mode }
    [ue_function(BlueprintCallable)]
    def ModeNumber():int { return int(self.Mode) }
    [ue_function(BlueprintCallable)]
    def SetConfig(value:Settings):void { self.Config = value }
    [ue_function(BlueprintCallable)]
    def GetConfig():Settings { return self.Config }
    [ue_function(BlueprintCallable)]
    def Evaluate(mode:State; data:Settings):Settings {
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
