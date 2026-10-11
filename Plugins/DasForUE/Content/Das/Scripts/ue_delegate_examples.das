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

// Parameter annotations survive on a method declaration (unlike a function type).
[ue_delegate]
struct FValueMutation {
    def signature(var Count:int&; var Text:string&;
                  @ue_out var Accepted:bool&; @ue_out var Description:string&):int {}
}

[ue_delegate]
struct FOutputValue {
    def signature(@ue_out var Value:int&):void {}
}

[ue_multicast]
struct FCountChanged {
    def signature(var Counter:int&):void {}
}

[ue_delegate]
struct FArrayMutation {
    def signature(var Values:array<double>&; @ue_out var Previous:array<double>&):void {}
}
