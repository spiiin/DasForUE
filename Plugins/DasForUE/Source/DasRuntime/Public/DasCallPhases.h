#pragma once
#include "CoreMinimal.h"
#include "HAL/PlatformTime.h"

// Phase profiler of the persistent call path (C++ or Blueprint -> script), for the call-cost benchmark
// (DasInteropProbe). Off by default: a mark is then one branch. Game thread only. A mark adds the time since the
// previous mark (or DasCallPhaseBegin) to its phase, so phases follow the call in order:
// Blueprint graph -> bridge -> RunCallback steps -> scope destructors -> back in the bridge and the graph.
enum class EDasCallPhase : uint8
{
    BlueprintGraph,   // ProcessEvent of the generated function up to the ExecuteDasFrame bridge
    BridgeModule,     // bridge: runtime module lookup
    BridgeScriptPath, // bridge: script name -> path
    Entry,            // RunCallback: role, entry name and lifecycle checks
    Frame,            // argument frame copy and validation, call contract, result class checks
    PruneSessions,    // dropping sessions of dead owners
    Session,          // path, initializer state, session lookup
    SourceCheck,      // hot reload: compare sources with disk
    Reload,           // compile and swap when sources changed
    PrepareVM,        // live/ended checks, context restart, execution scopes
    Function,         // entry function lookup and contract, entry bookkeeping
    Arguments,        // packing the frame into VM arguments, output frame and widget scopes
    Eval,             // the script function itself
    Result,           // unpacking the result and output values
    Exit,             // RunCallback scope destructors (event drain, struct collection), measured by the caller
    BridgeResult,     // bridge: lifecycle error record and result copy
    BlueprintReturn,  // rest of ProcessEvent after the bridge returns
    // Script -> authored method through a typed reference (peer.Method(...)): a reflected call frame.
    ScriptBetween,     // script code since the previous mark: result reads of the last call, the loop
    AuthoredContract,  // object identity check, method contract (JSON wire) lookup
    AuthoredFrame,     // UFunction lookup, call signature, parameter frame allocation
    AuthoredValidate,  // UFunction parameters against the contract
    AuthoredArguments, // argument writes by name
    InvokeChecks,      // ue_invoke_call checks before ProcessEvent
    InvokeOutputs,     // output pins and container outputs after ProcessEvent
    Count
};
struct FDasCallPhaseState
{
    bool bEnabled = false;
    uint64 Last = 0;
    uint64 Cycles[static_cast<int32>(EDasCallPhase::Count)] = {};
};
DASRUNTIME_API FDasCallPhaseState& DasCallPhaseState();
DASRUNTIME_API const TCHAR* DasCallPhaseName(EDasCallPhase Phase);
inline void DasCallPhaseBegin()
{
    auto& S = DasCallPhaseState();
    if (S.bEnabled)
        S.Last = FPlatformTime::Cycles64();
}
inline void DasCallPhaseMark(EDasCallPhase Phase)
{
    auto& S = DasCallPhaseState();
    if (S.bEnabled)
    {
        const uint64 Now = FPlatformTime::Cycles64();
        S.Cycles[static_cast<int32>(Phase)] += Now - S.Last;
        S.Last = Now;
    }
}
