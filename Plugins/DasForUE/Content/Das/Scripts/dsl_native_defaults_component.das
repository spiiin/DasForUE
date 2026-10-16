options gen2
require ue_dsl
[ue_class(module_id="nativedefaults",Blueprintable)]
class NativeDefaultsComponent : ActorComponent {
    defaults=ue_defaults() {
        self.bAutoActivate=(1==2)
    }
    [ue_function(BlueprintCallable)]
    def Read(input:int):int { if(self.bAutoActivate) { return -1 };return input+1 }
    [ue_function(BlueprintCallable)]
    def AppendNativeTag(value:UENameValue):int {
        push(self.ComponentTags,value)
        return int(length(self.ComponentTags))
    }
    [ue_function(BlueprintCallable)]
    def SetNativeTag(index:int; value:UENameValue):void { self.ComponentTags[index]=value }
    [ue_function(BlueprintCallable)]
    def ClearNativeTags():void { clear(self.ComponentTags) }
}
