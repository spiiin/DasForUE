options gen2
module ue_soft_record public
require ue_dsl public
require ue_soft_references public
[ue_struct]
struct SoftRecord {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Asset:SoftObject
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    ActorType:SoftActorClass
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Items:array<SoftObject>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Named:table<string;SoftActorClass>
}
