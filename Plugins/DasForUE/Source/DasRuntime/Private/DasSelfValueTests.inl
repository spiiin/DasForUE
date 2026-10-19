// Exercise script-visible field access, recovery and access policy together.
{
    const int32 Previous = First->health;
    const auto R = ObjectRun(TEXT("ue_set_self_int(\"health\",41)\nvar rejected=false\ntry { ue_set_self_string(\"health\",\"wrong\") } recover { rejected=true }\nverify(rejected)\nverify(ue_get_self_int(\"health\")==41)\nrejected=false\ntry { let wrong=ue_get_self_struct(\"health\") } recover { rejected=true }\nverify(rejected)\nrejected=false\ntry { ue_set_self_int(\"readOnly\",999) } recover { rejected=true }\nverify(rejected)\nverify(ue_get_self_int(\"readOnly\")==7)\nue_set_self_int(\"health\",42)\nreturn ue_get_self_int(\"health\")"));
    Expect(R.bSucceeded && R.Value == 42 && First->health == 42 && First->readOnly == 7,
           TEXT("self_values_rejected_access_preserves_field_and_context_recovers"), R.Error);
    First->health = Previous;
}
