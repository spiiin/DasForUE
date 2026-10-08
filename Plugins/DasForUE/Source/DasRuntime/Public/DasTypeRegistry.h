#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DasTypeRegistry.generated.h"

USTRUCT()
struct FDasTypeBinding {
 GENERATED_BODY()
 UPROPERTY() FString Id;
 UPROPERTY() bool bEnum=false;
 UPROPERTY() TObjectPtr<UObject> Type;
 UPROPERTY() FGuid StructGuid;
};

// Runtime identity catalog. Hard references make registered types cook dependencies.
UCLASS()
class DASRUNTIME_API UDasTypeRegistry : public UDataAsset {
 GENERATED_BODY()
public:
 UPROPERTY() TArray<FDasTypeBinding> Bindings;
 bool Validate(FString& Error)const;
 UObject* Resolve(const FString& Id,bool bEnum,FString& Error)const;
};
DASRUNTIME_API UObject* ResolveDasRegisteredType(const FString& RegistryPath,const FString& Id,bool bEnum,FString& Error);
