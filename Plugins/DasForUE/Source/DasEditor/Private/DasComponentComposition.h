#pragma once
#include "CoreMinimal.h"
#include "DasAuthoring.h"
class UBlueprint;
bool DasPrepareComponentComposition(const FDasDeclaredClass& Declaration, const FDasDeclaredClass& Effective,
                                    const FString& ActorPackage, TMap<FString, UBlueprint*>& Components,
                                    FString& Error);
bool DasApplyComponentComposition(UBlueprint* Blueprint, const FDasDeclaredClass& Declaration,
                                  const FDasDeclaredClass& Effective, const TMap<FString, UBlueprint*>& Components,
                                  bool Create, FString& Error, const FDasDeclaredClass* Old = nullptr);
