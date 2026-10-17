options gen2
module ue_callback_value_signals public
require ue_callback_value_types public

[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackStateSignal { signature:function<(value:CallbackState):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackRecordSignal { signature:function<(value:CallbackRecord):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackRecordRefSignal { def signature(var value:CallbackRecord&):void {} }
[ue_multicast]
struct CallbackNativeStateSignal { signature:function<(value:CallbackNativeState):void> }
[ue_multicast]
struct CallbackNativeRecordSignal { signature:function<(value:CallbackNativeRecord):void> }

[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackReferenceSignal { signature:function<(value:CallbackReferenceRecord):void> }
[ue_multicast]
struct CallbackObjectSignal { signature:function<(value:CallbackObjectRef):void> }
