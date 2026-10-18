#pragma once
#include "Commandlets/Commandlet.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "DasPerceptionCommandlet.generated.h"
class UAIPerceptionComponent;
UCLASS()
class ADasPerceptionHost : public AAIController {
 GENERATED_BODY()
public:
 UPROPERTY(BlueprintReadOnly) TObjectPtr<UAIPerceptionComponent> Perception;
};
UCLASS()
class UDasPerceptionObserver : public UObject {
 GENERATED_BODY()
public:
 TWeakObjectPtr<AActor> Expected;
 int32 SightOn=0,SightOff=0,HearingOn=0,Errors=0;
 UFUNCTION() void OnTarget(AActor* Actor,FAIStimulus Stimulus);
};
UCLASS()
class UDasPerceptionCommandlet : public UCommandlet {
 GENERATED_BODY()
public:
 UDasPerceptionCommandlet();
 virtual int32 Main(const FString& Params)override;
};
