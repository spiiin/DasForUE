#pragma once
#include "Commandlets/Commandlet.h"
#include "DasScriptUserWidget.h"
#include "DasInputNavigationCommandlet.generated.h"
UCLASS()
class UDasInputNavigationCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasInputNavigationCommandlet();
    virtual int32 Main(const FString& Params) override;
};
UCLASS()
class UDasInputNavigationFixture : public UDasScriptUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    void DestroyDuringNavigationForTest() { NativeDestruct(); }
};
