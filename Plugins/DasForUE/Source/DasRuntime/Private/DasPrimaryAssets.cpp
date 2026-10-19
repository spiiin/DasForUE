#include "DasPrimaryAssets.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
namespace
{
TMap<FPrimaryAssetId, TWeakPtr<FDasPrimaryAssetLease>> Leases;
}
FDasPrimaryAssetLease::~FDasPrimaryAssetLease()
{
    if (IsInGameThread())
        if (auto* M = Manager.Get())
        {
            // Never unload a replacement state installed by external/native code.
            const auto Expected = EngineHandle.Pin();
            if (Expected.IsValid() && M->GetPrimaryAssetHandle(Id) == Expected)
                M->UnloadPrimaryAsset(Id);
        }
}
TSharedPtr<FDasPrimaryAssetLease> AcquireDasPrimaryAsset(const FPrimaryAssetId& Id, const TArray<FName>& Bundles,
                                                         FString& Error)
{
    check(IsInGameThread());
    auto* M = UAssetManager::GetIfInitialized();
    if (!M)
    {
        Error = TEXT("AssetManager is not initialized");
        return {};
    }
    for (auto I = Leases.CreateIterator(); I; ++I)
        if (!I.Value().IsValid())
            I.RemoveCurrent();
    if (auto* Found = Leases.Find(Id))
        if (auto Lease = Found->Pin())
        {
            if (Lease->Bundles != Bundles)
            {
                Error = TEXT("Primary Asset has managed owners with a different bundle set; unload before changing bundles");
                return {};
            }
            if (auto Handle = Lease->EngineHandle.Pin(); Handle.IsValid() && M->GetPrimaryAssetHandle(Id) != Handle)
            {
                Error = TEXT("Primary Asset state changed outside the managed adapter");
                return {};
            }
            return Lease;
        }
    if (M->GetPrimaryAssetHandle(Id).IsValid())
    {
        Error = TEXT("Primary Asset already has an externally owned AssetManager load state");
        return {};
    }
    if (Leases.Num() >= 128)
    {
        Error = TEXT("Managed Primary Asset count limit reached");
        return {};
    }
    auto Lease = MakeShared<FDasPrimaryAssetLease>();
    Lease->Id = Id;
    Lease->Bundles = Bundles;
    Lease->Manager = M;
    Leases.Add(Id, Lease);
    return Lease;
}
