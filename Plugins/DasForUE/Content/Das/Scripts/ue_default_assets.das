options gen2
module ue_default_assets public
require ue_object_types public
[ue_native_object(path="/Script/Engine.CurveFloat")]
struct CurveAssetRef { handle:UObjectHandle }
def CurveAssetRef_from(value:UObjectHandle):CurveAssetRef { return CurveAssetRef(handle=ue_cast_nullable_object(value,"/Script/Engine.CurveFloat")) }

typedef CurveAssetKey = $UEObjectKey<CurveAssetRef>
