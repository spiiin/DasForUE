// Included in DasRuntime.cpp to observe existing per-context counters without
// adding instrumentation to normal execution or exposing VM internals publicly.
namespace DasPerformance
{
struct FCounters
{
    uint64 Allocations = 0, Reallocations = 0, Bytes = 0, LiveBytes = 0, Paths = 0, Lookups = 0, Orders = 0;
    int32 Snapshots = 0;
    SIZE_T SnapshotTableBytes = 0;
};
FUnrealDasContext* FindContext(UObject* Owner)
{
    for (const auto& Session : Sessions)
        if (Session->Owner.Get() == Owner && Session->Live)
            return Session->Live->Context.Get();
    return nullptr;
}
FCounters Read(FUnrealDasContext& C)
{
    FCounters R;
    for (das::AnyHeapAllocator* Heap : {C.heap.get(), static_cast<das::AnyHeapAllocator*>(C.stringHeap.get())})
        if (Heap)
        {
            R.Allocations += Heap->getTotalAllocations();
            R.Reallocations += Heap->getTotalReallocations();
            R.Bytes += Heap->getTotalBytesAllocated();
            R.LiveBytes += Heap->bytesAllocated();
        }
    R.Paths = C.SnapshotFieldPaths.BuildCount();
    R.Lookups = C.SnapshotFieldPaths.LookupCount();
    R.Orders = C.SnapshotKeyOrders.BuildCount();
    R.Snapshots = C.StructValues.Num();
    R.SnapshotTableBytes = C.StructValues.GetAllocatedSize();
    return R;
}
TSharedRef<FJsonObject> ResultRow(const FString& Name, int32 Size, int32 Operations, const TArray<double>& Times,
                                  TArray<TSharedPtr<FJsonValue>>&& Samples)
{
    auto Row = MakeShared<FJsonObject>();
    Row->SetStringField(TEXT("case"), Name);
    Row->SetNumberField(TEXT("container_size"), Size);
    Row->SetNumberField(TEXT("operations_per_sample"), Operations);
    Row->SetStringField(TEXT("unit"), TEXT("work iteration; not an individual UE call"));
    auto Sorted = Times;
    Sorted.Sort();
    const double Median = (Sorted[(Sorted.Num() - 1) / 2] + Sorted[Sorted.Num() / 2]) * .5,
                 P95 = Sorted[FMath::CeilToInt(Sorted.Num() * .95) - 1];
    Row->SetNumberField(TEXT("median_ms"), Median);
    Row->SetNumberField(TEXT("p95_ms"), P95);
    Row->SetNumberField(TEXT("median_ns_per_iteration"), Median * 1000000. / Operations);
    Row->SetArrayField(TEXT("samples"), MoveTemp(Samples));
    return Row;
}
}

bool RunDasPerformanceProbe(FString& Report, FString& Json, int32 Iterations, int32 Samples)
{
    using namespace DasPerformance;
    if (!IsInGameThread() || ActiveContext || Iterations < 8 || Iterations > 256 || Samples < 3 || Samples > 31)
    {
        Report = TEXT("Invalid benchmark scope or bounds");
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    if (Runtime.PersistentContextCount() != 0)
    {
        Report = TEXT("Benchmark requires an isolated process without persistent script contexts");
        return false;
    }
    TArray<TSharedPtr<FJsonValue>> Rows;
    bool Passed = true;
    FString Failure;
    const FString Script = Runtime.ScriptPath(TEXT("performance_probe.das"));
    auto* SnapshotProperty = FindFProperty<FStructProperty>(UDasSnapshotReadOwner::StaticClass(), TEXT("Snapshot"));
    const auto OldFlags = SnapshotProperty->GetPropertyFlags();
    SnapshotProperty->SetPropertyFlags(CPF_BlueprintVisible);
    ON_SCOPE_EXIT
    {
        SnapshotProperty->ClearPropertyFlags(CPF_BlueprintVisible & ~OldFlags);
    };
    for (int32 Size : {16, 256, 1024})
    {
        TStrongObjectPtr<UDasSnapshotReadOwner> Owner(NewObject<UDasSnapshotReadOwner>());
        TStrongObjectPtr<UDasObjectFixture> CallOwner(NewObject<UDasObjectFixture>());
        CallOwner->health = 7;
        ON_SCOPE_EXIT
        {
            Runtime.ReleaseObject(Owner.Get());
            Runtime.ReleaseObject(CallOwner.Get());
        };
        auto& Value = Owner->Snapshot;
        Value.Nested.Priority = 11;
        Value.Rows.Add(Value.Nested);
        for (int32 I = 0; I < Size; ++I)
        {
            Value.IntMap.Add(I, I);
            Value.IntSet.Add(I);
        }
        FInstancedStruct Native;
        Native.InitializeAs<FDasSnapshotReadProbe>(Value);
        FDasSnapshotReadProbe::CopyCount.Store(0);
        {
            FInstancedStruct CopyControl = Native;
            if (FDasSnapshotReadProbe::CopyCount.Load() == 0)
            {
                Passed = false;
                Failure = TEXT("native copy counter positive control failed");
            }
        }
        // Same resolver and storage; only preparation reuse differs. A larger
        // batch avoids timing timer resolution rather than path traversal.
        for (bool Cached : {false, true})
        {
            DasSnapshotFields::FPathCache Cache;
            DasSnapshotFields::TField<const uint8> Field{};
            FString Error;
            uint64 NativeBuilds = 0, NativeLookups = 0;
            auto Access = [&]()
            {
                if (Cached)
                    return Cache.Find(Native.GetScriptStruct(), Native.GetMemory(),
                                      TEXT("Rows.0.Priority"), Field, Error);
                ++NativeBuilds;
                DasSnapshotFields::FPath Path(TEXT("Rows.0.Priority"));
                return DasSnapshotFields::Resolve(Native.GetScriptStruct(), Native.GetMemory(), Path, Field, Error,
                                                  &NativeLookups);
            };
            if (!Access())
            {
                Passed = false;
                Failure = Error;
                break;
            }
            TArray<double> Times;
            TArray<TSharedPtr<FJsonValue>> Measurements;
            const int32 Count = Iterations * 64;
            for (int32 Sample = 0; Sample < Samples; ++Sample)
            {
                const uint64 BeforeBuilds = Cached ? Cache.BuildCount() : NativeBuilds,
                             BeforeLookups = Cached ? Cache.LookupCount() : NativeLookups;
                int32 Checksum = 0;
                const FString Region = FString::Printf(TEXT("DasPerformance/native_path_%s/size_%d/sample_%d"),
                                                            Cached ? TEXT("cached"): TEXT("uncached"), Size, Sample);
                FDasSnapshotReadProbe::CopyCount.Store(0);
                const double Start = FPlatformTime::Seconds();
                {
                    TRACE_CPUPROFILER_EVENT_SCOPE_TEXT(*Region);
                    for (int32 I = 0; I < Count; ++I)
                    {
                        if (!Access())
                        {
                            Passed = false;
                            break;
                        }
                        Checksum += *Field.Memory;
                    }
                }
                const double Milliseconds = (FPlatformTime::Seconds() - Start) * 1000.;
                Times.Add(Milliseconds);
                auto M = MakeShared<FJsonObject>();
                M->SetNumberField(TEXT("elapsed_ms"), Milliseconds);
                M->SetNumberField(TEXT("checksum"), Checksum);
                M->SetNumberField(TEXT("native_root_copies"), FDasSnapshotReadProbe::CopyCount.Load());
                M->SetNumberField(TEXT("path_builds"), (Cached ? Cache.BuildCount() : NativeBuilds) - BeforeBuilds);
                M->SetNumberField(TEXT("field_lookups"),
                                       (Cached ? Cache.LookupCount() : NativeLookups) - BeforeLookups);
                Measurements.Add(MakeShared<FJsonValueObject>(M));
                if (Checksum != Count * 11 || FDasSnapshotReadProbe::CopyCount.Load() != 0 ||
                    (Cached && Cache.LookupCount() != BeforeLookups))
                {
                    Passed = false;
                    Failure = TEXT("native path benchmark checksum/reuse failed");
                }
            }
            Rows.Add(MakeShared<FJsonValueObject>(
                ResultRow(Cached ? TEXT("native_path_cached")
                                 : TEXT("native_path_uncached"), Size, Count, Times, MoveTemp(Measurements))));
        }
        const TCHAR* Cases[] =
            {TEXT("baseline"),    TEXT("read_queries"), TEXT("keys"),
                                                             TEXT("single_edit"), TEXT("four_edits"),   TEXT("batch_four_edits"),
                                                                                                             TEXT("vm_to_ue"),    TEXT("ue_to_vm"),     TEXT("vm_allocations")};
        for (const TCHAR* Name : Cases)
        {
            const FString Case(Name);
            const bool Dispatch =
                           Case == TEXT("ue_to_vm"),
                                        Calls = Dispatch || Case == TEXT("vm_to_ue") || Case == TEXT("vm_allocations");
            UObject* Receiver = Calls ? static_cast<UObject*>(CallOwner.Get()) : Owner.Get();
            const FString Entry = Dispatch ? TEXT("scalar"): Case;
            auto Invoke = [&](int32 Input)
            {
                return Runtime.RunNamedPersistent(Script, Entry, Input, Receiver);
            };
            // Required entry compilation, source imports and cache construction
            // happen before timing. Keep every measured context identical.
            for (int32 Warm = 0; Warm < 2; ++Warm)
            {
                const auto R = Invoke(Iterations);
                if (!R.bSucceeded)
                {
                    Passed = false;
                    Failure = Case + TEXT(": ")+ R.Error;
                    break;
                }
            }
            auto* Context = FindContext(Receiver);
            if (!Passed || !Context)
                break;
            TArray<double> Times;
            TArray<TSharedPtr<FJsonValue>> Measurements;
            for (int32 Sample = 0; Sample < Samples; ++Sample)
            {
                const FString Region =
                    FString::Printf(TEXT("DasPerformance/%s/size_%d/sample_%d"), *Case, Size, Sample);
                const auto Before = Read(*Context);
                FDasSnapshotReadProbe::CopyCount.Store(0);
                const double Start = FPlatformTime::Seconds();
                int32 Checksum = 0;
                bool Successful = true;
                {
                    TRACE_CPUPROFILER_EVENT_SCOPE_TEXT(*Region);
                    if (Dispatch)
                    {
                        for (int32 I = 0; I < Iterations; ++I)
                        {
                            const auto R = Invoke(I);
                            Successful &= R.bSucceeded;
                            Checksum += R.Value;
                            if (!R.bSucceeded)
                                Failure = R.Error;
                        }
                    }
                    else
                    {
                        const auto R = Invoke(Iterations);
                        Successful = R.bSucceeded;
                        Checksum = R.Value;
                        if (!Successful)
                            Failure = R.Error;
                    }
                }
                const double Milliseconds = (FPlatformTime::Seconds() - Start) * 1000.;
                const int32 Copies = FDasSnapshotReadProbe::CopyCount.Load();
                if (FindContext(Receiver) != Context)
                {
                    Passed = false;
                    Failure = TEXT("compiled context changed during measurement");
                    break;
                }
                const auto After = Read(*Context);
                Times.Add(Milliseconds);
                int32 Expected = 0;
                for (int32 I = 0; I < Iterations; ++I)
                {
                    if (Case == TEXT("baseline"))
                        Expected += I & 7;
                    else if (Case == TEXT("read_queries"))
                        Expected += Size + 12;
                    else if (Case == TEXT("keys"))
                        Expected += I % Size;
                    else if (Case == TEXT("single_edit") || Dispatch)
                        Expected += I + 1;
                    else if (Case == TEXT("four_edits") || Case == TEXT("batch_four_edits"))
                        Expected += I + 4;
                    else if (Case == TEXT("vm_to_ue"))
                        Expected += 7;
                }
                if (Case == TEXT("vm_allocations"))
                    Expected = Iterations;
                const int32 ExpectedCopies =
                    Calls ? 0
                          : 1 + ((Case == TEXT("single_edit") || Case == TEXT("batch_four_edits"))
                                  ? Iterations
                                  : Case == TEXT("four_edits")? 4 * Iterations : 0);
                auto M = MakeShared<FJsonObject>();
                M->SetNumberField(TEXT("elapsed_ms"), Milliseconds);
                M->SetNumberField(TEXT("checksum"), Checksum);
                M->SetNumberField(TEXT("native_root_copies"), Copies);
                M->SetNumberField(TEXT("vm_allocations"), After.Allocations - Before.Allocations);
                M->SetNumberField(TEXT("vm_reallocations"), After.Reallocations - Before.Reallocations);
                M->SetNumberField(TEXT("vm_allocated_bytes"), After.Bytes - Before.Bytes);
                M->SetNumberField(TEXT("vm_live_bytes_after"), After.LiveBytes);
                M->SetNumberField(TEXT("snapshots_after"), After.Snapshots);
                M->SetNumberField(TEXT("snapshot_table_bytes_after"), After.SnapshotTableBytes);
                M->SetNumberField(TEXT("path_builds"), After.Paths - Before.Paths);
                M->SetNumberField(TEXT("field_lookups"), After.Lookups - Before.Lookups);
                M->SetNumberField(TEXT("key_order_builds"), After.Orders - Before.Orders);
                const bool Valid = Successful && Checksum == Expected && Copies == ExpectedCopies &&
                                   (Case != TEXT("vm_allocations") || After.Allocations + After.Reallocations >
                                                                          Before.Allocations + Before.Reallocations);
                M->SetBoolField(TEXT("valid"), Valid);
                Measurements.Add(MakeShared<FJsonValueObject>(M));
                if (!Valid)
                {
                    Passed = false;
                    Failure += FString::Printf(TEXT(" %s checksum=%d expected=%d copies=%d expected=%d"), *Case,
                                                    Checksum, Expected, Copies, ExpectedCopies);
                }
            }
            if (!Times.IsEmpty())
                Rows.Add(
                    MakeShared<FJsonValueObject>(ResultRow(Case, Size, Iterations, Times, MoveTemp(Measurements))));
            if (!Passed)
                break;
        }
        if (!Passed)
            break;
    }
    if (Runtime.PersistentContextCount() != 0)
    {
        Passed = false;
        Failure += TEXT(" persistent contexts leaked after benchmark");
    }
    auto Root = MakeShared<FJsonObject>();
    Root->SetStringField(TEXT("result"), Passed ? TEXT("PASS"): TEXT("FAIL"));
    Root->SetStringField(TEXT("error"), Failure);
    Root->SetStringField(TEXT("engine_version"), FEngineVersion::Current().ToString());
    Root->SetStringField(TEXT("timestamp_utc"), FDateTime::UtcNow().ToIso8601());
    Root->SetStringField(TEXT("build"), LexToString(FApp::GetBuildConfiguration()));
    Root->SetBoolField(TEXT("editor"), WITH_EDITOR != 0);
    Root->SetStringField(TEXT("mode"),
        TEXT("interpreter; warmed persistent public API; source checks, boundary cleanup and validation included; compilation excluded"));
    Root->SetBoolField(TEXT("memory_trace_requested"), FParse::Param(FCommandLine::Get(), TEXT("DasPerformanceTrace")));
    Root->SetStringField(TEXT("allocation_scope"),
        TEXT("daScript heap + string heap cumulative allocation/reallocation counters; excludes native UE allocations, FString, property frames and nested snapshot storage"));
    Root->SetStringField(
        TEXT("copy_scope"),
             TEXT("actual FDasSnapshotReadProbe native copy assignments; not all struct copies in the process"));
    Root->SetStringField(
        TEXT("memory_scope"),
             TEXT("after-call live VM bytes and native snapshot table capacity; not peaks or total process memory"));
    Root->SetNumberField(TEXT("iterations"), Iterations);
    Root->SetNumberField(TEXT("sample_count"), Samples);
    Root->SetArrayField(TEXT("cases"), Rows);
    FJsonSerializer::Serialize(Root, TJsonWriterFactory<>::Create(&Json));
    for (const auto& Row : Rows)
    {
        const auto O = Row->AsObject();
        Report += FString::Printf(TEXT("PERF %s size=%d median_ms=%.6f p95_ms=%.6f\n"),
                                       *O->GetStringField(TEXT("case")),
                                                          int32(O->GetNumberField(TEXT("container_size"))),
                                                                O->GetNumberField(TEXT("median_ms")),
                                                                                  O->GetNumberField(TEXT("p95_ms")));
    }
    Report += TEXT("DAS_PERFORMANCE_PROBE=")+ (Passed ? FString(TEXT("PASS")) : FString(TEXT("FAIL ")) + Failure);
    return Passed;
}
