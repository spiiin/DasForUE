options gen2
module ue_pickup_components public
require ue_object_types public
require ue_soft_references public

// Canonical native views shared by the generator and gameplay source.
[ue_native_object(path="/Script/Engine.StaticMesh")]
struct StaticMeshRef { handle:UObjectHandle }

typedef SoftStaticMesh = $UESoftObject<StaticMeshRef>
def StaticMeshRef_from(value:UObjectHandle):StaticMeshRef { return StaticMeshRef(handle=ue_cast_nullable_object(value,"/Script/Engine.StaticMesh")) }

[ue_native_object(path="/Script/Engine.StaticMeshComponent")]
struct StaticMeshComponentRef { handle:UObjectHandle }
def StaticMeshComponentRef_from(value:UObjectHandle):StaticMeshComponentRef { return StaticMeshComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.StaticMeshComponent")) }

[ue_native_object(path="/Script/Engine.SphereComponent")]
struct SphereComponentRef { handle:UObjectHandle }
def SphereComponentRef_from(value:UObjectHandle):SphereComponentRef { return SphereComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.SphereComponent")) }
