options gen2
require ue_dsl
require ue_soft_references
[ue_class]
class BadSoft : Actor {
    @ue_property=(EditAnywhere)
    Value:SoftObject = ue_soft(type<SoftActorClass>,"/Script/Engine.StaticMeshActor")
}
