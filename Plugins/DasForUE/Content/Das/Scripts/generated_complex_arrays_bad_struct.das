options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
    let values <- [S_Vector_4bd4f956(f_x = 1.0lf, f_y = 2.0lf, f_z = 3.0lf)]
    let result <- fixture_TransformPayloads(C_DasObjectFixture_5f019a81_from(ue_self_object()), values)
    return 1
}
