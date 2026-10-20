options gen2
require ue_gas

// Import the cue definitions first, assign the three generated classes in Details,
// place in a map with a player Pawn, then walk into/out of the visible sphere.
[ue_class(Blueprintable)]
class CuePlayground : Actor {
    settings=ue_properties(EditAnywhere,BlueprintReadOnly) {
        var Attributes:AttributeSetClass
        var HitEffect:AuthoredGameplayEffectClass
        var AuraEffect:AuthoredGameplayEffectClass
    }
    @ue_component
    @RootComponent
    Trigger:SphereComponentRef
    @ue_component
    @Attach="Trigger"
    Visual:StaticMeshComponentRef
    @ue_component
    Abilities:AuthoredAbilitySystemComponentRef
    @ue_property
    Aura:GASEffectResult
    @ue_property
    Inside:array<ActorRef>
    defaults=ue_defaults() {
        self.Visual.RelativeScale3D=ue_vector(0.3lf,0.3lf,0.3lf)
    }
    def BeginPlay():void {
        ue_gas_add_attributes(self.Abilities,self.Attributes)
        self.Visual.SetCollisionEnabled(ECollisionEnabled.NoCollision)
        self.Visual.SetStaticMesh(ue_soft_load(ue_soft(type<SoftStaticMesh>,"/Engine/BasicShapes/Sphere.Sphere")))
        self.Trigger.SetCollisionProfileName(ue_name_value("OverlapOnlyPawn"),true)
        self.Trigger.SetSphereRadius(140.0f,true)
        self.Trigger.SetGenerateOverlapEvents(true)
        self.Trigger.SetCollisionEnabled(ECollisionEnabled.QueryOnly)
    }
    [ue_function(BlueprintCallable)]
    def Hit():void {
        ue_gas_apply(self.Abilities,ue_gas_make_spec(self.Abilities,self.HitEffect))
    }
    [ue_function(BlueprintCallable)]
    def StartAura():void {
        if(ue_gas_effect_state(self.Aura).Active) { return }
        self.Aura=ue_gas_apply(self.Abilities,ue_gas_make_spec(self.Abilities,self.AuraEffect))
    }
    [ue_function(BlueprintCallable)]
    def StopAura():void {
        ue_gas_remove_effect(self.Aura)
    }
    [ue_event(name="OnActorBeginOverlap")]
    def Enter(overlapped:ActorRef; other:ActorRef):void {
        if(!ue_is_valid(other)) { return }
        for(actor in self.Inside) { if(ue_same_object(actor,other)) { return } }
        push(self.Inside,other)
        self.Hit()
        self.StartAura()
    }
    [ue_event(name="OnActorEndOverlap")]
    def Leave(overlapped:ActorRef; other:ActorRef):void {
        for(index in range(length(self.Inside))) {
            if(ue_same_object(self.Inside[index],other)) { erase(self.Inside,index); break }
        }
        if(length(self.Inside)==0) { self.StopAura() }
    }
}
