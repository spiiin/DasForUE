options gen2
require ue_dsl
require ue_gameplay_tags

[ue_class(Blueprintable)]
class TaggedItem : Actor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Native UE tag and tag-container properties, editable in the normal tag picker.
        var Kind:GameplayTag
        var ItemTags:GameplayTagContainer
    }
    [ue_function(BlueprintCallable)]
    def ConfigureSword():void {
        self.Kind=ue_tag("Item.Weapon.Sword")
        self.ItemTags=ue_tags(self.Kind)
    }
    [ue_function(BlueprintPure)]
    def Matches(query:GameplayTag):bool {return ue_tags_has(self.ItemTags,query)}
    [ue_function(BlueprintPure)]
    def EchoTag(value:GameplayTag):GameplayTag {return value}
    [ue_function(BlueprintPure)]
    def EchoTags(value:GameplayTagContainer):GameplayTagContainer {return value}
    [ue_function(BlueprintCallable)]
    def Exchange(@ue_ref var value:GameplayTag&):void {
        let previous=self.Kind
        self.Kind=value
        value=previous
    }
}
