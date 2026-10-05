options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
    let self = C_Actor_adf65d5b_from(ue_self_object())
    actor_SetTickGroup(self, E_ETickingGroup_c76e3870.v_TG_PrePhysics)
    let owner = actor_GetOwner(self)
    actor_SetOwner(self, owner)
    verify(actor_ActorHasTag(self, ue_name("DasEnumTag_17")))
    let scale = S_Vector_4bd4f956(f_x = double(2), f_y = double(3), f_z = double(4))
    actor_SetActorRelativeScale3D(self, scale)
    let observed = actor_GetActorRelativeScale3D(self)
    verify(observed.f_x == scale.f_x && observed.f_y == scale.f_y && observed.f_z == scale.f_z)
    let children <- actor_GetAttachedActors(self)
    verify(length(children) == 1 && C_Actor_adf65d5b_valid(children[0]))
    let child_owner = actor_GetOwner(children[0])
    verify(actor_ActorHasTag(child_owner, ue_name("DasEnumTag_17")))
    let location = actor_K2_GetActorLocation(self)
    return int(location.f_x)
}
