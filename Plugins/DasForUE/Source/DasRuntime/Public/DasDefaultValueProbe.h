#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
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

// Count real native struct copies, including copies hidden inside FInstancedStruct.
// This fixture is used by query regressions in both Editor and packaged runs.
USTRUCT()
struct DASRUNTIME_API FDasSnapshotReadProbe : public FDasDefaultBundleProbe {
 GENERATED_BODY()
 static TAtomic<int32> CopyCount;
 UPROPERTY() TMap<int32,int32> IntMap;
 UPROPERTY() TSet<int32> IntSet;
 FDasSnapshotReadProbe& operator=(const FDasSnapshotReadProbe& Other){
  ++CopyCount;FDasDefaultBundleProbe::operator=(Other);
  IntMap=Other.IntMap;IntSet=Other.IntSet;return *this;
 }
};
template<> struct TStructOpsTypeTraits<FDasSnapshotReadProbe>:TStructOpsTypeTraitsBase2<FDasSnapshotReadProbe>{enum{WithCopy=true};};
UCLASS()
class DASRUNTIME_API UDasSnapshotReadOwner : public UObject {
 GENERATED_BODY()
public:
 UPROPERTY() FDasSnapshotReadProbe Snapshot;
};
