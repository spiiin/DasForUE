options gen2
module ue_health_signals public
require ue_dsl public
[ue_multicast]
struct HealthChangedSignal { signature:function<(current:float; maximum:float):void> }
