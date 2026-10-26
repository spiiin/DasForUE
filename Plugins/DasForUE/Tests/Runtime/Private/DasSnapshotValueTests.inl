// Drafts must keep their reflected references alive, and release them on rollback.
{
    const auto BatchGC = ObjectRun(TEXT(R"das(
let original=ue_struct_value("/Script/DasRuntime.DasDefaultBundleProbe")
let made=ue_begin_call(ue_self_object(),"NewFixture")
ue_invoke_call(made)
let object=ue_result_object(made,"ReturnValue")
let changed=ue_struct_edit(original) $(var draft:UEStructValue) {
    draft=ue_struct_map_with_object(draft,"Nested.Lookup",ue_name_value("live"),object)
    ue_release_call(made)
    let gc=ue_begin_call(ue_self_object(),"ForceTestGC"); ue_invoke_call(gc); ue_release_call(gc)
    verify(ue_object_valid(object))
}
verify(ue_object_valid(ue_struct_map_get_object(changed,"Nested.Lookup",ue_name_value("live"))))
verify(ue_struct_map_size(original,"Nested.Lookup")==0)
let other=ue_begin_call(ue_self_object(),"NewFixture")
ue_invoke_call(other)
let discarded=ue_result_object(other,"ReturnValue")
var rejected=false
try {
    let unused=ue_struct_edit(original) $(var draft:UEStructValue) {
        draft=ue_struct_map_with_object(draft,"Nested.Lookup",ue_name_value("discarded"),discarded)
        ue_release_call(other)
        panic("discard draft reference")
    }
} recover { rejected=true }
let gc=ue_begin_call(ue_self_object(),"ForceTestGC"); ue_invoke_call(gc); ue_release_call(gc)
verify(rejected && !ue_object_valid(discarded) && ue_object_valid(object))
return 1
)das"));
    Expect(BatchGC.bSucceeded && BatchGC.Value == 1, TEXT("snapshot_batch_GC_roots_draft_and_releases_rollback"),
                                                          BatchGC.Error);
}
// Use the public snapshot adapters, including exception recovery in one context.
{
    Runtime.ReleaseObject(First.Get());
    TStrongObjectPtr<UDasObjectFixture> ReadTarget(NewObject<UDasObjectFixture>());
    FDasScalarValue ReadInput;
    ReadInput.Kind = 12;
    ReadInput.ObjectClass = FDasSnapshotReadProbe::StaticStruct()->GetPathName();
    ReadInput.StructValue.InitializeAs<FDasSnapshotReadProbe>();
    auto& Probe = ReadInput.StructValue.GetMutable<FDasSnapshotReadProbe>();
    Probe.IntMap.Add(-1, 7);
    Probe.IntMap.Add(2, 9);
    Probe.IntSet.Add(4);
    Probe.IntSet.Add(8);
    Probe.Nested.Priority = 11;
    Probe.Nested.Caption = NSLOCTEXT("SnapshotRead", "Caption", "Saved");
    Probe.Nested.Positions.Add(FVector(1, 2, 3));
    Probe.Nested.Tags.Add(TEXT("SnapshotReadTag_3"));
    Probe.Nested.Lookup.Add(TEXT("SnapshotReadAsset"), ReadTarget.Get());
    Probe.Rows.Add(Probe.Nested);
    Probe.Catalog.Add(TEXT("saved"), Probe.Nested);
    const auto& Frozen = ReadInput.StructValue;
    DasSnapshotFields::TField<const uint8> Borrowed{};
    FString FieldError;
    const bool Found = DasSnapshotFields::Find(Frozen.GetScriptStruct(), Frozen.GetMemory(),
                                               TEXT("Rows.0.Priority"), Borrowed, FieldError);
    static_assert(std::is_same_v<decltype(Borrowed.Memory), const uint8*>);
    Expect(Found && Borrowed.Memory == &Probe.Rows[0].Priority, TEXT("snapshot_read_field_aliases_const_source"),
                                                                     FieldError);
#include "DasSnapshotFieldsTests.inl"
    FDasSnapshotReadProbe::CopyCount.Store(0);
    {
        FInstancedStruct ControlCopy = Frozen;
        Expect(FDasSnapshotReadProbe::CopyCount.Load() > 0, TEXT("snapshot_read_copy_counter_detects_real_clone"));
    }
    TStrongObjectPtr<UDasSnapshotReadOwner> ReadOwner(NewObject<UDasSnapshotReadOwner>());
    ReadOwner->Snapshot = Probe;
    auto* SnapshotField = FindFProperty<FStructProperty>(ReadOwner->GetClass(), TEXT("Snapshot"));
    const auto PreviousFlags = SnapshotField->GetPropertyFlags();
    SnapshotField->SetPropertyFlags(CPF_BlueprintVisible);
    const bool Written = Write(TEXT(R"das(options gen2
require ue_foundation
var escaped:UEStructValue
[export]
def batch(input:int):int {
    let original=ue_get_self_struct("Snapshot")
    let changed=ue_struct_edit(original) $(var draft:UEStructValue) {
        escaped=draft
        draft=ue_struct_map_with_int(draft,"IntMap",-3,44)
        verify(ue_struct_map_key(draft,"IntMap",0)==-3)
        draft=ue_struct_map_with_int(draft,"IntMap",-5,45)
        verify(ue_struct_map_key(draft,"IntMap",0)==-5)
        draft=ue_struct_set_with_int(draft,"IntSet",-2,true)
        verify(ue_struct_set_element_int(draft,"IntSet",0)==-2)
        draft=ue_struct_set_with_int(draft,"IntSet",-4,true)
        verify(ue_struct_set_element_int(draft,"IntSet",0)==-4)
        draft=ue_struct_with_byte(draft,"Nested.Priority",uint8(42))
        draft=ue_struct_with_text(draft,"Nested.Caption",ue_text("Batch","Title","Changed"))
        draft=ue_struct_array_resize(draft,"Rows",2)
        draft=ue_struct_with_byte(draft,"Rows.1.Priority",uint8(43))
        verify(ue_struct_get_byte(draft,"Rows.1.Priority")==uint8(43))
    }
    verify(ue_struct_map_size(changed,"IntMap")==4 && ue_struct_set_size(changed,"IntSet")==4)
    verify(ue_struct_get_byte(changed,"Nested.Priority")==uint8(42))
    verify(ue_text_key(ue_struct_get_text(changed,"Nested.Caption"))=="Title")
    verify(ue_struct_array_size(changed,"Rows")==2 && ue_struct_get_byte(changed,"Rows.1.Priority")==uint8(43))
    verify(ue_struct_map_size(original,"IntMap")==2 && ue_struct_set_size(original,"IntSet")==2)
    verify(ue_struct_array_size(original,"Rows")==1 && ue_struct_get_byte(original,"Nested.Priority")==uint8(11))
    var expired=false
    try { ue_struct_map_size(escaped,"IntMap") } recover { expired=true }
    verify(expired)
    return 1
}
[export]
def batch_failures(input:int):int {
    let original=ue_get_self_struct("Snapshot")
    var rejected=0
    try {
        let unused=ue_struct_edit(original) $(var draft:UEStructValue) {
            escaped=draft
            draft=ue_struct_map_with_int(draft,"IntMap",-7,99)
            panic("batch rollback")
        }
    } recover { rejected+=1 }
    try { ue_struct_map_size(escaped,"IntMap") } recover { rejected+=1 }
    try {
        let unused=ue_struct_edit(original) $(var draft:UEStructValue) {
            escaped=draft
            draft=ue_struct_map_with_int(draft,"IntMap",-9,100)
            try { draft=ue_struct_with_string(draft,"Nested.Priority","wrong") } recover { rejected+=1 }
        }
    } recover { rejected+=1 }
    try { ue_struct_map_size(escaped,"IntMap") } recover { rejected+=1 }
    // A failed map value write may already have inserted the key in the draft.
    let wrong=ue_struct_value("/Script/CoreUObject.IntPoint")
    try {
        let unused=ue_struct_edit(original) $(var draft:UEStructValue) {
            escaped=draft
            try { draft=ue_struct_map_with_struct(draft,"Catalog","bad",wrong) } recover { rejected+=1 }
        }
    } recover { rejected+=1 }
    // An inner independent edit is allowed, but re-editing the active draft is not.
    let changed=ue_struct_edit(original) $(var draft:UEStructValue) {
        try { let unused=ue_struct_edit(draft) $(var inner:UEStructValue) {} } recover { rejected+=1 }
        let child=ue_struct_edit(original) $(var inner:UEStructValue) {
            inner=ue_struct_with_byte(inner,"Nested.Priority",uint8(54))
        }
        verify(ue_struct_get_byte(child,"Nested.Priority")==uint8(54))
        draft=ue_struct_with_byte(draft,"Nested.Priority",uint8(55))
    }
    verify(rejected==8 && ue_struct_map_size(original,"IntMap")==2 && ue_struct_map_size(original,"Catalog")==1)
    verify(ue_struct_get_byte(changed,"Nested.Priority")==uint8(55))
    return 1
}
[export]
def noop(input:int):int {
    let value=ue_get_self_struct("Snapshot")
    return ue_struct_map_size(value,"IntMap")-1
}
[export]
def queries(input:int):int {
    let value=ue_get_self_struct("Snapshot")
    for (i in range(0,8)) {
        verify(ue_struct_map_size(value,"IntMap")==2 && ue_struct_map_key(value,"IntMap",0)==-1)
        verify(ue_struct_map_get_int(value,"IntMap",-1)==7)
        verify(ue_struct_set_size(value,"IntSet")==2 && ue_struct_set_contains_int(value,"IntSet",4))
        verify(!ue_struct_set_contains_int(value,"IntSet",5) && ue_struct_set_element_int(value,"IntSet",0)==4)
        verify(ue_struct_array_size(value,"Rows")==1 && ue_struct_get_byte(value,"Rows.0.Priority")==uint8(11))
        verify(ue_struct_get_double(value,"Nested.Positions.0.X")==1.0lf)
        verify(ue_text_key(ue_struct_get_text(value,"Nested.Caption"))=="Caption")
        verify(ue_name_text(ue_struct_set_element_name(value,"Nested.Tags",0))=="SnapshotReadTag_3")
        verify(ue_name_text(ue_struct_map_key_name(value,"Nested.Lookup",0))=="SnapshotReadAsset")
        verify(ue_object_valid(ue_struct_map_get_object(value,"Nested.Lookup",ue_name_value("SnapshotReadAsset"))))
        verify(ue_struct_map_key_string(value,"Catalog",0)=="saved")
    }
    let nested=ue_struct_nested_get(value,"Rows",0)
    let saved=ue_struct_map_get_struct(value,"Catalog","saved")
    let changed=ue_struct_with_byte(nested,"Priority",uint8(99))
    verify(ue_struct_get_byte(changed,"Priority")==uint8(99) && ue_struct_get_byte(saved,"Priority")==uint8(11))
    verify(ue_struct_get_byte(value,"Rows.0.Priority")==uint8(11))
    var failures=0
    try { ue_struct_map_key(value,"IntMap",2) } recover { failures+=1 }
    try { ue_struct_set_element_int(value,"IntSet",-1) } recover { failures+=1 }
    try { ue_struct_get_byte(value,"Rows.01.Priority") } recover { failures+=1 }
    verify(failures==3)
    return 1
}
[export]
def changed(input:int):int {
    let original=ue_get_self_struct("Snapshot")
    verify(ue_struct_map_key(original,"IntMap",0)==-1)
    let extended=ue_struct_map_with_int(original,"IntMap",-3,44)
    verify(ue_struct_map_key(extended,"IntMap",0)==-3 && ue_struct_map_key(extended,"IntMap",1)==-1 && ue_struct_map_key(extended,"IntMap",2)==2)
    verify(ue_struct_map_size(original,"IntMap")==2 && ue_struct_map_key(original,"IntMap",0)==-1)
    verify(ue_struct_set_element_int(original,"IntSet",0)==4)
    let altered=ue_struct_set_with_int(original,"IntSet",-2,true)
    verify(ue_struct_set_element_int(altered,"IntSet",0)==-2 && ue_struct_set_element_int(original,"IntSet",0)==4)
    return 1
}
)das"));
    auto InvokeRead = [&](const TCHAR* Entry)
    {
        return Runtime.RunNamedPersistent(Scratch, Entry, 0, ReadOwner.Get());
    };
    auto Warm = InvokeRead(TEXT("noop"));
    auto WarmQueries = InvokeRead(TEXT("queries"));
    FDasSnapshotReadProbe::CopyCount.Store(0);
    const auto Baseline = InvokeRead(TEXT("noop"));
    const int32 TransportCopies = FDasSnapshotReadProbe::CopyCount.Load();
    FDasSnapshotReadProbe::CopyCount.Store(0);
    const auto Queries = InvokeRead(TEXT("queries"));
    const int32 QueryCopies = FDasSnapshotReadProbe::CopyCount.Load();
    Expect(Written && Warm.bSucceeded && WarmQueries.bSucceeded && Baseline.bSucceeded && Queries.bSucceeded &&
               Queries.Value == 1 && QueryCopies == TransportCopies,
           TEXT("snapshot_queries_add_zero_native_parent_copies"),
                FString::Printf(TEXT("transport=%d queries=%d %s %s"), TransportCopies, QueryCopies, *WarmQueries.Error,
                                     *Queries.Error));
    Expect(Probe.Rows[0].Priority == 11 && Probe.IntMap.Num() == 2 && Probe.IntSet.Num() == 2,
           TEXT("snapshot_queries_and_child_changes_preserve_input"));
    const auto Changed = InvokeRead(TEXT("changed"));
    Expect(Changed.bSucceeded && Changed.Value == 1, TEXT("snapshot_key_orders_isolate_changed_snapshots"),
                                                          Changed.Error);
    FDasSnapshotReadProbe::CopyCount.Store(0);
    const auto Batch = InvokeRead(TEXT("batch"));
    const int32 BatchCopies = FDasSnapshotReadProbe::CopyCount.Load();
    Expect(Batch.bSucceeded && Batch.Value == 1 && BatchCopies == TransportCopies + 1,
           TEXT("snapshot_batch_mixed_writes_clone_once_and_expire_draft"),
                FString::Printf(TEXT("copies=%d transport=%d %s"), BatchCopies, TransportCopies, *Batch.Error));
    const auto BatchFailures = InvokeRead(TEXT("batch_failures"));
    Expect(BatchFailures.bSucceeded && BatchFailures.Value == 1,
           TEXT("snapshot_batch_rollback_caught_write_poisoning_and_nested_scope"), BatchFailures.Error);
    Runtime.ReleaseObject(ReadOwner.Get());
    SnapshotField->ClearPropertyFlags(CPF_BlueprintVisible & ~PreviousFlags);
}
{
    const auto R = ObjectRun(TEXT(R"das(
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
    Expect(R.bSucceeded && R.Value == 19, TEXT("snapshot_values_leaf_nested_copy_and_failed_assignment_atomic"),
                                               R.Error);
}
{
    const auto R = ObjectRun(TEXT(R"das(
let original=ue_struct_value("/Script/DasRuntime.DasDefaultBundleProbe")
var changed=original
let wrong=ue_struct_value("/Script/CoreUObject.IntPoint")
var rejected=false
try { changed=ue_struct_map_with_struct(changed,"Catalog","new",wrong) } recover { rejected=true }
verify(rejected && ue_struct_map_size(original,"Catalog")==0 && ue_struct_map_size(changed,"Catalog")==0)
var child=ue_struct_value("/Script/DasRuntime.DasDefaultNestedProbe")
child=ue_struct_with_byte(child,"Priority",uint8(255))
child=ue_struct_with_text(child,"Caption",ue_text("SnapshotCodec","Caption","Saved"))
changed=ue_struct_map_with_struct(changed,"Catalog","new",child)
child=ue_struct_with_byte(child,"Priority",uint8(3))
let saved=ue_struct_map_get_struct(changed,"Catalog","new")
verify(ue_struct_get_byte(saved,"Priority")==uint8(255) && ue_text_key(ue_struct_get_text(saved,"Caption"))=="Caption")
verify(ue_struct_map_size(original,"Catalog")==0)
changed=ue_struct_array_resize(changed,"Rows",1)
changed=ue_struct_nested_with(changed,"Rows",0,saved)
rejected=false
try { changed=ue_struct_nested_with(changed,"Rows",2,child) } recover { rejected=true }
verify(rejected && ue_struct_get_byte(changed,"Rows.0.Priority")==uint8(255))
return 1
)das"));
    Expect(R.bSucceeded && R.Value == 1, TEXT("snapshot_values_map_insert_failure_nested_array_text_byte_isolation"),
                                              R.Error);
}
#if WITH_EDITOR
{
    auto* Enum = StaticEnum<EDasCallMode>();
    const int32 Index = Enum->GetIndexByValue(int64(EDasCallMode::Active));
    const bool Had = Enum->HasMetaData(TEXT("Hidden"), Index);
    const FString Previous = Enum->GetMetaData(TEXT("Hidden"), Index);
    Enum->SetMetaData(TEXT("Hidden"), TEXT("true"), Index);
    const auto R = ObjectRun(TEXT(R"das(
let value=ue_struct_with_enum(ue_struct_value("/Script/DasRuntime.DasNamedMode"),"Mode","/Script/DasRuntime.EDasCallMode","Active")
verify(ue_struct_get_enum(value,"Mode","/Script/DasRuntime.EDasCallMode")=="Active")
var rejected=false
try { let typed=ue_enum_value("/Script/DasRuntime.EDasCallMode","Active") } recover { rejected=true }
verify(rejected)
return 1
)das"));
    if (Had)
        Enum->SetMetaData(TEXT("Hidden"), *Previous, Index);
    else
        Enum->RemoveMetaData(TEXT("Hidden"), Index);
    Expect(R.bSucceeded && R.Value == 1, TEXT("snapshot_values_native_symbol_enum_policy_preserved"), R.Error);
}
#endif
