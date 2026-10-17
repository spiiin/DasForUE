options gen2
require ue_dsl
require ue_soft_references
require ue_soft_record
require native_soft_probe
[ue_class(Blueprintable,type_folder="/Game/DasSoftReferences/Types")]
class SoftReferenceActor : Actor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Asset:SoftObject = ue_soft(type<SoftObject>,"/Engine/BasicShapes/Cube.Cube")
        var ActorType:SoftActorClass = ue_soft(type<SoftActorClass>,"/Script/Engine.StaticMeshActor")
        var Assets:array<SoftObject> = [ue_soft(type<SoftObject>,"/Engine/BasicShapes/Cube.Cube")]
        var ClassAssets:array<SoftActorClass> = [ue_soft(type<SoftActorClass>,"/Script/Engine.StaticMeshActor")]
        var AssetsById:table<int;SoftObject>
        var ClassesByName:table<UENameValue;SoftActorClass>
        var Classes:table<string;SoftActorClass>
        var Record:SoftRecord
        var CollectionChecks:int=0
        var NativeChecks:int=0
        var Lazy:SoftObject
        var Empty:SoftObject
        var Ready:int=0
        var Errors:int=0
        var Rejected:int=0
    }
    [ue_function(BlueprintCallable)]
    def Echo(value:SoftObject):SoftObject { return value }
    [ue_function(BlueprintCallable)]
    def EchoClass(value:SoftActorClass):SoftActorClass { return value }
    [ue_function(BlueprintCallable)]
    def Change(var value:SoftObject&; @ue_out var previous:SoftObject&):SoftObject {
        previous=value;value=default<SoftObject>;return previous
    }
    [ue_function(BlueprintCallable)]
    def EchoAssets(value:array<SoftObject>):array<SoftObject> { return <- clone(value) }
    [ue_function(BlueprintCallable)]
    def ChangeAssets(var value:array<SoftObject>&; @ue_out var previous:array<SoftObject>&):array<SoftObject> {
        previous <- clone(value);clear(value);return <- clone(previous)
    }
    [ue_function(BlueprintCallable)]
    def EchoClasses(value:table<string;SoftActorClass>):table<string;SoftActorClass> { return <- clone(value) }
    [ue_function(BlueprintCallable)]
    def EchoRecord(value:SoftRecord):SoftRecord { return <- clone(value) }
    [ue_function(BlueprintCallable)]
    def CheckCollections():void {
        let path="/Game/NotLoaded/Asset.Asset"
        if(length(self.Assets)!=1 || ue_soft_path(self.Assets[0])!="/Engine/BasicShapes/Cube.Cube") { self.Errors+=1 }
        self.Assets[0]=ue_soft(type<SoftObject>,path)
        if(ue_soft_is_loaded(self.Assets[0])) { self.Errors+=1 }
        if(length(self.ClassAssets)!=1 || ue_soft_path(self.ClassAssets[0])!="/Script/Engine.StaticMeshActor") { self.Errors+=1 }
        self.ClassAssets[0]=default<SoftActorClass>
        if(!ue_soft_is_null(self.ClassAssets[0])) { self.Errors+=1 }
        self.AssetsById[7]=self.Assets[0]
        self.ClassesByName[ue_name_value("Enemy")]=self.ActorType
        if(ue_soft_path(self.AssetsById[7])!=path || ue_soft_path(self.ClassesByName[ue_name_value("Enemy")])!="/Script/Engine.StaticMeshActor") { self.Errors+=1 }
        let original <- self.EchoAssets(self.Assets)
        push(self.Assets,self.Asset)
        self.Assets[0]=self.Asset
        if(length(original)!=1 || ue_soft_path(original[0])!=path) { self.Errors+=1 }
        var values <- clone(original);var previous:array<SoftObject>
        let result <- self.ChangeAssets(values,previous)
        if(length(values)!=0 || length(previous)!=1 || ue_soft_path(previous[0])!=path || ue_soft_path(result[0])!=path) { self.Errors+=1 }
        self.Classes["base"]=self.ActorType
        var classes <- self.EchoClasses(self.Classes)
        self.Classes["base"]=default<SoftActorClass>
        if(!ue_soft_is_null(self.Classes["base"]) || ue_soft_path(classes["base"])!="/Script/Engine.StaticMeshActor") { self.Errors+=1 }
        var record:SoftRecord
        record.Asset=self.Asset;record.ActorType=self.ActorType
        push(record.Items,original[0]);record.Named["base"]=self.ActorType
        self.Record := self.EchoRecord(record)
        self.Record.Asset=original[0]
        self.Record.Items[0]=self.Asset
        if(ue_soft_path(self.Record.Asset)!=path || ue_soft_path(self.Record.Items[0])!=ue_soft_path(self.Asset) || ue_soft_path(self.Record.Named["base"])!="/Script/Engine.StaticMeshActor") { self.Errors+=1 }
        if(!ue_soft_is_null(self.Empty)) { self.Errors+=1 }
        self.CollectionChecks=1
    }
    def BeginPlay():void {
        self.CheckCollections()
        let owner=ue_ref(type<ActorRef>,ue_self_object())
        let native=ue_component_create(type<NativeHashProbeRef>,owner,"NativeSoftProbe")
        if(check_native_soft(native)) { self.NativeChecks=1 } else { self.Errors+=1 }


        if(ue_soft_path(self.Asset)!="/Engine/BasicShapes/Cube.Cube" || !ue_soft_is_null(self.Empty)) { self.Errors+=1 }
        let echoed=self.Echo(self.Asset)
        if(ue_soft_path(echoed)!=ue_soft_path(self.Asset)) { self.Errors+=1 }
        var value=self.Asset;var previous:SoftObject
        let result=self.Change(value,previous)
        if(!ue_soft_is_null(value) || ue_soft_path(previous)!=ue_soft_path(self.Asset) || ue_soft_path(result)!=ue_soft_path(self.Asset)) { self.Errors+=1 }
        let loaded=ue_soft_load(self.Asset)
        if(!ue_object_valid(loaded.handle) || !ue_soft_is_loaded(self.Asset)) { self.Errors+=1 }
        let cls=ue_soft_load(self.ActorType)
        if(ue_class_path(cls)!="/Script/Engine.StaticMeshActor" || ue_class_path(ue_soft_get(self.ActorType))!=ue_class_path(cls)) { self.Errors+=1 }
        try { let bad=ue_soft(type<SoftObject>,"bad path") } recover { self.Rejected+=1 }
        try { let bad=ue_soft(type<SoftActorClass>,"/Script/Engine.StaticMesh") } recover { self.Rejected+=1 }
        if(!ue_soft_is_null(self.Lazy)) {
            if(ue_soft_is_loaded(self.Lazy)) { self.Errors+=1 }
            ue_soft_load_async("lazy",self.Lazy,"AssetReady")
        }
        ue_soft_load_async("asset",self.Asset,"AssetReady")
        ue_soft_load_async("class",self.ActorType,"ClassReady")
        ue_soft_load_async("null",self.Empty,"NullReady")
        ue_soft_load_async("cancelled",self.Asset,"Cancelled")
        if(!ue_asset_cancel("cancelled")) { self.Errors+=1 }
    }
    [ue_function(BlueprintCallable)]
    def AssetReady(value:ObjectRef):void { if(!ue_object_valid(value.handle)) { self.Errors+=1 };self.Ready+=1 }
    [ue_function(BlueprintCallable)]
    def ClassReady(value:ActorClass):void { if(ue_class_path(value)!="/Script/Engine.StaticMeshActor") { self.Errors+=1 };self.Ready+=1 }
    [ue_function(BlueprintCallable)]
    def NullReady(value:ObjectRef):void { if(ue_object_valid(value.handle)) { self.Errors+=1 };self.Ready+=1 }
    [ue_function(BlueprintCallable)]
    def QueueForRelease():void { ue_soft_load_async("release",self.Asset,"Cancelled") }
    [ue_function(BlueprintCallable)]
    def Cancelled(value:ObjectRef):void { self.Errors+=1 }
}
