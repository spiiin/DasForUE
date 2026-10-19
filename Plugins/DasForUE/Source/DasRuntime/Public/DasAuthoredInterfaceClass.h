#pragma once
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Blueprint.h"
#include "DasAuthoredInterfaceClass.generated.h"
// Persistent contract identity survives Blueprint stripping during cook.
UCLASS()
class DASRUNTIME_API UDasAuthoredInterfaceClass : public UBlueprintGeneratedClass
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FString StableId;
    UPROPERTY()
    FString ContractJson;
};

UCLASS()
class DASRUNTIME_API UDasAuthoredInterfaceBlueprint : public UBlueprint
{
    GENERATED_BODY()
public:
    UPROPERTY(AssetRegistrySearchable)
    FString StableId;
#if WITH_EDITOR
    virtual UClass* GetBlueprintClass() const override { return UDasAuthoredInterfaceClass::StaticClass(); }
#endif
};
