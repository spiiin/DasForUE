// These execute the public script call adapters against real native UFunctions.
// A rejected replacement must preserve the previously assigned native value.
{
    const auto R = ObjectRun(TEXT("let c=ue_begin_call(ue_self_object(),\"MutateValues\")\nue_arg_int(c,\"Count\",7)\nvar rejected=false\ntry { ue_arg_string(c,\"Count\",\"wrong\") } recover { rejected=true }\nverify(rejected)\nue_arg_string(c,\"Text\",\"Kept\")\nue_invoke_call(c)\nverify(ue_result_int(c,\"Count\")==10)\nverify(ue_result_string(c,\"Text\")==\"Kept 世界\")\nue_release_call(c)\nreturn 1"));
    Expect(R.bSucceeded && R.Value == 1, TEXT("reflected_values_failed_write_keeps_assigned_input"), R.Error);
}
{
    const auto R = ObjectRun(TEXT("let c=ue_begin_call(ue_self_object(),\"EchoLegacy\")\nue_arg_enum(c,\"Input\",\"/Script/DasRuntime.EDasLegacyMode\",\"DasLegacyActive\")\nue_invoke_call(c)\nvar rejected=false\ntry { let raw=ue_result_byte(c,\"ReturnValue\") } recover { rejected=true }\nverify(rejected)\nverify(ue_result_enum(c,\"ReturnValue\",\"/Script/DasRuntime.EDasLegacyMode\")==\"DasLegacyActive\")\nue_release_call(c)\nreturn 1"));
    Expect(R.bSucceeded && R.Value == 1, TEXT("reflected_values_enum_not_raw_byte_and_read_recovers"), R.Error);
}
#if WITH_EDITOR
{
    // Low-level symbolic native calls deliberately ignore editor-only Hidden
    // metadata. This policy differs from authored enum value validation.
    auto* Enum = FindObject<UEnum>(nullptr, TEXT("/Script/DasRuntime.EDasLegacyMode"));
    const int32 Index = Enum->GetIndexByValue(DasLegacyActive);
    const bool Had = Enum->HasMetaData(TEXT("Hidden"), Index);
    const FString Previous = Enum->GetMetaData(TEXT("Hidden"), Index);
    Enum->SetMetaData(TEXT("Hidden"), TEXT("true"), Index);
    const auto R = ObjectRun(TEXT("let c=ue_begin_call(ue_self_object(),\"EchoLegacy\")\nue_arg_enum(c,\"Input\",\"/Script/DasRuntime.EDasLegacyMode\",\"DasLegacyActive\")\nue_invoke_call(c)\nverify(ue_result_enum(c,\"ReturnValue\",\"/Script/DasRuntime.EDasLegacyMode\")==\"DasLegacyActive\")\nue_release_call(c)\nreturn 1"));
    if (Had)
        Enum->SetMetaData(TEXT("Hidden"), *Previous, Index);
    else
        Enum->RemoveMetaData(TEXT("Hidden"), Index);
    Expect(R.bSucceeded && R.Value == 1, TEXT("reflected_values_native_hidden_enum_policy_preserved"), R.Error);
}
#endif
