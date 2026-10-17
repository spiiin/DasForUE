#pragma once
#include "CoreMinimal.h"
#include "DasDefaultValueProbe.generated.h"
USTRUCT()
struct FDasDefaultNestedProbe {
 GENERATED_BODY()
 UPROPERTY() TArray<FVector> Positions;
 UPROPERTY() TArray<TObjectPtr<UObject>> Assets;
 UPROPERTY() TMap<FName,TObjectPtr<UObject>> Lookup;
 UPROPERTY() TSet<FName> Tags;
 UPROPERTY() FText Caption;
 UPROPERTY() uint8 Priority=0;
};
USTRUCT()
struct FDasDefaultBundleProbe {
 GENERATED_BODY()
 UPROPERTY() FDasDefaultNestedProbe Nested;
 UPROPERTY() TArray<FDasDefaultNestedProbe> Rows;
 UPROPERTY() TMap<FString,FDasDefaultNestedProbe> Catalog;
};
DASRUNTIME_API bool CheckDasRecursiveDefaults(FString& Error);
