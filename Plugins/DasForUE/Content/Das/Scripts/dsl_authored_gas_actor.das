options gen2
require ue_gas

// Import gas_combat_definitions.das with Das.GASImport, then assign its classes in Details.
[ue_class(Blueprintable)]
class AuthoredGASActor : Actor {
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Abilities:AuthoredAbilitySystemComponentRef
    }
    settings=ue_properties(EditAnywhere,BlueprintReadOnly) {
        var Attributes:AttributeSetClass
        var Healing:AuthoredGameplayEffectClass
    }
    state=ue_properties(BlueprintReadWrite) {
        var LastHealth:float=0.0f
        var Changes:int=0
    }
    def BeginPlay():void {
        ue_gas_add_attributes(self.Abilities,self.Attributes)
        self.LastHealth=ue_gas_value(self.Abilities,"CombatAttributes","Health")
    }
    [ue_function(BlueprintCallable)]
    def Heal():bool {
        return ue_gas_apply(self.Abilities,ue_gas_make_spec(self.Abilities,self.Healing)).Applied
    }
    [ue_event(source="Abilities",name="AttributeChanged")]
    def OnAttribute(change:GASAttributeChange):void {
        self.Changes+=1
        if(ue_name_text(change.Attribute)=="Health") {self.LastHealth=change.NewValue}
    }
}
