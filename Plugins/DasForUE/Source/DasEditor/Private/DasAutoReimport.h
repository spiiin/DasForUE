#pragma once
#include "CoreMinimal.h"
#include "Containers/Ticker.h"

struct FFileChangeData;
class UBlueprint;

// Reimports DSL Blueprints when their .das source is saved (Das.DSL.AutoReimport, on by default). Uses the
// same safe automatic policy as Das.DSL.Reimport, also during PIE: additive declaration changes reinstance
// the running actors; unsafe changes are refused and reported. A change refused while PIE runs (a new
// default value, for example) is tried again when PIE ends. Blueprints whose script changed while the editor
// was closed or nobody saved are brought up to date when a map opens and before PIE starts.
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
    // Reimports every loaded DSL Blueprint whose declaration differs from its script.
    void ReimportStale();

private:
    void OnScriptsChanged(const TArray<FFileChangeData>& Changes);
    bool ProcessPending(float);
    void OnEndPIE(bool);
    bool RetryAfterPIE(float);
    void OnPreBeginPIE(bool);
    void OnMapOpened(const FString&, bool);
    bool CheckStale(float);
    TArray<UBlueprint*> Bound(const TSet<FString>* Files) const;
    void Reimport(UBlueprint* Blueprint, bool bRetry);

    TArray<TPair<FString, FDelegateHandle>> Watches;
    TSet<FString> Pending;
    TArray<TWeakObjectPtr<UBlueprint>> Deferred;
    // Script timestamp and declaration of each Blueprint at its last stale check.
    TMap<FString, TPair<FDateTime, FString>> Checked;
    FTSTicker::FDelegateHandle Ticker, RetryTicker, StaleTicker;
    FDelegateHandle EndPIEHandle, PreBeginPIEHandle, MapOpenedHandle;
};
