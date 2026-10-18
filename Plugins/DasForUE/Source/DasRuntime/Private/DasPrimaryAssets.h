#pragma once
#include "CoreMinimal.h"
#include "UObject/PrimaryAssetId.h"
#include "UObject/WeakObjectPtr.h"
class UAssetManager;
struct FStreamableHandle;
// Shared only among managed DAS owners. Native users must not mutate this state.
struct FDasPrimaryAssetLease {
 FPrimaryAssetId Id;
 TArray<FName> Bundles;
 TWeakObjectPtr<UAssetManager> Manager;
 TWeakPtr<FStreamableHandle> EngineHandle;
 ~FDasPrimaryAssetLease();
};
TSharedPtr<FDasPrimaryAssetLease> AcquireDasPrimaryAsset(const FPrimaryAssetId& Id,const TArray<FName>& Bundles,FString& Error);
