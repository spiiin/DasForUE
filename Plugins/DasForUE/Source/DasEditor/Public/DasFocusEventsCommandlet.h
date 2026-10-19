#pragma once
#include "Commandlets/Commandlet.h"
#include "DasScriptUserWidget.h"
#include "DasFocusEventsCommandlet.generated.h"
UCLASS()
class UDasFocusEventsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasFocusEventsCommandlet();
    virtual int32 Main(const FString& Params) override;
};
UCLASS()
class UDasFocusEventsFixture : public UDasScriptUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    void DestroyDuringFocusForTest() { NativeDestruct(); }
    UFUNCTION(BlueprintCallable)
    void ReenterFocusForTest()
    {
        NativeOnFocusLost(FFocusEvent(EFocusCause::Cleared, 37));
        bReentryRejected = LastInputError.Contains(TEXT("idle game-thread"));
    }
    bool bReentryRejected = false;
};
