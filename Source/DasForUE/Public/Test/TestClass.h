#pragma once

#include "CoreMinimal.h"

#include "TestClass.generated.h"

class UStaticMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class DASFORUE_API UTestClass : public UObject {
public:

private:
	GENERATED_BODY()
public:
	UTestClass();
public:

	UFUNCTION(BlueprintCallable)
	void SaySomething(FString Msg);

	UFUNCTION(BlueprintCallable)
	void SaySomethingElse(FString Msg);

	UFUNCTION(BlueprintCallable)
	void SaySomething4(int a, FString Msg, float b, int c);

};
