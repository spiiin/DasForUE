options gen2
module ue_native_self_refs public
require ue_scene_components public

// Pawn is not yet in the small generated Engine catalog. A native wrapper
// supplies exactly the same checked contract; ActorRef and SceneComponentRef
// are imported from their canonical modules.
[ue_native_object(path="/Script/Engine.Pawn")]
struct NativePawnRef { handle:UObjectHandle }
def NativePawnRef_from(value:UObjectHandle):NativePawnRef {
    return NativePawnRef(handle=ue_cast_nullable_object(value,"/Script/Engine.Pawn"))
}
def NativePawnRef_raw(value:NativePawnRef):UObjectHandle {
    return ue_cast_nullable_object(value.handle,"/Script/Engine.Pawn")
}
