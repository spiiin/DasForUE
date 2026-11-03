options gen2
require ue_dsl
require ue_object_types
require ue_int64_types

[ue_multicast]
struct Int64Changed { signature : function<(Value : int64) : void> }

var @ue_state lifetime : int64 = 9007199254740993l

[ue_class(module_id="int64test", type_folder="/Game/DasInt64/Types", Blueprintable)]
class Counter : ScriptObject {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Total : int64 = 9007199254740993l
    @ue_property=BlueprintReadWrite
    Zero : int64 = 0
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Computed : int64 = 9007199254740993l + 2l
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Record : Int64Record = Int64Record()
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Smallest : int64 = -9223372036854775808l
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Largest : int64 = 9223372036854775807l
    @ue_property=BlueprintReadWrite
    Values : array<int64>
    @ue_property=BlueprintReadWrite
    Balances : table<int64; int64>
    @ue_property=BlueprintReadWrite
    Seen : table<int64>

    @ue_property=BlueprintAssignable
    Changed : Int64Changed

    def BindEvents() : void { self.Changed += self.ReceiveChanged }

    def ReceiveChanged(value : int64) : void { self.Zero = value }

    [ue_function(BlueprintCallable)]
    def PersistentStep() : int64 { lifetime += 1l; return lifetime }

    [ue_function(BlueprintCallable)]
    def Echo(input : int64) : int64 { return input }

    [ue_function(BlueprintCallable)]
    def Accumulate(var value : int64&; @ue_out var doubled : int64&) : int64 {
        self.Total += value
        value = self.Total
        doubled = value + value
        return value
    }

    [ue_function(BlueprintCallable)]
    def Defaulted(value : int64 = 9007199254740993l) : int64 { return value }

    [ue_function(BlueprintCallable)]
    def NativeEcho(input : int64) : int64 {
        let call = ue_begin_static_call("/Script/DasRuntime.DasFunctionLibraryFixture", "UnsupportedInt64", "")
        ue_arg(call, "Value", input)
        ue_invoke_call(call)
        let result = ue_result_int64(call, "ReturnValue")
        ue_release_call(call)
        return result
    }

    [ue_function(BlueprintCallable)]
    def Containers() : bool {
        var values : array<int64>
        push(values, self.Total)
        push(values, self.Largest)
        self.Values := values
        self.Balances[self.Total] = self.Smallest
        insert(self.Seen, self.Largest)
        return (self.Record.Value == 9007199254740993l && self.Values[0] == self.Total && self.Values[1] == self.Largest &&
            self.Balances[self.Total] == self.Smallest && key_exists(self.Seen, self.Largest))
    }
}
