#pragma once
#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "DasDefinedEnum.generated.h"
// UUserDefinedEnum normalizes values when loading in Editor. This UEnum asset
// keeps the explicit values authored by daScript through serialization/cooking.
UCLASS()
class DASRUNTIME_API UDasDefinedEnum : public UEnum
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FString StableId;
    UPROPERTY()
    TMap<int64, FString> MemberIds;
    virtual bool IsAsset() const override
    {
        return HasAnyFlags(RF_Public) && !HasAnyFlags(RF_Transient | RF_ClassDefaultObject);
    }
};
