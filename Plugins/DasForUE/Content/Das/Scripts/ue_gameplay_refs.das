options gen2
module ue_gameplay_refs public
require ue_dsl public

[ue_object_ref(type="gameplaydemo.GameplayActor",catalog="ue_gameplay_catalog.json")]
struct GameplayActorRef { handle:UObjectHandle }
[ue_object_ref(type="gameplaydemo.GameplayBehavior",catalog="ue_gameplay_catalog.json")]
struct GameplayBehaviorRef { handle:UObjectHandle }

[ue_multicast]
struct GameplayScoreSignal { signature:function<(value:int):void> }
