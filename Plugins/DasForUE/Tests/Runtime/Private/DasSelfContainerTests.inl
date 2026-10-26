// In-place self container externs: element operations, range and type errors, readonly and the snapshot limit.
{
    First->SelfInts.Reset();
    First->SelfStrings.Reset();
    First->SelfScores.Reset();
    First->SelfIds.Reset();
    First->SelfLockedInts.Reset();
    const auto R = ObjectRun(TEXT(R"(__ue_self_array_push_int("SelfInts", 1)
__ue_self_array_push_int("SelfInts", 3)
__ue_self_array_insert_int("SelfInts", 2, 1l)
__ue_self_array_set_int("SelfInts", 0l, __ue_self_array_get_int("SelfInts", 0l) + 9)
__ue_self_array_erase("SelfInts", 1l, 1l)
__ue_self_array_push_string("SelfStrings", "a")
__ue_self_array_pop("SelfStrings")
var rejected = 0
try { __ue_self_array_set_int("SelfInts", 5l, 1) } recover { rejected += 1 }
try { __ue_self_array_insert_int("SelfInts", 1, 9l) } recover { rejected += 1 }
try { __ue_self_array_pop("SelfStrings") } recover { rejected += 1 }
try { __ue_self_array_push_float("SelfInts", 1.0) } recover { rejected += 1 }
try { __ue_self_array_push_int("SelfLockedInts", 1) } recover { rejected += 1 }
try { __ue_self_array_erase("SelfInts", 1l, 5l) } recover { rejected += 1 }
__ue_self_map_set_double("SelfScores", ue_name_value("Unit"), 1.5lf)
__ue_self_map_set_double("SelfScores", ue_name_value("UNIT"),
    __ue_self_map_get_double("SelfScores", ue_name_value("UNIT")) + 1.0lf)
verify(__ue_self_map_get_double("SelfScores", ue_name_value("Missing")) == 0.0lf)
verify(!__ue_self_map_erase_name("SelfScores", ue_name_value("Missing")))
__ue_self_set_insert_int("SelfIds", 4)
__ue_self_set_insert_int("SelfIds", 4)
__ue_self_set_insert_int("SelfIds", 5)
verify(__ue_self_set_erase_int("SelfIds", 5) && !__ue_self_set_erase_int("SelfIds", 5))
return rejected)"));
    const double* UnitScore = First->SelfScores.Find(TEXT("unit"));
    Expect(R.bSucceeded && R.Value == 6 && First->SelfInts == TArray<int32>{10, 3} && First->SelfStrings.IsEmpty() &&
               First->SelfScores.Num() == 1 && UnitScore && *UnitScore == 2.5 && First->SelfIds.Num() == 1 &&
               First->SelfIds.Contains(4) && First->SelfLockedInts.IsEmpty(),
           TEXT("self_containers_in_place_operations_and_errors"), R.Error);

    // Growth stops at the snapshot element limit, so the container stays readable as a snapshot.
    First->SelfInts.Init(1, 4096);
    const auto Limit = ObjectRun(TEXT(R"(var rejected = false
try { __ue_self_array_push_int("SelfInts", 2) } recover { rejected = true }
__ue_self_array_set_int("SelfInts", 4095l, 3)
let snapshot = ue_get_self_struct("SelfInts")
return rejected ? 1 : 0)"));
    Expect(Limit.bSucceeded && Limit.Value == 1 && First->SelfInts.Num() == 4096 && First->SelfInts.Last() == 3,
           TEXT("self_containers_snapshot_limit_kept"), Limit.Error);

    const auto Clear = ObjectRun(TEXT(R"(__ue_self_container_clear("SelfInts")
__ue_self_container_clear("SelfScores")
__ue_self_container_clear("SelfIds")
var rejected = false
try { __ue_self_container_clear("health") } recover { rejected = true }
return rejected ? 1 : 0)"));
    Expect(Clear.bSucceeded && Clear.Value == 1 && First->SelfInts.IsEmpty() && First->SelfScores.IsEmpty() &&
               First->SelfIds.IsEmpty(),
           TEXT("self_containers_clear"), Clear.Error);

    // Informational: push+pop on a 1024-element self int array in place, versus the snapshot read and write
    // that the former lowering performed around each operation (a lower bound: it also converted the
    // container to and from a VM array).
    First->SelfInts.Init(7, 1024);
    auto Time = [&](const TCHAR* Body, int32 Iterations, double& Ns)
    {
        auto Run = [&](int32 N)
        {
            return ObjectRun(FString::Printf(TEXT("for (i in range(%d)) {\n%s\n}\nreturn 1"), N, Body));
        };
        const double S0 = FPlatformTime::Seconds();
        const auto A = Run(1);
        const double S1 = FPlatformTime::Seconds();
        const auto B = Run(Iterations + 1);
        const double S2 = FPlatformTime::Seconds();
        Ns = FMath::Max(0.0, (S2 - S1) - (S1 - S0)) * 1e9 / (Iterations * 2);
        return A.bSucceeded && B.bSucceeded;
    };
    double InPlaceNs = 0, SnapshotNs = 0;
    const bool Timed =
        Time(TEXT("__ue_self_array_push_int(\"SelfInts\", i)\n__ue_self_array_pop(\"SelfInts\")"), 200000, InPlaceNs) &&
             Time(TEXT("ue_set_self_struct(\"SelfInts\", ue_get_self_struct(\"SelfInts\"))\n")
                      TEXT("ue_set_self_struct(\"SelfInts\", ue_get_self_struct(\"SelfInts\"))"), 500, SnapshotNs);
    Expect(Timed && First->SelfInts.Num() == 1024,
           TEXT("self_container_benchmark"),
                FString::Printf(TEXT("in_place=%.0f snapshot_round_trip=%.0f (ns per operation, 1024 ints)"), InPlaceNs,
                                     SnapshotNs));
    First->SelfInts.Reset();
}

{
    First->InteropCounts.Reset();
    First->InteropStrings.Reset();
    First->InteropBytes.Reset();
    const auto Interop = ObjectRun(TEXT(R"(
var key=3
__ue_self_map_set_int("InteropCounts",key,10)
verify(__ue_self_map_get_int("InteropCounts",key)==10)
var text="Unicode 世界"
__ue_self_map_set_double("InteropStrings",text,1.25lf)
var byte_key=uint8(8)
__ue_self_map_set_int("InteropBytes",byte_key,12)
var rejected=0
try { __ue_self_map_set_int("InteropCounts",text,30) } recover { rejected+=1 }
try { __ue_self_map_set_int("InteropLocked",key,30) } recover { rejected+=1 }
verify(__ue_self_map_get_int("InteropCounts",key)==10)
verify(__ue_self_map_get_double("InteropStrings",text)==1.25lf)
verify(__ue_self_map_get_int("InteropBytes",byte_key)==12)
return rejected)"));
    Expect(Interop.bSucceeded && Interop.Value == 2 && First->InteropCounts.Num() == 1 &&
               First->InteropLocked.IsEmpty(),
           TEXT("map_interop_ref_keys_and_atomic_failures"), Interop.Error);
    auto TimeMap = [&](bool Erased, int32 N)
    {
        const FString Prefix = Erased ? TEXT("__ue_self_map_"): TEXT("__ue_probe_typed_map_");
        const FString Body = FString::Printf(
            TEXT("for(i in range(%d)) { %sset_int(\"InteropCounts\",3,%sget_int(\"InteropCounts\",3)+1) }\nreturn 1"),
                 N, *Prefix, *Prefix);
        const double Start = FPlatformTime::Seconds();
        const auto Result = ObjectRun(Body);
        Expect(Result.bSucceeded, TEXT("map_interop_benchmark_runs"), Result.Error);
        return FPlatformTime::Seconds() - Start;
    };
    constexpr int32 N = 200000;
    const double MapTypedNs = FMath::Max(0.0, TimeMap(false, N + 1) - TimeMap(false, 1)) * 1e9 / (2 * N);
    const double MapInteropNs = FMath::Max(0.0, TimeMap(true, N + 1) - TimeMap(true, 1)) * 1e9 / (2 * N);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_MAP_INTEROP_BENCH typed_ns=%.2f interop_ns=%.2f ratio=%.3f"), MapTypedNs, MapInteropNs,
                MapTypedNs > 0 ? MapInteropNs / MapTypedNs : 0);
}

{
    const auto Inventory = ObjectRun(TEXT("return __ue_probe_map_binding_count()"));
    const int32 MapBindings = Inventory.Value;
    const bool Legacy = !Inventory.bSucceeded;
    Expect(MapBindings == 56 && !Legacy, TEXT("map_interop_linear_binding_inventory"));
    UE_LOG(LogTemp, Display, TEXT("DAS_MAP_INTEROP_BINDINGS legacy=196 current=%d"), MapBindings);
}
