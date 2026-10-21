#pragma once
#include "CoreMinimal.h"
#include "Containers/Ticker.h"

struct FFileChangeData;
class UBlueprint;

// Reimports DSL Blueprints when their .das source is saved (Das.DSL.AutoReimport, on by default). Uses the
// same safe automatic policy as Das.DSL.Reimport, also during PIE: additive declaration changes reinstance
// the running actors; unsafe changes are refused and reported. A change refused while PIE runs (a new
// default value, for example) is tried again when PIE ends.
class FDasAutoReimport
{
public:
    void Start();
    void Stop();
    // Reimports the loaded Blueprints bound to these .das files (full, normalized paths).
    void ReimportFiles(const TSet<FString>& Files);
    // Reimports the Blueprints refused during PIE; runs once PIE has ended.
    void RetryDeferred();
    int32 DeferredCount() const { return Deferred.Num(); }

private:
    void OnScriptsChanged(const TArray<FFileChangeData>& Changes);
    bool ProcessPending(float);
    void OnEndPIE(bool);
    bool RetryAfterPIE(float);
    void Reimport(UBlueprint* Blueprint, bool bRetry);

    TArray<TPair<FString, FDelegateHandle>> Watches;
    TSet<FString> Pending;
    TArray<TWeakObjectPtr<UBlueprint>> Deferred;
    FTSTicker::FDelegateHandle Ticker, RetryTicker;
    FDelegateHandle EndPIEHandle;
};
