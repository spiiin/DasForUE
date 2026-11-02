#pragma once
#include "CoreMinimal.h"
#include "DasAuthoring.h"
#include "UObject/StrongObjectPtr.h"
class UBlueprint;

struct FDasInterfaceChange
{
    UBlueprint* Blueprint = nullptr;
    FDasDeclaredClass Before, After;
};
// Checks graph consumers before changing signatures; rebuilds only compatible uses.
bool PrepareDasInterfaceMigration(const TArray<FDasInterfaceChange>& Changes, TArray<UBlueprint*>& Scope,
                                  TArray<TStrongObjectPtr<UObject>>& Assets, FString& Error);
bool RefreshDasInterfaceConsumers(const TArray<FDasInterfaceChange>& Changes, const TArray<UBlueprint*>& Scope,
                                  FString& Error);
