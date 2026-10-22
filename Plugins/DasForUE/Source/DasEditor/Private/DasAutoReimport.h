#pragma once
#include "CoreMinimal.h"
#include "Containers/Ticker.h"

struct FFileChangeData;
struct FDasReimportPlan;
class UBlueprint;
class SNotificationItem;

// Reimports DSL Blueprints when their .das source is saved (Das.DSL.AutoReimport, on by default). Uses the
// same safe automatic policy as Das.DSL.Reimport, also during PIE: additive declaration changes reinstance
// the running actors; unsafe changes are refused and reported. A change refused while PIE runs (a new
// default value, for example) is tried again when PIE ends. Blueprints whose script changed while the editor
// was closed or nobody saved are brought up to date when a map opens and before PIE starts. GAS assets
// (Das.GASImport) are updated in place the same way: every loaded asset remembers its script and folder.
// A field migration (rename, removal, numeric retype) that would also modify saved maps or other Blueprints
// waits for Apply in a notification that lists the changes and the assets (Details shows all of them).
// Struct scripts (ue_struct, imported with Das.DSL.Types.Apply) migrate their structs the same way: removals and
// numeric retypes, and changes that reach Blueprints, maps or DataTables, ask first.
class FDasAutoReimport
{
public:
    void Start();
    void Stop();
    // Reimports the loaded Blueprints bound to these .das files (full, normalized paths).
    void ReimportFiles(const TSet<FString>& Files);
    // Reimports the Blueprints refused during PIE; runs once PIE has ended.
    void RetryDeferred();
    int32 DeferredCount() const { return Deferred.Num() + DeferredGAS.Num() + DeferredTypes.Num(); }
    // Reimports every loaded DSL Blueprint whose declaration differs from its script.
    void ReimportStale();
    // Changes waiting for Apply / Cancel, by Blueprint path.
    int32 PendingConfirmations() const { return Confirmations.Num(); }
    // Keys of the waiting changes: a Blueprint path, or "types:<script>|<registry>" for struct scripts.
    TArray<FString> PendingKeys() const
    {
        TArray<FString> Keys;
        Confirmations.GetKeys(Keys);
        return Keys;
    }
    // Apply: migrates the class (or the structs) together with the assets that use it.
    void Confirm(FString Key);
    // Cancel: leaves everything as it is; the Blueprint Log keeps an Apply link.
    void Decline(FString Key);

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
    // (script, folder) of the loaded GAS assets imported from one of Files (all when null).
    TSet<TPair<FString, FString>> BoundGAS(const TSet<FString>* Files) const;
    void ReimportGAS(const TPair<FString, FString>& Import, bool bRetry);
    // (script, type registry) of the loaded authored structs imported from one of Files.
    TSet<TPair<FString, FString>> BoundTypes(const TSet<FString>& Files) const;
    void ReimportTypes(const TPair<FString, FString>& Import, bool bRetry);
    void AskConfirmation(const FString& Key, const FString& Name, const FDasReimportPlan& Plan);
    void ShowDetails(FString Key);
    void CloseConfirmation(const FString& Key);

    TArray<TPair<FString, FDelegateHandle>> Watches;
    TSet<FString> Pending;
    TArray<TWeakObjectPtr<UBlueprint>> Deferred;
    TSet<TPair<FString, FString>> DeferredGAS, DeferredTypes;
    TMap<FString, TWeakPtr<SNotificationItem>> Confirmations;
    TMap<FString, FString> PlanDetails, PlanNames;
    // Blueprints whose change the user applied: reimported with dependencies (also after PIE).
    TSet<FString> Confirmed;
    // Script timestamp and declaration of each Blueprint at its last stale check.
    TMap<FString, TPair<FDateTime, FString>> Checked;
    FTSTicker::FDelegateHandle Ticker, RetryTicker, StaleTicker;
    FDelegateHandle EndPIEHandle, PreBeginPIEHandle, MapOpenedHandle;
};
