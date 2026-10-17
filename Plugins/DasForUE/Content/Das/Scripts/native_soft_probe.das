options gen2
module native_soft_probe public
require ue_native_gameplay public

// The entire native path is exercised without loading either target asset.
def check_native_soft(api:NativeHashProbeRef):bool {
    let path="/Game/NotLoaded/Asset.Asset"
    let classPath="/Game/NotLoaded/Blueprint.Blueprint_C"
    let asset=ue_soft(type<SoftObject>,path)
    let cls=ue_soft(type<SoftActorClass>,classPath)
    api.SoftAsset=asset
    api.SoftActorType=cls
    verify(ue_soft_path(api.SoftAsset)==path && ue_soft_path(api.SoftActorType)==classPath,"native soft fields")
    verify(!ue_soft_is_loaded(api.SoftAsset) && !ue_soft_is_loaded(api.SoftActorType),"reading soft fields never loads")
    verify(ue_soft_path(api.EchoSoftAsset(asset))==path && ue_soft_path(api.EchoSoftClass(cls))==classPath,"native scalar parameters and returns")
    var changed=asset
    var previous:SoftObject
    api.ChangeSoftAsset(changed,previous)
    verify(ue_soft_is_null(changed) && ue_soft_path(previous)==path,"native scalar ref and out")
    var changedClass=cls
    var previousClass:SoftActorClass
    api.ChangeSoftClass(changedClass,previousClass)
    verify(ue_soft_is_null(changedClass) && ue_soft_path(previousClass)==classPath,"native soft class ref and out")
    var items:array<SoftObject>
    push(items,asset);push(items,default<SoftObject>)
    api.SoftAssets:=items
    api.SoftAssets[1]=asset
    var saved <- api.EchoSoftAssets(api.SoftAssets)
    api.SoftAssets[0]=default<SoftObject>
    verify(length(saved)==2 && ue_soft_path(saved[0])==path && ue_soft_is_null(api.SoftAssets[0]),"native soft array snapshot and indexed write")
    var before:array<SoftObject>
    api.ChangeSoftAssets(items,before)
    verify(length(items)==0 && length(before)==2 && ue_soft_path(before[0])==path && ue_soft_is_null(before[1]),"native soft array ref and out")
    var classes:table<string;SoftActorClass>
    classes["enemy"]=cls
    api.SoftClasses:=classes
    var copied <- api.EchoSoftClasses(api.SoftClasses)
    api.SoftClasses["enemy"]=default<SoftActorClass>
    verify(ue_soft_path(copied["enemy"])==classPath && ue_soft_is_null(api.SoftClasses["enemy"]),"native soft class map snapshot")
    var beforeClasses:table<string;SoftActorClass>
    api.ChangeSoftClasses(classes,beforeClasses)
    verify(length(classes)==0 && length(beforeClasses)==1 && ue_soft_path(beforeClasses["enemy"])==classPath,"native soft class map ref and out")
    var record=DasNativeSoftValue()
    record.Asset=asset;record.ActorType=cls
    push(record.Items,asset);record.Classes["enemy"]=cls
    api.SoftRecord:=api.EchoSoftRecord(record)
    api.SoftRecord.Items[0]=default<SoftObject>
    api.SoftRecord.Classes["enemy"]=default<SoftActorClass>
    verify(ue_soft_path(api.SoftRecord.Asset)==path && ue_soft_path(api.SoftRecord.ActorType)==classPath && ue_soft_is_null(api.SoftRecord.Items[0]) && ue_soft_is_null(api.SoftRecord.Classes["enemy"]),"native struct with soft fields and containers")
    verify(!ue_soft_is_loaded(api.SoftRecord.Asset) && !ue_soft_is_loaded(api.SoftRecord.ActorType),"native method roundtrips never load")
    api.SoftAsset=default<SoftObject>
    api.SoftActorType=default<SoftActorClass>
    return ue_soft_is_null(api.SoftAsset) && ue_soft_is_null(api.SoftActorType)
}
