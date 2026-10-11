options gen2
module ue_delegate_examples public
require ue_dsl public

// One reflected handler; returns its int result.
[ue_delegate]
struct FScoreQuery {
    signature:function<(Value:int):int>
}

// One reflected handler with the two inputs from the AngelScript example.
[ue_delegate]
struct FObjectValueDelegate {
    signature:function<(Object:UObjectHandle; Value:float):void>
}

// A signal with multiple reflected handlers.
[ue_multicast]
struct FScoreEvent {
    signature:function<(Counter:int):void>
}
