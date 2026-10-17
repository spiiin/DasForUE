options gen2
module ue_class_callback_signals public
require ue_object_types public
[ue_delegate]
struct ClassQuery { signature:function<(value:ActorClass):ActorClass> }
[ue_delegate]
struct ClassMutation {
    def signature(var value:ActorClass&; @ue_out var previous:ActorClass&):ActorClass {}
}
[ue_multicast]
struct ClassSignal { signature:function<(value:ActorClass):void> }
[ue_multicast]
struct MixedClassSignal { signature:function<(value:ActorClass; number:int):void> }
