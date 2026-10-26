// Included in DasRuntime.cpp: the A3 breakdown times the steps of RunCallback with the runtime's own internals
// (sessions, source snapshots, contract checks) without exposing them publicly.
namespace DasInterop
{
// Not inlined, so the loop really calls the function.
FORCENOINLINE int32 DirectAddHealth(UDasObjectFixture* Fixture, int32 Value)
{
    return Fixture->AddHealth(Value);
}
double Percentile(TArray<double> Values, double P)
{
    Values.Sort();
    return Values[FMath::Clamp(FMath::CeilToInt(Values.Num() * P) - 1, 0, Values.Num() - 1)];
}
double Median(TArray<double> Values)
{
    Values.Sort();
    return (Values[(Values.Num() - 1) / 2] + Values[Values.Num() / 2]) * .5;
}
int64 Triangle(int32 N)
{
    return int64(N) * (N + 1) / 2;
}
// Objects for interop_aot.das (mode 6): two actors outside any world, made by the probe.
TWeakObjectPtr<AActor> ObjectsActor, ObjectsOwner;
AActor* InteropActor()
{
    return ObjectsActor.Get();
}
AActor* InteropOwner()
{
    return ObjectsOwner.Get();
}
}
DAS_NATIVE_BINDS(InteropObjects, "interop_objects")
{
    Binds.Function<&DasInterop::InteropActor>(TEXT("interop_actor"));
    Binds.Function<&DasInterop::InteropOwner>(TEXT("interop_owner"));
}

void FDasInteropBench::Profile(const FString& Name, int32 Count, TFunctionRef<int64(int32)> Body)
{
    FDasInteropRow* Row = Rows.FindByPredicate([&](const FDasInteropRow& R) { return R.Name == Name; });
    if (!Row)
        return; // The measurement failed and was reported.
    auto& State = DasCallPhaseState();
    State = FDasCallPhaseState();
    State.bEnabled = true;
    const double Start = FPlatformTime::Seconds();
    Body(Count);
    Row->ProfiledNs = (FPlatformTime::Seconds() - Start) * 1e9 / Count;
    State.bEnabled = false;
    for (int32 I = 0; I < static_cast<int32>(EDasCallPhase::Count); ++I)
        if (State.Cycles[I])
            Row->PhaseNs.Emplace(DasCallPhaseName(static_cast<EDasCallPhase>(I)),
                                 FPlatformTime::ToSeconds64(State.Cycles[I]) * 1e9 / Count);
}
void FDasInteropBench::MeasurePair(const FString& Name, const FString& NameA, const FString& NameB,
                                   const FString& Group, int32 Count, TFunctionRef<int64(bool, int32)> Run,
                                   int64 Expected)
{
    const FString FailureBefore = Failure;
    auto Check = [&](bool bB, int64 Got)
    {
        if (Got != Expected)
            Fail(FString::Printf(TEXT("%s: checksum %lld, expected %lld"), bB ? *NameB : *NameA, Got, Expected));
        return Got == Expected;
    };
    for (int32 Warm = 0; Warm < 2; ++Warm)
        for (const bool bB : {false, true})
        {
            Run(bB, 0);
            if (!Check(bB, Run(bB, Count)))
                return;
        }
    FDasInteropRow Row[2] = {{NameA, Group, Count}, {NameB, Group, Count}};
    FDasInteropPair Pair{Name, NameA, NameB};
    for (int32 Round = 0; Round < Rounds; ++Round)
    {
        double Ns[2] = {};
        for (const bool bB : {Round % 2 == 1, Round % 2 == 0})
        {
            const FString Region = FString::Printf(TEXT("DasInterop/%s/round_%d"), bB ? *NameB : *NameA, Round);
            TRACE_CPUPROFILER_EVENT_SCOPE_TEXT(*Region);
            double Start = FPlatformTime::Seconds();
            const int64 Got = Run(bB, Count);
            const double FullSeconds = FPlatformTime::Seconds() - Start;
            Start = FPlatformTime::Seconds();
            Run(bB, 0);
            const double ZeroSeconds = FPlatformTime::Seconds() - Start;
            if (!Check(bB, Got))
                return;
            Ns[bB] = (FullSeconds - ZeroSeconds) * 1e9 / Count;
            Row[bB].SampleNs.Add(Ns[bB]);
        }
        if (Ns[0] > 0)
            Pair.Ratios.Add(Ns[1] / Ns[0]);
    }
    if (Failure != FailureBefore)
        return;
    for (FDasInteropRow& R : Row)
    {
        R.MedianNs = DasInterop::Median(R.SampleNs);
        R.P95Ns = DasInterop::Percentile(R.SampleNs, .95);
        Rows.Add(MoveTemp(R));
    }
    if (!Pair.Ratios.IsEmpty())
    {
        Pair.RatioMedian = DasInterop::Median(Pair.Ratios);
        Pair.RatioP05 = DasInterop::Percentile(Pair.Ratios, .05);
        Pair.RatioP95 = DasInterop::Percentile(Pair.Ratios, .95);
    }
    Pairs.Add(MoveTemp(Pair));
}
void FDasInteropBench::Fail(const FString& Reason)
{
    bPassed = false;
    Failure += (Failure.IsEmpty() ? TEXT("") : TEXT("; ")) + Reason;
}
void FDasInteropBench::Measure(const FString& Name, const FString& Group, int32 Count, TFunctionRef<int64(int32)> Body,
                               int64 Expected)
{
    MeasureDifference(Name, Group, Count, Body, []() { return int64(0); }, Expected);
}
void FDasInteropBench::MeasureDifference(const FString& Name, const FString& Group, int32 Count,
                                         TFunctionRef<int64(int32)> Full, TFunctionRef<int64()> Zero, int64 Expected)
{
    const FString FailureBefore = Failure;
    for (int32 Warm = 0; Warm < 2; ++Warm)
    {
        Zero();
        if (const int64 Got = Full(Count); Got != Expected)
        {
            Fail(FString::Printf(TEXT("%s: checksum %lld, expected %lld"), *Name, Got, Expected));
            return;
        }
    }
    FDasInteropRow Row{Name, Group, Count};
    for (int32 Sample = 0; Sample < Samples; ++Sample)
    {
        const FString Region = FString::Printf(TEXT("DasInterop/%s/sample_%d"), *Name, Sample);
        TRACE_CPUPROFILER_EVENT_SCOPE_TEXT(*Region);
        double Start = FPlatformTime::Seconds();
        const int64 Got = Full(Count);
        const double FullSeconds = FPlatformTime::Seconds() - Start;
        Start = FPlatformTime::Seconds();
        Zero();
        const double ZeroSeconds = FPlatformTime::Seconds() - Start;
        if (Got != Expected)
        {
            Fail(FString::Printf(TEXT("%s: checksum %lld, expected %lld"), *Name, Got, Expected));
            return;
        }
        Row.SampleNs.Add((FullSeconds - ZeroSeconds) * 1e9 / Count);
    }
    if (Failure != FailureBefore)
        return;
    Row.MedianNs = DasInterop::Median(Row.SampleNs);
    Row.P95Ns = DasInterop::Percentile(Row.SampleNs, .95);
    Rows.Add(MoveTemp(Row));
}
bool FDasInteropBench::Finish(FString& Report, FString& Json, const FString& Scope) const
{
    auto Root = MakeShared<FJsonObject>();
    Root->SetStringField(TEXT("result"), bPassed ? TEXT("PASS") : TEXT("FAIL"));
    Root->SetStringField(TEXT("error"), Failure);
    Root->SetStringField(TEXT("engine_version"), FEngineVersion::Current().ToString());
    Root->SetStringField(TEXT("timestamp_utc"), FDateTime::UtcNow().ToIso8601());
    Root->SetStringField(TEXT("build"), LexToString(FApp::GetBuildConfiguration()));
    Root->SetBoolField(TEXT("editor"), WITH_EDITOR != 0);
    Root->SetStringField(TEXT("scope"), Scope);
    Root->SetStringField(TEXT("unit"), TEXT("nanoseconds per call; script loops include 1/calls of one dispatch"));
    Root->SetNumberField(TEXT("sample_count"), Samples);
    TArray<TSharedPtr<FJsonValue>> Cases;
    for (const auto& Row : Rows)
    {
        auto O = MakeShared<FJsonObject>();
        O->SetStringField(TEXT("case"), Row.Name);
        O->SetStringField(TEXT("group"), Row.Group);
        O->SetNumberField(TEXT("calls_per_sample"), Row.Calls);
        O->SetNumberField(TEXT("median_ns_per_call"), Row.MedianNs);
        O->SetNumberField(TEXT("p95_ns_per_call"), Row.P95Ns);
        TArray<TSharedPtr<FJsonValue>> SamplesNs;
        for (double V : Row.SampleNs)
            SamplesNs.Add(MakeShared<FJsonValueNumber>(V));
        O->SetArrayField(TEXT("samples_ns_per_call"), SamplesNs);
        if (!Row.PhaseNs.IsEmpty())
        {
            auto Phases = MakeShared<FJsonObject>();
            for (const auto& Phase : Row.PhaseNs)
                Phases->SetNumberField(Phase.Key, Phase.Value);
            O->SetObjectField(TEXT("phases_ns_per_call"), Phases);
            O->SetNumberField(TEXT("profiled_ns_per_call"), Row.ProfiledNs);
        }
        Cases.Add(MakeShared<FJsonValueObject>(O));
        Report += FString::Printf(TEXT("INTEROP %-40s ns_per_call=%12.1f p95=%12.1f  [%s]\n"), *Row.Name, Row.MedianNs,
                                       Row.P95Ns, *Row.Group);
        for (const auto& Phase : Row.PhaseNs)
            Report += FString::Printf(TEXT("PHASE   %-40s %-20s %12.1f\n"), *Row.Name, *Phase.Key, Phase.Value);
    }
    Root->SetArrayField(TEXT("cases"), Cases);
    // B against A: faster or slower when the whole p5..p95 range of the round ratios is on one side of 1.
    Root->SetNumberField(TEXT("pair_rounds"), Rounds);
    TArray<TSharedPtr<FJsonValue>> PairValues;
    for (const auto& Pair : Pairs)
    {
        const TCHAR* Verdict = Pair.RatioP95 < 1   ? TEXT("faster")
                               : Pair.RatioP05 > 1 ? TEXT("slower")
                                                   : TEXT("same");
        auto O = MakeShared<FJsonObject>();
        O->SetStringField(TEXT("pair"), Pair.Name);
        O->SetStringField(TEXT("a"), Pair.NameA);
        O->SetStringField(TEXT("b"), Pair.NameB);
        O->SetNumberField(TEXT("ratio_median"), Pair.RatioMedian);
        O->SetNumberField(TEXT("ratio_p05"), Pair.RatioP05);
        O->SetNumberField(TEXT("ratio_p95"), Pair.RatioP95);
        O->SetStringField(TEXT("b_against_a"), Verdict);
        TArray<TSharedPtr<FJsonValue>> RatioValues;
        for (double V : Pair.Ratios)
            RatioValues.Add(MakeShared<FJsonValueNumber>(V));
        O->SetArrayField(TEXT("ratios"), RatioValues);
        PairValues.Add(MakeShared<FJsonValueObject>(O));
        Report += FString::Printf(TEXT("INTEROP_AB %-24s ratio=%6.3f [%6.3f..%6.3f] %s\n"), *Pair.Name,
                                  Pair.RatioMedian, Pair.RatioP05, Pair.RatioP95, Verdict);
    }
    Root->SetArrayField(TEXT("pairs"), PairValues);
    FJsonSerializer::Serialize(Root, TJsonWriterFactory<>::Create(&Json));
    Report += TEXT("DAS_INTEROP_PROBE=") + (bPassed ? FString(TEXT("PASS")) : TEXT("FAIL ") + Failure);
    return bPassed;
}

FDasScriptChangeCheckScope::FDasScriptChangeCheckScope(int32 Mode, float PollSeconds)
    : OldMode(CVarScriptChangeCheck.GetValueOnGameThread()), OldPoll(CVarScriptChangePollSeconds.GetValueOnGameThread())
{
    CVarScriptChangeCheck->Set(Mode, ECVF_SetByCode);
    CVarScriptChangePollSeconds->Set(PollSeconds, ECVF_SetByCode);
}
FDasScriptChangeCheckScope::~FDasScriptChangeCheckScope()
{
    CVarScriptChangeCheck->Set(OldMode, ECVF_SetByCode);
    CVarScriptChangePollSeconds->Set(OldPoll, ECVF_SetByCode);
}

namespace DasInterop
{
#if WITH_EDITOR
// ScriptPath cache: a project script shadows the plugin script of the same name once the folder change is
// reported, and stops shadowing after its removal is reported.
void CheckScriptPathCache(FDasInteropBench& B, FDasRuntimeModule& Runtime)
{
    FDasScriptChangeCheckScope Poll(1, 1e6f);
    const FString Name = TEXT("interop_shadow_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das");
    const FString Plugin = Runtime.ScriptPath(Name);
    const FString ProjectFolder = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Das/Scripts"));
    const FString Project = ProjectFolder / Name;
    const bool bNewFolder = !IFileManager::Get().DirectoryExists(*ProjectFolder);
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Project);
        if (bNewFolder)
            IFileManager::Get().DeleteDirectory(*ProjectFolder, false, true);
    };
    // Written and deleted through the engine: the write tracker reports both, no NotifyScriptsChanged needed.
    if (!FFileHelper::SaveStringToFile(TEXT("options gen2\n"), *Project))
        return B.Fail(TEXT("script_path_cache: cannot write ") + Project);
    const FString Added = Runtime.ScriptPath(Name);
    IFileManager::Get().Delete(*Project);
    const FString Removed = Runtime.ScriptPath(Name);
    if (!FPaths::IsSamePath(Added, Project) || Removed != Plugin)
        B.Fail(FString::Printf(TEXT("script_path_cache: plugin %s, added %s, removed %s"), *Plugin, *Added, *Removed));
}
#endif
// Watch mode keeps hot reload: an edit is invisible until it is reported (or the poll interval passes), runs
// right after the report, and a report without a real edit does not recompile.
void CheckWatchReload(FDasInteropBench& B, FDasRuntimeModule& Runtime, UObject* Self)
{
    const FString Path =
        Runtime.ScriptPath(TEXT("interop_watch_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das"));
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path);
    };
    auto Text = [](int32 Value)
    {
        return FString::Printf(TEXT("options gen2\n[export]\ndef probe(value:int):int { return %d }\n"), Value);
    };
    FDasScriptChangeCheckScope Watch(1, 1e6f);
    auto Call = [&]()
    {
        return Runtime.RunNamedPersistent(Path, TEXT("probe"), 0, Self);
    };
    if (!FFileHelper::SaveStringToFile(Text(1), *Path))
        return B.Fail(TEXT("watch_reload: cannot write the probe script"));
    const auto First = Call();
    // A write in this process: reported by the write tracker, the next call runs the new code.
    FFileHelper::SaveStringToFile(Text(2), *Path);
    const auto Written = Call();
    // The same bytes again: compared, not recompiled.
    FFileHelper::SaveStringToFile(Text(2), *Path);
    const auto Same = Call();
    // Another program's edit: written below the tracking layer, noticed by the poll only.
    {
        TUniquePtr<IFileHandle> Handle(FPlatformFileManager::Get().GetPlatformPhysical().OpenWrite(*Path));
        const FTCHARToUTF8 Utf8(*Text(3));
        if (!Handle || !Handle->Write(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length()))
            return B.Fail(TEXT("watch_reload: cannot write below the tracking layer"));
    }
    const auto Unpolled = Call();
    FDasScriptChangeCheckScope Poll(1, 0.f);
    const auto Polled = Call();
    // A deleted source: the reload is attempted and rejected, the live code stays.
    IFileManager::Get().Delete(*Path);
    const auto Deleted = Call();
    if (!First.bSucceeded || First.Value != 1 || !Written.bSucceeded || Written.Value != 2 ||
        Written.Revision != First.Revision + 1 || !Same.bSucceeded || Same.Revision != Written.Revision ||
        !Unpolled.bSucceeded || Unpolled.Value != 2 || !Polled.bSucceeded || Polled.Value != 3 ||
        Polled.Revision != Written.Revision + 1 || !Deleted.bSucceeded || Deleted.Value != 3 ||
        Deleted.ReloadError.IsEmpty())
        B.Fail(FString::Printf(TEXT("watch_reload: values %d/%d/%d/%d/%d/%d, revisions %u/%u/%u/%u/%u: %s %s"),
                                    First.Value, Written.Value, Same.Value, Unpolled.Value, Polled.Value, Deleted.Value,
                                    First.Revision, Written.Revision, Same.Revision, Polled.Revision, Deleted.Revision,
                                    *First.Error, *Written.Error));
}
// A script compiled once and called many times (C3): RunFile compiles on every call, and the compile time varies
// more than a short loop costs. Prepare with bUseAot links the program's AOT code; Entry is an exported
// function(input:int):int.
struct FPreparedMain
{
    FCompiledContext Live;
    bool bAot = false; // main runs compiled code
    FString Error;
    bool Prepare(const FString& Path, bool bUseAot, UObject* Owner, const char* Entry = "main")
    {
        das::TextWriter Output;
        auto Access = das::make_smart<FUnrealFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        Live.Access = Access;
        const FTCHARToUTF8 File(*Path);
        das::CodeOfPolicies Policies;
        Policies.aot = bUseAot;
        Live.Program = DSLCompile(File.Get(), Live.Access, Output, Live.Modules, Policies);
        if (!Live.Program || Live.Program->failed())
        {
            Error = Errors(Live.Program, Output);
            return false;
        }
        Live.Context = MakeUnique<FUnrealDasContext>(Live.Program->getContextStackSize());
        Live.Context->Self = Owner;
        FActiveContextScope Scope(*Live.Context);
        if (!Live.Program->simulate(*Live.Context, Output))
        {
            Error = Errors(Live.Program, Output);
            return false;
        }
        Live.Function = Live.Context->findFunction(Entry);
        if (!Live.Function)
        {
            Error = FString::Printf(TEXT("expected exported %hs(input:int):int"), Entry);
            return false;
        }
        bAot = Live.Function->aot;
        return true;
    }
    // main(Input) into Value; false with Error on a script error.
    bool Run(int32 Input, int64& Value)
    {
        auto& C = *Live.Context;
        C.restart();
        {
            TGuardValue<int32> Depth(C.ExecutionDepth, 1);
            FActiveContextScope Scope(C);
            vec4f Args[] = {das::cast<int32>::from(Input)};
            const vec4f Result = C.evalWithCatch(Live.Function, Args);
            if (const char* Exception = C.getException())
            {
                Error = UTF8_TO_TCHAR(Exception);
                return false;
            }
            Value = das::cast<int32>::to(Result);
        }
        C.restartHeaps(); // the run's strings: interop_aot.das has no globals that keep them
        return true;
    }
};
}

void RunDasInteropProbe(FDasInteropBench& B)
{
    using namespace DasInterop;
    if (!IsInGameThread() || ActiveContext)
    {
        B.Fail(TEXT("Interop benchmark requires the game thread outside script execution"));
        return;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Script = Runtime.ScriptPath(TEXT("interop_probe.das")),
                                              Aot = Runtime.ScriptPath(TEXT("interop_aot.das"));
    TStrongObjectPtr<UDasObjectFixture> Owner(NewObject<UDasObjectFixture>());
    UDasObjectFixture* F = Owner.Get();
    F->health = 7;
    ON_SCOPE_EXIT
    {
        Runtime.ReleaseObject(F);
    };
    // Per-sample call counts: enough work per sample to stay well above timer resolution.
    // AotCalls: iterations of a C3 sample; shorter rounds, more of them (FDasInteropBench::Rounds). Not a power of
    // two: XOR checksums of I & 7 or of I over 2^k iterations cancel to 0 and would not check the loop ran.
    constexpr int32 Direct = 1 << 20, Light = 1 << 14, Heavy = 256, AotCalls = (1 << 20) + 7;

    // A1. C++ -> C++: the floor for any call into gameplay code.
    B.Measure(
        TEXT("cpp_direct_call"), TEXT("A1 C++ -> C++"), Direct,
                                      [&](int32 N)
                                      {
                                          int64 S = 0;
                                          for (int32 I = 0; I < N; ++I)
                                              S += DirectAddHealth(F, 0);
                                          return S;
                                      },
                                      7ll * Direct);
    UFunction* AddHealth = F->FindFunction(TEXT("AddHealth"));
    auto* Input = CastField<FIntProperty>(AddHealth->FindPropertyByName(TEXT("Value")));
    auto* Return = CastField<FIntProperty>(AddHealth->GetReturnProperty());
    {
        FStructOnScope Params(AddHealth);
        B.Measure(
            TEXT("cpp_processevent_cached"),
                 TEXT("A1 C++ -> C++"), Direct,
                      [&](int32 N)
                      {
                          int64 S = 0;
                          for (int32 I = 0; I < N; ++I)
                          {
                              Input->SetPropertyValue_InContainer(Params.GetStructMemory(), 0);
                              F->ProcessEvent(AddHealth, Params.GetStructMemory());
                              S += Return->GetPropertyValue_InContainer(Params.GetStructMemory());
                          }
                          return S;
                      },
                      7ll * Direct);
    }
    B.Measure(
        TEXT("cpp_processevent_lookup"),
             TEXT("A1 C++ -> C++"), Light,
                  [&](int32 N)
                  {
                      int64 S = 0;
                      for (int32 I = 0; I < N; ++I)
                      {
                          UFunction* Fn = F->FindFunction(TEXT("AddHealth"));
                          FStructOnScope Params(Fn);
                          Input->SetPropertyValue_InContainer(Params.GetStructMemory(), 0);
                          F->ProcessEvent(Fn, Params.GetStructMemory());
                          S += Return->GetPropertyValue_InContainer(Params.GetStructMemory());
                      }
                      return S;
                  },
                  7ll * Light);

    // A2. C++ -> script through the public persistent API (one int in, one int out), with the source check
    // on every call (das.ScriptChangeCheck 0: commandlets, packaged games) and with a file watcher (1: editor).
    auto ScalarCalls = [&](const TCHAR* Case, int32 N)
    {
        int64 S = 0;
        for (int32 I = 0; I < N; ++I)
        {
            DasCallPhaseBegin();
            const auto R = Runtime.RunNamedPersistent(Script, TEXT("scalar"), I, F);
            DasCallPhaseMark(EDasCallPhase::Exit);
            if (!R.bSucceeded)
            {
                B.Fail(FString(Case) + TEXT(": ") + R.Error);
                return int64(-1);
            }
            S += R.Value;
        }
        return S;
    };
    {
        FDasScriptChangeCheckScope Always(0);
        B.Measure(
            TEXT("cpp_to_script_scalar_check_always"),
                 TEXT("A2 C++ -> script"), Heavy, [&](int32 N)
                      { return ScalarCalls(TEXT("cpp_to_script_scalar_check_always"), N); }, Triangle(Heavy));
    }
    {
        // The default: watch mode everywhere (writes in this process are reported by the write tracker).
        FDasScriptChangeCheckScope Default(-1);
        B.Measure(
            TEXT("cpp_to_script_scalar_check_default"),
                 TEXT("A2 C++ -> script"), Light, [&](int32 N)
                      { return ScalarCalls(TEXT("cpp_to_script_scalar_check_default"), N); }, Triangle(Light));
    }
    {
        FDasScriptChangeCheckScope Watch(1);
        const TCHAR* Case = TEXT("cpp_to_script_scalar_check_watch");
        B.Measure(
            Case, TEXT("A2 C++ -> script"), Light, [&](int32 N) { return ScalarCalls(Case, N); }, Triangle(Light));
        B.Profile(Case, Light, [&](int32 N) { return ScalarCalls(Case, N); });
        // The floor of RunCallback: no argument, no result.
        const TCHAR* Void = TEXT("cpp_to_script_void_check_watch");
        auto VoidCalls = [&](int32 N)
        {
            int64 S = 0;
            for (int32 I = 0; I < N; ++I)
            {
                DasCallPhaseBegin();
                const auto R = Runtime.RunNamedVoidPersistent(Script, TEXT("nothing"), F);
                DasCallPhaseMark(EDasCallPhase::Exit);
                if (!R.bSucceeded)
                {
                    B.Fail(FString(Void) + TEXT(": ") + R.Error);
                    return int64(-1);
                }
                ++S;
            }
            return S;
        };
        B.Measure(Void, TEXT("A2 C++ -> script"), Light, VoidCalls, Light);
        B.Profile(Void, Light, VoidCalls);
    }
    // Suspects of the bridge and of every call, on their own.
    B.Measure(
        TEXT("step_script_path_lookup"), TEXT("A3 C++ -> script, per step"), Light,
                                              [&](int32 N)
                                              {
                                                  int64 S = 0;
                                                  for (int32 I = 0; I < N; ++I)
                                                      S += Runtime.ScriptPath(TEXT("interop_probe.das")).Len();
                                                  return S;
                                              },
                                              int64(Light) * Script.Len());
    B.Measure(
        TEXT("step_prune_sessions"), TEXT("A3 C++ -> script, per step"), Light,
                                          [&](int32 N)
                                          {
                                              int64 S = 0;
                                              for (int32 I = 0; I < N; ++I)
                                              {
                                                  PruneSessions();
                                                  S += Sessions.Num();
                                              }
                                              return S;
                                          },
                                          int64(Light) * Sessions.Num());
    CheckWatchReload(B, Runtime, F);
#if WITH_EDITOR
    CheckScriptPathCache(B, Runtime);
#endif

    // A3. The steps of that call, each on its own.
    FString Full = FPaths::ConvertRelativePathToFull(Script);
    FPaths::NormalizeFilename(Full);
    FPaths::CollapseRelativeDirectories(Full);
    FPersistentSession* Session = nullptr;
    for (const auto& Existing : Sessions)
        if (Existing->Owner.Get() == F && Existing->Path == Full)
            Session = Existing.Get();
    if (!Session || !Session->Live)
        B.Fail(TEXT("A3: no live session after cpp_to_script_scalar"));
    else
    {
        B.Measure(
            TEXT("step_source_change_check"), TEXT("A3 C++ -> script, per step"), Heavy,
                                                   [&](int32 N)
                                                   {
                                                       int64 S = 0;
                                                       for (int32 I = 0; I < N; ++I)
                                                           for (const auto& Source : Session->AttemptedSources)
                                                               S += Source.Value.Matches(Source.Key) ? 1 : 0;
                                                       return S;
                                                   },
                                                   int64(Heavy) * Session->AttemptedSources.Num());
        B.Measure(
            TEXT("step_path_normalize"), TEXT("A3 C++ -> script, per step"), Light,
                                              [&](int32 N)
                                              {
                                                  int64 S = 0;
                                                  for (int32 I = 0; I < N; ++I)
                                                  {
                                                      FString P = FPaths::ConvertRelativePathToFull(Script);
                                                      FPaths::NormalizeFilename(P);
                                                      FPaths::CollapseRelativeDirectories(P);
                                                      S += P.Len();
                                                  }
                                                  return S;
                                              },
                                              int64(Light) * Full.Len());
        B.Measure(
            TEXT("step_session_lookup"), TEXT("A3 C++ -> script, per step"), Light,
                                              [&](int32 N)
                                              {
                                                  int64 S = 0;
                                                  for (int32 I = 0; I < N; ++I)
                                                      for (const auto& Existing : Sessions)
                                                          if (Existing->Owner.Get() == F && Existing->Path == Full)
                                                          {
                                                              ++S;
                                                              break;
                                                          }
                                                  return S;
                                              },
                                              Light);
        auto& Context = *Session->Live->Context;
        const FDasCallContract Contract = FDasCallContract::Scalar(EDasMethodSignature::Int);
        B.Measure(
            TEXT("step_find_and_verify_entry"), TEXT("A3 C++ -> script, per step"), Light,
                                                     [&](int32 N)
                                                     {
                                                         int64 S = 0;
                                                         for (int32 I = 0; I < N; ++I)
                                                             S += Context.findFunction("scalar") &&
                                                                  VerifyContract(Session->Live->Program, TEXT("scalar"),
                                                                                                              Contract);
                                                         return S;
                                                     },
                                                     Light);
        das::SimFunction* Scalar = Context.findFunction("scalar");
        B.Measure(
            TEXT("step_vm_eval"), TEXT("A3 C++ -> script, per step"), Light,
                                       [&](int32 N)
                                       {
                                           int64 S = 0;
                                           for (int32 I = 0; I < N; ++I)
                                           {
                                               vec4f Arguments[] = {das::cast<int32>::from(I)};
                                               S += das::cast<int32>::to(Context.evalWithCatch(Scalar, Arguments));
                                           }
                                           return Context.getException() ? int64(-1) : S;
                                       },
                                       Triangle(Light));
    }

    // B, C, D. One script entry per case; the loop runs inside the script, so one dispatch per sample.
    auto Loop = [&](const FString& File, const TCHAR* Entry, const TCHAR* Group, int64 Expected, bool bProfile = false)
    {
        FDasScriptChangeCheckScope Watch(1);
        auto Run = [&](int32 N)
        {
            DasCallPhaseBegin();
            const auto R = Runtime.RunNamedPersistent(File, Entry, N, F);
            if (!R.bSucceeded)
            {
                B.Fail(FString(Entry) + TEXT(": ") + R.Error);
                return int64(-1);
            }
            return int64(R.Value);
        };
        B.Measure(Entry, Group, Light, Run, Expected);
        // The loop runs inside the script: each of the N calls is profiled, the one outer dispatch adds 1/N.
        if (bProfile)
            B.Profile(Entry, Light, Run);
    };
    int64 Masked = 0;
    for (int32 I = 0; I < Light; ++I)
        Masked += I & 7;
    Loop(Script, TEXT("script_loop"), TEXT("C script -> script"), Masked);
    Loop(Script, TEXT("script_to_script_call"), TEXT("C script -> script"), Triangle(Light));
    Loop(Script, TEXT("script_to_extern"), TEXT("B script -> C++"), Triangle(Light));
    Loop(Script, TEXT("script_call_int_reflected"), TEXT("B script -> C++"), 7ll * Light);
    Loop(Script, TEXT("script_generated_object_call"), TEXT("B script -> C++"), 7ll * Light, true);
    Loop(Script, TEXT("script_generated_static_int"), TEXT("B script -> C++"), Triangle(Light), true);
    Loop(Script, TEXT("script_generated_static_vector"), TEXT("B script -> C++"), 5ll * Light);
    Loop(Script, TEXT("script_self_field_read_write"), TEXT("D properties"), 7ll * Light);
    Loop(Script, TEXT("script_object_field_read"), TEXT("D properties"), 7ll * Light);

    // D3. C++ reading a property: cached FProperty against the binding codec.
    auto* Health = FindFProperty<FIntProperty>(UDasObjectFixture::StaticClass(), TEXT("health"));
    B.Measure(
        TEXT("cpp_property_read_cached"), TEXT("D properties"), Direct,
                                               [&](int32 N)
                                               {
                                                   int64 S = 0;
                                                   for (int32 I = 0; I < N; ++I)
                                                       S += Health->GetPropertyValue_InContainer(F);
                                                   return S;
                                               },
                                               7ll * Direct);
    B.Measure(
        TEXT("cpp_property_read_codec"),
             TEXT("D properties"), Light,
                  [&](int32 N)
                  {
                      int64 S = 0;
                      FDasScalarValue Out;
                      FString Error;
                      for (int32 I = 0; I < N; ++I)
                          S += ReadDasPropertyValue(Health, Health->ContainerPtrToValuePtr<void>(F), Out, Error)
                                   ? Out.IntValue
                                   : -1;
                      return S;
                  },
                  7ll * Light);

    // C3. The same script loops compiled for AOT and for the interpreter. RunFile compiles on every call, so a
    // zero-iteration run is subtracted. Modes: 0 loop, 1 script call, 2 self field read, 3 scalar extern, 4 native
    // bind (DasNativeBinds.h, ue_time: days_in_year), 5 native binds with engine values (six calls an iteration),
    // 6 native binds with objects (five calls an iteration), 9 output parameters (three calls an iteration),
    // 10 vector arithmetic through ue_foundation's externs (five calls an iteration), 11 the same on float3, 12 ==
    // of a class type and an FName parameter (two calls an iteration), 13 an array result.
    TStrongObjectPtr<AActor> ActorPin(NewObject<AActor>(GetTransientPackage()));
    TStrongObjectPtr<AActor> OwnerPin(NewObject<AActor>(GetTransientPackage()));
    DasInterop::ObjectsActor = ActorPin.Get();
    DasInterop::ObjectsOwner = OwnerPin.Get();
    struct FAotMode
    {
        const TCHAR* Name;
        int32 Mode;
    };
    const FAotMode AotLoop{TEXT("loop"), 0}, AotCall{TEXT("script_call"), 1}, AotField{TEXT("self_field_read"), 2};
    const FAotMode AotExtern{TEXT("scalar_extern"), 3}, AotNative{TEXT("native_bind"), 4};
    const FAotMode AotValues{TEXT("native_values"), 5}, AotObjects{TEXT("native_objects"), 6};
    const FAotMode AotRefs{TEXT("native_refs"), 9}, AotMath{TEXT("vector_math"), 10};
    const FAotMode AotFloat3{TEXT("float3_math"), 11}, AotNames{TEXT("names_equality"), 12};
    const FAotMode AotArrays{TEXT("object_array"), 13};
    // Both paths compiled once; main is called again for every sample (FPreparedMain).
    FPreparedMain Prepared[2];
    bool bPrepared = true;
    for (const bool bAot : {false, true})
        if (!Prepared[bAot].Prepare(Aot, bAot, F) || Prepared[bAot].bAot != bAot)
        {
            B.Fail(FString::Printf(TEXT("%s: "), bAot ? TEXT("aot") : TEXT("interpreter")) +
                   (Prepared[bAot].Error.IsEmpty() ? FString(TEXT("AOT code for interop_aot.das is not linked; run "
                                                                  "Scripts/TestInteropPerformance.ps1 to generate it"))
                                                   : Prepared[bAot].Error));
            bPrepared = false;
        }
    for (const FAotMode& M : {AotLoop, AotCall, AotField, AotExtern, AotNative, AotValues, AotObjects, AotRefs, AotMath,
                              AotFloat3, AotNames, AotArrays})
    {
        if (!bPrepared)
            break;
        int64 Expected = 0;
        for (int32 I = 0; I < AotCalls; ++I)
        {
            // Mode 3: the state the first call stored (0) plus I. Mode 5: |rotated forward| 1 + day 29 + 24 characters of "2024-02-29T00:00:00.000Z"; mode 6: the
            // owner is valid (1) and casts to AActor (2).
            const int32 Value[] = {I & 7, I + 1, 7 + I, I, 365 + I, 1 + 29 + 24 + I, 3 + I};
            // Mode 9: a vector of length I & 7; modes 10 and 11: of length (I & 7) + 2, times I + 1.
            // Mode 12: the actors differ (1), no tag; mode 13: no components.
            Expected ^= M.Mode == 9    ? (I & 7) + I
                        : M.Mode == 12 ? 1 + I
                        : M.Mode == 13 ? I
                        : M.Mode >= 10 ? ((I & 7) + 2) * (I + 1)
                                       : Value[M.Mode];
        }
        // A = the interpreter, B = AOT, side by side (the ratio is the comparison; see FDasInteropPair).
        auto Run = [&](bool bAot, int32 N)
        {
            int64 Value = 0;
            if (!Prepared[bAot].Run(N * 16 + M.Mode, Value))
            {
                B.Fail(FString::Printf(TEXT("%s_%s: %s"), bAot ? TEXT("aot") : TEXT("interpreter"), M.Name,
                                       *Prepared[bAot].Error));
                return int64(-1);
            }
            return Value;
        };
        B.MeasurePair(M.Name, FString(TEXT("interpreter_")) + M.Name, FString(TEXT("aot_")) + M.Name,
                      TEXT("C3 AOT vs interpreter"), AotCalls, Run, Expected);
    }
    // C4. The parts of a compiled extern call, in a context scope like a script's: the role guard alone, Resolve
    // (checks, guard, the call site's cached entry), Finish, and the bridge as Call runs it (Resolve, then Finish
    // only when an error is pending) without the native function.
    {
        FUnrealDasContext Context(16 * 1024);
        Context.Self = F;
        FActiveContextScope Scope(Context);
        uint64 Key = 0;
        for (const auto& Entry : UEAotEntries)
            if (!Entry.Value.Loaded)
            {
                Key = Entry.Key;
                break;
            }
        B.Measure(
            TEXT("guard_ue_extern"), TEXT("C4 compiled call parts"), Direct,
            [&](int32 N)
            {
                for (int32 I = 0; I < N; ++I)
                    GuardUEExtern();
                return int64(N);
            },
            int64(Direct));
        dasue::aot::FCallSite Site;
        B.Measure(
            TEXT("aot_bridge"), TEXT("C4 compiled call parts"), Direct,
            [&](int32 N)
            {
                int64 S = 0;
                for (int32 I = 0; I < N; ++I)
                {
                    S += dasue::aot::Resolve(Key, &Context, Site) != nullptr;
                    if (dasue::aot::ErrorPending)
                        dasue::aot::Finish(Key, &Context);
                }
                return S;
            },
            int64(Direct));
        B.Measure(
            TEXT("aot_resolve"), TEXT("C4 compiled call parts"), Direct,
            [&](int32 N)
            {
                int64 S = 0;
                for (int32 I = 0; I < N; ++I)
                    S += dasue::aot::Resolve(Key, &Context, Site) != nullptr;
                return S;
            },
            int64(Direct));
        B.Measure(
            TEXT("aot_finish"), TEXT("C4 compiled call parts"), Direct,
            [&](int32 N)
            {
                for (int32 I = 0; I < N; ++I)
                    dasue::aot::Finish(Key, &Context);
                return int64(N);
            },
            int64(Direct));
    }
    // Object checks of compiled calls fail with the interpreter's message (mode 7: a null receiver, mode 8: a null
    // argument of a NonNullArgs bind, mode 14: a routed gameplay call's receiver outside the owner's world); only the
    // location differs, AOT errors have none. Mode 6 above proved that main is compiled (with the route in it).
    for (const int32 Mode : {7, 8, 14})
    {
        FString Errors[2];
        for (const bool bAot : {false, true})
        {
            const auto R = Runtime.RunFile(Aot, Mode, bAot, F);
            FString Text = R.bSucceeded ? FString() : R.Error;
            if (const int32 Bar = Text.Find(TEXT(" | ")); Bar != INDEX_NONE)
                Text.MidInline(Bar + 3);
            if (const int32 End = Text.Find(TEXT("\n")); End != INDEX_NONE)
                Text.LeftInline(End);
            Errors[bAot ? 1 : 0] = Text.TrimEnd();
        }
        if (Errors[0].IsEmpty() || Errors[0] != Errors[1])
            B.Fail(FString::Printf(TEXT("aot_object_error_%d: interpreter '%s', AOT '%s'"), Mode, *Errors[0],
                                   *Errors[1]));
    }
}

void RunDasBlueprintComparison(FDasInteropBench& B, AActor* Target, TFunctionRef<int64(int32, int32)> RunBlueprint)
{
    using namespace DasInterop;
    if (!IsInGameThread() || ActiveContext || !Target)
    {
        B.Fail(TEXT("Blueprint comparison requires the game thread outside script execution and a target actor"));
        return;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Aot = Runtime.ScriptPath(TEXT("interop_aot.das"));
    TStrongObjectPtr<UDasObjectFixture> Owner(NewObject<UDasObjectFixture>());
    Owner->health = 7;
    const TWeakObjectPtr<AActor> SavedActor = ObjectsActor;
    ObjectsActor = Target; // interop_actor() in mode 16
    ON_SCOPE_EXIT
    {
        ObjectsActor = SavedActor;
        Runtime.ReleaseObject(Owner.Get());
    };
    FPreparedMain Prepared[2];
    for (const bool bAot : {false, true})
        if (!Prepared[bAot].Prepare(Aot, bAot, Owner.Get(), "blueprint_workloads") || Prepared[bAot].bAot != bAot)
        {
            B.Fail(FString::Printf(TEXT("blueprint comparison, %s: "), bAot ? TEXT("aot") : TEXT("interpreter")) +
                   (Prepared[bAot].Error.IsEmpty() ? FString(TEXT("AOT code for interop_aot.das is not linked"))
                                                   : Prepared[bAot].Error));
            return;
        }
    // The C++ variant is also the reference for the checksum.
    auto Cpp = [&](int32 Workload, int32 N)
    {
        int64 Sum = 0;
        for (int32 I = 0; I < N; ++I)
            Sum += Workload == 0   ? I % 7
                   : Workload == 1 ? int32(Target->GetActorLocation().X)
                                   : int32((FVector(double(I % 7), 0, 0) + FVector(2, 0, 0)).Size());
        return Sum;
    };
    constexpr int32 Count = 1 << 18;
    const TCHAR* Workloads[] = {TEXT("int_loop"), TEXT("engine_call"), TEXT("vector_math")};
    const TCHAR* Variants[] = {TEXT("blueprint"), TEXT("cpp"), TEXT("interpreter"), TEXT("aot")};
    for (int32 W = 0; W < 3; ++W)
    {
        const int64 Expected = Cpp(W, Count);
        auto Run = [&](int32 V, int32 N) -> int64
        {
            if (V == 0)
                return RunBlueprint(W, N);
            if (V == 1)
                return Cpp(W, N);
            int64 Value = 0;
            if (!Prepared[V == 3].Run(N * 4 + W, Value))
            {
                B.Fail(FString::Printf(TEXT("%s_%s: %s"), Workloads[W], Variants[V], *Prepared[V == 3].Error));
                return -1;
            }
            return Value;
        };
        FDasInteropRow Rows[4];
        FDasInteropPair Pairs[3];
        bool bGood = true;
        for (int32 V = 0; V < 4 && bGood; ++V)
        {
            Rows[V] = {FString::Printf(TEXT("%s_%s"), Workloads[W], Variants[V]),
                       TEXT("F Blueprint against daScript"), Count};
            const int64 Got = Run(V, Count); // warm-up, and the checksum
            bGood = Got == Expected;
            if (!bGood)
                B.Fail(FString::Printf(TEXT("%s: checksum %lld, expected %lld"), *Rows[V].Name, Got, Expected));
        }
        for (int32 V = 1; V < 4; ++V)
            Pairs[V - 1] = {FString::Printf(TEXT("%s_%s_vs_blueprint"), Workloads[W], Variants[V]), Rows[0].Name,
                            Rows[V].Name};
        for (int32 Round = 0; Round < B.Rounds && bGood; ++Round)
        {
            double Ns[4] = {};
            for (int32 K = 0; K < 4 && bGood; ++K)
            {
                const int32 V = Round % 2 == 0 ? K : 3 - K;
                const double Start = FPlatformTime::Seconds();
                const int64 Got = Run(V, Count);
                Ns[V] = (FPlatformTime::Seconds() - Start) * 1e9 / Count;
                Rows[V].SampleNs.Add(Ns[V]);
                bGood = Got == Expected;
                if (!bGood)
                    B.Fail(FString::Printf(TEXT("%s: checksum %lld, expected %lld (round %d)"), *Rows[V].Name, Got,
                                           Expected, Round));
            }
            for (int32 V = 1; V < 4 && bGood; ++V)
                Pairs[V - 1].Ratios.Add(Ns[V] / FMath::Max(Ns[0], 1e-9));
        }
        if (!bGood)
            continue;
        for (FDasInteropRow& Row : Rows)
        {
            Row.MedianNs = Median(Row.SampleNs);
            Row.P95Ns = Percentile(Row.SampleNs, .95);
            B.Rows.Add(MoveTemp(Row));
        }
        for (FDasInteropPair& Pair : Pairs)
        {
            Pair.RatioMedian = Median(Pair.Ratios);
            Pair.RatioP05 = Percentile(Pair.Ratios, .05);
            Pair.RatioP95 = Percentile(Pair.Ratios, .95);
            B.Pairs.Add(MoveTemp(Pair));
        }
    }
}
