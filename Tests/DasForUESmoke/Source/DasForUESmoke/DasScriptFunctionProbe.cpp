// Typed calls of script functions from C++ (DasScript.h, TDasFunction): RunDasScriptFunctionProbe calls
// Content/Das/Scripts/script_function_probe.das with an actor in a game world as the owner, once with the fast path
// (das.ScriptFunctionFastPath 1) and once without it: both must give the same values and errors. Used by the
// automation test DasForUE.ScriptFunction and by the packaged flag -DasNativeBinds.
#include "Components/SceneComponent.h"
#include "DasRuntime.h"
#include "DasScript.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
#include <limits>

// Returns false with the failed checks in Report. World: a game world to spawn into, or nullptr for a temporary one.
bool RunDasScriptFunctionProbe(UWorld* GameWorld, FString& Report)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    bool bPassed = true;
    auto Check = [&](bool bOK, const FString& Name, const FString& Detail = FString())
    {
        bPassed &= bOK;
        Report += FString::Printf(TEXT("%s %s"), bOK ? TEXT("PASS") : TEXT("FAIL"), *Name);
        if (!Detail.IsEmpty()) // one line: script errors span several
            Report += TEXT(": ") + Detail.Replace(TEXT("\r"), TEXT("")).Replace(TEXT("\n"), TEXT(" | "));
        Report += TEXT("\n");
    };
    UWorld* World = GameWorld;
    if (!World)
    {
        World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("DasScriptFunctionProbe"));
        GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
        World->InitializeActorsForPlay(FURL());
    }
    AActor* Owner = World->SpawnActor<AActor>();
    AActor* Other = World->SpawnActor<AActor>();
    auto* Component = NewObject<USceneComponent>(Owner, TEXT("Root"));
    Owner->SetRootComponent(Component);
    Component->RegisterComponent();
    IConsoleVariable* FastPath = IConsoleManager::Get().FindConsoleVariable(TEXT("das.ScriptFunctionFastPath"));
    Check(FastPath != nullptr, TEXT("script_function_fast_path_cvar"));
    if (!FastPath)
        return false;
    const int32 FastPathBefore = FastPath->GetInt();
    const TCHAR* Script = TEXT("script_function_probe.das");

    for (const bool bFast : {true, false})
    {
        FastPath->Set(bFast ? 1 : 0, ECVF_SetByCode);
        const FString Mode = bFast ? TEXT("fast_") : TEXT("full_");
        auto Name = [&](const TCHAR* Case)
        {
            return FString(TEXT("script_function_")) + Mode + Case;
        };
        // Each function is called twice with the same TDasFunction: the first call takes the full path, the
        // second the fast one when it is on. Both must return a value Good accepts.
        auto Twice = [&](const TCHAR* Case, const auto& Function, auto Good, const auto&... Args)
        {
            const auto First = Function.Call(Owner, Args...);
            const uint64 Before = DasScriptFunctionFastCalls();
            const auto Second = Function.Call(Owner, Args...);
            const bool bTookFast = DasScriptFunctionFastCalls() == Before + 1;
            bool bOK = !First.HasError() && !Second.HasError() && bTookFast == bFast;
            if constexpr (!std::is_same_v<std::decay_t<decltype(First)>, TValueOrError<void, FString>>)
                bOK = bOK && Good(First.GetValue()) && Good(Second.GetValue());
            Check(bOK, Name(Case),
                  First.HasError()    ? First.GetError().Left(300)
                  : Second.HasError() ? Second.GetError().Left(300)
                                      : FString(bTookFast ? TEXT("fast") : TEXT("full")));
        };
        auto Is = [](auto Expected)
        {
            return [Expected](const auto& Value) { return Value == Expected; };
        };
        Twice(TEXT("bool"), TDasFunction<bool(bool)>(Script, TEXT("negate")), Is(false), true);
        Twice(TEXT("int_float"), TDasFunction<int32(int32, float)>(Script, TEXT("add")), Is(42), 40, 2.5f);
        Twice(TEXT("float"), TDasFunction<float(float)>(Script, TEXT("scale")), Is(2.5f), 1.25f);
        Twice(TEXT("double"), TDasFunction<double(double)>(Script, TEXT("half")), Is(2.5), 5.0);
        Twice(TEXT("byte"), TDasFunction<uint8(uint8)>(Script, TEXT("next_byte")), Is(uint8(42)), uint8(41));
        Twice(TEXT("string"), TDasFunction<FString(const FString&)>(Script, TEXT("greet")),
              Is(FString(TEXT("hi das"))), FString(TEXT("das")));
        Twice(TEXT("name"), TDasFunction<FName(FName)>(Script, TEXT("suffixed")), Is(FName("Tag_x")), FName("Tag"));
        Twice(TEXT("text"), TDasFunction<FText(const FText&)>(Script, TEXT("echo_text")),
              [](const FText& Value) { return Value.ToString() == TEXT("Hello"); }, FText::FromString(TEXT("Hello")));
        Twice(TEXT("vector"), TDasFunction<FVector(const FVector&)>(Script, TEXT("doubled")), Is(FVector(2, 4, 6)),
              FVector(1, 2, 3));
        Twice(TEXT("rotator"), TDasFunction<FRotator(FRotator)>(Script, TEXT("turned")), Is(FRotator(10, 110, 30)),
              FRotator(10, 20, 30));
        const FTransform Transform(FRotator(0, 45, 0), FVector(1, 2, 3), FVector(2, 2, 2));
        Twice(TEXT("transform"), TDasFunction<FTransform(const FTransform&)>(Script, TEXT("same_transform")),
              [&](const FTransform& Value) { return Value.Equals(Transform); }, Transform);
        // Types without call frame slots (both paths pack them the same way).
        Twice(TEXT("int64"), TDasFunction<int64(int64)>(Script, TEXT("next_long")), Is((int64(1) << 40) + 1),
              int64(1) << 40);
        Twice(TEXT("vector2d"), TDasFunction<FVector2D(const FVector2D&)>(Script, TEXT("swapped")),
              Is(FVector2D(2, 1)), FVector2D(1, 2));
        Twice(TEXT("quat"), TDasFunction<FQuat(const FQuat&)>(Script, TEXT("conjugate")),
              Is(FQuat(-0.1, -0.2, -0.3, 0.9)), FQuat(0.1, 0.2, 0.3, 0.9));
        Twice(TEXT("linear_color"), TDasFunction<FLinearColor(FLinearColor)>(Script, TEXT("dimmed")),
              Is(FLinearColor(0.25f, 0.5f, 1.f, 0.75f)), FLinearColor(0.5f, 1.f, 2.f, 0.75f));
        Twice(TEXT("color"), TDasFunction<FColor(FColor)>(Script, TEXT("rotated")), Is(FColor(20, 30, 10, 40)),
              FColor(10, 20, 30, 40));
        Twice(TEXT("date_time"), TDasFunction<FDateTime(FDateTime, FTimespan)>(Script, TEXT("later")),
              Is(FDateTime(2024, 2, 29, 12)), FDateTime(2024, 2, 28, 12), FTimespan::FromDays(1));
        Twice(TEXT("timespan"), TDasFunction<FTimespan(const FTimespan&)>(Script, TEXT("twice_span")),
              Is(FTimespan::FromHours(3)), FTimespan::FromMinutes(90));
        // Objects: the same object back; a result of another class reads as nullptr.
        Twice(TEXT("object"), TDasFunction<AActor*(AActor*)>(Script, TEXT("same_object")), Is(Owner), Owner);
        Twice(TEXT("object_other_class"), TDasFunction<AActor*(UObject*)>(Script, TEXT("same_object")),
              Is(static_cast<AActor*>(nullptr)), static_cast<UObject*>(Component));
        Twice(TEXT("several_parameters"), TDasFunction<FString(int32, float, FString, FVector)>(Script, TEXT("describe")),
              Is(FString(TEXT("wave:3:5:7"))), 3, 0.5f, FString(TEXT("wave")), FVector(7, 0, 0));
        // The owner's session keeps its globals between calls, on either path.
        {
            const auto Before = TDasFunction<int32()>(Script, TEXT("call_count")).Call(Owner);
            Twice(TEXT("void"), TDasFunction<void()>(Script, TEXT("count_call")),
                  [](const auto&...) { return true; });
            const auto After = TDasFunction<int32()>(Script, TEXT("call_count")).Call(Owner);
            Check(!Before.HasError() && !After.HasError() && After.GetValue() == Before.GetValue() + 2,
                  Name(TEXT("session_state")),
                  After.HasError() ? After.GetError().Left(300) : FString::FromInt(After.GetValue()));
        }

        // Errors: values, never crashes. Each comes from the runtime itself (not a compile error of the probe
        // script) and names the C++ signature.
        auto Refused = [&](const TCHAR* Case, const auto& Result, const TCHAR* Runtime, const TCHAR* Cpp)
        {
            const FString Error = Result.HasError() ? Result.GetError() : FString();
            Check(Result.HasError() && !Error.Contains(TEXT("[DAS:compile]")) && Error.Contains(Runtime) &&
                      Error.Contains(Cpp),
                  Name(Case), Result.HasError() ? Error.Left(400) : FString(TEXT("no error")));
        };
        Refused(TEXT("wrong_signature_refused"), TDasFunction<int32(int32, int32)>(Script, TEXT("add")).Call(Owner, 1, 2),
                TEXT("Expected exported add"), TEXT("(C++ expects add(int, int) : int)"));
        Refused(TEXT("wrong_result_refused"),
                TDasFunction<FString(int32, float)>(Script, TEXT("add")).Call(Owner, 1, 2.f),
                TEXT("Expected exported add"), TEXT("(C++ expects add(int, float) : string)"));
        Refused(TEXT("missing_refused"), TDasFunction<int32(int32)>(Script, TEXT("missing")).Call(Owner, 1),
                TEXT("Expected exported missing"), TEXT("(C++ expects missing(int) : int)"));
        Refused(TEXT("not_exported_refused"), TDasFunction<int32(int32)>(Script, TEXT("hidden")).Call(Owner, 1),
                TEXT("Expected exported hidden"), TEXT("(C++ expects hidden(int) : int)"));
        Refused(TEXT("int_for_int64_refused"), TDasFunction<int32(int32)>(Script, TEXT("next_long")).Call(Owner, 1),
                TEXT("Expected exported next_long"), TEXT("(C++ expects next_long(int) : int)"));
        Refused(TEXT("linear_color_for_color_refused"),
                TDasFunction<FColor(FLinearColor)>(Script, TEXT("rotated")).Call(Owner, FLinearColor::White),
                TEXT("Expected exported rotated"), TEXT("(C++ expects rotated(UELinearColor) : UEColor)"));
        Refused(TEXT("nan_vector2d_refused"),
                TDasFunction<FVector2D(FVector2D)>(Script, TEXT("swapped"))
                    .Call(Owner, FVector2D(std::numeric_limits<double>::quiet_NaN(), 0)),
                TEXT("Invalid/non-finite math struct input"), TEXT("(C++ expects swapped(UEVector2D) : UEVector2D)"));
        Refused(TEXT("null_owner_refused"), TDasFunction<int32(int32)>(Script, TEXT("fails")).Call(nullptr, 1),
                TEXT("the owner object is null or being destroyed"), TEXT("(C++ expects fails(int) : int)"));
        Refused(TEXT("script_error_reported"), TDasFunction<int32(int32)>(Script, TEXT("fails")).Call(Owner, 3),
                TEXT("[DAS:runtime]"), TEXT("(C++ expects fails(int) : int)"));
        // A script error on the second call (the fast path when it is on), then a good call again.
        {
            const TDasFunction<int32(int32)> Checked(Script, TEXT("checked"));
            const auto Good = Checked.Call(Owner, 1);
            Refused(TEXT("script_error_after_success"), Checked.Call(Owner, -1), TEXT("negative value"),
                    TEXT("(C++ expects checked(int) : int)"));
            const auto Again = Checked.Call(Owner, 2);
            Check(!Good.HasError() && !Again.HasError() && Again.GetValue() == 2, Name(TEXT("recovers_after_error")),
                  Again.HasError() ? Again.GetError().Left(300) : FString());
        }
        // An object argument that is being destroyed: refused like on the full path.
        {
            AActor* Doomed = World->SpawnActor<AActor>();
            const TDasFunction<AActor*(AActor*)> Same(Script, TEXT("same_object"));
            const auto Alive = Same.Call(Owner, Doomed);
            Doomed->Destroy();
            Refused(TEXT("destroyed_argument_refused"), Same.Call(Owner, Doomed),
                    TEXT("Object argument is expired or has wrong class"),
                    TEXT("(C++ expects same_object(UObjectHandle) : UObjectHandle)"));
            Check(!Alive.HasError() && Alive.GetValue() == Doomed, Name(TEXT("live_argument")));
        }
    }
    FastPath->Set(1, ECVF_SetByCode);

    // When the fast path is not taken: another owner, a changed script.
    {
        const TDasFunction<int32(int32, float)> Add(Script, TEXT("add"));
        Add.Call(Owner, 1, 1.f);
        const uint64 Before = DasScriptFunctionFastCalls();
        const auto OnOther = Add.Call(Other, 1, 1.f); // another owner: its own session, the full path
        const auto Back = Add.Call(Owner, 1, 1.f);    // the site now remembers Other: the full path again
        const auto Again = Add.Call(Owner, 1, 1.f);   // and fast once more
        Check(!OnOther.HasError() && !Back.HasError() && !Again.HasError() &&
                  DasScriptFunctionFastCalls() == Before + 1,
              TEXT("script_function_other_owner_takes_full_path"));
    }
    // Hot reload: a changed script runs its new body (the first call after the change takes the full path). A
    // reload that breaks the signature C++ calls is rejected and the previous version keeps running.
    {
        const TCHAR* Reload = TEXT("script_function_reload.das");
        const FString Path = R.ScriptPath(Reload);
        auto Write = [&](const TCHAR* Body)
        {
            FFileHelper::SaveStringToFile(FString(TEXT("options gen2\n[export]\n")) + Body, *Path,
                                          FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        };
        const TDasFunction<int32(int32)> Version(Reload, TEXT("version"));
        Write(TEXT("def version(base:int):int { return base + 1 }\n"));
        const auto V1 = Version.Call(Owner, 10);
        const auto V1Fast = Version.Call(Owner, 10);
        Write(TEXT("def version(base:int):int { return base + 2 }\n"));
        const uint64 Before = DasScriptFunctionFastCalls();
        const auto V2 = Version.Call(Owner, 10);
        const bool bReloadedOnFullPath = DasScriptFunctionFastCalls() == Before;
        Write(TEXT("def version(base:float):int { return int(base) }\n"));
        const auto V3 = Version.Call(Owner, 10);
        IFileManager::Get().Delete(*Path);
        Check(!V1.HasError() && V1.GetValue() == 11 && !V1Fast.HasError() && V1Fast.GetValue() == 11 &&
                  !V2.HasError() && V2.GetValue() == 12 && bReloadedOnFullPath,
              TEXT("script_function_hot_reload"),
              V1.HasError() ? V1.GetError().Left(300) : V2.HasError() ? V2.GetError().Left(300) : FString());
        Check(!V3.HasError() && V3.GetValue() == 12, TEXT("script_function_reload_signature_change_rejected"),
              V3.HasError() ? V3.GetError().Left(300) : FString::FromInt(V3.GetValue()));
    }
    // The same for a kind only typed calls have: the session keeps the int64 contract through reloads.
    {
        const TCHAR* Reload = TEXT("script_function_reload_int64.das");
        const FString Path = R.ScriptPath(Reload);
        auto Write = [&](const TCHAR* Body)
        {
            FFileHelper::SaveStringToFile(FString(TEXT("options gen2\n[export]\n")) + Body, *Path,
                                          FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        };
        const TDasFunction<int64(int64)> Long(Reload, TEXT("long_version"));
        Write(TEXT("def long_version(base:int64):int64 { return base + 1l }\n"));
        const auto L1 = Long.Call(Owner, int64(5));
        Write(TEXT("def long_version(base:int64):int64 { return base + 2l }\n"));
        const auto L2 = Long.Call(Owner, int64(5));
        Write(TEXT("def long_version(base:int):int64 { return int64(base) }\n"));
        const auto L3 = Long.Call(Owner, int64(5));
        IFileManager::Get().Delete(*Path);
        Check(!L1.HasError() && L1.GetValue() == 6 && !L2.HasError() && L2.GetValue() == 7 && !L3.HasError() &&
                  L3.GetValue() == 7,
              TEXT("script_function_reload_int64_signature_change_rejected"),
              L1.HasError()   ? L1.GetError().Left(300)
              : L2.HasError() ? L2.GetError().Left(300)
              : L3.HasError() ? L3.GetError().Left(300)
                              : FString::Printf(TEXT("%lld %lld %lld"), L1.GetValue(), L2.GetValue(), L3.GetValue()));
    }
    Check(TDasFunction<int32(int32, float)>(Script, TEXT("add")).Signature() == TEXT("add(int, float) : int"),
          TEXT("script_function_signature_text"));

    // Cost of one call (informational; the machine's speed varies between runs).
    auto Time = [&](bool bFast)
    {
        FastPath->Set(bFast ? 1 : 0, ECVF_SetByCode);
        const TDasFunction<int32(int32, float)> Add(Script, TEXT("add"));
        constexpr int32 Count = 1 << 13;
        Add.Call(Owner, 0, 0.f);
        int64 Sum = 0;
        const double Start = FPlatformTime::Seconds();
        for (int32 I = 0; I < Count; ++I)
        {
            const auto Result = Add.Call(Owner, I, 1.f);
            Sum += Result.HasError() ? -1000000 : Result.GetValue();
        }
        const double Ns = (FPlatformTime::Seconds() - Start) * 1e9 / Count;
        return Sum == int64(Count) * (Count - 1) / 2 + Count ? Ns : -1.0;
    };
    const double FastNs = Time(true), FullNs = Time(false);
    Check(FastNs > 0 && FullNs > 0, TEXT("script_function_bench"),
          FString::Printf(TEXT("add(int, float): fast %.0f ns, full %.0f ns a call"), FastNs, FullNs));
    FastPath->Set(FastPathBefore, ECVF_SetByCode);

    R.ReleaseObject(Owner);
    R.ReleaseObject(Other);
    Owner->Destroy();
    Other->Destroy();
    if (!GameWorld)
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    }
    return bPassed;
}

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasScriptFunctionTest, "DasForUE.ScriptFunction",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasScriptFunctionTest::RunTest(const FString&)
{
    FString Report;
    const bool bPassed = RunDasScriptFunctionProbe(nullptr, Report);
    TArray<FString> Lines;
    Report.ParseIntoArrayLines(Lines);
    for (const FString& Line : Lines)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_SCRIPT_FUNCTION %s"), *Line);
        if (Line.StartsWith(TEXT("FAIL "), ESearchCase::CaseSensitive))
            AddError(Line);
    }
    return bPassed;
}
#endif
