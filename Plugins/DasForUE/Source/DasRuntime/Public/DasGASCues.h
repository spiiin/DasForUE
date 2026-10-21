#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayCueNotify_Static.h"
#include "DasGASAdvancedTypes.h"
#include "DasGASCues.generated.h"
// Native routing is present in Shipping too; no debug-manager delegate is used.
UCLASS(BlueprintType, Blueprintable)
class DASRUNTIME_API UDasGameplayCueNotify : public UGameplayCueNotify_Static
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FString DefinitionId;
    UPROPERTY()
    FString DefinitionHash;
    UPROPERTY()
    FString ScriptFile;
    UPROPERTY()
    FString ScriptContract;
    UPROPERTY()
    TMap<FName, FString> Hooks;
    UDasGameplayCueNotify(const FObjectInitializer& ObjectInitializer);
    virtual void HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type Event,
                                   const FGameplayCueParameters& Parameters) override;
};
// One session per target ASC and notify class; static notify CDOs never own mutable DAS state.
UCLASS()
class DASRUNTIME_API UDasGASCueSession : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(Transient)
    TWeakObjectPtr<AActor> Target;
    UPROPERTY(Transient)
    TObjectPtr<UDasGameplayCueNotify> Definition;
    UPROPERTY(Transient)
    TArray<FDasGASCueEvent> Pending;
    bool Closed = false;
    bool Live() const;
    void Close();
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void BeginDestroy() override;
};
DASRUNTIME_API bool RegisterDasGASCueAssets(const TArray<TSubclassOf<UDasGameplayCueNotify>>& Classes);
