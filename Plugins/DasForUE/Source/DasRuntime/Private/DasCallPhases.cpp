#include "DasCallPhases.h"

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
