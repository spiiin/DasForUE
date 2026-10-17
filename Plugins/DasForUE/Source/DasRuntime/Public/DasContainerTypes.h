#pragma once
#include "CoreMinimal.h"
#include "DasContainerTypes.generated.h"

// Internal owned transport; public authored function pins use Values' native container type.
USTRUCT()
struct DASRUNTIME_API FDasArrayIntCarrier {
    GENERATED_BODY()
    UPROPERTY() TArray<int32> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasArrayFloatCarrier {
    GENERATED_BODY()
    UPROPERTY() TArray<float> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasArrayDoubleCarrier {
    GENERATED_BODY()
    UPROPERTY() TArray<double> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasArrayBoolCarrier {
    GENERATED_BODY()
    UPROPERTY() TArray<bool> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasArrayStringCarrier {
    GENERATED_BODY()
    UPROPERTY() TArray<FString> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasArrayNameCarrier {
    GENERATED_BODY()
    UPROPERTY() TArray<FName> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasSetIntCarrier {
    GENERATED_BODY()
    UPROPERTY() TSet<int32> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasSetStringCarrier {
    GENERATED_BODY()
    UPROPERTY() TSet<FString> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapIntIntCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<int32,int32> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapIntFloatCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<int32,float> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapIntDoubleCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<int32,double> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapIntBoolCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<int32,bool> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapIntStringCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<int32,FString> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapIntNameCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<int32,FName> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapStringIntCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<FString,int32> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapStringFloatCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<FString,float> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapStringDoubleCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<FString,double> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapStringBoolCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<FString,bool> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapStringStringCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<FString,FString> Values;
};
USTRUCT()
struct DASRUNTIME_API FDasMapStringNameCarrier {
    GENERATED_BODY()
    UPROPERTY() TMap<FString,FName> Values;
};

USTRUCT()
struct DASRUNTIME_API FDasSetNameCarrier { GENERATED_BODY() UPROPERTY() TSet<FName> Values; };

USTRUCT()
struct DASRUNTIME_API FDasArrayByteCarrier { GENERATED_BODY() UPROPERTY() TArray<uint8> Values; };
USTRUCT()
struct DASRUNTIME_API FDasArrayTextCarrier { GENERATED_BODY() UPROPERTY() TArray<FText> Values; };
USTRUCT()
struct DASRUNTIME_API FDasSetByteCarrier { GENERATED_BODY() UPROPERTY() TSet<uint8> Values; };
USTRUCT()
struct DASRUNTIME_API FDasMapIntByteCarrier { GENERATED_BODY() UPROPERTY() TMap<int32,uint8> Values; };
USTRUCT()
struct DASRUNTIME_API FDasMapIntTextCarrier { GENERATED_BODY() UPROPERTY() TMap<int32,FText> Values; };
USTRUCT()
struct DASRUNTIME_API FDasMapStringByteCarrier { GENERATED_BODY() UPROPERTY() TMap<FString,uint8> Values; };
USTRUCT()
struct DASRUNTIME_API FDasMapStringTextCarrier { GENERATED_BODY() UPROPERTY() TMap<FString,FText> Values; };

struct FDasDeclaredType;
class UScriptStruct;class FProperty;
DASRUNTIME_API void ResetDasContainerCarriers();
DASRUNTIME_API UScriptStruct* DasContainerCarrier(const FDasDeclaredType& Type);
DASRUNTIME_API UScriptStruct* DasContainerCarrierForProperty(const FProperty* Property);
DASRUNTIME_API FProperty* DasContainerProperty(const FString& Path);

DASRUNTIME_API bool IsDasObjectHashProperty(const FProperty* Property);
