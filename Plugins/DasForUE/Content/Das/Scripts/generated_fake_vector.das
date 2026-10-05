options gen2
require ue_generated
require ue_foundation
struct FakeVector { f_x : double; f_y : double; f_z : double }
[export]
def main(input:int):int {
    actor_SetActorRelativeScale3D(C_Actor_adf65d5b_from(ue_self_object()), FakeVector(f_x = double(1), f_y = double(2), f_z = double(3)))
    return 1
}
