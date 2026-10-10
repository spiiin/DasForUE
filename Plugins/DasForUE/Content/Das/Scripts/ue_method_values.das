options gen2
module ue_method_values public
require ue_dsl public
require ue_object_types public
[ue_enum(module_id="methodvalues", BlueprintType)]
enum MethodState {
    Idle=7
    Active=42
}
[ue_enum(module_id="methodvalues", BlueprintType)]
enum OtherMethodState {
    Idle=7
    Active=42
}
[ue_struct(module_id="methodvalues", BlueprintType)]
struct MethodPoint {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count:int=2
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Mode:MethodState=MethodState.Active
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Owner:ActorRef
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Events:DasEventInterface
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Position:UEVector=ue_vector(1.0lf,2.0lf,3.0lf)
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Note:string="seed"
}
[ue_struct(module_id="methodvalues", BlueprintType)]
struct MethodSettings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Point:MethodPoint=MethodPoint()
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Total:int=3
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Orientation:UERotator=ue_rotator(10.0lf,20.0lf,30.0lf)
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Pose:UETransform
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Items:array<MethodPoint>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Weights:table<string; double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Tags:table<string>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Samples:array<double>
}

[ue_struct(module_id="methodvalues", BlueprintType)]
struct MethodEnvelope {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Payload:MethodSettings=MethodSettings()
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Sentinel:int=91
}

[ue_struct(module_id="methodvalues", BlueprintType)]
struct MethodGroup {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Scores:array<double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Points:array<MethodPoint>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Weights:table<string;double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Tags:table<string>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Sentinel:int=65
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Path:array<UEVector>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    States:array<MethodState>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Actors:array<ActorRef>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    EventRefs:array<DasEventInterface>
}
