// Use the public snapshot adapters, including exception recovery in one context.
{
    const auto R=ObjectRun(TEXT(R"das(
let original=ue_struct_with_int(ue_struct_value("/Script/DasRuntime.DasCallPayload"),"Count",17)
var changed=ue_struct_with_int(original,"Count",19)
var rejected=false
try { changed=ue_struct_with_string(changed,"Count","wrong") } recover { rejected=true }
verify(rejected && ue_struct_get_int(original,"Count")==17 && ue_struct_get_int(changed,"Count")==19)
let metrics=ue_struct_with_float(ue_struct_nested_get(original,"Metrics",-1),"Ratio",2.5)
changed=ue_struct_nested_with(changed,"Metrics",-1,metrics)
verify(ue_struct_get_float(original,"Metrics.Ratio")==0.0 && ue_struct_get_float(changed,"Metrics.Ratio")==2.5)
rejected=false
try { changed=ue_struct_nested_with(changed,"Metrics",-1,original) } recover { rejected=true }
verify(rejected && ue_struct_get_float(changed,"Metrics.Ratio")==2.5)
return ue_struct_get_int(changed,"Count")
)das"));
    Expect(R.bSucceeded && R.Value==19,TEXT("snapshot_values_leaf_nested_copy_and_failed_assignment_atomic"),R.Error);
}
{
    const auto R=ObjectRun(TEXT(R"das(
let original=ue_struct_value("/Script/DasRuntime.DasDefaultBundleProbe")
var changed=original
let wrong=ue_struct_value("/Script/CoreUObject.IntPoint")
var rejected=false
try { changed=__ue_struct_map_with_string_struct(changed,"Catalog","new",wrong) } recover { rejected=true }
verify(rejected && ue_struct_map_size(original,"Catalog")==0 && ue_struct_map_size(changed,"Catalog")==0)
var child=ue_struct_value("/Script/DasRuntime.DasDefaultNestedProbe")
child=ue_struct_with_byte(child,"Priority",uint8(255))
child=ue_struct_with_text(child,"Caption",ue_text("SnapshotCodec","Caption","Saved"))
changed=__ue_struct_map_with_string_struct(changed,"Catalog","new",child)
child=ue_struct_with_byte(child,"Priority",uint8(3))
let saved=__ue_struct_map_get_string_struct(changed,"Catalog","new")
verify(ue_struct_get_byte(saved,"Priority")==uint8(255) && ue_text_key(ue_struct_get_text(saved,"Caption"))=="Caption")
verify(ue_struct_map_size(original,"Catalog")==0)
changed=ue_struct_array_resize(changed,"Rows",1)
changed=ue_struct_nested_with(changed,"Rows",0,saved)
rejected=false
try { changed=ue_struct_nested_with(changed,"Rows",2,child) } recover { rejected=true }
verify(rejected && ue_struct_get_byte(changed,"Rows.0.Priority")==uint8(255))
return 1
)das"));
    Expect(R.bSucceeded && R.Value==1,TEXT("snapshot_values_map_insert_failure_nested_array_text_byte_isolation"),R.Error);
}
#if WITH_EDITOR
{
    auto* Enum=StaticEnum<EDasCallMode>();const int32 Index=Enum->GetIndexByValue(int64(EDasCallMode::Active));
    const bool Had=Enum->HasMetaData(TEXT("Hidden"),Index);const FString Previous=Enum->GetMetaData(TEXT("Hidden"),Index);
    Enum->SetMetaData(TEXT("Hidden"),TEXT("true"),Index);
    const auto R=ObjectRun(TEXT(R"das(
let value=ue_struct_with_enum(ue_struct_value("/Script/DasRuntime.DasNamedMode"),"Mode","/Script/DasRuntime.EDasCallMode","Active")
verify(ue_struct_get_enum(value,"Mode","/Script/DasRuntime.EDasCallMode")=="Active")
var rejected=false
try { let typed=ue_enum_value("/Script/DasRuntime.EDasCallMode","Active") } recover { rejected=true }
verify(rejected)
return 1
)das"));
    if(Had)Enum->SetMetaData(TEXT("Hidden"),*Previous,Index);else Enum->RemoveMetaData(TEXT("Hidden"),Index);
    Expect(R.bSucceeded && R.Value==1,TEXT("snapshot_values_native_symbol_enum_policy_preserved"),R.Error);
}
#endif
