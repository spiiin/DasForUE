#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasWorldComponentLibrary.generated.h"

class UActorComponent;
class UInstancedStaticMeshComponent;

// Generic world-owned components and native stable ISM IDs. No gameplay policy.
UCLASS()
class DASRUNTIME_API UDasWorldComponentLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="daScript|Components", meta=(WorldContext="WorldContextObject"))
    static UActorComponent* CreateNativeWorldComponent(UObject* WorldContextObject,
                                                       const FString& NativeClassPath, FName ComponentName);

    UFUNCTION(BlueprintCallable, Category="daScript|Components", meta=(WorldContext="WorldContextObject"))
    static bool RegisterWorldComponent(UObject* WorldContextObject, UActorComponent* Component);

    UFUNCTION(BlueprintCallable, Category="daScript|Components")
    static bool DestroyWorldComponent(UActorComponent* Component);

    UFUNCTION(BlueprintCallable, Category="daScript|Instances")
    static TArray<int32> AddInstancesById(UInstancedStaticMeshComponent* Mesh,
                                         const TArray<FTransform>& Transforms, bool WorldSpace,
                                         bool UpdateNavigation);

    UFUNCTION(BlueprintCallable, Category="daScript|Instances")
    static bool RemoveInstancesById(UInstancedStaticMeshComponent* Mesh, const TArray<int32>& Ids,
                                    bool UpdateNavigation);

    UFUNCTION(BlueprintPure, Category="daScript|Instances")
    static int32 GetInstanceIndexById(UInstancedStaticMeshComponent* Mesh, int32 Id);
};
