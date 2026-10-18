options gen2
options gc
require ue_dsl
require ue_native_gameplay
require ue_access_context_api

[ue_class(module_id="accesscontext",Blueprintable)]
class AccessContext : Actor {
    @ue_property=(BlueprintReadWrite)
    Probe:NativeHashProbeRef
    @ue_property=(BlueprintReadWrite)
    Mode:int=0
    @ue_property=(BlueprintReadWrite)
    Result:int=0
    def ConstructionScript():void {
        if(ue_object_valid(self.Probe.handle)) {
            self.Probe.SafeContextBump()
            self.Probe.SemanticScore=5
            self.Result=self.Probe.SemanticScore
            if(self.Mode==1) { self.Probe.UnsafeContextBump() }
            if(self.Mode==2) { self.Probe.BlueprintLevel=3.0 }
            if(self.Mode==3) { ue_context_static_UnsafeContextProbe() }
            if(self.Mode==4) { ue_context_UnsafeContextBump(C_DasNativeHashProbeComponent_8c9611a8_from(self.Probe.handle)) }
        }
    }
}

[export]
def main(input:int):int {
    let ref=NativeHashProbeRef_from(ue_self_object())
    ref.SemanticScore=8
    if(ref.SemanticScore!=17) { return -1 }
    ref.bSemanticEnabled=true
    if(!ref.bSemanticEnabled) { return -2 }
    var numbers:array<int>
    push(numbers,3)
    ref.SemanticNumbers=numbers
    let observed <- ref.SemanticNumbers
    if(length(observed)!=3 || observed[1]!=42 || observed[2]!=999) { return -3 }
    ref.SemanticDirection=ue_vector(2.0lf,3.0lf,4.0lf)
    if(ref.SemanticDirection.x!=5.0lf || ref.SemanticDirection.z!=8.0lf) { return -4 }
    ref.BlueprintLevel=3.0
    if(ref.BlueprintLevel!=3.25) { return -5 }
    if(ref.UnsafeContextBump()<=0) { return -6 }
    if(ue_context_static_UnsafeContextProbe()<=0) { return -7 }
    return 42
}
