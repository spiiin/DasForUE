options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
    let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
    let values = fixture_MutateValues(self, 7, "Привет")
    verify(values.out_count == 10 && values.out_returnvalue == 20)
    verify(values.out_description == "Привет 世界" && values.out_accepted)
    let modes = fixture_ExchangeMode(self, E_EDasCallMode_aac7a973.v_Active, E_EDasCallMode_aac7a973.v_Paused)
    verify(modes.out_previous == E_EDasCallMode_aac7a973.v_Paused)
    verify(modes.out_current == E_EDasCallMode_aac7a973.v_Active)
    let names = fixture_ExchangeName(self, ue_name("Generated_17"), ue_name(""))
    verify(names.out_previous.value == "None")
    verify(names.out_returnvalue.value == "Generated_17")
    let record = fixture_ExchangeNamedMode(self, S_DasNamedMode_5e046658(f_mode = E_EDasCallMode_aac7a973.v_Paused, f_key = ue_name("Record_23")))
    verify(record.out_copy.f_mode == E_EDasCallMode_aac7a973.v_Active)
    verify(record.out_copy.f_key.value == "Record_23")
    let source = S_DasCallPayload_a9a75418(
        f_metrics = S_DasCallMetrics_8fde8e48(f_ratio = 1.5),
        f_count = 2, f_label = "data", f_benabled = false, f_weight = double(2.5))
    let payload = fixture_TransformPayload(self, source, 3, false, "!")
    verify(payload.f_count == 5)
    verify(payload.f_label == "data!")
    verify(payload.f_metrics.f_ratio == 4.5)
    let updated = fixture_MutatePayload(self, source)
    verify(updated.out_payload.f_count == 6 && updated.out_copy.f_count == 6)
    verify(updated.out_copy.f_label == "data!")
    verify(source.f_count == 2 && source.f_label == "data")
    let reused = fixture_TransformPayload(self, updated.out_copy, 0, false, "?")
    verify(reused.f_count == 6 && reused.f_label == "data!?")
    verify(reused.f_metrics.f_ratio == 4.5)
    return values.out_count
}
