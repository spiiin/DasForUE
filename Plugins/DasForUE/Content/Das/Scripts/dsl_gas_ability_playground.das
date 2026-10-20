options gen2
require ue_gas

// Place facing a target with an authored ASC/PlaygroundStats and Visibility collision.
// Wire player input to BeginAim, ConfirmAim and CancelAim; the actor's transform is the aim ray.
[ue_class(Blueprintable)]
class AbilityPlayground : Actor {
    settings=ue_properties(EditAnywhere,BlueprintReadOnly) {
        var Attributes:AttributeSetClass
        var HitEffect:AuthoredGameplayEffectClass
        var AuraEffect:AuthoredGameplayEffectClass
        var StrikeAbility:AuthoredGameplayAbilityClass
        var Range:float=600.0f
        var Radius:float=5.0f
        var AimTimeout:float=5.0f
    }
    state=ue_properties(BlueprintReadWrite) {
        var Grant:GASAbilityGrant
        var PreviewTarget:ActorRef
        var LastTarget:ActorRef
        var Shots:int=0
        var CancelledShots:int=0
    }
    @ue_component
    @RootComponent
    Origin:SceneComponentRef
    @ue_component
    Abilities:AuthoredAbilitySystemComponentRef
    def BeginPlay():void {
        ue_gas_add_attributes(self.Abilities,self.Attributes)
        self.Grant=ue_gas_give_ability(self.Abilities,self.StrikeAbility)
    }
    [ue_function(BlueprintCallable)]
    def BeginAim():void { ue_gas_activate_ability(self.Grant) }
    [ue_function(BlueprintCallable)]
    def ConfirmAim():void {
        self.Abilities.SendAuthoredGameplayEvent(ue_name_value("GameplayEvent.Das.Sample.StrikeConfirm"),0.0f,default<ActorRef>,default<ActorRef>)
    }
    [ue_function(BlueprintCallable)]
    def CancelAim():void { ue_gas_cancel_ability(self.Grant) }
}
