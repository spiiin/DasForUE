#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasDelegateCommandlet.generated.h"
UCLASS(Blueprintable)
class UDasDelegateValidationReceiver : public UObject
{
    GENERATED_BODY()
public:
    int32 Count = 0;
    UFUNCTION(BlueprintCallable)
    void Receive(int32 Counter) { Count += Counter; }
};
UCLASS()
class UDasDelegateCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasDelegateCommandlet();
    virtual int32 Main(const FString& Params) override;
};
