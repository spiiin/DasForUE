#pragma once
#include "CoreMinimal.h"
#include "DasAsyncPhysicsBridge.h"
class UPrimitiveComponent;
// Native test harness only: capability IDs never escape to another script VM.
class IDasScopedPhysicsBodyProbe
{
public:
    virtual ~IDasScopedPhysicsBodyProbe() = default;
    virtual bool VerifyIsolation(const FDasAsyncPhysicsStep& Step, FString& Error) = 0;
};
DASRUNTIME_API TUniquePtr<IDasScopedPhysicsBodyProbe>
PrepareDasScopedPhysicsBodyProbeForTests(UObject* Owner, UPrimitiveComponent* Body, FString& Error);
