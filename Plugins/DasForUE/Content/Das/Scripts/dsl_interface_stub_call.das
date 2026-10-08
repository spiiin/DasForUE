options gen2
options gc
require ue_dsl
[ue_interface(module_id="stubcall", BlueprintType)]
class Contract {
    [ue_function(BlueprintCallable)]
    def Read(input:int):int {}
}
[export]
def main(input:int):int {
    unsafe {
        var inscope contract = new Contract()
        return contract->Read(input)
    }
}
