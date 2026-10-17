#pragma once
#include "DasAuthoring.h"
#include "UObject/WeakObjectPtr.h"

// Private runtime service. Paths are the existing canonical carrier names;
// descriptors and layout fingerprints guard reuse of those names.
class FDasContainerRegistry
{
public:
    explicit FDasContainerRegistry(int32 InLimit = 16384) : Limit(InLimit) {}
    ~FDasContainerRegistry();
    FDasContainerRegistry(const FDasContainerRegistry&) = delete;
    FDasContainerRegistry& operator=(const FDasContainerRegistry&) = delete;

    UScriptStruct* GetOrCreate(const FString& Path, const FDasDeclaredType& Type, FString& Error);
    void Reset();
    int32 Num() const { return Entries.Num(); }

private:
    struct FEntry
    {
        TWeakObjectPtr<UScriptStruct> Carrier;
        FString Fingerprint;
    };
    TMap<FString, FEntry> Entries;
    int32 Limit;
};

bool CheckDasContainerRegistry(FString& Error);
