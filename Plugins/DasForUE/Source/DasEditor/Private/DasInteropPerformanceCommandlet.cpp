#include "DasInteropPerformanceCommandlet.h"
#include "DasDSLService.h"
#include "DasInteropProbe.h"
#include "DasCallPhases.h"
#include "DasNativeBinds.h"
#include "DasProbeActor.h"
#include "DasPropertyValues.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Misc/Parse.h"
#include "Misc/ScopeExit.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

namespace
{
constexpr const TCHAR* InteropScript = TEXT("interop_probe_actor.das");
int64 Triangle(int32 N)
{
    return int64(N) * (N + 1) / 2;
}
FDasScalarValue IntArgument(int32 Value)
{
    FDasScalarValue Out;
    Out.Kind = 0;
    Out.IntValue = Value;
    return Out;
}
// Sets a generated Blueprint function parameter from its own type (a DSL float may be generated as double).
bool SetParam(FProperty* P, void* Frame, int32 I, float F, const FString& S, const FVector& V)
{
    if (auto* Int = CastField<FIntProperty>(P))
        Int->SetPropertyValue_InContainer(Frame, I);
    else if (auto* Float = CastField<FFloatProperty>(P))
        Float->SetPropertyValue_InContainer(Frame, F);
    else if (auto* Double = CastField<FDoubleProperty>(P))
        Double->SetPropertyValue_InContainer(Frame, F);
    else if (auto* Str = CastField<FStrProperty>(P))
        Str->SetPropertyValue_InContainer(Frame, S);
    else if (auto* Struct = CastField<FStructProperty>(P); Struct && Struct->Struct == TBaseStructure<FVector>::Get())
        *Struct->ContainerPtrToValuePtr<FVector>(Frame) = V;
    else
        return false;
    return true;
}
// Cases on an imported DSL actor: the generated Blueprint function (what a Blueprint or a C++ ProcessEvent caller
// pays), its ExecuteDasFrame bridge alone, method calls inside the script and property reads.
void RunEditorCases(FDasInteropBench& B)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Package =
        TEXT("/Game/DasInteropProbe/Run_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_InteropProbe");
    UBlueprint* BP = nullptr;
    FString Error;
    if (!FDasDSLService::Apply(InteropScript, Package, TEXT("interopprobe.InteropProbe"), BP, Error) || !BP ||
                               !BP->GeneratedClass)
    {
        B.Fail(TEXT("Import of interop_probe_actor.das failed: ") + Error);
        return;
    }
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Editor, false));
    FActorSpawnParameters Spawn;
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto* Actor = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
    ON_SCOPE_EXIT
    {
        if (Actor)
            Runtime.ReleaseObject(Actor);
        World->DestroyWorld(false);
    };
    if (!Actor)
    {
        B.Fail(TEXT("Cannot spawn the InteropProbe actor"));
        return;
    }
    FEditorScriptExecutionGuard Guard;
    constexpr int32 Heavy = 256, Light = 1 << 14, Direct = 1 << 20;
    const FString Abc = TEXT("abc");
    const FVector Unit(1, 0, 0);
    // Echo(i) == i + 1; Mixed(i, 2.0, "abc", (1,0,0)) == i + 6.
    auto EchoSum = [](int32 N)
    {
        return Triangle(N);
    };
    auto MixedSum = [](int32 N)
    {
        return Triangle(N) + 5ll * N;
    };
    auto MixedArgs = [&](int32 I)
    {
        FDasScalarValue F, S, V;
        F.Kind = 1;
        F.FloatValue = 2.f;
        S.Kind = 5;
        S.StringValue = Abc;
        V.Kind = 9;
        V.VectorValue = Unit;
        return TArray<FDasScalarValue>{IntArgument(I), F, S, V};
    };
    // A4 and A5 run with the source check on every call (_check_always) and with a file watcher (_check_watch).
    struct FMode
    {
        const TCHAR* Suffix;
        int32 Mode, Count;
    };
    for (const FMode& M : {FMode{TEXT("_check_always"), 0, Heavy}, FMode{TEXT("_check_watch"), 1, Light}})
    {
        FDasScriptChangeCheckScope ModeScope(M.Mode);
        // A4. The bridge the generated graph calls: a frame of FDasScalarValue in, one out.
        auto Frame = [&](const TCHAR* Name, const TCHAR* Entry, TFunctionRef<TArray<FDasScalarValue>(int32)> Args,
                         TFunctionRef<int64(int32)> Sum)
        {
            const FString Case = FString(Name) + M.Suffix;
            auto Calls = [&](int32 N)
            {
                int64 S = 0;
                for (int32 I = 0; I < N; ++I)
                {
                    const TArray<FDasScalarValue> Arguments = Args(I);
                    DasCallPhaseBegin();
                    const FDasScalarValue R = Actor->ExecuteDasFrame(InteropScript, Entry, Arguments, 0, FString());
                    if (R.Kind != 0)
                    {
                        B.Fail(Case + TEXT(": ") + Actor->LastDasError);
                        return int64(-1);
                    }
                    S += R.IntValue;
                }
                return S;
            };
            B.Measure(Case, TEXT("A4 C++ -> script, frame bridge"), M.Count, Calls, Sum(M.Count));
            if (M.Mode == 1)
                B.Profile(Case, M.Count, Calls);
        };
        Frame(
            TEXT("frame_bridge_int"), TEXT("ue_InteropProbe_Echo"),
                                           [](int32 I) { return TArray<FDasScalarValue>{IntArgument(I)}; }, EchoSum);
        Frame(TEXT("frame_bridge_mixed"), TEXT("ue_InteropProbe_Mixed"), MixedArgs, MixedSum);

        // A5. ProcessEvent on the generated Blueprint function: the whole path of a Blueprint or C++ caller.
        auto Event = [&](const TCHAR* Name, const TCHAR* Method, TFunctionRef<int64(int32)> Sum)
        {
            const FString Case = FString(Name) + M.Suffix;
            UFunction* Function = Actor->FindFunction(Method);
            auto* Return = Function ? CastField<FIntProperty>(Function->GetReturnProperty()) : nullptr;
            if (!Return)
            {
                B.Fail(Case + TEXT(": generated function or its int result is missing"));
                return;
            }
            FStructOnScope Params(Function);
            auto Calls = [&](int32 N)
            {
                int64 S = 0;
                void* Memory = Params.GetStructMemory();
                for (int32 I = 0; I < N; ++I)
                {
                    for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It)
                        if (!(It->PropertyFlags & CPF_ReturnParm) && !SetParam(*It, Memory, I, 2.f, Abc, Unit))
                        {
                            B.Fail(Case + TEXT(": unsupported parameter ") + It->GetName());
                            return int64(-1);
                        }
                    DasCallPhaseBegin();
                    Actor->ProcessEvent(Function, Memory);
                    DasCallPhaseMark(EDasCallPhase::BlueprintReturn);
                    S += Return->GetPropertyValue_InContainer(Memory);
                }
                return S;
            };
            B.Measure(Case, TEXT("A5 C++/Blueprint -> script, ProcessEvent"), M.Count, Calls, Sum(M.Count));
            if (M.Mode == 1)
                B.Profile(Case, M.Count, Calls);
        };
        Event(TEXT("processevent_generated_int"), TEXT("Echo"), EchoSum);
        Event(TEXT("processevent_generated_mixed"), TEXT("Mixed"), MixedSum);
    }
    // A4 without the bridge: the same frame straight into RunNamedFramePersistent.
    {
        FDasScriptChangeCheckScope Watch(1);
        const FString Path = Runtime.ScriptPath(InteropScript);
        const TCHAR* Case = TEXT("frame_direct_check_watch");
        auto Calls = [&](int32 N)
        {
            int64 S = 0;
            for (int32 I = 0; I < N; ++I)
            {
                const TArray<FDasScalarValue> Arguments{IntArgument(I)};
                DasCallPhaseBegin();
                const auto R = Runtime.RunNamedFramePersistent(Path, TEXT("ue_InteropProbe_Echo"), Arguments,
                                                                          EDasMethodSignature::Int, Actor, FString());
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
        B.Measure(Case, TEXT("A4 C++ -> script, frame bridge"), Light, Calls, Triangle(Light));
        B.Profile(Case, Light, Calls);
    }

    // B, C, D. Loops inside the script, one dispatch per sample; with a file watcher, so that dispatch and the
    // script-to-script calls through typed references do not re-read sources.
    auto Loop = [&](const TCHAR* Case, const TCHAR* Entry, const TCHAR* Group, int64 Expected, bool bProfile = false)
    {
        FDasScriptChangeCheckScope Watch(1);
        auto Run = [&](int32 N)
        {
            DasCallPhaseBegin();
            const FDasScalarValue R = Actor->ExecuteDasFrame(InteropScript, Entry, {IntArgument(N)}, 0, FString());
            if (R.Kind != 0)
            {
                B.Fail(FString(Case) + TEXT(": ") + Actor->LastDasError);
                return int64(-1);
            }
            return int64(R.IntValue);
        };
        B.Measure(Case, Group, Light, Run, Expected);
        // The loop runs inside the script: each of the N calls is profiled, the one outer dispatch adds 1/N.
        if (bProfile)
            B.Profile(Case, Light, Run);
    };
    Loop(TEXT("script_self_method_call"), TEXT("ue_InteropProbe_DirectCalls"), TEXT("C script -> script"),
                                                                                    Triangle(Light));
    Loop(TEXT("script_authored_ref_method_call"), TEXT("ue_InteropProbe_AuthoredCalls"), TEXT("C script -> script"),
                                                                                              Triangle(Light), true);
    Loop(TEXT("script_authored_field_read"), TEXT("ue_InteropProbe_FieldReads"), TEXT("D properties"), 7ll * Light);
    Loop(TEXT("script_native_self_field_read"), TEXT("ue_InteropProbe_NativeFieldReads"), TEXT("D properties"),
                                                                                               7ll * Light);

    // E. Script -> C++ through a native bind (DasNativeBinds.h): the module "smoke_native" exists only in the smoke
    // project, elsewhere the case is skipped (a registered module is built by the first compilation).
    if (!DasNativeModuleStatus(TEXT("smoke_native")).EndsWith(TEXT("is not registered")))
    {
        FDasScriptChangeCheckScope Watch(1);
        const FString Path = Runtime.ScriptPath(TEXT("native_binds_probe.das"));
        const TCHAR* Case = TEXT("script_native_bind_call");
        auto Run = [&](int32 N)
        {
            const auto R = Runtime.RunNamedPersistent(Path, TEXT("bench"), N, Actor);
            if (!R.bSucceeded)
            {
                B.Fail(FString(Case) + TEXT(": ") + R.Error);
                return int64(-1);
            }
            return int64(R.Value);
        };
        B.Measure(Case, TEXT("E script -> C++"), Light, Run, Triangle(Light));
    }
    if (!DasNativeModuleStatus(TEXT("smoke_native")).EndsWith(TEXT("is not registered")))
    {
        FDasScriptChangeCheckScope Watch(1);
        const FString Path = Runtime.ScriptPath(TEXT("native_binds_objects.das"));
        const TCHAR* Case = TEXT("script_native_method_call");
        auto Run = [&](int32 N)
        {
            const auto R = Runtime.RunNamedPersistent(Path, TEXT("bench_method"), N, Actor);
            if (!R.bSucceeded)
            {
                B.Fail(FString(Case) + TEXT(": ") + R.Error);
                return int64(-1);
            }
            return int64(R.Value);
        };
        B.Measure(Case, TEXT("E script -> C++"), Light, Run, int64(Light));
    }

    // D3. C++ reading the authored property.
    auto* Value = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Value"));
    if (!Value)
    {
        B.Fail(TEXT("Authored property Value is missing"));
        return;
    }
    B.Measure(
        TEXT("cpp_authored_property_read_cached"), TEXT("D properties"), Direct,
                                                        [&](int32 N)
                                                        {
                                                            int64 S = 0;
                                                            for (int32 I = 0; I < N; ++I)
                                                                S += Value->GetPropertyValue_InContainer(Actor);
                                                            return S;
                                                        },
                                                        7ll * Direct);
    B.Measure(
        TEXT("cpp_authored_property_read_codec"),
             TEXT("D properties"), Light,
                  [&](int32 N)
                  {
                      int64 S = 0;
                      FDasScalarValue Out;
                      FString ReadError;
                      for (int32 I = 0; I < N; ++I)
                          S += ReadDasPropertyValue(Value, Value->ContainerPtrToValuePtr<void>(Actor), Out, ReadError)
                                   ? Out.IntValue
                                   : -1;
                      return S;
                  },
                  7ll * Light);
}
}

UDasInteropPerformanceCommandlet::UDasInteropPerformanceCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasInteropPerformanceCommandlet::Main(const FString& Params)
{
    FDasInteropBench Bench;
    FString Output, Report, Json;
    FParse::Value(*Params, TEXT("PerfSamples="), Bench.Samples);
    FParse::Value(*Params, TEXT("PerfOutput="), Output);
    Bench.Samples = FMath::Max(Bench.Samples, 1);
    RunDasInteropProbe(Bench);
    RunEditorCases(Bench);
    bool Passed = Bench.Finish(Report, Json, TEXT("editor"));
    if (!Output.IsEmpty())
        Passed &= FFileHelper::SaveStringToFile(Json, *Output, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    UE_LOG(LogTemp, Display, TEXT("%s"), *Report);
    UE_LOG(LogTemp, Display, TEXT("DAS_INTEROP_PERFORMANCE_RESULT=%s"), Passed ? TEXT("PASS") : TEXT("FAIL"));
    return Passed ? 0 : 1;
}
