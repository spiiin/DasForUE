options gen2
module ue_callback_container_signals public
require ue_callback_value_signals public
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackStatesSignal { signature:function<(value:array<CallbackState>):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackRecordsSignal { signature:function<(value:array<CallbackRecord>):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackMathSignal { signature:function<(value:array<CallbackNativeRecord>):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackRecordMapSignal { signature:function<(value:table<string;CallbackRecord>):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackTextsSignal { signature:function<(value:table<int;FText>):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackStatesSetSignal { signature:function<(value:table<CallbackState>):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackIntsSignal { signature:function<(value:table<int>):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackObjectsSignal { signature:function<(value:array<CallbackObjectRef>):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackObjectMapSignal { signature:function<(value:table<string;CallbackObjectRef>):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackNativeStatesSignal { signature:function<(value:array<CallbackNativeState>):void> }
[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackMixedSignal { signature:function<(number:int;text:FText;flag:uint8;mode:CallbackState;record:CallbackRecord;values:array<int>):void> }

[ue_multicast(type_folder="/Game/DasCallbackObjects/Types")]
struct CallbackShapesSignal { signature:function<(position:UEVector;tag:UENameValue;texts:table<int;FText>;states:table<CallbackState>):void> }
