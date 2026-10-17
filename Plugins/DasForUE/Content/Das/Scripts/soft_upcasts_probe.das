options gen2
require ue_soft_upcast_types
require ue_native_gameplay

def accept_object(value:SoftObject):string { return ue_soft_path(value) }
def return_parent(value:SoftCurve):SoftObject { return value }
def closest(value:SoftActorClass):int { return 1 }
def closest(value:SoftObjectClass):int { return 2 }
def exact(value:SoftObject):int { return 1 }
def exact(value:SoftCurve):int { return 2 }
def produce(path:string; var count:int&):SoftCurve {
    count+=1
    return ue_soft(type<SoftCurve>,path)
}
[export]
def main(input:int):int {
    let path="/Game/NotLoaded/Curve.Curve"
    let curve=ue_soft(type<SoftCurve>,path)
    var object:SoftObject=curve
    verify(ue_soft_path(object)==path && !ue_soft_is_loaded(object),"typed initializer widens without loading")
    object=curve
    verify(accept_object(curve)==path && ue_soft_path(return_parent(curve))==path,"parameters and returns widen")
    verify(exact(curve)==2,"exact overload wins")
    let widened=ue_soft_upcast(type<SoftObject>,curve)
    verify(ue_soft_path(widened)==path && ue_soft_path(ue_soft_upcast(type<SoftCurve>,curve))==path,"explicit parent and identity conversions")
    var count=0
    let once:SoftObject=produce(path,count)
    verify(count==1 && ue_soft_path(once)==path,"source is evaluated once")
    let empty:SoftObject=default<SoftCurve>
    verify(ue_soft_is_null(empty),"typed null widens")
    let classPath="/Game/NotLoaded/Enemy.Enemy_C"
    let child=ue_soft(type<SoftMeshActorClass>,classPath)
    let actor:SoftActorClass=child
    let root:SoftObjectClass=actor
    verify(ue_soft_path(root)==classPath && !ue_soft_is_loaded(root) && closest(child)==1,"class chain and nearest parent overload")
    let emptyClass:SoftActorClass=default<SoftMeshActorClass>
    verify(ue_soft_is_null(emptyClass),"soft class null widens")
    let owner=ue_ref(type<ActorRef>,ue_self_object())
    let api=ue_component_get(type<NativeHashProbeRef>,owner,"SoftUpcasts")
    api.SoftAsset=curve
    api.SoftActorType=child
    verify(ue_soft_path(api.SoftAsset)==path && ue_soft_path(api.SoftActorType)==classPath,"native property assignments widen")
    verify(ue_soft_path(api.EchoSoftAsset(curve))==path && ue_soft_path(api.EchoSoftClass(child))==classPath,"native method inputs widen")
    var previous:SoftObject
    api.ChangeSoftAsset(object,previous)
    verify(ue_soft_is_null(object) && ue_soft_path(previous)==path && ue_soft_path(curve)==path,"ref remains exact; derived source is unchanged")
    verify(!ue_soft_is_loaded(curve) && !ue_soft_is_loaded(child),"conversions never load assets")
    return input+1
}
