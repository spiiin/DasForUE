#pragma once
#include "Commandlets/Commandlet.h"
#include "DasScriptUserWidget.h"
#include "DasPaintCommandlet.generated.h"
UCLASS()
class UDasPaintCommandlet:public UCommandlet {GENERATED_BODY() public:UDasPaintCommandlet();virtual int32 Main(const FString& Params)override;};
UCLASS()
class UDasPaintFixture:public UDasScriptUserWidget {
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable) void CollectPaintGarbageForTest();
 UFUNCTION(BlueprintCallable) bool NestedPaintForTest();
 UFUNCTION(BlueprintCallable) void DestructDuringPaintForTest(){NativeDestruct();}
};
