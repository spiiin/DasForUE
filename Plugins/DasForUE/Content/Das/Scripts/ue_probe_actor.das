options gen2
module ue_probe_actor public
require ue_dsl public
[ue_native_object(path="/Script/DasRuntime.DasProbeActor")]
struct ProbeActorRef {handle:UObjectHandle}

[ue_native_base(path="/Script/DasRuntime.DasProbeActor")]
class ProbeActor {}
