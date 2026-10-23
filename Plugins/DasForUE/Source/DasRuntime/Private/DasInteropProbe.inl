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
}

FDasCallPhaseState& DasCallPhaseState()
{
    static FDasCallPhaseState State;
    return State;
}
const TCHAR* DasCallPhaseName(EDasCallPhase Phase)
{
    // clang-format off
    static const TCHAR* Names[] = {
        TEXT("blueprint_graph"), TEXT("bridge_module"), TEXT("bridge_script_path"), TEXT("entry"), TEXT("frame"),
        TEXT("prune_sessions"), TEXT("session"), TEXT("source_check"), TEXT("reload"), TEXT("prepare_vm"),
        TEXT("function"), TEXT("arguments"), TEXT("eval"), TEXT("result"), TEXT("exit"), TEXT("bridge_result"),
        TEXT("blueprint_return"), TEXT("script_between"), TEXT("authored_contract"), TEXT("authored_frame"),
        TEXT("authored_validate"), TEXT("call_arguments"), TEXT("invoke_checks"), TEXT("invoke_outputs"),
        TEXT("call_begin"), TEXT("call_signature"), TEXT("call_result"), TEXT("call_release")};
    // clang-format on
    static_assert(UE_ARRAY_COUNT(Names) == static_cast<int32>(EDasCallPhase::Count));
    return Names[static_cast<int32>(Phase)];
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
    constexpr int32 Direct = 1 << 20, Light = 1 << 14, Heavy = 256, AotCalls = 1 << 22;

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
    // zero-iteration run is subtracted. Modes: 0 loop, 1 script call, 2 self field read, 3 scalar extern.
    struct FAotMode
    {
        const TCHAR* Name;
        int32 Mode;
    };
    for (const FAotMode& M :
         {FAotMode{TEXT("loop"), 0}, FAotMode{TEXT("script_call"), 1}, FAotMode{TEXT("self_field_read"), 2},
                                                                                FAotMode{TEXT("scalar_extern"), 3}})
    {
        int64 Expected = 0;
        for (int32 I = 0; I < AotCalls; ++I)
            Expected ^= M.Mode == 0 ? (I & 7) : M.Mode == 1 ? I + 1 : M.Mode == 2 ? 7 + I : I;
        for (bool bAot : {false, true})
        {
            const FString Name = FString::Printf(TEXT("%s_%s"), bAot ? TEXT("aot") : TEXT("interpreter"), M.Name);
            auto Run = [&](int32 N)
            {
                const auto R = Runtime.RunFile(Aot, N * 10 + M.Mode, bAot, F);
                if (!R.bSucceeded || R.bUsedAot != bAot)
                {
                    B.Fail(Name + (R.bSucceeded ? TEXT(": AOT code for interop_aot.das is not linked; run "
                                                       "Scripts/TestInteropPerformance.ps1 to generate it")
                                                : TEXT(": ") + R.Error));
                    return int64(-1);
                }
                return int64(R.Value);
            };
            B.MeasureDifference(Name, TEXT("C3 AOT vs interpreter"), AotCalls, Run, [&]() { return Run(0); }, Expected);
        }
    }
}
