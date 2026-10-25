#pragma once
#include "CoreMinimal.h"
#include "Templates/Function.h"

// Call-cost benchmark between C++, Blueprint and daScript (Docs/Dev/Architecture/Performance.md). Informational:
// each case reports nanoseconds per call (median and p95 over samples) and checks a checksum; no thresholds.
struct DASRUNTIME_API FDasInteropRow
{
    FString Name, Group;
    int32 Calls = 0;
    double MedianNs = 0, P95Ns = 0;
    TArray<double> SampleNs;
    // Profile: ns per call of each EDasCallPhase met, and the whole call with the phase marks on.
    TArray<TPair<FString, double>> PhaseNs;
    double ProfiledNs = 0;
};
// Two paths of one case timed side by side (B = AOT against A = the interpreter): the ratio B / A of each round
// does not depend on the machine's state, which shifts absolute times between runs by up to 2x.
struct DASRUNTIME_API FDasInteropPair
{
    FString Name, NameA, NameB;
    double RatioMedian = 0, RatioP05 = 0, RatioP95 = 0;
    TArray<double> Ratios;
};
struct DASRUNTIME_API FDasInteropBench
{
    int32 Samples = 9;
    int32 Rounds = 21; // MeasurePair
    bool bPassed = true;
    FString Failure;
    TArray<FDasInteropRow> Rows;
    TArray<FDasInteropPair> Pairs;
    // Warms Body(Count) twice, then times it Samples times; every run must return Expected.
    void Measure(const FString& Name, const FString& Group, int32 Count, TFunctionRef<int64(int32)> Body,
                 int64 Expected);
    // Same, but each sample is Full(Count) minus Zero(): removes a fixed per-run cost such as script compilation.
    void MeasureDifference(const FString& Name, const FString& Group, int32 Count, TFunctionRef<int64(int32)> Full,
                           TFunctionRef<int64()> Zero, int64 Expected);
    // Paths A and B in Rounds rounds (A then B, then B then A, against drift); a sample is Run(Path, Count) minus
    // Run(Path, 0), every run of Count must return Expected. Adds the rows NameA and NameB and the pair Name.
    void MeasurePair(const FString& Name, const FString& NameA, const FString& NameB, const FString& Group,
                     int32 Count, TFunctionRef<int64(bool bB, int32 N)> Run, int64 Expected);
    // Runs Body(Count) once more with the call phase profiler on and attaches the phases to the measured row
    // Name. Body calls DasCallPhaseBegin before each call and marks the phase that ends in the caller.
    void Profile(const FString& Name, int32 Count, TFunctionRef<int64(int32)> Body);
    void Fail(const FString& Reason);
    // "INTEROP <case> ns_per_call=..." and "INTEROP_AB <pair> ... ratio=..." lines and the JSON report; returns
    // bPassed.
    bool Finish(FString& Report, FString& Json, const FString& Scope) const;
};
// Sets das.ScriptChangeCheck (and the watch poll interval) for a scope: 0 compare sources on every call,
// 1 only after NotifyScriptsChanged or the poll interval.
struct DASRUNTIME_API FDasScriptChangeCheckScope
{
    explicit FDasScriptChangeCheckScope(int32 Mode, float PollSeconds = 1.f);
    ~FDasScriptChangeCheckScope();

private:
    int32 OldMode;
    float OldPoll;
};
// Cases that need no imported assets (editor and packaged game): C++ -> script, script -> C++/UE, script calls,
// properties, and AOT against the interpreter. The editor commandlet adds the cases that need a DSL class.
DASRUNTIME_API void RunDasInteropProbe(FDasInteropBench& Bench);
