#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasFrameLibrary.h"
#include "DasBlueprintLibrary.generated.h"
class UDasTypeRegistry;

// Infrastructure CDO owns library VMs; no authored instance or lifecycle exists.
UCLASS()
class DASRUNTIME_API UDasBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UPROPERTY()
    TObjectPtr<UDasTypeRegistry> TypeRegistry;
    UPROPERTY()
    TArray<TObjectPtr<UObject>> ValueTypes;
    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    static FDasScalarValue ExecuteLibraryFrame(const FString& LibraryContract, const FString& ScriptFile,
                                               const FString& EntryPoint, const TArray<FDasScalarValue>& Arguments,
                                               int32 ResultKind, const FString& ResultClass);
    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    static FDasScalarValue ExecuteLibraryFrameOutputs(const FString& LibraryContract, const FString& ScriptFile,
                                                      const FString& EntryPoint,
                                                      const TArray<FDasScalarValue>& Arguments, int32 ResultKind,
                                                      const FString& ResultClass, TArray<FDasScalarValue>& Outputs);
};
