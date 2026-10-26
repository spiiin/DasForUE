options gen2
require ue_dsl
require ue_probe_actor
[ue_object_ref(script="dsl_construction_call_worker.das",type="ConstructionCallWorker")]
struct ConstructionCallWorkerRef { handle:UObjectHandle }
[ue_object_ref(script="ue_inheritance_base.das",type="BaseCounter")]
struct ConstructionCallPeerRef { handle:UObjectHandle }
[ue_class(module_id="constructioncalls",Blueprintable)]
class ConstructionCalls : ProbeActor {
    @ue_component
    Worker:ConstructionCallWorkerRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    CounterTarget:ConstructionCallPeerRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    FailureMode:int=0
    @ue_property=(BlueprintReadWrite)
    Result:int=0
    @ue_property=(BlueprintReadWrite)
    InitCount:int=0
    def Initialize():void { self.InitCount+=1 }
    def ConstructionScript():void {
        self.Worker.FailureMode=self.FailureMode
        self.Result=self.Worker.Evaluate(7)+self.Worker.Evaluate(7)
        if(ue_object_valid(self.CounterTarget.handle)) { self.Result+=self.CounterTarget.ReadScore() }
    }
}
