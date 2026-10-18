options gen2
require ue_dsl public
[ue_native_object(path="/Script/Engine.BoxComponent")]
struct AsyncBoxRef { handle:UObjectHandle }
def physics_flag(body:UObjectHandle; method:string; argument:string; value:bool) {
    let call=ue_begin_call(body,method)
    ue_require_call_context(call,false)
    ue_arg_bool(call,argument,value)
    ue_invoke_call(call)
    ue_release_call(call)
}
[ue_class(module_id="asyncphysicsdsl",Blueprintable),
 ue_async_physics(script="async_physics_thruster_tick.das",body_0="Body")]
class Thruster {
    @ue_component
    @RootComponent
    Body:AsyncBoxRef
    @ue_property=(BlueprintReadWrite)
    Initializes:int=0
    @ue_property=(BlueprintReadWrite)
    Begins:int=0
    @ue_property=(BlueprintReadWrite)
    PhysicsSeconds:double=0.0lf
    @ue_property=(BlueprintReadWrite)
    Running:bool=false
    [ue_lifecycle]
    def Initialize():void {
        self.Initializes+=1
        let body=ue_get_self_object("Body")
        let collision=ue_begin_call(body,"SetCollisionEnabled")
        ue_require_call_context(collision,false)
        ue_arg_enum(collision,"newtype","/Script/Engine.ECollisionEnabled","QueryAndPhysics")
        ue_invoke_call(collision)
        ue_release_call(collision)
        physics_flag(body,"SetEnableGravity","bgravityenabled",false)
        physics_flag(body,"SetSimulatePhysics","bsimulate",true)
        let damping=ue_begin_call(body,"SetLinearDamping")
        ue_require_call_context(damping,false)
        ue_arg_float(damping,"indamping",0.0)
        ue_invoke_call(damping)
        ue_release_call(damping)
        ue_async_physics_set_input(0,1000.0lf)
    }
    [ue_lifecycle]
    def BeginPlay():void { self.Begins+=1 }
    [ue_lifecycle]
    def Tick(dt:float):void {
        self.Running=ue_async_physics_running()
        if(ue_async_physics_has_output()) { self.PhysicsSeconds=ue_async_physics_output(0) }
    }
    [ue_function(BlueprintCallable)]
    def SetForce(value:double):void { ue_async_physics_set_input(0,value) }
}
