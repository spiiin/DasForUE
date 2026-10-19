#pragma once
#include "Commandlets/Commandlet.h"
#include "Engine/DataAsset.h"
#include "DasPrimaryAssetsCommandlet.generated.h"
UCLASS()
class UDasPrimaryAssetFixture : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, Category = "Fixture")
    int32 Score = 73;
};
UCLASS()
class UDasPrimaryAssetsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasPrimaryAssetsCommandlet();
    virtual int32 Main(const FString& Params) override;
};
