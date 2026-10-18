#pragma once
#include "Commandlets/Commandlet.h"
#include "DasScriptUserWidget.h"
#include "DasDragCommandlet.generated.h"
UCLASS()
class UDasDragCommandlet:public UCommandlet {GENERATED_BODY() public:UDasDragCommandlet();virtual int32 Main(const FString& Params)override;};
UCLASS()
class UDasDragFixture:public UDasScriptUserWidget {
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable) void DestroyDuringDragForTest(){NativeDestruct();}
 UFUNCTION(BlueprintCallable) void CollectDragGarbageForTest();
};
