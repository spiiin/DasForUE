options gen2
require ue_gas

[ue_class(Blueprintable)]
class AbilityTarget : Actor {
    settings=ue_properties(EditAnywhere,BlueprintReadOnly) { var Attributes:AttributeSetClass }
    @ue_component
    @RootComponent
    HitBody:SphereComponentRef
    @ue_component
    @Attach="HitBody"
    Visual:StaticMeshComponentRef
    @ue_component
    Abilities:AuthoredAbilitySystemComponentRef
    defaults=ue_defaults() { self.Visual.RelativeScale3D=ue_vector(0.5lf,0.5lf,0.5lf) }
    def BeginPlay():void {
        ue_gas_add_attributes(self.Abilities,self.Attributes)
        self.Visual.SetCollisionEnabled(ECollisionEnabled.NoCollision)
        self.Visual.SetStaticMesh(ue_soft_load(ue_soft(type<SoftStaticMesh>,"/Engine/BasicShapes/Sphere.Sphere")))
        self.HitBody.SetCollisionProfileName(ue_name_value("BlockAllDynamic"),true)
        self.HitBody.SetSphereRadius(25.0f,true)
        self.HitBody.SetCollisionEnabled(ECollisionEnabled.QueryOnly)
    }
}
