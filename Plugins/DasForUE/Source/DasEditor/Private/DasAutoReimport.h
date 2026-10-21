#pragma once
#include "CoreMinimal.h"
#include "Containers/Ticker.h"

struct FFileChangeData;

// Reimports DSL Blueprints when their .das source is saved (Das.DSL.AutoReimport, on by default). Uses the
// same safe automatic policy as Das.DSL.Reimport, also during PIE: additive declaration changes reinstance
// the running actors; unsafe changes are refused and reported.
class FDasAutoReimport
{
public:
    void Start();
    void Stop();

private:
    void OnScriptsChanged(const TArray<FFileChangeData>& Changes);
    bool ProcessPending(float);

    TArray<TPair<FString, FDelegateHandle>> Watches;
    TSet<FString> Pending;
    FTSTicker::FDelegateHandle Ticker;
};
