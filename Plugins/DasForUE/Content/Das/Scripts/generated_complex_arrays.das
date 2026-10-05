options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
    let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
    let payload = S_DasCallPayload_a9a75418(f_metrics = S_DasCallMetrics_8fde8e48(f_ratio = 1.5), f_count = 2, f_label = "Привет", f_benabled = true, f_weight = 1.23456789012345lf)
    let inputs <- [payload, payload]
    let transformed <- fixture_TransformPayloads(self, inputs)
    verify(transformed[0].f_count == 6 && transformed[1].f_label == "Привет 世界")
    verify(transformed[0].f_metrics.f_ratio == 3.0 && transformed[0].f_weight == 1.23456789012345lf)
    verify(inputs[0].f_count == 2 && inputs[1].f_label == "Привет")
    let reused <- fixture_TransformPayloads(self, transformed)
    verify(reused[0].f_count == 10 && reused[0].f_metrics.f_ratio == 6.0)
    let named <- [S_DasNamedMode_5e046658(f_mode = E_EDasCallMode_aac7a973.v_Idle, f_key = ue_name("Original_17"))]
    let mutated <- fixture_MutateNamedModes(self, named)
    verify(mutated.out_returnvalue == 2 && length(mutated.out_values) == 2)
    verify(mutated.out_copy[0].f_mode == E_EDasCallMode_aac7a973.v_Active)
    verify(mutated.out_copy[1].f_key.value == "Added_23" && named[0].f_mode == E_EDasCallMode_aac7a973.v_Idle)
    let modes <- [E_EDasCallMode_aac7a973.v_Active, E_EDasCallMode_aac7a973.v_Paused]
    let mode_result <- fixture_EchoModes(self, modes)
    verify(mode_result[1] == E_EDasCallMode_aac7a973.v_Paused)
    let fresh = fixture_NewFixture(self)
    let objects <- [self, fresh, C_DasObjectFixture_5f019a81_null()]
    let echoed <- fixture_EchoFixtureObjects(self, objects) // collects GC inside ProcessEvent
    verify(length(echoed) == 3 && C_DasObjectFixture_5f019a81_valid(echoed[1]))
    verify(!C_DasObjectFixture_5f019a81_valid(echoed[2]))
    let swapped <- fixture_SwapFixtureObjects(self, objects)
    verify(swapped.out_returnvalue == 2 && length(swapped.out_values) == 2 && length(swapped.out_copy) == 3)
    verify(C_DasObjectFixture_5f019a81_valid(swapped.out_values[1]))
    verify(length(objects) == 3)
    let made <- fixture_MakeFixtureObjects(self)
    verify(length(made) == 3 && C_DasObjectFixture_5f019a81_valid(made[0]) && !C_DasObjectFixture_5f019a81_valid(made[1]))
    var empty_payloads : array<S_DasCallPayload_a9a75418>
    let empty_result <- fixture_TransformPayloads(self, empty_payloads)
    verify(length(empty_result) == 0)
    var empty_objects : array<C_DasObjectFixture_5f019a81>
    let empty_object_result <- fixture_EchoFixtureObjects(self, empty_objects)
    verify(length(empty_object_result) == 0)
    return 1
}
