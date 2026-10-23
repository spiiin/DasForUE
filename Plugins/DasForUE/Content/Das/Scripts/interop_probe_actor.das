options gen2
require ue_dsl
require ue_native_gameplay public
require interop_probe_refs

// DSL class for the editor part of the call-cost benchmark (DasInteropPerformanceCommandlet).
[ue_class(module_id="interopprobe", Blueprintable)]
class InteropProbe : Actor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Value:int=7

    [ue_function(BlueprintCallable)]
    def Echo(x:int):int { return x+1 }

    [ue_function(BlueprintCallable)]
    def Mixed(a:int; b:float; c:string; d:UEVector):int { return a+int(b)+int(length(c))+int(d.x) }

    // Method calls inside one script: on self, and through a typed reference to the same object.
    [ue_function(BlueprintCallable)]
    def DirectCalls(count:int):int {
        var sum=0
        for (i in range(0,count)) { sum+=self.Echo(i) }
        return sum
    }
    [ue_function(BlueprintCallable)]
    def AuthoredCalls(count:int):int {
        let peer=ue_ref(type<InteropProbeRef>,ue_self_object())
        var sum=0
        for (i in range(0,count)) { sum+=peer.Echo(i) }
        return sum
    }
    // Authored property through self, and a native Actor property through self.
    [ue_function(BlueprintCallable)]
    def FieldReads(count:int):int {
        var sum=0
        for (i in range(0,count)) { sum+=self.Value }
        return sum
    }
    [ue_function(BlueprintCallable)]
    def NativeFieldReads(count:int):int {
        // InitialLifeSpan is BlueprintReadOnly and 0 on a spawned actor; + 7 keeps the checksum.
        var sum=0
        for (i in range(0,count)) { sum+=int(self.InitialLifeSpan)+7 }
        return sum
    }
}
