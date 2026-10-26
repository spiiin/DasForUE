#pragma once
#include "DasRuntime.h"
// Native test fixture: compile on ordinary GT, exercise FFI policy without
// exposing RunFile/compilation or a script callback API to PhysicsPreSim.
class DASRUNTIME_API IDasExecutionGuardProbe
{
public:
    virtual ~IDasExecutionGuardProbe() = default;
    virtual FDasExecutionResult Evaluate(int32 Mode) = 0;
    virtual int32 ExternRegistrationCount() const = 0;
};
DASRUNTIME_API TUniquePtr<IDasExecutionGuardProbe>
PrepareDasExecutionGuardProbeForTests(const FString& Path, UObject* Owner, FString& Error);
