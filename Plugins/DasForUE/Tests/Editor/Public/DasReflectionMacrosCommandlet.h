#pragma once
#include "Commandlets/Commandlet.h"
#include "GameFramework/Actor.h"
#include "DasProbeInterface.h"
#include "DasReflectionMacrosCommandlet.generated.h"
UCLASS()
class ADasReflectionMacroActor : public AActor, public IDasProbeInterface
{
    GENERATED_BODY()
public:
    virtual int32 Query_Implementation(int32 Input) override { return Input + 37; }
    virtual FString Label_Implementation() override { return TEXT("reflection"); }
    virtual TArray<double> Weights_Implementation(const TArray<double>& Values) override { return Values; }
    virtual void Stats_Implementation(int32& Counter, int32& Total) override
    {
        Counter += 2;
        Total = Counter + 3;
    }
};
UCLASS()
class UDasReflectionMacrosCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasReflectionMacrosCommandlet();
    virtual int32 Main(const FString& Params) override;
};
