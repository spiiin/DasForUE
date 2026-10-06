options gen2
require ue_dsl
var remembered = ""
[ue_class(module_id="text",Blueprintable)]
class TextCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    DSLText : string = "Привет"
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    DSLPrecise : double = 1.23456789012345lf
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    DSLKey : UENameValue = ue_name_value("Initial_7")

    [ue_function(BlueprintCallable)]
    def Echo(value:string; suffix:string):string {
        self.DSLText = value + suffix
        remembered = value
        return self.DSLText
    }
    [ue_function(BlueprintCallable)]
    def Recall():string { return remembered }
    [ue_function(BlueprintCallable)]
    def Precise(value:double; delta:double):double {
        self.DSLPrecise = value + delta
        return self.DSLPrecise
    }
    [ue_function(BlueprintCallable)]
    def Rename(value:UENameValue):UENameValue { self.DSLKey = value; return self.DSLKey }
    [ue_function(BlueprintCallable)]
    def NamesEqual(a:UENameValue; b:UENameValue):bool { return ue_names_equal(a,b) }
}
