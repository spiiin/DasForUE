options gen2
module ue_callback_value_types public
require ue_native_gameplay public

[ue_enum(BlueprintType)]
enum CallbackState {
    Idle=7
    Active=42
}
[ue_enum(BlueprintType)]
enum OtherCallbackState {
    Idle=7
    Active=42
}
[ue_struct(BlueprintType)]
struct CallbackRecord {
    @ue_property=(BlueprintReadWrite)
    Score:int=0
    @ue_property=(BlueprintReadWrite)
    Title:FText=ue_text_invariant("")
    @ue_property=(BlueprintReadWrite)
    Values:array<int>
    @ue_property=(BlueprintReadWrite)
    Mode:CallbackState=CallbackState.Idle
}
[ue_native_enum(path="/Script/DasRuntime.EDasCallbackMode")]
enum CallbackNativeState {
    Idle=0
    Active=7
    Paused=42
}
typedef CallbackNativeRecord = Vector4
[ue_native_object(path="/Script/CoreUObject.Object")]
struct CallbackObjectRef { handle:UObjectHandle }
[ue_struct(BlueprintType)]
struct CallbackReferenceRecord {
    @ue_property=(BlueprintReadWrite)
    Target:CallbackObjectRef
    @ue_property=(BlueprintReadWrite)
    Targets:array<CallbackObjectRef>
    @ue_property=(BlueprintReadWrite)
    Lookup:table<string;CallbackObjectRef>
}

[ue_native_object(path="/Script/Engine.Actor")]
struct CallbackActorRef { handle:UObjectHandle }

def CallbackObjectRef_from(value:UObjectHandle):CallbackObjectRef { return CallbackObjectRef(handle=ue_cast_nullable_object(value,"/Script/CoreUObject.Object")) }
def CallbackActorRef_from(value:UObjectHandle):CallbackActorRef { return CallbackActorRef(handle=ue_cast_nullable_object(value,"/Script/Engine.Actor")) }
