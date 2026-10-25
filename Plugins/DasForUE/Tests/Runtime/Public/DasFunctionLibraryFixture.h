#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasFunctionLibraryFixture.generated.h"
UCLASS()
class DASRUNTIME_API UDasFunctionLibraryFixture : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    inline static int32 ContextMutations = 0;
    UFUNCTION(BlueprintCallable, Category = "daScript Test", meta = (UnsafeDuringActorConstruction = "true"))
    static int32 UnsafeContextProbe() { return ++ContextMutations; }

    UFUNCTION(BlueprintPure, Category = "daScript Test")
    static int32 Add(int32 A, int32 B);
    UFUNCTION(BlueprintPure, Category = "daScript Test", meta = (WorldContext = "Context"))
    static bool IsContextActor(const UObject* Context);
    UFUNCTION(BlueprintPure, Category = "daScript Test", meta = (WorldContext = "Context"))
    static int32 ContextWorldId(const UObject* Context);
    UFUNCTION(BlueprintPure, Category = "daScript Test")
    static TSubclassOf<AActor> EchoActorClass(TSubclassOf<AActor> Value);
    UFUNCTION(BlueprintPure, Category = "daScript Test")
    static UClass* EchoAnyClass(UClass* Value);
    UFUNCTION(BlueprintCallable, Category = "daScript Test")
    static void ClassOutputs(TSubclassOf<AActor> Value, TSubclassOf<AActor>& Output);
    UFUNCTION(BlueprintPure, Category = "daScript Test")
    static TArray<TSubclassOf<AActor>> EchoClassArray(const TArray<TSubclassOf<AActor>>& Values);
    UFUNCTION(BlueprintPure, Category = "daScript Test")
    static int64 UnsupportedInt64(int64 Value);
};
