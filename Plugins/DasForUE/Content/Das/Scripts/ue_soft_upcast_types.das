options gen2
module ue_soft_upcast_types public
require ue_soft_references public
require ue_default_assets public

[ue_native_object(path="/Script/Engine.StaticMeshActor")]
struct SoftMeshActorRef { handle:UObjectHandle }
def SoftMeshActorRef_from(value:UObjectHandle):SoftMeshActorRef {
    return SoftMeshActorRef(handle=ue_cast_nullable_object(value,"/Script/Engine.StaticMeshActor"))
}
typedef SoftCurve = $UESoftObject<CurveAssetRef>
typedef SoftCurveClass = $UESoftClass<CurveAssetRef>
typedef SoftMeshActorClass = $UESoftClass<SoftMeshActorRef>
typedef SoftObjectClass = $UESoftClass<ObjectRef>
